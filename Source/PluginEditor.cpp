/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PhasexAudioProcessorEditor::PhasexAudioProcessorEditor(PhasexAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.

	startTimerHz(30);//自动重绘
	//setBufferedToImage(true);//设置缓冲
	bufferImage = juce::Image(juce::Image::RGB, 256, 64, true);//设置缓冲区
	setSize(64 * 12, 120 + 64 * 1);

	S_APFN.setText("NumAPF");
	S_CTOF.setText("CTOF");
	S_FDBK.setText("FDBK");
	S_LFORATE.setText("FREQ");
	S_CTOFAMOUNT.setText("Amount");
	S_CTOFRATE.setText("CutRATE");
	S_DELAYN.setText("DelayN");
	S_DRY.setText("DRY");
	S_WET.setText("WET");
	S_THROUGH.setText("THROUGH");
	S_WIDTH.setText("WIDTH");
	S_EXPVAL.setText("EXPValue");

	S_APFN.ParamLink(audioProcessor.GetParams(), "apfn");
	S_CTOF.ParamLink(audioProcessor.GetParams(), "ctof");
	S_FDBK.ParamLink(audioProcessor.GetParams(), "fdbk");
	S_LFORATE.ParamLink(audioProcessor.GetParams(), "lforate");
	S_CTOFAMOUNT.ParamLink(audioProcessor.GetParams(), "ctofamount");
	S_CTOFRATE.ParamLink(audioProcessor.GetParams(), "ctofrate");
	S_DELAYN.ParamLink(audioProcessor.GetParams(), "delayn");
	S_DRY.ParamLink(audioProcessor.GetParams(), "dry");
	S_WET.ParamLink(audioProcessor.GetParams(), "wet");
	S_THROUGH.ParamLink(audioProcessor.GetParams(), "through");
	S_WIDTH.ParamLink(audioProcessor.GetParams(), "width");
	S_EXPVAL.ParamLink(audioProcessor.GetParams(), "expval");

	addAndMakeVisible(S_APFN);
	addAndMakeVisible(S_CTOF);
	addAndMakeVisible(S_FDBK);
	addAndMakeVisible(S_LFORATE);
	addAndMakeVisible(S_CTOFAMOUNT);
	addAndMakeVisible(S_CTOFRATE);
	addAndMakeVisible(S_DELAYN);
	addAndMakeVisible(S_DRY);
	addAndMakeVisible(S_WET);
	addAndMakeVisible(S_THROUGH);
	addAndMakeVisible(S_WIDTH);
	addAndMakeVisible(S_EXPVAL);
}

PhasexAudioProcessorEditor::~PhasexAudioProcessorEditor()
{
}

//==============================================================================
void PhasexAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colour(0x00, 0x00, 0x00));

	//g.setColour (juce::Colours::white);
	//g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

	/*
	g.setFont(juce::Font("FIXEDSYS", 16.0, 1));
	g.setColour(juce::Colour(0x77, 0x77, 0x77));
	g.drawText("Phasex V1.0 (L-MODEL) Hiirofox&Ristekv", 0, 120 + 64 * 1 - 16, 64 * 5, 16, 0);
	*/



	g.drawImage(bufferImage, 64 * 7, 48, 256, 64, 0, 0, 256, 64);
	g.setColour(juce::Colour(0xFF, 0xFF, 0xFF));
	g.drawLine(64 * 7, 48, 64 * 11, 48, 1.0);
	g.drawLine(64 * 7, 48, 64 * 7, 48 + 64, 1.0);
	g.drawLine(64 * 11, 48, 64 * 11, 48 + 64, 1.0);
	g.drawLine(64 * 7, 48 + 64, 64 * 11, 48 + 64, 1.0);
}

void PhasexAudioProcessorEditor::timerCallback()
{
	repaint();
	juce::Graphics bufferGraph(bufferImage);//缓冲区绘制工具对接
	bufferGraph.fillAll(juce::Colour(0, 0, 0));
	bufferGraph.setOpacity(0.75f);
	bufferGraph.setColour(juce::Colour(0x77, 0x77, 0xff));
	for (int j = 0, lstx = -1; j < audioProcessor.apfn; ++j)
	{
		int x = (0.5 + (audioProcessor.apfctofs[j] + audioProcessor.nowctof) * 0.5) * 64 * 4;
		if (lstx != x)
		{
			bufferGraph.drawLine(x, 0, x, 64, 0.5);
			lstx = x;
		}
	}
}

void PhasexAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	S_APFN.setPos(64 * 0 + 32 + 24, 32 + 24);
	S_CTOF.setPos(64 * 1 + 32 + 24, 32 + 24);
	S_FDBK.setPos(64 * 3 + 32 + 24, 32 + 24);
	S_LFORATE.setPos(64 * 2 + 32 + 24, 32 + 24);
	S_CTOFAMOUNT.setPos(64 * 1 + 32 + 24, 32 + 24 + 64 * 1);
	S_CTOFRATE.setPos(64 * 2 + 32 + 24, 32 + 24 + 64 * 1);
	S_DELAYN.setPos(64 * 0 + 32 + 24, 32 + 24 + 64 * 1);
	S_DRY.setPos(64 * 4 + 32 + 24, 32 + 24);
	S_WET.setPos(64 * 4 + 32 + 24, 32 + 24 + 64 * 1);
	S_THROUGH.setPos(64 * 3 + 32 + 24, 32 + 24 + 64 * 1);

	S_WIDTH.setPos(64 * 5 + 32 + 24, 32 + 24 + 64 * 0);
	S_EXPVAL.setPos(64 * 5 + 32 + 24, 32 + 24 + 64 * 1);
}
