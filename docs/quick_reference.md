# Quick Reference for Plugin Development
Mapping out a plugin template...these are mostly notes for my own reference for now.

## Overview
- Vertex names (METASOUND_PARAM)
- Operator class (inherit from `TExecutableOperator`)
  - *Public*
    - Constructor
    - Execute (void)
    - GetNodeInfo (FNodeClassMetadata)
    - DeclareVertexInterface (FVertexInterface)
    - GetInputs (FDataReferenceCollection)
    - GetOutputs (FDataReferenceCollection)
    - CreateOperator (TUniquePtr<IOperator>)
  - *Private*
    - Inputs
    - Outputs
- Class definition (inherit from `FNodeFacade`)

## Misc Notes
- [TArray](https://dev.epicgames.com/documentation/en-us/unreal-engine/array-containers-in-unreal-engine)
 - Add()
 - SetNum() 
 - Num() 
 - get length

---

## References

- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart)