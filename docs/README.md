A collection of custom nodes for Unreal Engine's MetaSounds, soon to be released as the *Branches* plugin.

![Screenshot of a selection of custom nodes in Metasound](svg/Nodes.svg)

# Nodes

<!-- - [Bool To Audio](BoolToAudio.html)
- [Clock Divider](ClockDivider.html)
- [Impulse](Impulse.html)
- [Dust](Dust.html)
- [Edge](Edge.html)
- [EDO](EDO.html)
- [Phase Disperser](PhaseDisperser.html)
- [Sample and Hold](SampleandHold.html)
- [Shift Register](ShiftRegister.html)
- [Slew (audio)](Slew(audio).html)
- [Slew (float)](Slew(float).html)
- [Stereo Balance](StereoBalance.html)
- [Stereo Crossfade](StereoCrossfade.html)
- [Stereo Gain](StereoGain.html)
- [Stereo Inverter](StereoInverter.html)
- [Stereo Width](StereoWidth.html)
- [Tuning](Tuning.html) -->

| Node                                                                  | Category         | Description                                                                                                         |
|-----------------------------------------------------------------------|------------------|---------------------------------------------------------------------------------------------------------------------|
| [`Bool To Audio`](BoolToAudio.html)                     | Conversions      | Convert a boolean value to an audio signal, with optional rise and fall times.                                      |
| [`Clock Divider`](ClockDivider.html)                     | Triggers         | Divide a trigger into eight density levels.                                                                         |
| [`Impulse`](Impulse.html)                                   | Generators       | Trigger a one-sample impulse (unipolar or alternating polarity per impulse).                                        |
| [`Dust`](Dust.html)                                     | Generators       | A randomly timed impulse generator (unipolar or alternating polarity per impulse) with density control and audio-rate modulation. |
| [`Edge`](Edge.html)                                     | Envelopes        | Detect upward and downward changes in an input audio signal, with optional debounce.                                |
| [`EDO`](EDO.html)                                       | Tuning           | Generate frequencies for tuning systems using equally divided octaves (float) with a MIDI note input. Set a reference frequency and reference MIDI note (defaults to A440). |
| [`Phase Disperser`](PhaseDisperser.html)               | Filters          | A chain of allpass filters to soften transients and add that classic laser/slinky-style effect.                     |
| [`Sample And Hold (audio trigger)`](SampleandHold.html) | Filters          | An alternative sample and hold unit with an audio-rate trigger.                                                     |
| [`Shift Register`](ShiftRegister.html)                 | Modulation       | An eight-stage shift register for floats.                                                                           |
| [`Slew (audio)`](Slew(audio).html)                     | Filters          | A slew limiter to smooth out the rise and fall times of an audio signal.                                            |
| [`Slew (float)`](Slew(float).html)                     | Filters          | A slew limiter to smooth out the rise and fall times of a float value.                                              |
| [`Stereo Balance`](StereoBalance.html)                  | Spatialization   | Adjust the balance of a stereo signal.                                                                              |
| [`Stereo Crossfade`](StereoCrossfade.html)             | Envelopes        | Cross fade between two stereo signals.                                                                              |
| [`Stereo Gain`](StereoGain.html)                       | Mix              | Adjust gain for a stereo signal.                                                                                    |
| [`Stereo Inverter`](StereoInverter.html)               | Spatialization   | Invert and/or swap stereo channels.                                                                                 |
| [`Stereo Width`](StereoWidth.html)                     | Spatialization   | Stereo width adjustment (0-200%), using mid-side processing.                                                        |
| [`Tuning`](Tuning.html)                                 | Tuning           | Quantize a float value to a custom 12-note tuning, with adjustment in cents per-note.                               |
| [`Zero Crossing`](ZeroCrossing.html)                                 | Zero Crossing           | Detect zero crossings in an input audio signal, with optional debounce.                               |

---

[Charles Matthews 2024](https://github.com/matthewscharles/)