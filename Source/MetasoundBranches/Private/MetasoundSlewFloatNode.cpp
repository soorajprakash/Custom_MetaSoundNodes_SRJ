// Copyright Charles Matthews. All Rights Reserved.

#include "MetasoundBranches/Public/MetasoundSlewFloatNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundSlewNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace SlewFloatNodeNames
    {
        METASOUND_PARAM(InputSignal, "In", "Float to smooth.");
        METASOUND_PARAM(InputRiseTime, "Rise Time", "Rise time in seconds.");
        METASOUND_PARAM(InputFallTime, "Fall Time", "Fall time in seconds.");

        METASOUND_PARAM(OutputSignal, "Out", "Slew rate limited  float.");
    }

    // Operator Class - defines the way the node is described, created, and executed
    class FSlewFloatOperator : public TExecutableOperator<FSlewFloatOperator>
    {
    public:
        // Constructor
        FSlewFloatOperator(
            const FOperatorSettings& InSettings,
            const FFloatReadRef& InSignal,
            const FTimeReadRef& InRiseTime,
            const FTimeReadRef& InFallTime,
            int32 InSampleRate)
            : InputSignal(InSignal)
            , InputRiseTime(InRiseTime)
            , InputFallTime(InFallTime)
            , OutputSignal(FFloatWriteRef::CreateNew(0.0f))
            , PreviousOutputSample(0.0f)
            , SampleRate(InSampleRate)
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace SlewFloatNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<FTime>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputRiseTime)),
                    TInputDataVertexModel<FTime>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputFallTime))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal))
                )
            );

            return Interface;
        }

        // Metadata about the node
        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
            {
                FNodeClassMetadata Metadata;
                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Slew (Float)"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("SlewFloatDisplayName", "Slew (Float)");
                Metadata.Description = METASOUND_LOCTEXT("SlewFloatDesc", "Smooth the rise and fall times of an incoming float value.");
                Metadata.Author = "Charles Matthews";
                Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                Metadata.DefaultInterface = DeclareVertexInterface();
                Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Branches") };
                Metadata.Keywords = TArray<FText>(); // Keywords for searching

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        // Input Data References
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace SlewFloatNodeNames;

            FDataReferenceCollection InputDataReferences;
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputRiseTime), InputRiseTime);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputFallTime), InputFallTime);

            return InputDataReferences;
        }

        // Output Data References
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace SlewFloatNodeNames;

            FDataReferenceCollection OutputDataReferences;
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        // Operator Factory Method
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace SlewFloatNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            // Retrieve input references or use default values
            TDataReadReference<float> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputSignal),
                InParams.OperatorSettings
            );

            TDataReadReference<FTime> InputRiseTime = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FTime>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputRiseTime),
                InParams.OperatorSettings
            );

            TDataReadReference<FTime> InputFallTime = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FTime>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputFallTime),
                InParams.OperatorSettings
            );

            int32 SampleRate = InParams.OperatorSettings.GetActualBlockRate(); // For float processing, use block rate

            return MakeUnique<FSlewFloatOperator>(InParams.OperatorSettings, InputSignal, InputRiseTime, InputFallTime, SampleRate);
        }

        // Primary node functionality
        virtual void Execute()
        {
            float SignalSample = *InputSignal;

            float RiseTimeSeconds = InputRiseTime->GetSeconds();
            float FallTimeSeconds = InputFallTime->GetSeconds();

            // Calculate alpha values based on rise and fall times
            // Alpha = exp(-1 / (time * sample rate))
            float RiseAlpha = (RiseTimeSeconds > 0.0f) ? FMath::Exp(-1.0f / (RiseTimeSeconds * SampleRate)) : 0.0f;
            float FallAlpha = (FallTimeSeconds > 0.0f) ? FMath::Exp(-1.0f / (FallTimeSeconds * SampleRate)) : 0.0f;

            float OutputSample = PreviousOutputSample;

            if (SignalSample > PreviousOutputSample)
            {
                OutputSample = RiseAlpha * PreviousOutputSample + (1.0f - RiseAlpha) * SignalSample;
            }
            else if (SignalSample < PreviousOutputSample)
            {
                OutputSample = FallAlpha * PreviousOutputSample + (1.0f - FallAlpha) * SignalSample;
            }
            else
            {
                OutputSample = SignalSample;
            }

            *OutputSignal = OutputSample;
            PreviousOutputSample = OutputSample;
        }

    private:
        // Input References
        FFloatReadRef InputSignal;
        FTimeReadRef InputRiseTime;
        FTimeReadRef InputFallTime;

        // Output Reference
        FFloatWriteRef OutputSignal;

        // State Variable
        float PreviousOutputSample;

        // Sample Rate
        int32 SampleRate;
    };

    // Node Facade Class
    class FSlewFloatNode : public FNodeFacade
    {
    public:
        FSlewFloatNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSlewFloatOperator>())
        {
        }
    };

    // Register the Node
    METASOUND_REGISTER_NODE(FSlewFloatNode);
}

#undef LOCTEXT_NAMESPACE