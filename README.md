# OpenAudioDSP

Shared SHARC audio processing for OpenMixerProject consoles. 
Initially extracted from OpenX32 commit `fd94919c1b2fbed21c7ff8e51ca316abf37dcf5f` (DSP1 0.71).

## Layout
- `src/mixer`: channel processing, EQ, gate, compressor, buses, matrix, monitoring,
  and delay processing, originally DSP1.
- `src/effects`: effects rack, individual effects, RTA, buffers, and math helpers,
  originally DSP2.
- `targets/x32/dsp1` and `targets/x32/dsp2`: CCES projects for the X32/M32
  ADSP-21371s, including boot, PLL, SDRAM, SRU, SPORT, SPI/control protocol,
  interrupt handlers, and linker/startup files.
- `targets/x32/dps1-ng`: the original experimental debugger project, unchanged.
- `tools/TrainNeuralNetwork`: DeFeedback training and offline processing tools.

## Build for X32/M32
1. Install Analog Devices CrossCore Embedded Studio 2.12.1 with SHARC support and
   a valid license, as required by the original projects.
2. Import **Existing Projects into Workspace** from `targets/x32`, selecting
   `dsp1` and `dsp2`. Do **not** copy projects into the workspace: their relative
   `processing` links refer to this checkout's `src` directories.
3. Select **Release** and build both projects. Their processor remains ADSP-21371;
   boot format, SPI configuration, numerical options, and linker maps are retained.
4. Use `targets/x32/dsp1/Release/dsp1.ldr` and
   `targets/x32/dsp2/Release/dsp2.ldr` with the existing OpenX32 DSP loader.

After changing `system.svc`, check the custom SDRAM/linker layout against each
target's `src/Readme.txt`; CCES regeneration can overwrite it.
OpenX32's Linux build does not compile SHARC firmware. It still expects the same
`dsp1.ldr`/`dsp2.ldr` filenames in its firmware package.

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
