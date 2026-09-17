# ZUMBO VST3 v1.0 — FL Studio 12 / Windows

This is a Windows-only VST3 instrument source project.

## Important for FL Studio 12

FL Studio 12 supports VST3 instruments. For a VST3 plugin on Windows, install the built
`ZUMBO.vst3` bundle into:

C:\Program Files\Common Files\VST3\

Do not put a VST3 bundle into the VST2 `.dll` folder.

Then in FL Studio:
1. Open Plugin Manager / Manage plugins.
2. Enable Verify plugins.
3. Scan.
4. Search for ZUMBO under Generators / VST3.

If FL Studio 12 is 32-bit, a 64-bit-only build cannot load. Build the Win32 configuration
with a compatible 32-bit Visual Studio toolchain, or use 64-bit FL Studio 12 with a Win64 build.

## Build

Requirements:
- Windows
- Visual Studio with C++ desktop development
- CMake 3.22+
- Internet on first configure to fetch JUCE 8.0.8

For x64:
  cmake -B build64 -A x64
  cmake --build build64 --config Release

For 32-bit (only if your Visual Studio/JUCE toolchain supports Win32):
  cmake -B build32 -A Win32
  cmake --build build32 --config Release

The project is VST3-only.

## What changed in v2

- 100 actual factory presets, parameterized in C++
- Preset browser in the plugin GUI
- 3 oscillator waveform selection
- Analog drift
- Unison
- Sub/noise
- ADSR
- LFO parameter foundation
- GRAIND synthetic granular texture controls
- Psytrance-oriented bass/lead/screech/acid/pluck/pad/FX bank
- Explicit VST3 instrument category
- Windows x64 target plus optional Win32 configuration

This is still a source/build project. The `.vst3` binary must be compiled on Windows.
