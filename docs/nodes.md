# Nodes

## Bool To Audio
### Inputs

| Name      | Description                              | Type    |
|-----------|------------------------------------------|---------|
| Value     | Boolean input to convert to audio.       | Bool    |
| Rise Time | Rise time in seconds.                    | Time    |
| Fall Time | Fall time in seconds.                    | Time    |

### Outputs

| Name   | Description             | Type         |
|--------|-------------------------|--------------|
| Out | Audio signal.           | Audio |

## Click
### Inputs

| Name     | Description                                      | Type    |
|----------|--------------------------------------------------|---------|
| Trigger  | Trigger input to generate an impulse.            | Trigger |
| Bi-Polar | Toggle between bipolar and unipolar impulse output. | Bool    |

### Outputs

| Name           | Description                | Type         |
|----------------|----------------------------|--------------|
| On Trigger | Trigger passthrough.       | Trigger |
| Impulse Output | Generated impulse output.  | Audio |

## Clock Divider
### Inputs

| Name    | Description                              | Type    |
|---------|------------------------------------------|---------|
| Trigger | Input trigger to the clock divider.      | Trigger |
| Reset   | Reset the clock divider.                 | Trigger |

### Outputs

| Name          | Description                          | Type    |
|---------------|--------------------------------------|---------|
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
| Density | Probability of impulse generation. | Float        |
| Modulation        | Density control signal.                    | Audio |

### Outputs

| Name           | Description                | Type         |
|----------------|----------------------------|--------------|
| Impulse Output | Generated impulse output.  | Audio |

## Edge

### Inputs

| Name      | Description                                             | Type         |
|-----------|---------------------------------------------------------|--------------|
| In        | Input audio to monitor for edge detection.              | Audio |
| Debounce  | Debounce time in seconds to prevent rapid triggering.    | Time        |

### Outputs

| Name  | Description                | Type    |
|-------|----------------------------|---------|
| Rise  | Trigger on rise.           | Trigger |
| Fall  | Trigger on fall.           | Trigger |

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
| In       | Incoming audio.                                  | Audio |
| Stages   | Number of allpass filter stages to apply (1-128).| Int32        |

### Outputs

| Name | Description                | Type         |
|------|----------------------------|--------------|
| Out  | Phase-dispersed audio.     | Audio |

### Inputs

| Name       | Description                                             | Type         |
|------------|---------------------------------------------------------|--------------|
| Signal     | Audio rate signal to be sampled.                        | Audio |
| Trigger    | Audio rate signal that triggers the sample and hold.    | Audio |
| Threshold  | Float value that determines when the trigger signal is considered "on". | Float        |

### Outputs

| Name    | Description                          | Type         |
|---------|--------------------------------------|--------------|
| Out  | The resulting sampled signal.        | Audio |

## Shift Register
### Inputs

| Name    | Description                              | Type         |
|---------|------------------------------------------|--------------|
| Signal  | Input signal to be shifted.              | Float        |
| Trigger | Trigger input to shift the register.     | Trigger      |

### Outputs

| Name         | Description                          | Type         |
|--------------|--------------------------------------|--------------|
| Stage 1 | Shifted output at stage 1.           | Float        |
| Stage 2 | Shifted output at stage 2.           | Float        |
| Stage 3 | Shifted output at stage 3.           | Float        |
| Stage 4 | Shifted output at stage 4.           | Float        |
| Stage 5 | Shifted output at stage 5.           | Float        |
| Stage 6 | Shifted output at stage 6.           | Float        |
| Stage 7 | Shifted output at stage 7.           | Float        |
| Stage 8 | Shifted output at stage 8.           | Float        |

## Slew (audio)
### Inputs

| Name      | Description                              | Type         |
|-----------|------------------------------------------|--------------|
| In        | Audio signal to smooth.                  | Audio |
| Rise Time | Rise time in seconds.                    | Time         |
| Fall Time | Fall time in seconds.                    | Time         |

### Outputs

| Name | Description                          | Type         |
|------|--------------------------------------|--------------|
| Out  | Slew rate limited output signal.     | Audio |

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

## Stereo Balance
### Inputs

| Name   | Description                                              | Type         |
|--------|----------------------------------------------------------|--------------|
| In L   | Left channel.                                            | Audio |
| In R   | Right channel.                                           | Audio |
| Balance  | Balance control ranging from -1.0 (full left) to 1.0 (full right).      | Float        |

### Outputs

| Name  | Description                                              | Type         |
|-------|----------------------------------------------------------|--------------|
| Out L | Left channel of the adjusted stereo output signal.       | Audio |
| Out R | Right channel of the adjusted stereo output signal.      | Audio |

## Stereo Crossfade
### Inputs

| Name       | Description                              | Type         |
|------------|------------------------------------------|--------------|
| In1 L      | Left channel of first input.             | Audio |
| In1 R      | Right channel of first input.            | Audio |
| In2 L      | Left channel of second input.            | Audio |
| In2 R      | Right channel of second input.           | Audio |
| Crossfade  | Crossfade between the two inputs (0.0 to 1.0). | Float        |

### Outputs

| Name  | Description                          | Type         |
|-------|--------------------------------------|--------------|
| Out L | Left channel of the output signal.   | Audio |
| Out R | Right channel of the output signal.  | Audio |

## Stereo Gain
### Inputs

| Name   | Description                                              | Type         |
|--------|----------------------------------------------------------|--------------|
| In L   | Left channel.                                            | Audio |
| In R   | Right channel.                                           | Audio |
| Balance  | Gain Input (Lin)", "Gain control (0.0 to 1.0).      | Float        |

### Outputs

| Name  | Description                                              | Type         |
|-------|----------------------------------------------------------|--------------|
| Out L | Left channel of the adjusted stereo output signal.       | Audio |
| Out R | Right channel of the adjusted stereo output signal.      | Audio |

## Stereo Inverter
### Inputs

| Name         | Description                              | Type         |
|--------------|------------------------------------------|--------------|
| In L         | Left channel audio input.                | Audio |
| In R         | Right channel audio input.               | Audio |
| Invert L     | Invert the polarity of the left channel. | Bool         |
| Invert R     | Invert the polarity of the right channel.| Bool         |
| Swap L/R     | Swap the left and right channels.        | Bool         |

### Outputs

| Name  | Description                | Type         |
|-------|----------------------------|--------------|
| Out L | Left output channel.       | Audio |
| Out R | Right output channel.      | Audio |


## Stereo Width
### Inputs

| Name   | Description                                              | Type         |
|--------|----------------------------------------------------------|--------------|
| In L   | Left channel.                                            | Audio |
| In R   | Right channel.                                           | Audio |
| Width  | Stereo width factor ranging from 0 to 200% (0 - 2).      | Float        |

### Outputs

| Name  | Description                                              | Type         |
|-------|----------------------------------------------------------|--------------|
| Out L | Left channel of the adjusted stereo output signal.       | Audio |
| Out R | Right channel of the adjusted stereo output signal.      | Audio |

## Tuning
### Inputs

| Name                | Description                                      | Type  |
|---------------------|--------------------------------------------------|-------|
| Note Number    | Input MIDI note number (integer).                | Int32 |
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

