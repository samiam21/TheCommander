#include "TheCommander.h"

/**
 * Audio callback to process each enabled effect
 */
void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        float wet = in[i];

        // Process the mod effect audio

        // Add the chorus
        wet = chorus.Process(wet);

        // Add the reverb
        if (!reverbOff)
        {
            wet = reverb.Process(wet);
        }

        // Balance the output prior to applying the level control
        wet = balancer.Process(wet, in[i]);

        // Output the processed audio
        out[i] = wet * outputLevel;
    }
}

void InitializeControls()
{
    // Initialize the ADC
    AdcChannelConfig adcConfig[MAX_KNOBS];
    adcConfig[KNOB_1_CHN].InitSingle(hw->GetPin(effectPotPin1));
    adcConfig[KNOB_2_CHN].InitSingle(hw->GetPin(effectPotPin2));
    adcConfig[KNOB_3_CHN].InitSingle(hw->GetPin(effectPotPin3));
    adcConfig[KNOB_4_CHN].InitSingle(hw->GetPin(effectPotPin4));
    adcConfig[KNOB_5_CHN].InitSingle(hw->GetPin(effectPotPin5));
    adcConfig[KNOB_6_CHN].InitSingle(hw->GetPin(effectPotPin6));
    hw->adc.Init(adcConfig, MAX_KNOBS);
    hw->adc.Start();

    // TODO: Find a better way to do this?
    // Give the ADC time to start up
    System::Delay(500);

    // Initialize the output level knob
    outputLevelKnob.Init(hw, KNOB_3_CHN, outputLevel, outputLevelMin, outputLevelMax);

    // Initialize the effect knobs
    reverb.ConfigureKnobPositions(-1, KNOB_6_CHN, -1);
    chorus.ConfigureKnobPositions(KNOB_5_CHN, KNOB_1_CHN, KNOB_2_CHN, -1);

    // Initialize the toggles
    reverbToggle.Init(hw->GetPin(effectTogglePin3));
    modTypeToggle.Init(hw->GetPin(effectTogglePin2));
}

void InitializeEffects()
{
    // Initialize the balancer
    balancer.Init(hw->AudioSampleRate());

    // Initialize the effects
    reverb.Setup(hw);
    chorus.Setup(hw);
}

int main(void)
{
    // Initialize the hardware
    hw->Init();

    // Update the block size and sample rate to minimize noise
    hw->SetAudioBlockSize(DAISY_BLOCKSIZE);
    hw->SetAudioSampleRate(DAISY_SAMPLE_RATE);

    // Initialize the input controls
    InitializeControls();

    // Initialize the effect objects
    InitializeEffects();

    // Start the audio processing
    debugPrintln(hw, "Starting Audio");
    hw->StartAudio(AudioCallback);

    // Turn on the onboard LED
    hw->SetLed(true);

    while (1)
    {
        // Check the reverb toggle
        reverbOff = reverbToggle.ReadToggle();

        // Check the level knob
        if (outputLevelKnob.SetNewValue(outputLevel))
        {
            debugPrintlnF(hw, "Updated the output level to: %f", outputLevel);
        }

        // Run the effect loop functions
        reverb.Loop(true);
        chorus.Loop(true);
    }
}
