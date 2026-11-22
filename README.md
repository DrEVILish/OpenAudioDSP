# OpenAudioDSP

Shared SHARC audio processing for OpenMixerProject

## Bugs and Ideas

Please report Bugs or submit Ideas in our central [Ticketsystem](https://github.com/OpenMixerProject/Ticketsystem/issues)

## Layout
- `src/mixer`: channel processing, monitoring and delay processing (originally DSP1)
- `src/effects`: effects rack, individual effects, RTA (originally DSP2)
- `targets/x32/dsp1` and `targets/x32/dsp2`: CCES projects for the X32/M32 ADSP-21371

## Build for X32/M32
1. Install Analog Devices CrossCore Embedded Studio 2.12.1
2. Select `File` -> `Open Projects from File System ...`, open from `targets/x32` selecting `dsp1` and `dsp2`
   
   <img width="343" height="179" alt="image" src="https://github.com/user-attachments/assets/3f865e4f-5aa5-4bf5-aee6-7e193b846e35" />

3. Select **Release** and build both projects
4. Use `targets/x32/dsp1/Release/dsp1.ldr` and `targets/x32/dsp2/Release/dsp2.ldr`

## Target interface and WING port

Each target supplies its own `defines.h` and `version.h` through the include path.
The X32 profiles retain the existing graph, feature flags, channel indices, block
size, memory addresses, and SPI packet sizes. They are not universal defaults.

`src/mixer/audio_io.h` and `src/effects/audio_io.h` describe the block transport:

- The target owns the interleaved RX/TX words and SPORT DMA descriptors. Call
  `audioIoInit()` before `audioInit()` and before enabling audio interrupts.
- Preserve buffer placement/alignment and the completed-block handoff. The mixer
  selects the completed half via `bufferSwitch`; the effects rack advances
  `audioBufferOffset` by one block after processing.
- FPGA-facing X32 words are signed PCM; inter-DSP words contain **float bit
  patterns**, not PCM. Preserve that distinction when connecting another FPGA.
- The mixer calls `audioIoPublishMeters()` once per processed block. The X32
  implementation retains the original SPI meter layout and tap selection.
- The effects rack calls `audioIoBeginBlock()` after deinterleaving. The X32 uses
  it to reset its S/PDIF side-channel cursors; a target without those side channels
  can implement an empty function.
