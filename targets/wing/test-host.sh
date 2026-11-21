#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu

wing=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
build=$(mktemp -d "${TMPDIR:-/tmp}/openaudiodsp-wing.XXXXXX")
trap 'rm -rf "$build"' EXIT

for dsp in 1 2 3 4; do
    "${CXX:-c++}" -O2 -DWING_DSP="$dsp" "$wing/audio.cpp" "$wing/audio_test.cpp" -o "$build/dsp$dsp"
    "$build/dsp$dsp"
    printf 'DSP%s WING block vectors passed\n' "$dsp"
done
