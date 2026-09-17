cmake_minimum_required(VERSION 3.22)
project(ZUMBO VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(ZUMBO
    COMPANY_NAME "ZUMBO Audio"
    COMPANY_WEBSITE "https://example.invalid/zumbo"
    BUNDLE_ID "com.zumbo.audio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Zmbo
    PLUGIN_CODE Zmbo
    FORMATS VST3
    VST3_CATEGORIES Instrument Synth
    PRODUCT_NAME "ZUMBO"
)

target_sources(ZUMBO PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/ZumboVoice.h
    Source/ZumboSound.h
    Source/FactoryPresets.h
)

target_compile_definitions(ZUMBO PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(ZUMBO PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
)

if(MSVC)
    target_compile_options(ZUMBO PRIVATE /O2 /fp:fast)
endif()
