#include "MetasoundBranches/Public/MetasoundStereoBalanceNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros
#include "Math/UnrealMathUtility.h"          // For FMath functions

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_BalanceNode"

namespace Metasound
{
    namespace BalanceNodeNames
    {
        METASOUND_PARAM(InputLeftSignal, "In L", "Left channel of the stereo input signal.");
        METASOUND_PARAM(InputRightSignal, "R :", "Right channel of the stereo input signal.");
        METASOUND_PARAM(InputBalance, "Balance", "Balance control ranging from -1.0 (full left) to 1.0 (full right).");

        METASOUND_PARAM(OutputLeftSignal, "Left Output", "Left channel of the adjusted stereo output signal.");
        METASOUND_PARAM(OutputRightSignal, "Right Output", "Right channel of the adjusted stereo output signal.");
    }

    class FBalanceOperator : public TExecutableOperator<FBalanceOperator>
    {
    public:
        FBalanceOperator(
            const FOperatorSettings& InSettings,
            const FAudioBufferReadRef& InLeftSignal,
            const FAudioBufferReadRef& InRightSignal,
            const FFloatReadRef& InBalance)
            : InputLeftSignal(InLeftSignal)
            , InputRightSignal(InRightSignal)
            , InputBalance(InBalance)
            , OutputLeftSignal(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputRightSignal(FAudioBufferWriteRef::CreateNew(InSettings))
        {
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace BalanceNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputLeftSignal)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputRightSignal)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputBalance), 0.0f) // Default balance is centered
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputLeftSignal)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputRightSignal))
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

                    FNodeClassMetadata Metadata;
                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Stereo Balance"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("StereoGainNodeDisplayName", "Stereo Balance");
                Metadata.Description = METASOUND_LOCTEXT("StereoGainNodeDesc", "Adjusts the balance of a stereo signal.");
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
            using namespace BalanceNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputLeftSignal), InputLeftSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputRightSignal), InputRightSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputBalance), InputBalance);

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace BalanceNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputLeftSignal), OutputLeftSignal);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputRightSignal), OutputRightSignal);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace BalanceNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputLeftSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputLeftSignal), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputRightSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputRightSignal), InParams.OperatorSettings);
            TDataReadReference<float> InputBalance = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputBalance), InParams.OperatorSettings);

            return MakeUnique<FBalanceOperator>(InParams.OperatorSettings, InputLeftSignal, InputRightSignal, InputBalance);
        }

        void Execute()
        {
            int32 NumFrames = InputLeftSignal->Num();

            const float* LeftData = InputLeftSignal->GetData();
            const float* RightData = InputRightSignal->GetData();
            float* OutputLeftData = OutputLeftSignal->GetData();
            float* OutputRightData = OutputRightSignal->GetData();

            float Balance = FMath::Clamp(*InputBalance, -1.0f, 1.0f);

            const float PI_OVER_4 = PI / 4.0f;
            float Angle = Balance * PI_OVER_4;

            float LeftGain = FMath::Cos(Angle);
            float RightGain = FMath::Sin(Angle);

            float GainNormalization = 1.0f / FMath::Sqrt(LeftGain * LeftGain + RightGain * RightGain);
            LeftGain *= GainNormalization;
            RightGain *= GainNormalization;

            for (int32 i = 0; i < NumFrames; ++i)
            {
                OutputLeftData[i] = LeftData[i] * LeftGain;
                OutputRightData[i] = RightData[i] * RightGain;
            }
        }

    private:

        // Inputs
        FAudioBufferReadRef InputLeftSignal;
        FAudioBufferReadRef InputRightSignal;
        FFloatReadRef InputBalance;

        // Outputs
        FAudioBufferWriteRef OutputLeftSignal;
        FAudioBufferWriteRef OutputRightSignal;
    };

    class FBalanceNode : public FNodeFacade
    {
    public:
        FBalanceNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FBalanceOperator>())
        {
        }
    };

    METASOUND_REGISTER_NODE(FBalanceNode);
}

#undef LOCTEXT_NAMESPACE