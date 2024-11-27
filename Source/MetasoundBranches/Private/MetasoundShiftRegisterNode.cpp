#include "MetasoundBranches/Public/MetasoundShiftRegisterNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ShiftRegisterNode"

namespace Metasound
{
    namespace ShiftRegisterNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Input signal.");
        METASOUND_PARAM(InputTrigger, "Trigger", "Trigger signal.");

        METASOUND_PARAM(OutputSignal1, "Shifted Signal 1", "Shifted output signal at stage 1.");
        METASOUND_PARAM(OutputSignal2, "Shifted Signal 2", "Shifted output signal at stage 2.");
        METASOUND_PARAM(OutputSignal3, "Shifted Signal 3", "Shifted output signal at stage 3.");
        METASOUND_PARAM(OutputSignal4, "Shifted Signal 4", "Shifted output signal at stage 4.");
        METASOUND_PARAM(OutputSignal5, "Shifted Signal 5", "Shifted output signal at stage 5.");
        METASOUND_PARAM(OutputSignal6, "Shifted Signal 6", "Shifted output signal at stage 6.");
        METASOUND_PARAM(OutputSignal7, "Shifted Signal 7", "Shifted output signal at stage 7.");
        METASOUND_PARAM(OutputSignal8, "Shifted Signal 8", "Shifted output signal at stage 8.");
    }

    class FShiftRegisterOperator : public TExecutableOperator<FShiftRegisterOperator>
    {
    public:
        FShiftRegisterOperator(
            const FOperatorSettings& InSettings,
            const FAudioBufferReadRef& InInputSignal,
            const FAudioBufferReadRef& InInputTrigger)
            : InputSignal(InInputSignal)
            , InputTrigger(InInputTrigger)
            , OutputSignal1(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal2(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal3(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal4(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal5(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal6(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal7(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal8(FAudioBufferWriteRef::CreateNew(InSettings))
            , ShiftedValue1(0.0f)
            , ShiftedValue2(0.0f)
            , ShiftedValue3(0.0f)
            , ShiftedValue4(0.0f)
            , ShiftedValue5(0.0f)
            , ShiftedValue6(0.0f)
            , ShiftedValue7(0.0f)
            , ShiftedValue8(0.0f)
            , PreviousTriggerValue(0.0f)
        {
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace ShiftRegisterNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal1)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal2)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal3)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal4)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal5)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal6)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal7)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal8))
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

                    Metadata.ClassName = { StandardNodes::Namespace, TEXT("Shift Register"), StandardNodes::AudioVariant };
                    Metadata.MajorVersion = 1;
                    Metadata.MinorVersion = 0;
                    Metadata.DisplayName = METASOUND_LOCTEXT("ShiftRegisterNodeDisplayName", "Shift Register");
                    Metadata.Description = METASOUND_LOCTEXT("ShiftRegisterNodeDesc", "Shift register node with eight stages.");
                    Metadata.Author = PluginAuthor;
                    Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                    Metadata.DefaultInterface = DeclareVertexInterface();
                    Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Branches") };
                    Metadata.Keywords = TArray<FText>();

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

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace ShiftRegisterNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal1), OutputSignal1);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal2), OutputSignal2);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal3), OutputSignal3);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal4), OutputSignal4);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal5), OutputSignal5);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal6), OutputSignal6);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal7), OutputSignal7);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal8), OutputSignal8);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace ShiftRegisterNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger), InParams.OperatorSettings);

            return MakeUnique<FShiftRegisterOperator>(
                InParams.OperatorSettings,
                InputSignal,
                InputTrigger
            );
        }

        void Execute()
        {
            int32 NumFrames = InputSignal->Num();
            const float* SignalData = InputSignal->GetData();
            const float* TriggerData = InputTrigger->GetData();

            float* OutputData1 = OutputSignal1->GetData();
            float* OutputData2 = OutputSignal2->GetData();
            float* OutputData3 = OutputSignal3->GetData();
            float* OutputData4 = OutputSignal4->GetData();
            float* OutputData5 = OutputSignal5->GetData();
            float* OutputData6 = OutputSignal6->GetData();
            float* OutputData7 = OutputSignal7->GetData();
            float* OutputData8 = OutputSignal8->GetData();

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float CurrentTriggerValue = TriggerData[i];

                if (PreviousTriggerValue <= 0.0f && CurrentTriggerValue > 0.0f)
                {
                    ShiftedValue8 = ShiftedValue7;
                    ShiftedValue7 = ShiftedValue6;
                    ShiftedValue6 = ShiftedValue5;
                    ShiftedValue5 = ShiftedValue4;
                    ShiftedValue4 = ShiftedValue3;
                    ShiftedValue3 = ShiftedValue2;
                    ShiftedValue2 = ShiftedValue1;
                    ShiftedValue1 = SignalData[i];
                }

                OutputData1[i] = ShiftedValue1;
                OutputData2[i] = ShiftedValue2;
                OutputData3[i] = ShiftedValue3;
                OutputData4[i] = ShiftedValue4;
                OutputData5[i] = ShiftedValue5;
                OutputData6[i] = ShiftedValue6;
                OutputData7[i] = ShiftedValue7;
                OutputData8[i] = ShiftedValue8;

                PreviousTriggerValue = CurrentTriggerValue;
            }
        }

    private:
        FAudioBufferReadRef InputSignal;
        FAudioBufferReadRef InputTrigger;

        FAudioBufferWriteRef OutputSignal1;
        FAudioBufferWriteRef OutputSignal2;
        FAudioBufferWriteRef OutputSignal3;
        FAudioBufferWriteRef OutputSignal4;
        FAudioBufferWriteRef OutputSignal5;
        FAudioBufferWriteRef OutputSignal6;
        FAudioBufferWriteRef OutputSignal7;
        FAudioBufferWriteRef OutputSignal8;

        float ShiftedValue1;
        float ShiftedValue2;
        float ShiftedValue3;
        float ShiftedValue4;
        float ShiftedValue5;
        float ShiftedValue6;
        float ShiftedValue7;
        float ShiftedValue8;
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