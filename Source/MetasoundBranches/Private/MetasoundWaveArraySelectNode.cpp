#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundFacade.h"				 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros
#include "MetasoundWave.h"

#define LOCTEXT_NAMESPACE "WaveArraySelectNode"

namespace Metasound
{
	namespace WaveAssetArraySelectNodeNames
	{
		METASOUND_PARAM(InputBoolValue, "BoolCondition", "Bool Condition")
		METASOUND_PARAM(InputTrueArray, "True Array", "Array for TRUE")
		METASOUND_PARAM(InputFalseArray, "False Array", "Array for FALSE")
		METASOUND_PARAM(OutputSelectedArray, "Output Array", "Array for given Boolean")
	}

	class FWaveArraySelectOperator : public TExecutableOperator<FWaveArraySelectOperator>
	{
		using FArrayDataReadReference = TDataReadReference<TArray<FWaveAsset>>;
		using FArrayDataWriteReference = TDataWriteReference<TArray<FWaveAsset>>;
		
		// Inputs
		FBoolReadRef BoolCondition;
		FArrayDataReadReference TrueArray;
		FArrayDataReadReference FalseArray;

		// Outputs
		FArrayDataWriteReference OutputArray;

	public:
		FWaveArraySelectOperator(const FOperatorSettings& InSettings,
		                         const FBoolReadRef& InBoolConditionValue,
		                         const FArrayDataReadReference& InTrueArrayValue,
		                         const FArrayDataReadReference& InFalseArrayValue)
			: BoolCondition(InBoolConditionValue)
			, TrueArray(InTrueArrayValue)
			, FalseArray(InFalseArrayValue)
			, OutputArray(FArrayDataWriteReference::CreateNew())
		{
		}

		void Execute()
		{
			*OutputArray = *BoolCondition ? *TrueArray : *FalseArray;
		}

		static const FVertexInterface& DeclareVertexInterface()
		{
			using namespace WaveAssetArraySelectNodeNames;

			static const FVertexInterface Interface(
				FInputVertexInterface(
					TInputDataVertex<bool>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputBoolValue)),
					TInputDataVertex<TArray<FWaveAsset>>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrueArray)),
					TInputDataVertex<TArray<FWaveAsset>>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputFalseArray))
				),
				FOutputVertexInterface(
					TOutputDataVertex<TArray<FWaveAsset>>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSelectedArray))
				)
			);

			return Interface;
		}

		static const FNodeClassMetadata& GetNodeInfo()
		{
			auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
			{
				FNodeClassMetadata Metadata;
				Metadata.ClassName = {TEXT("UE"), TEXT("Select"), TEXT("WaveAssetArray")};
				Metadata.MajorVersion = 1;
				Metadata.MinorVersion = 0;
				Metadata.DisplayName = METASOUND_LOCTEXT("WaveAssetArraySelectDisplayName", "Select Wave Asset Array");
				Metadata.Description = METASOUND_LOCTEXT("WaveAssetArraySelectDesc",
				                                         "Select one of the two input wave asset arrays using the input boolean");
				Metadata.Author = "Sooraj Prakash";
				Metadata.PromptIfMissing = PluginNodeMissingPrompt;
				Metadata.DefaultInterface = DeclareVertexInterface();
				Metadata.CategoryHierarchy = {METASOUND_LOCTEXT("Custom", "Branches")};
				Metadata.Keywords = TArray<FText>();

				return Metadata;
			};

			static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
			return Metadata;
		}

		// virtual FDataReferenceCollection GetInputs() const override
		// {
		// 	using namespace WaveAssetArraySelectNodeNames;
		// 	FDataReferenceCollection InputDataReferences;
		//
		// 	InputDataReferences.AddDataReadReference(
		// 		METASOUND_GET_PARAM_NAME(InputBoolValue),
		// 		BoolCondition);
		//
		// 	InputDataReferences.AddDataReadReference(
		// 		METASOUND_GET_PARAM_NAME(InputTrueArray),
		// 		TrueArray);
		//
		// 	InputDataReferences.AddDataReadReference(
		// 		METASOUND_GET_PARAM_NAME(InputFalseArray),
		// 		FalseArray);
		//
		// 	return InputDataReferences;
		// }

		virtual void BindInputs(FInputVertexInterfaceData& InVertexData) override
		{
			using namespace WaveAssetArraySelectNodeNames;

			InVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InputBoolValue),
			                            BoolCondition);
			InVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InputTrueArray),
			                            TrueArray);
			InVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InputFalseArray),
			                            FalseArray);
		}

		virtual void BindOutputs(FOutputVertexInterfaceData& InVertexData) override
		{
			using namespace WaveAssetArraySelectNodeNames;

			InVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(OutputSelectedArray),
			                             OutputArray);
		}

		// virtual FDataReferenceCollection GetOutputs() const override
		// {
		// 	using namespace WaveAssetArraySelectNodeNames;
		// 	FDataReferenceCollection OutputDataReferences;
		//
		// 	OutputDataReferences.AddDataReadReference(
		// 		METASOUND_GET_PARAM_NAME(OutputSelectedArray),
		// 		OutputArray);
		//
		// 	return OutputDataReferences;
		// }

		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutErrors)
		{
			using namespace WaveAssetArraySelectNodeNames;

			const FInputVertexInterfaceData& InputData = InParams.InputData;

			TDataReadReference<bool> InputBool = InputData.GetOrCreateDefaultDataReadReference<bool>(
				METASOUND_GET_PARAM_NAME(InputBoolValue),
				InParams.OperatorSettings
			);

			TDataReadReference<TArray<FWaveAsset>> InputTrueArray = InputData.GetOrCreateDefaultDataReadReference<TArray<FWaveAsset>>(
				METASOUND_GET_PARAM_NAME(InputTrueArray),
				InParams.OperatorSettings
			);

			TDataReadReference<TArray<FWaveAsset>> InputFalseArray = InputData.GetOrCreateDefaultDataReadReference<TArray<FWaveAsset>>(
				METASOUND_GET_PARAM_NAME(InputFalseArray),
				InParams.OperatorSettings
			);

			return MakeUnique<FWaveArraySelectOperator>(InParams.OperatorSettings, InputBool, InputTrueArray, InputFalseArray);
		}
	};

	class FWaveAssetArraySelectorNode : public FNodeFacade
	{
	public:
		FWaveAssetArraySelectorNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID,
			              TFacadeOperatorClass<FWaveArraySelectOperator>())
		{
		}
	};

	METASOUND_REGISTER_NODE(FWaveAssetArraySelectorNode);
}
#undef LOCTEXT_NAMESPACE
