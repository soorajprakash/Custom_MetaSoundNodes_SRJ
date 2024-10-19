#include "MetasoundShiftRegisterNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ShiftRegisterNode"

namespace Metasound
{
    namespace ShiftRegisterNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Input signal to sample.");
        METASOUND_PARAM(InputTrigger, "Trigger", "Trigger signal.");
        METASOUND_PARAM(InputThreshold, "Threshold", "Threshold for trigger.");

        METASOUND_PARAM(OutputStage1, "Output 1", "Shift register output stage 1.");
        METASOUND_PARAM(OutputStage2, "Output 2", "Shift register output stage 2.");
        METASOUND_PARAM(OutputStage3, "Output 3", "Shift register output stage 3.");
        METASOUND_PARAM(OutputStage4, "Output 4", "Shift register output stage 4.");
        METASOUND_PARAM(OutputStage5, "Output 5", "Shift register output stage 5.");
        METASOUND_PARAM(OutputStage6, "Output 6", "Shift register output stage 6.");
        METASOUND_PARAM(OutputStage7, "Output 7", "Shift register output stage 7.");
        METASOUND_PARAM(OutputStage8, "Output 8", "Shift register output stage 8.");
    }

    class FShiftRegisterOperator : public TExecutableOperator<FShiftRegisterOperator>
    {
    public:
        static constexpr int32 NumStages = 8;

        FShiftRegisterOperator(
            const FAudioBufferReadRef& InSignal,
            const FAudioBufferReadRef& InTrigger,
            const FFloatReadRef& InThreshold)
            : InputSignal(InSignal)
            , InputTrigger(InTrigger)
            , InputThreshold(InThreshold)
            , SampledValues(NumStages, 0.0f)
            , PreviousTriggerValue(0.0f)
        {
            // Initialize output buffers
            for (int32 i = 0; i < NumStages; ++i)
            {
                OutputSignals.Add(FAudioBufferWriteRef::CreateNew(InSignal->Num()));
            }
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace ShiftRegisterNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputThreshold))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage1)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage2)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage3)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage4)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage5)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage6)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage7)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputStage8))
                )
            );

            return Interface;
        }

        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
            {
                FNodeClassMetadata Metadata;

                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Shift Register"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("ShiftRegisterDisplayName", "Shift Register");
                Metadata.Description = METASOUND_LOCTEXT("ShiftRegisterDesc", "Shift the input signal across 8 inputs on trigger.");
                Metadata.Author = PluginAuthor;
                Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                Metadata.DefaultInterface = DeclareVertexInterface();
                Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Registers") };
                Metadata.Keywords = { METASOUND_LOCTEXT("Shift"), METASOUND_LOCTEXT("Register"), METASOUND_LOCTEXT("Delay") };

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace ShiftRegisterNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger), InputTrigger);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputThreshold), InputThreshold);

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace ShiftRegisterNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage1), OutputSignals[0]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage2), OutputSignals[1]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage3), OutputSignals[2]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage4), OutputSignals[3]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage5), OutputSignals[4]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage6), OutputSignals[5]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage7), OutputSignals[6]);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputStage8), OutputSignals[7]);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace ShiftRegisterNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger), InParams.OperatorSettings);
            TDataReadReference<float> InputThreshold = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputThreshold), InParams.OperatorSettings);

            return MakeUnique<FShiftRegisterOperator>(InputSignal, InputTrigger, InputThreshold);
        }

        void Execute()
        {
            int32 NumFrames = InputSignal->Num();

            const float* SignalData = InputSignal->GetData();
            const float* TriggerData = InputTrigger->GetData();
            float Threshold = *InputThreshold;

            // Prepare output data pointers
            TArray<float*> OutputDataPointers;
            for (int32 i = 0; i < NumStages; ++i)
            {
                OutputDataPointers.Add(OutputSignals[i]->GetData());
            }

            for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
            {
                float CurrentTriggerValue = TriggerData[FrameIndex];

                // Detect rising edge
                if (PreviousTriggerValue < Threshold && CurrentTriggerValue >= Threshold)
                {
                    // Shift the values
                    for (int32 Stage = NumStages - 1; Stage > 0; --Stage)
                    {
                        SampledValues[Stage] = SampledValues[Stage - 1];
                    }
                    // Sample the new input signal
                    SampledValues[0] = SignalData[FrameIndex];
                }

                // Output the sampled values to each output buffer
                for (int32 Stage = 0; Stage < NumStages; ++Stage)
                {
                    OutputDataPointers[Stage][FrameIndex] = SampledValues[Stage];
                }

                // Update previous trigger value
                PreviousTriggerValue = CurrentTriggerValue;
            }
        }

    private:
        // Inputs
        FAudioBufferReadRef InputSignal;
        FAudioBufferReadRef InputTrigger;
        FFloatReadRef InputThreshold;

        // Outputs
        TArray<FAudioBufferWriteRef> OutputSignals;

        // Internal variables
        TArray<float> SampledValues;
        float PreviousTriggerValue;
    };

    class FShiftRegisterNode : public FNodeFacade
    {
    public:
        FShiftRegisterNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FShiftRegisterOperator>())
        {
        }
    };

    METASOUND_REGISTER_NODE(FShiftRegisterNode);
}

#undef LOCTEXT_NAMESPACE