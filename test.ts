// Smoke tests: verify blocks compile and type signatures are correct.
// Hardware behavior (soundLevel, onSound) must be tested on a physical device.
analogMic.useAnalogMicOnPin(AnalogPin.P1)                // default gain=7, normFactor=80
analogMic.useAnalogMicOnPin(AnalogPin.P1, 0, 80)         // explicit gain=0 (modules with onboard amp)
analogMic.useAnalogMicOnPin(AnalogPin.P1, 7, 100)        // custom normFactor
analogMic.setAnalogMicEnabled(false)                      // pause
analogMic.setAnalogMicEnabled(true)                       // resume
