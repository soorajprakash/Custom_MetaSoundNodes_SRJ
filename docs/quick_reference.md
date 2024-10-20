# Quick Reference for Plugin Development
Mapping out a plugin template...these are mostly notes for my own reference for now.

## Overview
- Vertex names (`METASOUND_PARAM`)
- Operator class (inherit from `TExecutableOperator`)
  - *Public*
    - Constructor
    - Execute (`void`)
    - GetNodeInfo (`FNodeClassMetadata`)
    - DeclareVertexInterface (`FVertexInterface`)
    - GetInputs (`FDataReferenceCollection`)
    - GetOutputs (`FDataReferenceCollection`)
    - CreateOperator (`TUniquePtr<IOperator>`)
  - *Private*
    - Inputs
    - Outputs
- Class definition (inherit from `FNodeFacade`)

## Checklists
### Add an input
- [ ] Add a new `METASOUND_PARAM` to vertex names in the node's namespace
- [ ] Add a reference to the input buffer to operator parameters (constructor)
- [ ] Initialise a corresponding variable
- [ ] Add the new input (TInputDataVertexModel) to DeclareVertexInterface
- [ ] Add DataReference to GetInputs
- [ ] Add GetDataReadReferenceOrConstructWithVertexDefault to CreateOperator
- [ ] Add private variable for the input

## Misc Notes
- [TArray](https://dev.epicgames.com/documentation/en-us/unreal-engine/array-containers-in-unreal-engine)
  - Add()
  - SetNum() 
  - Num() 
  - get length
- METASOUND_PARAM: METASOUND_PARAM(NAME, NAME_TEXT, TOOLTIP_TEXT)
---

## References

- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart)