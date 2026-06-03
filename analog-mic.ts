//% weight=100 color=#AA278F icon=""
//% block="Analog Mic"
namespace analogMic {
    /**
     * Use an analog microphone on an edge connector pin instead of the
     * built-in microphone. Call this at the start of your program before
     * any other sound blocks. Calling it more than once has no effect.
     *
     * Defaults (gain=7, normFactor=80) match the built-in mic behaviour exactly.
     * For modules with onboard amplification (e.g. MAX4466), start with gain=0.
     *
     * Valid ADC-capable pins: P0, P1, P2 (confirmed); P3, P4, P10 (also ADC-capable
     * but may conflict with other hardware). Do NOT use button pins (P5, P11).
     *
     * @param pin        analog-capable edge connector pin, e.g. AnalogPin.P1
     * @param gain       ADC hardware gain 0-7; default 7 matches built-in mic
     * @param normFactor normalisation factor 0-1000 (divided by 1000 internally); default 80 = 0.08
     */
    //% blockId=analog_mic_use_pin
    //% block="use analog microphone on pin %pin || gain %gain normFactor %normFactor"
    //% pin.fieldEditor="gridpicker"
    //% gain.min=0 gain.max=7 gain.defl=7
    //% normFactor.min=0 normFactor.max=1000 normFactor.defl=80
    //% expandableArgumentMode="toggle"
    //% group="Setup"
    export function useAnalogMicOnPin(
        pin: AnalogPin,
        gain: number = 7,
        normFactor: number = 80
    ): void {
        return;
    }

    /**
     * Pause or resume the analog microphone. Use before deep sleep to stop
     * ADC sampling and reduce power consumption. Resume before reading
     * soundLevel() or using onSound() events.
     * @param enabled true to resume mic, false to pause
     */
    //% blockId=analog_mic_set_enabled
    //% block="set analog microphone enabled %enabled"
    //% group="Setup"
    export function setAnalogMicEnabled(enabled: boolean): void {
        return;
    }
}
