#include "pxt.h"

#if MICROBIT_CODAL
#include "MicroBitAudio.h"
#include "StreamSplitter.h"
#include "StreamNormalizer.h"
#include "LevelDetectorSPL.h"
#include "NRF52ADC.h"

namespace analogMic {

bool initialized = false;
volatile bool altMicActive = true;  // filterOn flag; set false to pause during deep sleep

/* gain NRF52ADC hardware gain step, 0-7 (7 = built-in default; external modules
/       with onboard amp may clip -- start at 0 and increase during bring-up)
/ normFactor StreamNormalizer normalisation factor, integer 0-1000 representing
/             0.000-1.000 (divided by 1000.0f before passing to CODAL).
/             Default 80 -> 0.08f, matches built-in mic.
*/
//%
void useAnalogMicOnPin(int pinId, int gain, int normFactor) {
    if (initialized) return;  // Silent no-op if called twice -- by design
    initialized = true;

    // Resolve AnalogPin enum to hardware pin.
    // pxt::getPin() is the correct pattern (confirmed from pxt-microbit/libs/core/pins.cpp:128).
    // On CODAL builds MicroBitPin IS NRF52Pin via typedef -- cast is safe.
    NRF52Pin *targetPin = (NRF52Pin*) pxt::getPin((int)pinId);
    if (!targetPin) {
        target_panic(DEVICE_HARDWARE_CONFIGURATION_ERROR);
        return;
    }

    // Acquire an ADC channel for the analog pin.
    NRF52ADCChannel *altMic = uBit.adc.getChannel(*targetPin, false);
    if (!altMic) {
        target_panic(DEVICE_HARDWARE_CONFIGURATION_ERROR);  // pin is not ADC-capable
        return;
    }

    altMic->setSampleRate(11000);  // CONFIG_AUDIO_DEFAULT_MICROPHONE_SAMPLERATE
    // gain 0-7; 0 = no additional gain (recommended for modules with onboard amp like MAX4466).
    // 7 matches built-in mic defaults -- may clip with amplified modules.
    altMic->setGain(gain, 0);  // second arg = reference voltage selection, 0 = default

    // Build a parallel pipeline.
    // Order follows MicroBitAudio constructor: create splitter -> attach stages -> enable.
    // CODAL uses cooperative scheduling -- no fiber yield here, so pointer swap is safe.
    StreamSplitter *altRawSplitter = new StreamSplitter(altMic->output);
    altRawSplitter->filterOn(&altMicActive);  // gates frames; set altMicActive=false to pause

    // StreamNormalizer(source, normFactor, activate, outputFormat, outputRate)
    float normFactorF = normFactor / 1000.0f;  // int promoted to float before division -- no truncation
    StreamNormalizer *altProcessor = new StreamNormalizer(
        *altRawSplitter->createChannel(),  // channel 0 -> processor
        normFactorF,
        true,
        DATASTREAM_FORMAT_8BIT_SIGNED,
        10);

    // Use generateDynamicID() to avoid CodalComponent registration collision with
    // the orphaned original splitter (which was also registered as DEVICE_ID_SPLITTER).
    // Callers use uBit.audio.splitter pointer directly -- device ID is irrelevant to them.
    StreamSplitter *altSplitter = new StreamSplitter(
        altProcessor->output);  // default: generateDynamicID()

    // DEVICE_ID_SYSTEM_LEVEL_DETECTOR must match the original so that pxt-microbit's
    // onSound()/soundLevel() event registrations pick up events from this instance.
    LevelDetectorSPL *altLevelSPL = new LevelDetectorSPL(
        *altRawSplitter->createChannel(),  // channel 1 -> levelSPL
        85.0, 65.0, 16.0, 35.0f,
        DEVICE_ID_SYSTEM_LEVEL_DETECTOR);

    // Disable original PDM mic: sets micEnabled=false, runmic=low, deactivates ADC channel.
    uBit.audio.deactivateMic();

    // Hot-swap all pipeline pointers, including mic so periodicCallback() tracks altMic
    // rather than the original PDM channel (prevents re-activation of built-in mic).
    uBit.audio.mic         = altMic;   // prevents periodicCallback() re-activation hazard
    uBit.audio.levelSPL    = altLevelSPL;
    uBit.audio.processor   = altProcessor;
    uBit.audio.splitter    = altSplitter;
    uBit.audio.rawSplitter = altRawSplitter;

    altMic->enable();
    // Note: micFilter (LowPassFilter*) declared in MicroBitAudio.h is NOT initialized
    // in MicroBitAudio's constructor (verified from CODAL source) -- not in live path.
}

//%
void setAnalogMicEnabled(bool enabled) {
    altMicActive = enabled;
}

} // namespace analogMic
#endif
