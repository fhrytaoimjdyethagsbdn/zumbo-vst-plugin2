#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ZumboAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ZumboAudioProcessorEditor(ZumboAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    ZumboAudioProcessor& processor;
    juce::ComboBox presetBox;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attach;
    std::unique_ptr<juce::ComboBoxAttachment> presetAttachmentDummy;
    void addKnob(const char* id,const char* label,int x,int y);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZumboAudioProcessorEditor)
};
