/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "math.h"
#include "HiirofoxDSP/HiirofoxDSP.h"
#include "SynthData.h"

//==============================================================================
PhasexAudioProcessor::PhasexAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{/*
	pData = new PhasexData;
	InitPhasexPhaser(&pData->phsl);
	InitPhasexPhaser(&pData->phsr);
	*/
}

juce::AudioProcessorValueTreeState::ParameterLayout PhasexAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("apfn", "NumAPF", 0.0, 0.99999999, 0.1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("ctof", "Cutoff", -1.0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fdbk", "Feedback", -1.0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("lforate", "LFORate", -1.0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("ctofamount", "Amount", 0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("ctofrate", "RATE", 0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("delayn", "DelayTime", 0, 0.999999999, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("dry", "DRY", 0, 1.0, 1.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("wet", "WET", 0, 1.0, 1.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("through", "THROUGH", 0, 1.0, 1.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("width", "WIDTH", 0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("expval", "EXPVAL", -1.0, 1.0, 0.0));
	return layout;
}

PhasexAudioProcessor::~PhasexAudioProcessor()
{
	free(SynthData);
	SynthData = NULL;
}

//==============================================================================
const juce::String PhasexAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool PhasexAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool PhasexAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool PhasexAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double PhasexAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int PhasexAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int PhasexAudioProcessor::getCurrentProgram()
{
	return 0;
}

void PhasexAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PhasexAudioProcessor::getProgramName(int index)
{
	return "BarberpolePhaser";
}

void PhasexAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void PhasexAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
}

void PhasexAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhasexAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void PhasexAudioProcessor::UpdataParam()
{
	PhasexData* pData = (PhasexData*)SynthData;

	SampleRate = getSampleRate();
	apfn = *Params.getRawParameterValue("apfn");
	ctof = *Params.getRawParameterValue("ctof");
	fdbk = *Params.getRawParameterValue("fdbk");
	lforate = *Params.getRawParameterValue("lforate");
	ctofamount = *Params.getRawParameterValue("ctofamount");
	ctofrate = *Params.getRawParameterValue("ctofrate");
	delayn = *Params.getRawParameterValue("delayn");
	dry = *Params.getRawParameterValue("dry");
	wet = *Params.getRawParameterValue("wet");
	through = *Params.getRawParameterValue("through");
	width = *Params.getRawParameterValue("width");
	expval = *Params.getRawParameterValue("expval");

	apfn *= apfn * (MaxPhaserAPFN - 1);
	delayn *= delayn * (MaxNewAPF1DelayLen - 1);

	ctof = sqrt(fabs(ctof)) * (ctof > 0 ? 1 : -1);
	fdbk *= 1.01;
	ctofrate = ctofrate * ctofrate * ctofrate * ctofrate * ctofrate * 2.0 * M_PI * 8000.0 / SampleRate;
	inv = lforate < 0 ? -1.0 : 1.0;
	lforate = fabs(lforate);
	lforate = lforate * lforate * lforate * lforate * lforate * 2.0 * M_PI * 8000.0 / SampleRate;
	if (expval > 0)
	{
		expval = expval * expval * 2.0 + 1.0;
	}
	else
	{
		expval = 1.0 / (expval * expval + 1.0);
	}

	for (int j = 0; j < apfn; ++j)
	{
		apfctofs[j] = (pow((float)j / apfn, expval) * 2.0 - 1.0) * width;
	}
}

void PhasexAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	PhasexData* pData = (PhasexData*)SynthData;
	if (SynthData == NULL)//init
	{
		SynthData = (void*)malloc(sizeof(PhasexData));
		pData = (PhasexData*)SynthData;
		memset(pData, 0, sizeof(PhasexData));
		//InitPhasexPhaser(&pData->phsl);
		//InitPhasexPhaser(&pData->phsr);
	}
	auto numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	UpdataParam();//更新参数

	for (int i = 0; i < numSamples; ++i)
	{
		float datl = recbufl[i];
		float datr = recbufr[i];
		float dat2l = datl;
		float dat2r = datr;

		//float datl = (float)(rand() % 10000) / 100000.0 * (rand() % 2 ? 1 : -1);
		//float datr = (float)(rand() % 10000) / 100000.0 * (rand() % 2 ? 1 : -1);
		float thrl = pData->fdbkl * fdbk;
		float thrr = pData->fdbkr * fdbk;

		datl = datl + thrl;
		datr = datr + thrr;

		nowctof = ctof2 + ctofamount * cos(pData->ctoft);
		//if (nowctof > 1.0)nowctof = 1.0;
		//if (nowctof < -1.0)nowctof = -1.0;
		if (nowctof + width > 1.0)nowctof = 1.0 - width;
		if (nowctof - width < -1.0)nowctof = width - 1.0;

		pData->ctoft += ctofrate;
		//float tmp2l = PhasexPhaser(&pData->phsl, datl, nowctof, apfn, delayn + 1);
		//float tmp2r = PhasexPhaser(&pData->phsr, datr, nowctof, apfn, delayn + 1);
		float tmp2l = PhasexPhaserMultiCtof(&pData->phsl, datl, apfctofs, nowctof, apfn, delayn + 1);
		float tmp2r = PhasexPhaserMultiCtof(&pData->phsr, datr, apfctofs, nowctof, apfn, delayn + 1);
		float rel = HilbertTransRe(&pData->hilbl, tmp2l);
		float iml = HilbertTransIm(&pData->hilbl, tmp2l);
		float rer = HilbertTransRe(&pData->hilbr, tmp2r);
		float imr = HilbertTransIm(&pData->hilbr, tmp2r);
		float sinx = sin(pData->lfot), cosx = cos(pData->lfot);
		float tmp1l = rel * cosx - iml * sinx;
		float tmp1r = rer * cosx - imr * sinx;
		pData->lfot += lforate2 * inv;

		if (pData->lfot >= 2.0 * M_PI) pData->lfot -= 2.0 * M_PI;
		else if (pData->lfot < 0) pData->lfot += 2.0 * M_PI;

		lforate2 += 0.001 * (lforate - lforate2);//平滑参数
		ctof2 += 0.005 * (ctof - ctof2);//平滑ctof的直流

		float outl = 0;
		float outr = 0;
		outl = tmp1l;
		outr = tmp1r;
		//outl = outl - LPF1(&pData->hpf1l, outl, 0.005, 0);
		//outr = outr - LPF1(&pData->hpf1r, outr, 0.005, 0);
		pData->fdbkl = outl;
		pData->fdbkr = outr;
		pData->fdbkl = pData->fdbkl >= LimV ? ((pData->fdbkl - LimV) * LimK + LimV) : pData->fdbkl;
		pData->fdbkl = pData->fdbkl < -LimV ? ((pData->fdbkl + LimV) * LimK - LimV) : pData->fdbkl;
		pData->fdbkr = pData->fdbkr >= LimV ? ((pData->fdbkr - LimV) * LimK + LimV) : pData->fdbkr;
		pData->fdbkr = pData->fdbkr < -LimV ? ((pData->fdbkr + LimV) * LimK - LimV) : pData->fdbkr;

		wavbufl[i] = outl * wet + dat2l * dry + thrl * through;
		wavbufr[i] = outr * wet + dat2r * dry + thrr * through;
	}
}

//==============================================================================
bool PhasexAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhasexAudioProcessor::createEditor()
{
	return new PhasexAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//自动绘制(调试)
}

//==============================================================================

void PhasexAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	auto state = Params.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PhasexAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(Params.state.getType()))
		{
			Params.replaceState(juce::ValueTree::fromXml(*xmlState));
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PhasexAudioProcessor();
}
