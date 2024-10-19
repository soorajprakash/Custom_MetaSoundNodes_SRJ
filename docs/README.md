# Documentation
As a starting point for the documentation, I'm throwing together some of my notes on the custom nodes.  I'll try to break a few ideas down along the way, partly to consolidate my own learning.

This was my first attempt at writing a plugin for Unreal Engine, and with hindsight I would have tried a more generic model to start with.
In particular, I still need to work out how best to approach the header files -- this got mixed up with troubleshooting the build process.

If you're browsing the source code and see some odd choices, please let me know!

## Audio Rate Sample and Hold

### Function
The output signal is held at the last value of the input signal when the trigger signal crosses a threshold.

The existing sample and hold function in Metasound takes an audio rate signal as input, and a trigger signal as float.

A purely audio-rate version seemed like a good starting point for my first custom node in Metasound, since it's one of my go-to building blocks in other environments such as Pd.

### Inputs

| Name       | Description                                             | Type |
|------------|---------------------------------------------------------|------|
| Signal     | Audio rate signal to be sampled.                        | Audio Buffer |
| Trigger    | Audio rate signal that triggers the sample and hold.    | Audio Buffer |
| Threshold  | Float value that determines when the trigger signal is considered "on". | Float |

### Outputs

| Name    | Description                          | Type |
|---------|--------------------------------------|------|
| Output  | The resulting sampled signal.        | Audio Buffer |

### C++ Implementation (Custom Metasound Node)

I used Anna Lantz's tutorial [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart) as a starting point.
According to the tutorial, this can live in a single .cpp file, but I found that I needed to include a header during troubleshooting (the issue might have been in the build process).

The tutorial breaks everything down nicely, so I won't duplicate it here. 
Most of the .cpp file is occupied by setting up the node and its pins (inlets/outlets), and registering the node. 
The implementation of the sample and hold process itself is quite straightforward, as long as you don't mind pointers (which are pretty much everywhere in this context anyway).

In the following code snippets, we could think of the operator class constructor and execute function as "setup" and "loop" respectively.

#### Constructor
```C++
// Formatting modified to help separate the parameters and initialisation list

FSahOperator(
    // Arguments
    const FAudioBufferReadRef& InSignal,
    const FAudioBufferReadRef& InTrigger,
    const FFloatReadRef& InThreshold
    )

    :
    
    // Initialisation list
    InputSignal(InSignal),
    InputTrigger(InTrigger),
    InputThreshold(InThreshold),
    OutputSignal(FAudioBufferWriteRef::CreateNew(InSignal->Num())),
    SampledValue(0.0f),
    PreviousTriggerValue(0.0f)
    {
        // Nothing to do here...
    }
```

The constructor has three parameters: pointers to the buffers for input signal, the trigger signal, and the threshold. 
Member variables are then set in an initialization list, so the body of the constructor is empty in this case.
`SampledValue` and `PreviousTriggerValue` are just regular float variables.

#### Execute
```C++
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

            // Detect rising edge (compare to the previous trigger value)
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
```

Although we're working on individual samples, we have to work on them in chunks. The `Execute` function processes an audio buffer/block of samples.
The block is broken down into "frames" -- each of which represents a sample point when information is read from each of the input buffers/inlets[^1].

Within our loop of the `Execute` function, we need to loop through each of these frames: checking the trigger signal against the threshold, and writing the corresponding entry in the output buffer accordingly.
`SignalData` and `TriggerData` point to arrays of floats, as returned by `FAudioBufferReadRef`.

#### Misc Notes
I'm attempting to follow [Epic's coding standards](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine?application_version=5.4) to the best of my understanding, for example:
- use PascalCase throughout
- prefixing type names e.g. `F` to indicate class definitions for structs containing floats
- header files start with `#pragma once` (only include the header once in a compilation)

### Bonus: Pure Data Implementation
While drafting this documentation, I thought that a Pure Data signal flow might help illustrate the process for people less familiar with DSP code. 
However, keeping everything in audio rate complicates matters, so we don't exactly end up with a typical Pd patch. In fact, if anything, this probably helps illustrate how C++ makes more sense than visual patching at this lower level of abstraction.

Pure Data already has a sample and hold object `[samphold~]`, which accepts audio rate signals through both inlets, but here's a version that adds a bonus inlet for the threshold:

![Pure Data version of the sample and hold object](./SaH_audiorate_Pd.png)
- [Pure Data abstraction](./SaH_audiorate.pd)
- [Example patch](./SaH_audiorate_example.pd)

Unlike other environments like Max, there's no built-in object for `[>~]` (a "greater than" comparison) in Pd. 
Instead, I tend to use the `[expr~]` object, which evaluates a C-like expression.[^2]

Inputs are defined within the expression as `$v1`, `$v2`, etc.
An `if` statement is formatted as follows: if(condition, outputIfTrue, outputIfFalse).
Multiple outputs can be defined by adding another expression after a semicolon (in this case, simply duplicating the input trigger value).

We can use a variable (represented by `[value]` or `[v]`) for `InputThreshold`. 
`InputTrigger` is represented by `$v1`, and `InputSignal` is `$v2`.
Other key variables would be `SampledValue` and `previousTriggerValue`. 
`[expr~]` does not allow setting variables directly within the expression, and we can't create a feedback loop even if the values don't go through the same path.  
Instead, we need to send the information out through the patch, which we do through delay lines with time set to 0 (`[delwrite~]` and `[delread~]`).
The expr~ object is used to detect when the trigger signal crosses the threshold, and then the value of the input signal is sampled and held until the next trigger event.

---

## References
- [TArray](https://dev.epicgames.com/documentation/en-us/unreal-engine/array-containers-in-unreal-engine)
- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart)
- [Sample and Hold on SweetWater inSync](https://www.sweetwater.com/insync/a-simple-guide-to-modulation-sample-and-hold/)

---

[^1]: Not to be confused with video frames, etc.
[^2]: We can actually use `[fexpr~]` instead: `fexpr~ if($x2[-1] < threshold && $x2[0] >= threshold, $x1[0], $y[0]);` (I've included this as [Pd implementation 2](./Sah_audiorate_fexpr.pd))...but this point, we're not really patching any more.  Besides, the above example highlights similar feedback-related barriers that we might encounter if we were to try to recreate this using Metasound abstractions.