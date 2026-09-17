#include "PluginProcessor.h"
#include "PluginEditor.h"

ZumboAudioProcessor::ZumboAudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),
  apvts(*this,nullptr,"PARAMETERS",createParameters())
{
    for(int i=0;i<32;++i) synth.addVoice(new ZumboVoice(apvts));
    synth.addSound(new ZumboSound());
    loadPreset(99);
}

juce::AudioProcessorValueTreeState::ParameterLayout ZumboAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    auto f=[&](const char* id,const char* n,float a,float b,float d){
        p.push_back(std::make_unique<juce::AudioParameterFloat>(id,n,juce::NormalisableRange<float>(a,b,0.001f),d));
    };
    auto i=[&](const char* id,const char* n,int a,int b,int d){
        p.push_back(std::make_unique<juce::AudioParameterInt>(id,n,a,b,d));
    };
    i("osc1","OSC 1",0,3,0); i("osc2","OSC 2",0,3,0); i("osc3","OSC 3",0,3,0);
    f("mix1","OSC1 Mix",0,1,.75f); f("mix2","OSC2 Mix",0,1,.25f); f("mix3","OSC3 Mix",0,1,.10f);
    f("sub","Sub",0,1,.25f); f("noise","Noise",0,1,0);
    f("unison","Unison",0,1,.10f); f("drift","Analog Drift",0,1,.20f);
    f("cutoff","Cutoff",40,18000,5000); f("resonance","Resonance",0,1,.2f); f("drive","Drive",0,1,.1f);
    f("attack","Attack",.001f,3,.005f); f("decay","Decay",.005f,5,.25f); f("sustain","Sustain",0,1,.75f); f("release","Release",.01f,6,.2f); f("glide","Glide",0,1,0);
    f("lfo1Rate","LFO1 Rate",0.05f,20,2); f("lfo1Depth","LFO1 Depth",0,1,.05f);
    f("lfo2Rate","LFO2 Rate",0.05f,20,4); f("lfo2Depth","LFO2 Depth",0,1,.02f);
    f("grainSize","Grain Size",.002f,1,.15f); f("grainDensity","Grain Density",0,100,25);
    f("grainPitch","Grain Pitch",-1,1,0); f("grainSpray","Grain Spray",0,1,.1f); f("grainRandom","Grain Random",0,1,.1f); f("grainMix","GRAIND Mix",0,1,0);
    return {p.begin(),p.end()};
}

void ZumboAudioProcessor::prepareToPlay(double sr,int)
{
    synth.setCurrentPlaybackSampleRate(sr);
}

bool ZumboAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    auto ch=l.getMainOutputChannelSet();
    return ch==juce::AudioChannelSet::mono() || ch==juce::AudioChannelSet::stereo();
}

void ZumboAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m)
{
    juce::ScopedNoDenormals noDenormals;
    b.clear();
    synth.renderNextBlock(b,m,0,b.getNumSamples());
}

void ZumboAudioProcessor::loadPreset(int index)
{
    auto& p=getFactoryPresets().at(juce::jlimit(0,(int)getFactoryPresets().size()-1,index));
    auto set=[&](const char* id,float v){ if(auto* x=apvts.getRawParameterValue(id)) x->store(v); };
    set("osc1",p.osc1); set("osc2",p.osc2); set("osc3",p.osc3);
    set("mix1",p.mix1); set("mix2",p.mix2); set("mix3",p.mix3);
    set("sub",p.sub); set("noise",p.noise); set("unison",p.unison); set("drift",p.drift);
    set("cutoff",p.cutoff); set("resonance",p.resonance); set("drive",p.drive);
    set("attack",p.attack); set("decay",p.decay); set("sustain",p.sustain); set("release",p.release); set("glide",p.glide);
    set("lfo1Rate",p.lfo1Rate); set("lfo1Depth",p.lfo1Depth); set("lfo2Rate",p.lfo2Rate); set("lfo2Depth",p.lfo2Depth);
    set("grainSize",p.grainSize); set("grainDensity",p.grainDensity); set("grainPitch",p.grainPitch);
    set("grainSpray",p.grainSpray); set("grainRandom",p.grainRandom); set("grainMix",p.grainMix);
    currentPreset=index;
}

void ZumboAudioProcessor::setCurrentProgram(int i){ loadPreset(i); }
const juce::String ZumboAudioProcessor::getProgramName(int i){ return getFactoryPresets().at(juce::jlimit(0,99,i)).name; }

void ZumboAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if(auto xml=apvts.copyState().createXml()) copyXmlToBinary(*xml,dest);
}

void ZumboAudioProcessor::setStateInformation(const void* data,int size)
{
    if(auto xml=getXmlFromBinary(data,size))
        if(xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new ZumboAudioProcessor(); }
