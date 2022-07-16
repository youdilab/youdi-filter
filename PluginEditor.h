/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class YoudiFilterOneAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public juce::Button::Listener
{
public:
    YoudiFilterOneAudioProcessorEditor (YoudiFilterOneAudioProcessor&);
    ~YoudiFilterOneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //Override button click
    void buttonClicked(juce::Button*) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    YoudiFilterOneAudioProcessor& audioProcessor;
    
    //GUI Controls
    juce::Slider dialFrequency;
    juce::ComboBox selectFilterType;
    juce::ToggleButton toggleOnOff;
    juce::TextButton radioFilterLow;
    juce::TextButton radioFilterHigh;    
    
    //Connections between GUI controls and ValueTree
    juce::AudioProcessorValueTreeState::SliderAttachment dialFrequencyAttachment;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment selectFilterTypeAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment toggleOnOffAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment radioFilterLowAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment radioFilterHighAttachment;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YoudiFilterOneAudioProcessorEditor)
};
