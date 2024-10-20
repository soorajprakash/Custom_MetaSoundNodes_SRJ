#include "MetasoundBranches/Public/MetasoundSlewNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

#define LOCTEXT_NAMESPACE "MetasoundSlewNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace SlewNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Audio signal to smooth.");
        METASOUND_PARAM(InputRiseTime, "Rise Time", "Rise time in seconds.");
        METASOUND_PARAM(InputFallTime, "Fall Time", "Fall time in seconds.");

        METASOUND_PARAM(OutputSignal, "Output", "Slew rate limited output signal.");
    }

    // Operator Class - defines the way the node is described, created and executed
    class FSlewOperator : public TExecutableOperator<FSlewOperator>
    {
    public:
        // Constructor
        FSlewOperator(
            const FAudioBufferReadRef& InSignal,
            const FFloatReadRef& InRiseTime,
            const FFloatReadRef& InFallTime,
            int32 InSampleRate)
            : InputSignal(InSignal)
            , InputRiseTime(InRiseTime)
            , InputFallTime(InFallTime)
            , OutputSignal(FAudioBufferWriteRef::CreateNew(InputSignal->Num()))
            , PreviousOutputSample(0.0f)
            , SampleRate(InSampleRate)
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace SlewNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputRiseTime)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputFallTime))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal))
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
                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Slew"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("SlewDisplayName", "Slew");
                Metadata.Description = METASOUND_LOCTEXT("SlewDesc", "Smooth the rise and fall times of an incoming signal.");
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

        // Input Data References
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace SlewNodeNames;

            FDataReferenceCollection InputDataReferences;
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputRiseTime), InputRiseTime);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputFallTime), InputFallTime);

            return InputDataReferences;
        }

        // Output Data References
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace SlewNodeNames;

            FDataReferenceCollection OutputDataReferences;
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        // Operator Factory Method
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace SlewNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            // Retrieve input references or use default values
            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputSignal),
                InParams.OperatorSettings
            );

            TDataReadReference<float> InputRiseTime = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputRiseTime),
                InParams.OperatorSettings
            );

            TDataReadReference<float> InputFallTime = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputFallTime),
                InParams.OperatorSettings
            );

            int32 SampleRate = 44100; // Default sample rate

            return MakeUnique<FSlewOperator>(InputSignal, InputRiseTime, InputFallTime, SampleRate);
        }

        // Primary node functionality
        virtual void Execute()
        {
            int32 NumFrames = InputSignal->Num();

            const float* SignalData = InputSignal->GetData();
            float* OutputDataPtr = OutputSignal->GetData();

            float RiseTime = *InputRiseTime;
            float FallTime = *InputFallTime;

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float SignalSample = SignalData[i];

                // Calculate alpha values based on rise and fall times
                // Restrict lower bound to 0
                // Alpha = exp(-1 / (time * sample rate))
                
                float RiseAlpha = (RiseTime > 0.0f) ? FMath::Exp(-1.0f / (RiseTime * SampleRate)) : 0.0f;
                float FallAlpha = (FallTime > 0.0f) ? FMath::Exp(-1.0f / (FallTime * SampleRate)) : 0.0f;

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

                OutputDataPtr[i] = OutputSample;
                PreviousOutputSample = OutputSample;
            }
        }

    private:
        // Input References
        FAudioBufferReadRef InputSignal;
        FFloatReadRef InputRiseTime;
        FFloatReadRef InputFallTime;

        // Output Reference
        FAudioBufferWriteRef OutputSignal;

        // State Variable
        float PreviousOutputSample;

        // Sample Rate
        int32 SampleRate;
    };

    // Node Facade Class
    class FSlewNode : public FNodeFacade
    {
    public:
        FSlewNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSlewOperator>())
        {
        }
    };

    // Register the Node
    METASOUND_REGISTER_NODE(FSlewNode);
}

#undef LOCTEXT_NAMESPACE