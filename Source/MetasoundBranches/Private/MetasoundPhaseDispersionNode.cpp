#include "MetasoundBranches/Public/MetasoundPhaseDispersionNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_PhaseDispersionNode"

namespace Metasound
{
    namespace PhaseDispersionNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "Input audio signal.");
        METASOUND_PARAM(OutputSignal, "Output", "Phase-dispersed output signal.");
    }

    class FPhaseDispersionOperator : public TExecutableOperator<FPhaseDispersionOperator>
    {
    public:
        FPhaseDispersionOperator(const FAudioBufferReadRef& InSignal)
            : InputSignal(InSignal)
            , OutputSignal(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
        {
            AllPassFilters.SetNum(10);
            for (int32 i = 0; i < 10; ++i)
            {
                AllPassFilters[i].Init();
            }
        }

        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace PhaseDispersionNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal))
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
                Metadata.ClassName = { StandardNodes::Namespace, TEXT("PhaseDispersion"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("PhaseDispersionNodeDisplayName", "Phase Dispersion");
                Metadata.Description = METASOUND_LOCTEXT("PhaseDispersionNodeDesc", "Applies ten allpass filters to the incoming signal.");
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
            using namespace PhaseDispersionNodeNames;

            FDataReferenceCollection InputDataReferences;
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);

            return InputDataReferences;
        }

        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace PhaseDispersionNodeNames;

            FDataReferenceCollection OutputDataReferences;
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace PhaseDispersionNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal), InParams.OperatorSettings);

            return MakeUnique<FPhaseDispersionOperator>(InputSignal);
        }

        void Execute()
        {
            int32 NumFrames = InputSignal->Num();
            const float* InputData = InputSignal->GetData();
            float* OutputData = OutputSignal->GetData();

            // Copy input to output to start processing
            FMemory::Memcpy(OutputData, InputData, NumFrames * sizeof(float));

            for (int32 i = 0; i < 10; ++i)
            {
                AllPassFilters[i].ProcessBuffer(OutputData, NumFrames);
            }
        }

    private:
        class FAllPassFilter
        {
        public:
            void Init()
            {
                DelayBuffer.SetNumZeroed(MaxDelaySamples);
                WriteIndex = 0;
                Feedback = 0;
                DelaySamples = MaxDelaySamples / 2;
            }

            void ProcessBuffer(float* InOutBuffer, int32 NumSamples)
            {
                for (int32 i = 0; i < NumSamples; ++i)
                {
                    float InSample = InOutBuffer[i];
                    float DelayedSample = DelayBuffer[WriteIndex];

                    float OutSample = -Feedback * InSample + DelayedSample;
                    DelayBuffer[WriteIndex] = InSample + Feedback * OutSample;

                    InOutBuffer[i] = OutSample;

                    WriteIndex = (WriteIndex + 1) % MaxDelaySamples;
                }
            }

        private:
            TArray<float> DelayBuffer;
            int32 WriteIndex;
            int32 DelaySamples;
            float Feedback;

            static const int32 MaxDelaySamples = 1024; // Adjust as needed
        };

        // Inputs
        FAudioBufferReadRef InputSignal;

        // Outputs
        FAudioBufferWriteRef OutputSignal;

        // Allpass filters
        TArray<FAllPassFilter> AllPassFilters;
    };

    class FPhaseDispersionNode : public FNodeFacade
    {
    public:
        FPhaseDispersionNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FPhaseDispersionOperator>())
        {
        }
    };

    METASOUND_REGISTER_NODE(FPhaseDispersionNode);
}

#undef LOCTEXT_NAMESPACE