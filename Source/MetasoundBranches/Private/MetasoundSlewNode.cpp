// Name: MetasoundSlewNode
// Inlets: signal, rise time, fall time (audio, float, float)
// Outlets: signal (audio)

// Work with seconds/milliseconds instead of samples
// If so, need to get sample rate
// Set defaults for rise and fall times (0)

// SlewNode.cpp

#include "Metasound/Metasound.h"
#include "Metasound/Nodes/MetasoundNodeRegistrationMacro.h"
#include "Metasound/MetasoundStandardNodesNames.h"
#include "Metasound/MetasoundFacade.h"
#include "Metasound/MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundSlewNode"

namespace Metasound
{
    // Define unique namespace and node name
    namespace SlewNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Input audio signal to be slew rate limited.");
        METASOUND_PARAM(InputRiseTime, "Rise Time", "Rise time in seconds for slew rate limiting.");
        METASOUND_PARAM(InputFallTime, "Fall Time", "Fall time in seconds for slew rate limiting.");

        METASOUND_PARAM(OutputSignal, "Output", "Slew rate limited output signal.");
    }

    // Operator Class - Implements the node's processing logic
    class FSlewOperator : public TExecutableOperator<FSlewOperator>
    {
    public:
        // Constructor
        FSlewOperator(
            const FAudioBufferReadRef& InSignal,
            const TDataReadReference<float>& InRiseTime,
            const TDataReadReference<float>& InFallTime,
            int32 InSampleRate)
            : InputSignal(InSignal)
            , InputRiseTime(InRiseTime)
            , InputFallTime(InFallTime)
            , OutputSignal(FAudioBufferWriteRef::CreateNew(InputSignal->Num()))
            , PreviousOutputSample(0.0f)
            , SampleRate(InSampleRate)
        {
        }

        // Vertex Interface Declaration
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
                Metadata.DisplayName = METASOUND_LOCTEXT("SlewDisplayName", "Slew Rate Limiter");
                Metadata.Description = METASOUND_LOCTEXT("SlewDesc", "Applies slew rate limiting to an input audio signal with separate rise and fall times.");
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

            // Retrieve sample rate from the environment
            int32 SampleRate = InParams.GetSampleRate();

            return MakeUnique<FSlewOperator>(InputSignal, InputRiseTime, InputFallTime, SampleRate);
        }

        // Processing Logic
        virtual void Execute() override
        {
            int32 NumFrames = InputSignal->Num();

            const float* SignalData = InputSignal->GetData();
            const float* RiseTimeData = InputRiseTime->GetData();
            const float* FallTimeData = InputFallTime->GetData();
            float* OutputDataPtr = OutputSignal->GetData();

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float SignalSample = SignalData[i];
                float RiseTime = RiseTimeData[i];
                float FallTime = FallTimeData[i];

                // Calculate alpha values based on rise and fall times
                // ? does UE/Metasound allow restricting the lower bound of float inputs to 0 ?
                // (For now, if time is greater than 0, calculate alpha value, else set to 0)
                // Alpha = exp(-1 / (time * sample rate))
                
                float RiseAlpha = (RiseTime > 0.0f) ? FMath::Exp(-1.0f / (RiseTime * SampleRate)) : 0.0f;
                float FallAlpha = (FallTime > 0.0f) ? FMath::Exp(-1.0f / (FallTime * SampleRate)) : 0.0f;

                float OutputSample = PreviousOutputSample;

                if (SignalSample > PreviousOutputSample)
                {
                    // Rising edge
                    OutputSample = RiseAlpha * PreviousOutputSample + (1.0f - RiseAlpha) * SignalSample;
                }
                else if (SignalSample < PreviousOutputSample)
                {
                    // Falling edge
                    OutputSample = FallAlpha * PreviousOutputSample + (1.0f - FallAlpha) * SignalSample;
                }
                else
                {
                    // No change
                    OutputSample = SignalSample;
                }

                OutputDataPtr[i] = OutputSample;
                PreviousOutputSample = OutputSample;
            }
        }

    private:
        // Input References
        FAudioBufferReadRef InputSignal;
        TDataReadReference<float> InputRiseTime;
        TDataReadReference<float> InputFallTime;

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