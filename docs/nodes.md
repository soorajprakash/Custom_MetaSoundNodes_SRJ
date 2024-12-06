# Nodes

## Bool To Audio
### Inputs

| Name      | Description                              | Type         |
|-----------|------------------------------------------|--------------|
| Value     | Boolean input to convert to audio.       | Bool         |
| Rise Time | Rise time in seconds.                    | Time         |
| Fall Time | Fall time in seconds.                    | Time         |

### Outputs

| Name   | Description             | Type         |
|--------|-------------------------|--------------|
| Output | Audio signal.           | Audio Buffer |

## Click
### Inputs

| Name     | Description                                      | Type    |
|----------|--------------------------------------------------|---------|
| Trigger  | Trigger input to generate an impulse.            | Trigger |
| Bi-Polar | Toggle between bipolar and unipolar impulse output. | Bool    |

### Outputs

| Name           | Description                | Type         |
|----------------|----------------------------|--------------|
| Impulse Output | Generated impulse output.  | Audio Buffer |

## Clock Divider
### Inputs

| Name          | Description                         | Type    |
|---------------|-------------------------------------|---------|
| Trigger       | Input trigger to the clock divider. | Trigger |
| Reset         | Reset the clock divider.            | Trigger |

### Outputs

| Name           | Description                          | Type    |
|----------------|--------------------------------------|---------|
| 1 | Output trigger for division 1.       | Trigger |
| 2 | Output trigger for division 2.       | Trigger |
| 3 | Output trigger for division 3.       | Trigger |
| 4 | Output trigger for division 4.       | Trigger |
| 5 | Output trigger for division 5.       | Trigger |
| 6 | Output trigger for division 6.       | Trigger |
| 7 | Output trigger for division 7.       | Trigger |
| 8 | Output trigger for division 8.       | Trigger |

## Dust
### Inputs

| Name           | Description                                      | Type         |
|----------------|--------------------------------------------------|--------------|
| Enabled        | Enable or disable the dust node.                 | Bool         |
| Bi-Polar       | Toggle between bipolar and unipolar impulse output. | Bool         |
| Density Offset | Offset added to the absolute value of the density input. | Float        |
| Density        | Input density control signal.                    | Audio Buffer |

### Outputs

| Name           | Description                | Type         |
|----------------|----------------------------|--------------|
| Impulse Output | Generated impulse output.  | Audio Buffer |

## EDO
### Inputs

| Name                | Description                                      | Type  |
|---------------------|--------------------------------------------------|-------|
| Note Number         | Input MIDI note number (integer).                | Int32 |
| Reference Frequency | Reference frequency (float).                     | Float |
| Reference MIDI Note | Reference MIDI note (integer).                   | Int32 |
| Divisions           | Number of divisions of the octave (integer).     | Int32 |

### Outputs

| Name      | Description                | Type  |
|-----------|----------------------------|-------|
| Frequency | Output frequency (float).  | Float |

## Phase Disperser
### Inputs

| Name     | Description                                      | Type         |
|----------|--------------------------------------------------|--------------|
| In       | Incoming audio.                                  | Audio Buffer |
| Stages   | Number of allpass filter stages to apply (1-128).| Int32        |

### Outputs

| Name | Description                | Type         |
|------|----------------------------|--------------|
| Out  | Phase-dispersed audio.     | Audio Buffer |

### Inputs

| Name       | Description                                             | Type         |
|------------|---------------------------------------------------------|--------------|
| Signal     | Audio rate signal to be sampled.                        | Audio Buffer |
| Trigger    | Audio rate signal that triggers the sample and hold.    | Audio Buffer |
| Threshold  | Float value that determines when the trigger signal is considered "on". | Float        |

### Outputs

| Name    | Description                          | Type         |
|---------|--------------------------------------|--------------|
| Output  | The resulting sampled signal.        | Audio Buffer |

## Shift Register
### Inputs

| Name    | Description                              | Type         |
|---------|------------------------------------------|--------------|
| Signal  | Input signal to be shifted.              | Float        |
| Trigger | Trigger input to shift the register.     | Trigger      |

### Outputs

| Name         | Description                          | Type         |
|--------------|--------------------------------------|--------------|
| OutputSignal1 | Output signal for stage 1.           | Float        |
| OutputSignal2 | Output signal for stage 2.           | Float        |
| OutputSignal3 | Output signal for stage 3.           | Float        |
| OutputSignal4 | Output signal for stage 4.           | Float        |
| OutputSignal5 | Output signal for stage 5.           | Float        |
| OutputSignal6 | Output signal for stage 6.           | Float        |
| OutputSignal7 | Output signal for stage 7.           | Float        |
| OutputSignal8 | Output signal for stage 8.           | Float        |

## Slew (audio)
### Inputs

| Name      | Description                              | Type         |
|-----------|------------------------------------------|--------------|
| In        | Audio signal to smooth.                  | Audio Buffer |
| Rise Time | Rise time in seconds.                    | Time         |
| Fall Time | Fall time in seconds.                    | Time         |

### Outputs

| Name | Description                          | Type         |
|------|--------------------------------------|--------------|
| Out  | Slew rate limited output signal.     | Audio Buffer |

## Slew (float)
### Inputs

| Name      | Description                              | Type  |
|-----------|------------------------------------------|-------|
| In        | Float to smooth.                         | Float |
| Rise Time | Rise time in seconds.                    | Time  |
| Fall Time | Fall time in seconds.                    | Time  |

### Outputs

| Name | Description                | Type  |
|------|----------------------------|-------|
| Out  | Slew rate limited float.   | Float |

## Stereo Crossfade
### Inputs

| Name       | Description                              | Type         |
|------------|------------------------------------------|--------------|
| In1 L      | Left channel of first input.             | Audio Buffer |
| In1 R      | Right channel of first input.            | Audio Buffer |
| In2 L      | Left channel of second input.            | Audio Buffer |
| In2 R      | Right channel of second input.           | Audio Buffer |
| Crossfade  | Crossfade between the two inputs (0.0 to 1.0). | Float        |

### Outputs

| Name  | Description                          | Type         |
|-------|--------------------------------------|--------------|
| Out L | Left channel of the output signal.   | Audio Buffer |
| Out R | Right channel of the output signal.  | Audio Buffer |

## Stereo Inverter
### Inputs

| Name         | Description                              | Type         |
|--------------|------------------------------------------|--------------|
| In L         | Left channel audio input.                | Audio Buffer |
| In R         | Right channel audio input.               | Audio Buffer |
| Invert L     | Invert the polarity of the left channel. | Bool         |
| Invert R     | Invert the polarity of the right channel.| Bool         |
| Swap L/R     | Swap the left and right channels.        | Bool         |

### Outputs

| Name  | Description                | Type         |
|-------|----------------------------|--------------|
| Out L | Left output channel.       | Audio Buffer |
| Out R | Right output channel.      | Audio Buffer |


## Stereo Width
### Inputs

| Name   | Description                                              | Type         |
|--------|----------------------------------------------------------|--------------|
| In L   | Left channel.                                            | Audio Buffer |
| In R   | Right channel.                                           | Audio Buffer |
| Width  | Stereo width factor ranging from 0 to 200% (0 - 2).      | Float        |

### Outputs

| Name  | Description                                              | Type         |
|-------|----------------------------------------------------------|--------------|
| Out L | Left channel of the adjusted stereo output signal.       | Audio Buffer |
| Out R | Right channel of the adjusted stereo output signal.      | Audio Buffer |

## Tuning
### Inputs

| Name                | Description                                      | Type  |
|---------------------|--------------------------------------------------|-------|
| MIDI Note Number    | Input MIDI note number (integer).                | Int32 |
| +/- Cents C         | Tuning adjustment for note 0 in cents.           | Float |
| +/- Cents C♯ / D♭   | Tuning adjustment for note 1 in cents.           | Float |
| +/- Cents D         | Tuning adjustment for note 2 in cents.           | Float |
| +/- Cents D♯ / E♭   | Tuning adjustment for note 3 in cents.           | Float |
| +/- Cents E         | Tuning adjustment for note 4 in cents.           | Float |
| +/- Cents F         | Tuning adjustment for note 5 in cents.           | Float |
| +/- Cents F♯ / G♭   | Tuning adjustment for note 6 in cents.           | Float |
| +/- Cents G         | Tuning adjustment for note 7 in cents.           | Float |
| +/- Cents G♯ / A♭   | Tuning adjustment for note 8 in cents.           | Float |
| +/- Cents A         | Tuning adjustment for note 9 in cents.           | Float |
| +/- Cents A♯ / B♭   | Tuning adjustment for note 10 in cents.          | Float |
| +/- Cents B         | Tuning adjustment for note 11 in cents.          | Float |

### Outputs

| Name      | Description                | Type  |
|-----------|----------------------------|-------|
| Frequency | Output frequency (float).  | Float |

