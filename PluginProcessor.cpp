/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
YoudiFilterOneAudioProcessor::YoudiFilterOneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Initialization of Sample Rate to a standard value.
    //Later, the sample rate will be detected from the audio buffer itself in PrepareToPlay()
    sampleRate = 44100;
}

YoudiFilterOneAudioProcessor::~YoudiFilterOneAudioProcessor()
{
}

//==============================================================================
const juce::String YoudiFilterOneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YoudiFilterOneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YoudiFilterOneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YoudiFilterOneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YoudiFilterOneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YoudiFilterOneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YoudiFilterOneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void YoudiFilterOneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String YoudiFilterOneAudioProcessor::getProgramName (int index)
{
    return {};
}

void YoudiFilterOneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void YoudiFilterOneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //ProcessSpec is declared and parameters are set to describe the context
    juce::dsp::ProcessSpec processSpec;

    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.sampleRate = sampleRate;
    processSpec.numChannels = getNumOutputChannels();


    filterProcessDuplicate.prepare(processSpec);
    
    //Storing the sample rate in a variable to be used outside this method in processBlock
    sampleRate = processSpec.sampleRate;
}

void YoudiFilterOneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool YoudiFilterOneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void YoudiFilterOneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.


    //Check if the filter is activated
    if (*valueTree.getRawParameterValue("ACTIVE")) {
        //Read Cutoff frequency
        float frequency = *valueTree.getRawParameterValue("CUTOFF");

        //Read which filter type is selected in RADIOLOW and RADIOHIGH parameters
        bool radioLowSelected = *valueTree.getRawParameterValue("RADIOLOW");
        bool radioHighSelected = *valueTree.getRawParameterValue("RADIOHIGH");

        //Set state of duplicate process
        //For Low-pass filter selection
        if (radioLowSelected && !radioHighSelected) {
            *filterProcessDuplicate.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, frequency);
        }
        //For High-pass filter selection
        else if (!radioLowSelected && radioHighSelected) {
            *filterProcessDuplicate.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, frequency);
        }

        //Process audio
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> processContext(block);
        filterProcessDuplicate.process(processContext);
    }    


    /*for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }*/
}

//==============================================================================
bool YoudiFilterOneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* YoudiFilterOneAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
    return new YoudiFilterOneAudioProcessorEditor (*this);
}

//==============================================================================
void YoudiFilterOneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void YoudiFilterOneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout YoudiFilterOneAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout paramLayout;

    //Add Filter Activation/Bypass Parameters
    paramLayout.add(std::make_unique<juce::AudioParameterBool>("ACTIVE", "ON/OFF", true));

    //Add Filter Parameters
    
    //Cutoff Frequency
    paramLayout.add(std::make_unique<juce::AudioParameterFloat>("CUTOFF","Cutoff Frequency(Hz)",20.0f,20000.0f,500.0f));
    

    //Radio group selection of filter type
    paramLayout.add(std::make_unique<juce::AudioParameterBool>("RADIOLOW", "Low-pass", true));
    paramLayout.add(std::make_unique<juce::AudioParameterBool>("RADIOHIGH", "High-pass", false));

    return paramLayout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YoudiFilterOneAudioProcessor();
}
