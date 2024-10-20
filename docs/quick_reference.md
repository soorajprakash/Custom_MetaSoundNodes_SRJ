# Quick Reference for Plugin Development
Mapping out a plugin template...these notes are mostly for my own reference, for the time being.

## Overview
- Vertex names (`METASOUND_PARAM`)
- Operator class (inherit from `TExecutableOperator`)
  - **Public**
    - Constructor
    - GetNodeInfo (`FNodeClassMetadata`)
    - *DeclareVertexInterface* (`FVertexInterface`)
    - *GetInputs* (`FDataReferenceCollection`)
    - *GetOutputs* (`FDataReferenceCollection`)
    - *CreateOperator* (`TUniquePtr<IOperator>`)
    - Execute (`void`)
  - **Private**
    - Inputs
    - Outputs
    - Internal variables
- Node Class (inherit from `FNodeFacade`)

Items in *italics* must use the namespace described in the vertex names.

## Misc Notes
- [TArray](https://dev.epicgames.com/documentation/en-us/unreal-engine/array-containers-in-unreal-engine)
  - Add()
  - SetNum() 
  - Num() 
  - get length

---

## References

- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart)
- [Factory method pattern (Wikipedia)](https://en.wikipedia.org/wiki/Factory_method_pattern)
- [Epic Games Coding Standards](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine?application_version=5.4)
