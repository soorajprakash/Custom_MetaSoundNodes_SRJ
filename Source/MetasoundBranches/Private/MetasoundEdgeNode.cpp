#include "MetasoundBranches/Public/MetasoundEdgeNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros
#include "Modules/ModuleManager.h"           // ModuleManager (is this still necessary?)

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
            float InSampleRate)
            : InputSignal(InSignal)
            , InputDebounce(InDebounce)
            , OutputTriggerRise(WriteRefInit<bool>(false))
            , OutputTriggerFall(WriteRefInit<bool>(false))
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
                    TOutputDataVertexModel<bool>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputTriggerRise)),
                    TOutputDataVertexModel<bool>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputTriggerFall))
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

            return MakeUnique<FEdgeOperator>(InputSignal, InputDebounce, SampleRate);
        }

        void Execute()
        {
            int32 NumFrames = InputSignal->Num();
            const float* SignalData = InputSignal->GetData();
            bool* TriggerRiseData = OutputTriggerRise.Get();
            bool* TriggerFallData = OutputTriggerFall.Get();

            // Convert debounce time from seconds to samples
            if (DebounceSamples == 0)
            {
                DebounceSamples = FMath::RoundToInt(*InputDebounce * SampleRate);
            }

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float CurrentSignal = SignalData[i];
                TriggerRiseData[i] = false;
                TriggerFallData[i] = false;

                // Detect rising edge
                if (PreviousSignalValue < CurrentSignal)
                {
                    if (DebounceCounter <= 0)
                    {
                        TriggerRiseData[i] = true;
                        DebounceCounter = DebounceSamples;
                    }
                }
                // Detect falling edge
                else if (PreviousSignalValue > CurrentSignal)
                {
                    if (DebounceCounter <= 0)
                    {
                        TriggerFallData[i] = true;
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
        FBoolWriteRef OutputTriggerRise;
        FBoolWriteRef OutputTriggerFall;

        // Internal variables
        float PreviousSignalValue;
        int32 DebounceSamples;
        int32 DebounceCounter;
        float SampleRate; // New member variable for sample rate
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