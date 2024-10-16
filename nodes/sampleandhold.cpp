#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SampleAndHoldNode"

namespace Metasound
{
    // Vertex Names - define your node's inputs and outputs here
    namespace SampleAndHoldNodeNames
    {
        METASOUND_PARAM(InputSignal, "Signal", "The input signal to sample.");
        METASOUND_PARAM(InputTrigger, "Trigger", "The trigger signal.");
        METASOUND_PARAM(InputThreshold, "Threshold", "Threshold for the trigger.");

        METASOUND_PARAM(OutputSignal, "Output", "The sampled output signal.");
    }

    // Operator Class - defines the way your node is described, created and executed
    class FSampleAndHoldOperator : public TExecutableOperator<FSampleAndHoldOperator>
    {
    public:
        // Constructor
        FSampleAndHoldOperator(
            const FAudioBufferReadRef& InSignal,
            const FAudioBufferReadRef& InTrigger,
            const FFloatReadRef& InThreshold)
            : InputSignal(InSignal)
            , InputTrigger(InTrigger)
            , InputThreshold(InThreshold)
            , OutputSignal(FAudioBufferWriteRef::CreateNew(InSignal->Num()))
            , SampledValue(0.0f)
            , PreviousTriggerValue(0.0f)
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace SampleAndHoldNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal)),
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger)),
                    TInputDataVertexModel<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputThreshold))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal))
                )
            );

            return Interface;
        }

        // Retrieves necessary metadata about your node
        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
            {
                FVertexInterface NodeInterface = DeclareVertexInterface();

                FNodeClassMetadata Metadata
                {
                    FNodeClassName { StandardNodes::Namespace, "Sample and Hold", StandardNodes::AudioVariant }, 
                    1, // Major Version
                    0, // Minor Version
                    METASOUND_LOCTEXT("SampleAndHoldNodeDisplayName", "Sample and Hold"),
                    METASOUND_LOCTEXT("SampleAndHoldNodeDesc", "Samples an input signal when a trigger crosses a threshold and holds it until the next trigger."),
                    PluginAuthor,
                    PluginNodeMissingPrompt,
                    NodeInterface,
                    { }, // Category Hierarchy 
                    { }, // Keywords for searching
                    FNodeDisplayStyle{}
                };

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        // Allows MetaSound graph to interact with your node's inputs
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace SampleAndHoldNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger), InputTrigger);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputThreshold), InputThreshold);

            return InputDataReferences;
        }

        // Allows MetaSound graph to interact with your node's outputs
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace SampleAndHoldNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        // Used to instantiate a new runtime instance of your node
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace SampleAndHoldNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger), InParams.OperatorSettings);
            TDataReadReference<float> InputThreshold = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputThreshold), InParams.OperatorSettings);

            return MakeUnique<FSampleAndHoldOperator>(InputSignal, InputTrigger, InputThreshold);
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
    class FSampleAndHoldNode : public FNodeFacade
    {
    public:
        FSampleAndHoldNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSampleAndHoldOperator>())
        {
        }
    };

    // Register node
    METASOUND_REGISTER_NODE(FSampleAndHoldNode);
}

#undef LOCTEXT_NAMESPACE
