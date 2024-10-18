# Audio Rate Sample and Hold

The sample and hold function in Metasynth takes an audio rate signal as input, and a trigger signal as float.

A purely audio-rate version seemed like a good starting point for my first custom node in Metasynth, since it's one of my go-to building blocks in other environments.
The threshold control remains a float, as it's rare that we'd be setting this all that frequently.

## Function
The output signal is held at the last value of the input signal when the trigger signal crosses a threshold.

## Pure Data Implementation
Pure Data already has a sample and hold object [samphold~], which accepts audio rate signals through both inlets.
I thought this might help illustrate the process for people less familiar with DSP code. 
However, since the idea is to stay in audio rate, we don't exactly end up with a typical Pd patch.

![Pure Data version of the sample and hold object](./docs/SaH_audiorate_Pd.png)
- [Pure Data abstraction](./docs/SaH_audiorate.pd)
- [Example patch](./docs/SaH_audiorate_example.pd)]

Unlike other environments like Max, there's no built-in object for `[>~]` (a "greater than" comparison) in Pd. 
Instead, we can use the `[expr~]` object, which evaluates of a C-like expression.
Inputs are defined within the expression as `$v1`, `$v2`, etc.
Multiple outputs can be defined by adding another expression after a semicolon (in this case, simply duplicating the input trigger value).

We can use a variable (represented by `[value]` or `[v]`) for `InputThreshold`. 
`InputTrigger` is represented by `$v1`, and `InputSignal` is `$v2`.
Other key variables would be `SampledValue` and `previousTriggerValue`. 
As far as I know, `[expr~]` does not allow setting variables directly within the expression, and we can't create a feedback loop even if the values don't go through the same path[^1].  
Instead, we need to send the information out through the patch, which we do through delay lines with time set to 0 (`[delwrite~]` and `[delread~]`).
The expr~ object is used to detect when the trigger signal crosses the threshold, and then the value of the input signal is sampled and held until the next trigger event.

---
[^1]: We may be able to get around this by using [fexpr~] instead...


## C++ Implementation

Most of the file is occupied by setting up the node in the Unreal Engine plugin system. 
The actual implementation of the sample and hold is quite straightforward, and we don't have to jump through the same kind of hoops as we did for the Pd version.

### Constructor ("setup"):
```cpp
 FSahOperator(
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
```

### Execute ("loop"):
```cpp
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
```

