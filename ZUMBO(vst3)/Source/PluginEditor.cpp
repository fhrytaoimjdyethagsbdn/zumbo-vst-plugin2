#include "PluginEditor.h"

ZumboAudioProcessorEditor::ZumboAudioProcessorEditor(ZumboAudioProcessor& p):AudioProcessorEditor(&p),processor(p)
{
    setSize(1450,850);
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected("SELECT PRESET");
    int n=1;
    for(auto& x:getFactoryPresets()) presetBox.addItem(x.name,n++);
    presetBox.setSelectedId(processor.getCurrentProgram()+1);
    presetBox.onChange=[this]{ processor.loadPreset(presetBox.getSelectedId()-1); repaint(); };

    const char* ids[]={"cutoff","resonance","drive","drift","unison","sub","noise","attack","decay","sustain","release","lfo1Rate","lfo1Depth","lfo2Rate","lfo2Depth","grainSize","grainDensity","grainPitch","grainSpray","grainRandom","grainMix"};
    const char* names[]={"CUTOFF","RES","DRIVE","DRIFT","UNISON","SUB","NOISE","A","D","S","R","LFO1 RATE","LFO1 DEPTH","LFO2 RATE","LFO2 DEPTH","GRAIN SIZE","DENSITY","GRAIN PITCH","SPRAY","RANDOM","GRAIND MIX"};
    for(int i=0;i<21;++i) addKnob(ids[i],names[i],50+(i%11)*125,320+(i/11)*220);
}

void ZumboAudioProcessorEditor::addKnob(const char* id,const char* label,int x,int y)
{
    auto s=std::make_unique<juce::Slider>();
    s->setName(label); s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s->setTextBoxStyle(juce::Slider::TextBoxBelow,false,80,18);
    s->setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xffe8a63c));
    s->setColour(juce::Slider::thumbColourId,juce::Colours::white);
    s->setBounds(x,y,105,145);
    addAndMakeVisible(*s);
    attach.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,id,*s));
    sliders.push_back(std::move(s));
}

void ZumboAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff07090d));
    g.setColour(juce::Colour(0xffe8a63c));
    g.setFont(48.0f); g.drawText("ZUMBO",40,25,330,60,juce::Justification::left);
    g.setFont(14.0f); g.setColour(juce::Colours::lightgrey);
    g.drawText("VST3 • WINDOWS x64 • PSYTRANCE SYNTH",42,82,450,25,juce::Justification::left);

    auto panel=[&](int x,int y,int w,int h,const char* title){
        g.setColour(juce::Colour(0xff121722)); g.fillRoundedRectangle((float)x,(float)y,(float)w,(float)h,16);
        g.setColour(juce::Colour(0xff2c3442)); g.drawRoundedRectangle((float)x,(float)y,(float)w,(float)h,16,1);
        g.setColour(juce::Colour(0xffe8a63c)); g.setFont(18.0f); g.drawText(title,x+18,y+12,w-36,25,juce::Justification::left);
    };
    panel(30,110,900,190,"OSCILLATOR CORE");
    panel(950,110,470,190,"GRAIND");
    panel(30,300,1390,500,"SYNTHESIS / MODULATION");

    g.setColour(juce::Colour(0xff303746));
    g.fillRoundedRectangle(55,160,850,100,12);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("OSC 1     OSC 2     OSC 3     •     SUB     NOISE     •     UNISON",75,180,790,30,juce::Justification::left);
    g.setFont(14.0f); g.setColour(juce::Colours::lightgrey);
    g.drawText("SAW  /  SQUARE-PULSE  /  TRIANGLE  /  SINE     |     ANALOG DRIFT",75,215,790,25,juce::Justification::left);

    g.setColour(juce::Colour(0xff303746)); g.fillRoundedRectangle(975,160,420,100,12);
    g.setColour(juce::Colour(0xffe8a63c)); g.setFont(18.0f);
    g.drawText("GRAIN FIELD",995,180,380,25,juce::Justification::left);
    for(int i=0;i<24;++i){
        float xx=990+(i*17)%390; float yy=215+(i*29)%35;
        g.fillEllipse(xx,yy,5,5);
    }

    g.setColour(juce::Colours::grey); g.setFont(12.0f);
    g.drawText("100 FACTORY PRESETS • PSY BASS • GOA • SCREECH • ACID • LEADS • PLUCKS • ATMOS • GRAIND • FX",45,785,1350,20,juce::Justification::centred);
}

void ZumboAudioProcessorEditor::resized()
{
    presetBox.setBounds(980,35,400,38);
}
