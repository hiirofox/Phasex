/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LM_slider.h"

//==============================================================================
/**
*/
class PhasexAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
	PhasexAudioProcessorEditor(PhasexAudioProcessor&);
	~PhasexAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	PhasexAudioProcessor& audioProcessor;
	
	juce::Image bufferImage;//»æÖÆµÄ»º³åÇø

	LMKnob S_APFN, S_CTOF, S_FDBK, S_LFORATE;
	LMKnob S_CTOFAMOUNT, S_CTOFRATE;
	LMKnob S_DELAYN;
	LMKnob S_DRY, S_WET, S_THROUGH;

	LMKnob S_WIDTH, S_EXPVAL;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhasexAudioProcessorEditor)
};
