#include "MetasoundBranches/Public/MetasoundSahNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros
#include "Modules/ModuleManager.h"           // ModuleManager (is this still necessary?)

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SahNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace SahNodeNames
    {
        METASOUND_PARAM(InputSignal1, "Signal 1", "Input signal to sample 1.");
        METASOUND_PARAM(InputTrigger1, "Trigger 1", "Trigger signal 1.");
        METASOUND_PARAM(InputSignal2, "Signal 2", "Input signal to sample 2.");
        METASOUND_PARAM(InputTrigger2, "Trigger 2", "Trigger signal 2.");
        METASOUND_PARAM(InputSignal3, "Signal 3", "Input signal to sample 3.");
        METASOUND_PARAM(InputTrigger3, "Trigger 3", "Trigger signal 3.");
        METASOUND_PARAM(InputSignal4, "Signal 4", "Input signal to sample 4.");
        METASOUND_PARAM(InputTrigger4, "Trigger 4", "Trigger signal 4.");
        METASOUND_PARAM(InputThreshold, "Threshold", "Threshold for triggers.");

        METASOUND_PARAM(OutputSignal, "Output", "Sampled output signal.");
    }

    // Operator Class - defines the way the node is described, created and executed
    class FSahOperator : public TExecutableOperator<FSahOperator>
    {
    public:
        // Constructor
        FSahOperator(
            const FAudioBufferReadRef& InSignal1,
            const FAudioBufferReadRef& InTrigger1,
            const FAudioBufferReadRef& InSignal2,
            const FAudioBufferReadRef& InTrigger2,
            const FAudioBufferReadRef& InSignal3,
            const FAudioBufferReadRef& InTrigger3,
            const FAudioBufferReadRef& InSignal4,
            const FAudioBufferReadRef& InTrigger4,
            const FFloatReadRef& InThreshold)
            : InputSignal1(InSignal1)
            , InputTrigger1(InTrigger)
            : InputSignal2(InSignal2)
            , InputTrigger2(InTrigger)
            : InputSignal3(InSignal3)
            , InputTrigger3(InTrigger)
            : InputSignal4(InSignal4)
            , InputTrigger4(InTrigger)
            , InputThreshold(InThreshold)
            , OutputSignal1(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
            , OutputSignal2(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
            , OutputSignal3(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
            , OutputSignal4(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
            , SampledValue1(0.0f)
            , SampledValue2(0.0f)
            , SampledValue3(0.0f)
            , SampledValue4(0.0f)
            , PreviousTriggerValue1(0.0f)
            , PreviousTriggerValue2(0.0f)
            , PreviousTriggerValue3(0.0f)
            , PreviousTriggerValue4(0.0f)
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace SahNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal1)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger1)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal2)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger2)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal3)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger3)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal4)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger4)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputThreshold))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal1))
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal2))
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal3))
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal4))
                )
            );

            return Interface;
        }

        // Retrieves necessary metadata about the node
        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
                {
                    FVertexInterface NodeInterface = DeclareVertexInterface();

                    FNodeClassMetadata Metadata;

                    Metadata.ClassName = { StandardNodes::Namespace, TEXT("Sample and Hold"), StandardNodes::AudioVariant };
                    Metadata.MajorVersion = 1;
                    Metadata.MinorVersion = 0;
                    Metadata.DisplayName = METASOUND_LOCTEXT("SahNodeDisplayName", "SaH Bank");
                    Metadata.Description = METASOUND_LOCTEXT("SahNodeDesc", "Bank of 4 sample and hold modules.");
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

        // Allows MetaSound graph to interact with the node's inputs
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace SahNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal1), InputSignal1);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger1), InputTrigger1);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal1), InputSignal2);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger1), InputTrigger2);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal1), InputSignal3);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger1), InputTrigger3);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal1), InputSignal4);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger1), InputTrigger4);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputThreshold), InputThreshold);

            return InputDataReferences;
        }

        // Allows MetaSound graph to interact with the node's outputs
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace SahNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        // Used to instantiate a new runtime instance of the node
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace SahNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal1), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger1), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal2), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger2), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal3), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger3), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal4), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger4), InParams.OperatorSettings);
            TDataReadReference<float> InputThreshold = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputThreshold), InParams.OperatorSettings);

            return MakeUnique<FSahOperator>(
                InputSignal1, 
                InputTrigger1, 
                InputSignal2, 
                InputTrigger2, 
                InputSignal3, 
                InputTrigger3, 
                InputSignal4, 
                InputTrigger4, 
                InputThreshold
            );
        }

        // Primary node functionality
        void Execute()
        {
            int32 NumFrames = InputSignal->Num();

            const float* SignalData = InputSignal->GetData();
            const float* TriggerData = InputTrigger->GetData();
            float* OutputData = OutputSignal->GetData();

            float Threshold = *InputThreshold;

            for (int32 i = 0; i < NumFrames; ++i)
            {
                float CurrentTriggerValue = TriggerData[i];

                // Detect rising edge
                if (PreviousTriggerValue < Threshold && CurrentTriggerValue >= Threshold)
                {
                    // Sample the input signal
                    SampledValue = SignalData[i];
                }

                // Output the sampled value
                OutputData[i] = SampledValue;

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
        FAudioBufferWriteRef OutputSignal;

        // Internal variables
        float SampledValue;
        float PreviousTriggerValue;
    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FSahNode : public FNodeFacade
    {
    public:
        FSahNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSahOperator>())
        {
        }
    };

    // Register node
    METASOUND_REGISTER_NODE(FSahNode);
}

#undef LOCTEXT_NAMESPACE