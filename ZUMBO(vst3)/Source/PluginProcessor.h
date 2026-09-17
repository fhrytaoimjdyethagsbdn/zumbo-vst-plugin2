#pragma once
#include <JuceHeader.h>
#include "ZumboVoice.h"

class ZumboAudioProcessor : public juce::AudioProcessor
{
public:
    ZumboAudioProcessor();
    const juce::String getName() const override { return "ZUMBO"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return (int)getFactoryPresets().size(); }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int,const juce::String&) override {}
    void prepareToPlay(double,int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void loadPreset(int index);

private:
    juce::Synthesiser synth;
    int currentPreset=99;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZumboAudioProcessor)
};
