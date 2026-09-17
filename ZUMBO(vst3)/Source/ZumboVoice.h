#pragma once
#include <JuceHeader.h>
#include <random>
#include "ZumboSound.h"

class ZumboVoice : public juce::SynthesiserVoice
{
public:
    explicit ZumboVoice(juce::AudioProcessorValueTreeState& s) : state(s) {}

    bool canPlaySound(juce::SynthesiserSound* s) override { return dynamic_cast<ZumboSound*>(s) != nullptr; }

    void startNote(int note, float vel, juce::SynthesiserSound*, int) override
    {
        baseHz = juce::MidiMessage::getMidiNoteInHertz(note);
        velocity = vel;
        phase = {0.0, 0.0, 0.0};
        env = 0.0f;
        releasing = false;
        grainClock = 0.0f;
        rng.seed((unsigned)(note * 1009 + (int)getSampleRate() + voiceId++));
    }

    void stopNote(float, bool allowTailOff) override
    {
        releasing = true;
        if (!allowTailOff) clearCurrentNote();
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& b, const juce::MidiBuffer&, int start, int count) override
    {
        const double sr = getSampleRate();
        if (sr <= 0) return;
        const float attack = state.getRawParameterValue("attack")->load();
        const float decay = state.getRawParameterValue("decay")->load();
        const float sustain = state.getRawParameterValue("sustain")->load();
        const float release = state.getRawParameterValue("release")->load();
        const float cutoff = state.getRawParameterValue("cutoff")->load();
        const float res = state.getRawParameterValue("resonance")->load();
        const float drive = state.getRawParameterValue("drive")->load();
        const float driftAmt = state.getRawParameterValue("drift")->load();
        const float uniAmt = state.getRawParameterValue("unison")->load();
        const float sub = state.getRawParameterValue("sub")->load();
        const float noise = state.getRawParameterValue("noise")->load();
        const float mix1 = state.getRawParameterValue("mix1")->load();
        const float mix2 = state.getRawParameterValue("mix2")->load();
        const float mix3 = state.getRawParameterValue("mix3")->load();
        const int w1 = (int)state.getRawParameterValue("osc1")->load();
        const int w2 = (int)state.getRawParameterValue("osc2")->load();
        const int w3 = (int)state.getRawParameterValue("osc3")->load();
        const float gSize = state.getRawParameterValue("grainSize")->load();
        const float gDensity = state.getRawParameterValue("grainDensity")->load();
        const float gPitch = state.getRawParameterValue("grainPitch")->load();
        const float gSpray = state.getRawParameterValue("grainSpray")->load();
        const float gRandom = state.getRawParameterValue("grainRandom")->load();
        const float gMix = state.getRawParameterValue("grainMix")->load();

        std::uniform_real_distribution<float> r(-1.0f, 1.0f);
        auto osc = [](double ph, int w)
        {
            const double t = ph - std::floor(ph);
            switch(w)
            {
                case 0: return (float)(2.0*t - 1.0);                 // saw
                case 1: return t < 0.5 ? 1.0f : -1.0f;              // square
                case 2: return (float)(4.0*std::abs(t-0.5)-1.0);    // triangle
                case 3: return (float)std::sin(juce::MathConstants<double>::twoPi*t); // sine
                default: return 0.0f;
            }
        };

        for (int i=0;i<count;++i)
        {
            if (!releasing)
            {
                if (env < 1.0f) env += 1.0f / (std::max(0.001f, attack) * (float)sr);
                else if (env > sustain) env -= (1.0f-sustain)/(std::max(0.005f, decay)*(float)sr);
            }
            else
            {
                env -= 1.0f/(std::max(0.01f, release)*(float)sr);
                if (env <= 0.0f) { env=0.0f; clearCurrentNote(); }
            }

            drift += (r(rng)*0.000002f-drift)*0.0015f;
            const double d1 = baseHz*(1.0 + driftAmt*drift);
            const double d2 = baseHz*(1.0 + driftAmt*drift*0.7)*(1.0+0.0018*uniAmt);
            const double d3 = baseHz*(1.0 + driftAmt*drift*1.3)*(1.0-0.0018*uniAmt);

            phase[0]+=d1/sr; phase[1]+=d2/sr; phase[2]+=d3/sr;
            for(auto& x:phase) x-=std::floor(x);

            float x = 0.42f*mix1*osc(phase[0],w1)
                    + 0.30f*mix2*osc(phase[1],w2)
                    + 0.22f*mix3*osc(phase[2],w3);
            x += 0.18f*sub*osc(phase[0]*0.5,w1);
            x += noise*0.10f*r(rng);

            // Synthetic grain cloud: short randomized grain bursts around the oscillator signal.
            grainClock += gDensity * 0.0006f;
            if (grainClock >= 1.0f) { grainClock -= 1.0f; grainAge = 0.0f; grainSeed = r(rng); }
            grainAge += 1.0f/(std::max(0.002f,gSize)*sr*0.001f);
            float grain = 0.0f;
            if (grainAge < 1.0f)
            {
                const float envG = std::sin(juce::MathConstants<float>::pi * grainAge);
                const float pp = 1.0f + gPitch*0.5f + gSpray*grainSeed*0.5f;
                grain = envG * osc(phase[0]*pp + grainSeed*gRandom*0.15, w1);
            }
            x = x*(1.0f-gMix) + grain*gMix*0.45f;

            x *= velocity*env;
            const float sat = 1.0f + drive*7.0f;
            x = std::tanh(x*sat)/std::tanh(sat);

            // Stable one-pole low-pass; resonance adds a small feedback emphasis.
            const float a = juce::jlimit(0.001f,0.999f, 1.0f-std::exp(-2.0f*juce::MathConstants<float>::pi*cutoff/(float)sr));
            lp += a*(x-lp);
            const float y = lp*(1.0f+0.18f*res);

            b.addSample(0,start+i,y);
            if (b.getNumChannels()>1) b.addSample(1,start+i,y);
        }
    }

private:
    juce::AudioProcessorValueTreeState& state;
    double baseHz=440.0;
    double phase[3]{};
    float velocity=0, env=0, drift=0, lp=0, grainClock=0, grainAge=1, grainSeed=0;
    bool releasing=false;
    std::mt19937 rng{1234};
    inline static int voiceId=0;
};
