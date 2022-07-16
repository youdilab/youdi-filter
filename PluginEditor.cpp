/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Declaration of colour constants used throughout the GUI
const juce::Colour colourDarkGreen = juce::Colour::fromFloatRGBA(0.023f, 0.274f, 0.207f, 1.0f);
const juce::Colour colourLightGreen = juce::Colour::fromFloatRGBA(0.317f, 0.572f, 0.349f, 1.0f);
const juce::Colour colourLightOrange = juce::Colour::fromFloatRGBA(0.941f, 0.733f, 0.384f, 1.0f);
const juce::Colour colourLightYellow = juce::Colour::fromFloatRGBA(0.956f, 0.933f, 0.662f, 1.0f);
const juce::Colour colourTransparent = juce::Colour::fromFloatRGBA(0.023f, 0.274f, 0.207f, 0.0f);
const juce::Colour colourDarkGrey = juce::Colour::fromFloatRGBA(0.003f, 0.094f, 0.070f, 1.0f);

const int filterTypeRadioGroup = 1;

//==============================================================================
//Constructor includes initializations of attachments between the value tree and GUI controls
YoudiFilterOneAudioProcessorEditor::YoudiFilterOneAudioProcessorEditor (YoudiFilterOneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    dialFrequencyAttachment(audioProcessor.valueTree,"CUTOFF",dialFrequency),
    toggleOnOffAttachment(audioProcessor.valueTree, "ACTIVE", toggleOnOff),
    radioFilterLowAttachment(audioProcessor.valueTree, "RADIOLOW", radioFilterLow),
    radioFilterHighAttachment(audioProcessor.valueTree, "RADIOHIGH", radioFilterHigh),
    radioFilterLow("Low","Switch to Low Pass Filter"),
    radioFilterHigh("High","Switch to High Pass Filter")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    //Define properties and initial parameters of GUI controls
    
    dialFrequency.setTextValueSuffix(" Hz");
    dialFrequency.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dialFrequency.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    
    radioFilterLow.setRadioGroupId(filterTypeRadioGroup);
    radioFilterHigh.setRadioGroupId(filterTypeRadioGroup);
    
    radioFilterLow.setClickingTogglesState(true);
    radioFilterHigh.setClickingTogglesState(true);
    
    radioFilterLow.setConnectedEdges(juce::Button::ConnectedOnRight);
    radioFilterHigh.setConnectedEdges(juce::Button::ConnectedOnLeft);

    radioFilterLow.setToggleState(true,juce::NotificationType::dontSendNotification);
    radioFilterHigh.setToggleState(false, juce::NotificationType::dontSendNotification);

    toggleOnOff.addListener(this);

    addAndMakeVisible(toggleOnOff);
    addAndMakeVisible(dialFrequency);
    addAndMakeVisible(radioFilterLow);
    addAndMakeVisible(radioFilterHigh);

    setSize(250, 200);

    //Set colour of "ACTIVE" toggle button text based on state
    if (toggleOnOff.getToggleState()) {
        toggleOnOff.setColour(juce::ToggleButton::textColourId, colourLightOrange);
    }
    else {
        toggleOnOff.setColour(juce::ToggleButton::textColourId, colourDarkGrey);
    }

    toggleOnOff.setColour(juce::ToggleButton::tickColourId, colourLightOrange);
    toggleOnOff.setColour(juce::ToggleButton::tickDisabledColourId, colourLightGreen);

    toggleOnOff.setToggleState(true, juce::NotificationType::sendNotification);
    toggleOnOff.setButtonText("Filter ON");

    dialFrequency.setColour(juce::Slider::thumbColourId, colourLightOrange);
    dialFrequency.setColour(juce::Slider::textBoxOutlineColourId, colourTransparent);
    dialFrequency.setColour(juce::Slider::textBoxTextColourId, colourLightOrange);

    radioFilterLow.setColour(juce::TextButton::buttonColourId, colourDarkGrey);
    radioFilterLow.setColour(juce::TextButton::buttonOnColourId, colourLightOrange);
    radioFilterLow.setColour(juce::TextButton::textColourOffId, colourDarkGreen);
    radioFilterLow.setColour(juce::TextButton::textColourOnId, colourLightYellow);

    radioFilterHigh.setColour(juce::TextButton::buttonColourId, colourDarkGrey);
    radioFilterHigh.setColour(juce::TextButton::buttonOnColourId, colourLightOrange);
    radioFilterHigh.setColour(juce::TextButton::textColourOffId, colourDarkGreen);
    radioFilterHigh.setColour(juce::TextButton::textColourOnId, colourLightYellow);   
}

YoudiFilterOneAudioProcessorEditor::~YoudiFilterOneAudioProcessorEditor()
{
}

//==============================================================================
void YoudiFilterOneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(colourDarkGreen);
    g.setColour(colourLightGreen);
    g.setFont (15.0f);
}

void YoudiFilterOneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //Evaluate total area
    auto total_width = getWidth();
    auto total_height = getHeight();
    auto total_area = getLocalBounds();

    //Define constants
    auto border = 0.04*total_height;
    auto buttonAreaHeight = 0.20*total_height;

    //Allocate recangular areas for different GUI controls
    auto buttonArea = total_area.removeFromTop(buttonAreaHeight);
    auto buttonAreaLeft = buttonArea.removeFromLeft(total_width/2);
    auto& buttonAreaRight = buttonArea.reduced(border);
    auto& lowFilterArea = buttonAreaRight.removeFromLeft((total_width / 4) -border).withTrimmedLeft(border);
    auto& highFilterArea = buttonAreaRight.withTrimmedRight(border);
    auto& onOffButtonArea = buttonAreaLeft.reduced(border);
    auto& dialFrequencyArea = total_area.reduced(border);

    //Place GUI controls in the respective area within the window
    toggleOnOff.setBounds(onOffButtonArea);   
    dialFrequency.setBounds(dialFrequencyArea);
    radioFilterLow.setBounds(lowFilterArea);
    radioFilterHigh.setBounds(highFilterArea);
}

void YoudiFilterOneAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //Handle clicks on 'ACTIVE' button
    //Button appearance will change based on its status
    if (button == &toggleOnOff) {
        if (toggleOnOff.getToggleState()) {
            toggleOnOff.setColour(juce::ToggleButton::textColourId, colourLightOrange);
        }
        else {
            toggleOnOff.setColour(juce::ToggleButton::textColourId, colourDarkGrey);
        }
    }
}
