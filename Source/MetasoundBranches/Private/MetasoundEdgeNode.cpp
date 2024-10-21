#include "MetasoundBranches/Public/MetasoundEdgeNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros
#include "MetasoundTrigger.h"                // For FTriggerWriteRef and FTrigger

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_Edge"

namespace Metasound
{
    namespace EdgeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Input signal to monitor for edge detection.");
        METASOUND_PARAM(InputDebounce, "Debounce", "Debounce time in seconds to prevent rapid triggering.");

        METASOUND_PARAM(OutputTriggerRise, "TriggerRise", "Trigger on rise.");
        METASOUND_PARAM(OutputTriggerFall, "TriggerFall", "Trigger on fall.");
    }

    class FEdgeOperator : public TExecutableOperator<FEdgeOperator>
    {
    public:
        // Constructor
        FEdgeOperator(
            const FAudioBufferReadRef& InSignal,
            const FFloatReadRef& InDebounce,
            float InSampleRate,
            const FOperatorSettings& InSettings)
            : InputSignal(InSignal)
            , InputDebounce(InDebounce)
            , OutputTriggerRise(FTriggerWriteRef::CreateNew(InSettings))
            , OutputTriggerFall(FTriggerWriteRef::CreateNew(InSettings))
            , PreviousSignalValue(0.0f)
            , DebounceSamples(0)
            , DebounceCounter(0)
            , SampleRate(InSampleRate)
        {
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace EdgeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputDebounce))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputTriggerRise)),
                    TOutputDataVertexModel<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputTriggerFall))
                )
            );

            return Interface;
        }

        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
            {
                FVertexInterface NodeInterface = DeclareVertexInterface();

                FNodeClassMetadata Metadata;

                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Edge"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("EdgeNodeDisplayName", "Edge");
                Metadata.Description = METASOUND_LOCTEXT("EdgeNodeDesc", "Detects upward and downward changes in an input audio signal, with optional debounce.");
                Metadata.Author = PluginAuthor;
                Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                Metadata.DefaultInterface = DeclareVertexInterface();
                Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Branches") };
                Metadata.Keywords = TArray<FText>(); // Keywords for searching

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace EdgeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputDebounce), InputDebounce);

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace EdgeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputTriggerRise), OutputTriggerRise);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputTriggerFall), OutputTriggerFall);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace EdgeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(
                InputInterface, METASOUND_GET_PARAM_NAME(InputSignal), InParams.OperatorSettings);

            TDataReadReference<float> InputDebounce = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(
                InputInterface, METASOUND_GET_PARAM_NAME(InputDebounce), InParams.OperatorSettings);

            float SampleRate = InParams.OperatorSettings.GetSampleRate();

            return MakeUnique<FEdgeOperator>(InputSignal, InputDebounce, SampleRate, InParams.OperatorSettings);
        }

        void Execute()
        {
            // Get number of frames
            int32 NumFrames = InputSignal->Num();
            const float* SignalData = InputSignal->GetData();

            // Trigger outputs
            FTriggerWriteRef TriggerRise = OutputTriggerRise;
            FTriggerWriteRef TriggerFall = OutputTriggerFall;

            // Convert debounce time from seconds to samples
            if (DebounceSamples == 0)
            {
                DebounceSamples = FMath::RoundToInt(*InputDebounce * SampleRate);
            }

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float CurrentSignal = SignalData[i];

                // Detect rising edge
                if (PreviousSignalValue < CurrentSignal)
                {
                    if (DebounceCounter <= 0)
                    {
                        TriggerRise->TriggerFrame(i);
                        DebounceCounter = DebounceSamples;
                    }
                }
                // Detect falling edge
                else if (PreviousSignalValue > CurrentSignal)
                {
                    if (DebounceCounter <= 0)
                    {
                        TriggerFall->TriggerFrame(i);
                        DebounceCounter = DebounceSamples;
                    }
                }

                // Update previous signal value
                PreviousSignalValue = CurrentSignal;

                // Decrement debounce counter
                if (DebounceCounter > 0)
                {
                    DebounceCounter--;
                }
            }
        }

    private:
        // Inputs
        FAudioBufferReadRef InputSignal;
        FFloatReadRef InputDebounce;

        // Outputs
        FTriggerWriteRef OutputTriggerRise;
        FTriggerWriteRef OutputTriggerFall;

        // Internal variables
        float PreviousSignalValue;
        int32 DebounceSamples;
        int32 DebounceCounter;
        float SampleRate;
    };

    class FEdgeNode : public FNodeFacade
    {
    public:
        FEdgeNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FEdgeOperator>())
        {
        }
    };

    METASOUND_REGISTER_NODE(FEdgeNode);
}

#undef LOCTEXT_NAMESPACE