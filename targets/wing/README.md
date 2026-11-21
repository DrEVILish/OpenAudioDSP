# WING Compact T55 SHARC bring-up

Four ADSP-21489 targets share a SPORT/DMA driver. Assigned slots bypass
bit-exactly; unused slots on active TX lanes are zero. Unused DAI outputs are
high-Z. No X32 firmware changes, effects or runtime commands are required.

## Paired audio link

Use the byte-identical [stock-direction TDM16 board profile](dsp_board_profile.json),
SHA-256 `3b29934cdc1c704de69b6b028bc71387ab7c50cb74e811fb35ca0f64664a3e9d`.
Stock supplied pin directions, not the logical slot map. The FPGA is clock
master: 48 kHz, 16 × 32-bit words per lane, 24.576 MHz BCLK, MSB first, signed
Q4.28. RX and TX clocks and FS must be phase-aligned; active-high FS spans
one BCLK, one BCLK before slot 0's MSB. RX uses DAI08/14; TX uses DAI04/18.
Linear slot = `16 × lane + slot`; SPORT lanes are ordered A before B.

| Core | Assigned slots |
| --- | --- |
| DSP1 / IC20 | 0–55 inputs, 56–63 FX returns |
| DSP2 / IC31 | 0–31 buses, 32–47 matrices, 48–49 mains |
| DSP3 / IC32 | 0–23 inputs, 24–31 FX returns |
| DSP4 / IC33 | 0–15 eight stereo FX pairs, L/R at `2p`/`2p+1` |

## Build and safety

```sh
sh targets/wing/test-host.sh
python3 targets/wing/build.py --out <new-staging-directory>
```

Use CCES 2.12.1. The default gateway uses `elfloader-21489`; native CCES uses
`--cces-bin` and `elfloader`. The loader flags are `-bSPIHOST -fBINARY
-HostWidth 8` with `489_spi.dxe`. The build stages the four `.ldr`s, SHA256SUMS,
the board profile, and source/toolchain provenance. `--dxe-only` does not
generate loaders. `--sim` runs numerical block vectors on the ADSP-21371
simulator, not a 21489 peripheral model.

**Offline only: keep FS and return-valid disabled.** A loader digest identifies
bytes, not readiness. The firmware does not program PLL or SDRAM. At 24.576 MHz
BCLK, slave TX requires nominal CCLK ≥393.216 MHz (ADSP-2148x Rev. H, pp. 23/36).
Qualify fitted speed grade, CLKIN tolerances, PLL, CKRE=0 and complete board
timing; do not use the older TDM32 profile. DSP3 reset also remains unqualified.
Once qualified, measure every lane and verify the intended two-block latency
and DMA deadline on a muted setup before trusting audio. `wingSportArmed` only
indicates local configuration; it is not frame sync or a return-valid signal.
