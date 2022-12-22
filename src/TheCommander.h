#ifndef COMMANDER_H
#define COMMANDER_H

#include "../include/PedalConfig.h"
#include "../lib/DaisyInputs/Knob.h"
#include "../lib/DaisyInputs/SimpleToggle.h"
#include "../lib/DaisyEffects/Reverb.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;

// Declare a DaisySeed object called hw
DaisySeed *hw;

// Declare the effects objects being used
Reverb reverb;

// Declare the toggle switches and parameters
SimpleToggle modTypeToggle;
SimpleToggle reverbToggle;
bool reverbOff = true;
bool phaserType = true;

/**
 * Audio callback to process each enabled effect
 */
void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size);

/**
 * Initializes the input controls
 */
void InitializeControls();

/**
 * Initializes the effects
 */
void InitializeEffects();

#endif