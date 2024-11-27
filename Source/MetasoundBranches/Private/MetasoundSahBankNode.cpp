#include "MetasoundBranches/Public/MetasoundSahBankNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SahBankNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace SahBankNodeNames
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

        METASOUND_PARAM(OutputSignal1, "Output 1", "Sampled output signal.");
        METASOUND_PARAM(OutputSignal2, "Output 2", "Sampled output signal.");
        METASOUND_PARAM(OutputSignal3, "Output 3", "Sampled output signal.");
        METASOUND_PARAM(OutputSignal4, "Output 4", "Sampled output signal.");
    }

    // Operator Class - defines the way the node is described, created and executed
    class FSahBankOperator : public TExecutableOperator<FSahBankOperator>
    {
    public:
        // Constructor
        FSahBankOperator(
            const FOperatorSettings& InSettings,
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
            , InputTrigger1(InTrigger1)
            , InputSignal2(InSignal2)
            , InputTrigger2(InTrigger2)
            , InputSignal3(InSignal3)
            , InputTrigger3(InTrigger3)
            , InputSignal4(InSignal4)
            , InputTrigger4(InTrigger4)
            , InputThreshold(InThreshold)
            , OutputSignal1(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal2(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal3(FAudioBufferWriteRef::CreateNew(InSettings))
            , OutputSignal4(FAudioBufferWriteRef::CreateNew(InSettings))
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
            using namespace SahBankNodeNames;

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
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal1)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal2)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal3)),
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

                    Metadata.ClassName = { StandardNodes::Namespace, TEXT("SaH Bank"), StandardNodes::AudioVariant };
                    Metadata.MajorVersion = 1;
                    Metadata.MinorVersion = 0;
                    Metadata.DisplayName = METASOUND_LOCTEXT("SahBankNodeDisplayName", "Sample And Hold Bank");
                    Metadata.Description = METASOUND_LOCTEXT("SahBankNodeDesc", "Bank of four sample and hold modules.");
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

        // Allows MetaSound graph to interact with the node's inputs
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace SahBankNodeNames;

            FDataReferenceCollection InputDataReferences;

            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal1), InputSignal1);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger1), InputTrigger1);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal2), InputSignal2);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger2), InputTrigger2);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal3), InputSignal3);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger3), InputTrigger3);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal4), InputSignal4);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger4), InputTrigger4);
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputThreshold), InputThreshold);

            return InputDataReferences;
        }

        // Allows MetaSound graph to interact with the node's outputs
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace SahBankNodeNames;

            FDataReferenceCollection OutputDataReferences;

            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal1), OutputSignal1);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal2), OutputSignal2);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal3), OutputSignal3);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal4), OutputSignal4);

            return OutputDataReferences;
        }

        // Used to instantiate a new runtime instance of the node
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace SahBankNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FAudioBuffer> InputSignal1 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal1), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger1 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger1), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal2 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal2), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger2 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger2), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal3 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal3), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger3 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger3), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputSignal4 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputSignal4), InParams.OperatorSettings);
            TDataReadReference<FAudioBuffer> InputTrigger4 = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(InputInterface, METASOUND_GET_PARAM_NAME(InputTrigger4), InParams.OperatorSettings);
            TDataReadReference<float> InputThreshold = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InputThreshold), InParams.OperatorSettings);

            return MakeUnique<FSahBankOperator>(
                InParams.OperatorSettings,
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
            int32 NumFrames = InputSignal1->Num();
           
            const float* SignalData1 = InputSignal1->GetData();
            const float* TriggerData1 = InputTrigger1->GetData();
            float* OutputData1 = OutputSignal1->GetData();
            
            const float* SignalData2 = InputSignal2->GetData();
            const float* TriggerData2 = InputTrigger2->GetData();
            float* OutputData2 = OutputSignal2->GetData();
            
            const float* SignalData3 = InputSignal3->GetData();
            const float* TriggerData3 = InputTrigger3->GetData();
            float* OutputData3 = OutputSignal3->GetData();
            
            const float* SignalData4 = InputSignal4->GetData();
            const float* TriggerData4 = InputTrigger4->GetData();
            float* OutputData4 = OutputSignal4->GetData();

            float Threshold = *InputThreshold;

            for (int32 i = 0; i < NumFrames; ++i)
            {
                // 1
                float CurrentTriggerValue1 = TriggerData1[i];

                if (PreviousTriggerValue1 < Threshold && CurrentTriggerValue1 >= Threshold)
                {
                    SampledValue1 = SignalData1[i];
                }

                OutputData1[i] = SampledValue1;
                PreviousTriggerValue1 = CurrentTriggerValue1;
                
                // 2
                float CurrentTriggerValue2 = TriggerData2[i];

                if (PreviousTriggerValue2 < Threshold && CurrentTriggerValue2 >= Threshold)
                {
                    SampledValue2 = SignalData2[i];
                }

                OutputData2[i] = SampledValue2;
                PreviousTriggerValue2 = CurrentTriggerValue2;
                
                // 3
                float CurrentTriggerValue3 = TriggerData3[i];

                if (PreviousTriggerValue3 < Threshold && CurrentTriggerValue3 >= Threshold)
                {
                    SampledValue3 = SignalData3[i];
                }

                OutputData3[i] = SampledValue3;
                PreviousTriggerValue3 = CurrentTriggerValue3;
                
                // 4
                float CurrentTriggerValue4 = TriggerData4[i];

                if (PreviousTriggerValue4 < Threshold && CurrentTriggerValue4 >= Threshold)
                {
                    SampledValue4 = SignalData4[i];
                }

                OutputData4[i] = SampledValue4;
                PreviousTriggerValue4 = CurrentTriggerValue4;
                
            }
        }

    private:

        // Inputs
        FAudioBufferReadRef InputSignal1;
        FAudioBufferReadRef InputTrigger1;
        FAudioBufferReadRef InputSignal2;
        FAudioBufferReadRef InputTrigger2;
        FAudioBufferReadRef InputSignal3;
        FAudioBufferReadRef InputTrigger3;
        FAudioBufferReadRef InputSignal4;
        FAudioBufferReadRef InputTrigger4;
        FFloatReadRef InputThreshold;

        // Outputs
        FAudioBufferWriteRef OutputSignal1;
        FAudioBufferWriteRef OutputSignal2;
        FAudioBufferWriteRef OutputSignal3;
        FAudioBufferWriteRef OutputSignal4;

        // Internal variables
        float SampledValue1;
        float SampledValue2;
        float SampledValue3;
        float SampledValue4;
        float PreviousTriggerValue1;
        float PreviousTriggerValue2;
        float PreviousTriggerValue3;
        float PreviousTriggerValue4;
    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FSahBankNode : public FNodeFacade
    {
    public:
        FSahBankNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSahBankOperator>())
        {
        }
    };

    // Register node
    METASOUND_REGISTER_NODE(FSahBankNode);
}

#undef LOCTEXT_NAMESPACE