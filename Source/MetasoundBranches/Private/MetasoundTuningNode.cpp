#include "MetasoundBranches/Public/MetasoundTuningNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_TuningNode"

namespace Metasound
{
    namespace TuningNodeNames
    {
        METASOUND_PARAM(InputMIDINoteNumber, "MIDI Note Number", "Input MIDI note number (integer).");
        METASOUND_PARAM(InputFineTune, "Fine Tune", "Fine tuning adjustment (float).");
        METASOUND_PARAM(InputTuningCents0, "+/- Cents C", "Tuning adjustment for note 0 in cents.");
        METASOUND_PARAM(InputTuningCents1, "+/- Cents C♯ / D♭", "Tuning adjustment for note 1 in cents.");
        METASOUND_PARAM(InputTuningCents2, "+/- Cents D", "Tuning adjustment for note 2 in cents.");
        METASOUND_PARAM(InputTuningCents3, "+/- Cents D♯ / E♭", "Tuning adjustment for note 3 in cents.");
        METASOUND_PARAM(InputTuningCents4, "+/- Cents E", "Tuning adjustment for note 4 in cents.");
        METASOUND_PARAM(InputTuningCents5, "+/- Cents F", "Tuning adjustment for note 5 in cents.");
        METASOUND_PARAM(InputTuningCents6, "+/- Cents F♯ / G♭", "Tuning adjustment for note 6 in cents.");
        METASOUND_PARAM(InputTuningCents7, "+/- Cents G", "Tuning adjustment for note 7 in cents.");
        METASOUND_PARAM(InputTuningCents8, "+/- Cents G♯ / A♭", "Tuning adjustment for note 8 in cents.");
        METASOUND_PARAM(InputTuningCents9, "+/- Cents A", "Tuning adjustment for note 9 in cents.");
        METASOUND_PARAM(InputTuningCents10, "+/- Cents A♯ / B♭", "Tuning adjustment for note 10 in cents.");
        METASOUND_PARAM(InputTuningCents11, "+/- Cents B", "Tuning adjustment for note 11 in cents.");
        METASOUND_PARAM(OutputFrequency, "Frequency", "Output frequency (float).");
    }

    class FTuningNodeOperator : public TExecutableOperator<FTuningNodeOperator>
    {
    public:
        FTuningNodeOperator(
            const FOperatorSettings& InSettings,
            const FInt32ReadRef& InMIDINoteNumber,
            const FFloatReadRef& InFineTune,
            const FFloatReadRef& InTuningCents0,
            const FFloatReadRef& InTuningCents1,
            const FFloatReadRef& InTuningCents2,
            const FFloatReadRef& InTuningCents3,
            const FFloatReadRef& InTuningCents4,
            const FFloatReadRef& InTuningCents5,
            const FFloatReadRef& InTuningCents6,
            const FFloatReadRef& InTuningCents7,
            const FFloatReadRef& InTuningCents8,
            const FFloatReadRef& InTuningCents9,
            const FFloatReadRef& InTuningCents10,
            const FFloatReadRef& InTuningCents11)
            : MIDINoteNumber(InMIDINoteNumber)
            , FineTune(InFineTune)
            , TuningCents0(InTuningCents0)
            , TuningCents1(InTuningCents1)
            , TuningCents2(InTuningCents2)
            , TuningCents3(InTuningCents3)
            , TuningCents4(InTuningCents4)
            , TuningCents5(InTuningCents5)
            , TuningCents6(InTuningCents6)
            , TuningCents7(InTuningCents7)
            , TuningCents8(InTuningCents8)
            , TuningCents9(InTuningCents9)
            , TuningCents10(InTuningCents10)
            , TuningCents11(InTuningCents11)
            , OutputFrequency(FFloatWriteRef::CreateNew(0.0f))
        {
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace TuningNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<int32>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputMIDINoteNumber)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputFineTune)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents0)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents1)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents2)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents3)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents4)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents5)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents6)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents7)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents8)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents9)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents10)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTuningCents11))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputFrequency))
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

                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Tuning"), TEXT("Float") };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("TuningNodeDisplayName", "Tuning");
                Metadata.Description = METASOUND_LOCTEXT("TuningNodeDesc", "Generates a frequency based on custom tuning per-note.");
                Metadata.Author = "Charles Matthews";
                Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                Metadata.DefaultInterface = NodeInterface;
                Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Branches") };
                Metadata.Keywords = TArray<FText>();

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace TuningNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputMIDINoteNumber), MIDINoteNumber);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputFineTune), FineTune);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents0), TuningCents0);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents1), TuningCents1);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents2), TuningCents2);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents3), TuningCents3);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents4), TuningCents4);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents5), TuningCents5);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents6), TuningCents6);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents7), TuningCents7);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents8), TuningCents8);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents9), TuningCents9);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents10), TuningCents10);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTuningCents11), TuningCents11);

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace TuningNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputFrequency), OutputFrequency);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace TuningNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<int32> MIDINoteNumber = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<int32>(InputInterface, METASOUND_GET_PARAM_NAME(InputMIDINoteNumber), InParams.OperatorSettings);
            TDataReadReference<float> FineTune = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputFineTune), InParams.OperatorSettings);

            TDataReadReference<float> TuningCents0 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents0), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents1 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents1), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents2 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents2), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents3 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents3), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents4 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents4), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents5 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents5), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents6 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents6), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents7 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents7), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents8 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents8), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents9 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents9), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents10 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents10), InParams.OperatorSettings);
            TDataReadReference<float> TuningCents11 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputTuningCents11), InParams.OperatorSettings);

            return MakeUnique<FTuningNodeOperator>(
                InParams.OperatorSettings,
                MIDINoteNumber,
                FineTune,
                TuningCents0,
                TuningCents1,
                TuningCents2,
                TuningCents3,
                TuningCents4,
                TuningCents5,
                TuningCents6,
                TuningCents7,
                TuningCents8,
                TuningCents9,
                TuningCents10,
                TuningCents11
            );
        }

        void Execute()
        {
            int32 midiNote = *MIDINoteNumber;
            float fineTune = *FineTune;
            float noteWithFineTune = midiNote + fineTune;

            int32 noteInOctave = midiNote % 12;

            float tuningAdjustmentCents = 0.0f;

            switch (noteInOctave)
            {
                case 0: tuningAdjustmentCents = *TuningCents0; break;
                case 1: tuningAdjustmentCents = *TuningCents1; break;
                case 2: tuningAdjustmentCents = *TuningCents2; break;
                case 3: tuningAdjustmentCents = *TuningCents3; break;
                case 4: tuningAdjustmentCents = *TuningCents4; break;
                case 5: tuningAdjustmentCents = *TuningCents5; break;
                case 6: tuningAdjustmentCents = *TuningCents6; break;
                case 7: tuningAdjustmentCents = *TuningCents7; break;
                case 8: tuningAdjustmentCents = *TuningCents8; break;
                case 9: tuningAdjustmentCents = *TuningCents9; break;
                case 10: tuningAdjustmentCents = *TuningCents10; break;
                case 11: tuningAdjustmentCents = *TuningCents11; break;
                default: break;
            }

            // Testing: calculate the base frequency without the tuning adjustment
            float baseFrequency = 440.0f * powf(2.0f, (noteWithFineTune - 69.0f) / 12.0f);

            // Apply the tuning adjustment to the frequency directly
            float frequency = baseFrequency * powf(2.0f, tuningAdjustmentCents / 1200.0f);

            *OutputFrequency = frequency;
        }

    private:
        FInt32ReadRef MIDINoteNumber;
        FFloatReadRef FineTune;
        FFloatReadRef TuningCents0;
        FFloatReadRef TuningCents1;
        FFloatReadRef TuningCents2;
        FFloatReadRef TuningCents3;
        FFloatReadRef TuningCents4;
        FFloatReadRef TuningCents5;
        FFloatReadRef TuningCents6;
        FFloatReadRef TuningCents7;
        FFloatReadRef TuningCents8;
        FFloatReadRef TuningCents9;
        FFloatReadRef TuningCents10;
        FFloatReadRef TuningCents11;
        FFloatWriteRef OutputFrequency;
    };

    class FTuningNode : public FNodeFacade
    {
    public:
        FTuningNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FTuningNodeOperator>())
        {
        }
    };

    METASOUND_REGISTER_NODE(FTuningNode);
}

#undef LOCTEXT_NAMESPACE