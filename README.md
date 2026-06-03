# microbit-mic

A MakeCode extension for micro:bit v2 that redirects the audio pipeline to an
external analog microphone on an edge connector pin. Other extensions that use
`soundLevel()`, `onSound()`, or audio recording work transparently with the new
mic — no changes required.

## Hardware

Wire a standard analog electret mic module (e.g. MAX4466, MAX9814) to the micro:bit:

| Mic module | micro:bit |
|------------|-----------|
| VCC        | 3V        |
| GND        | GND       |
| OUT        | P1 (or any ADC-capable pin) |

**ADC-capable pins**: P0, P1, P2 (confirmed). P3, P4, P10 also work but may
conflict with other hardware. Do not use button pins (P5, P11).

## Usage

Place the setup block at the very start of your program, before any sound blocks:

```blocks
analogMic.useAnalogMicOnPin(AnalogPin.P1)
```

After that, all standard sound blocks read from the analog mic:

```blocks
basic.forever(function () {
    basic.showNumber(input.soundLevel())
})
```

```blocks
input.onSound(DetectedSound.Loud, function () {
    basic.showIcon(IconNames.Heart)
})
```

### Gain setting

The `gain` parameter (0–7) controls ADC hardware amplification. Default is 7,
which matches the built-in mic. **If you have a module with onboard amplification
(e.g. MAX4466 with gain pot), start with `gain=0`** to avoid clipping, then
increase if `soundLevel()` stays near zero.

```blocks
analogMic.useAnalogMicOnPin(AnalogPin.P1, 0, 80)
```

### Normalisation factor

The `normFactor` parameter (0–1000) controls the `StreamNormalizer` gain in the
audio pipeline. The value is divided by 1000 internally (`80` → `0.08`). Default
`80` matches the built-in mic behaviour. Increase to boost quiet signals; decrease
to reduce clipping on loud signals.

### Deep sleep / power saving

The analog mic runs continuously by default. To stop ADC sampling during sleep:

```blocks
analogMic.setAnalogMicEnabled(false)
// ... sleep or low-power operations ...
analogMic.setAnalogMicEnabled(true)
```

## Limitations

- **micro:bit v2 only** — requires the nRF52833 ADC and CODAL runtime.
- **Call before audio blocks** — the swap must happen before any `soundLevel()`,
  `onSound()`, or audio-recording blocks run. Behaviour is undefined if called after.
- **One-way swap** — calling `useAnalogMicOnPin` more than once has no effect.
  The swap cannot be reversed at runtime.
- **Deep sleep** — the analog mic continues sampling during deep sleep unless
  `setAnalogMicEnabled(false)` is called first.

## How it works

The CODAL `MicroBitAudio` class exposes its pipeline components as public
pointers (`levelSPL`, `processor`, `splitter`, `rawSplitter`, `mic`). This
extension builds a parallel pipeline for the specified analog pin and hot-swaps
those pointers at runtime. The built-in PDM mic is disabled via `deactivateMic()`.
Because `DEVICE_ID_SYSTEM_LEVEL_DETECTOR` is preserved, all pxt-microbit sound
event registrations fire from the new mic without modification.

See [design.md](design.md) for full architecture details and engineering review notes.

## License

MIT
