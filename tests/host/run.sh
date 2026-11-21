#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$(mktemp -d "${TMPDIR:-/tmp}/openaudiodsp-host.XXXXXX")
trap 'rm -rf "$build"' EXIT
trap 'exit 1' HUP INT TERM

for kernel in fader gate compressor; do
    # CFLAGS intentionally splits into compiler arguments.
    "${CC:-cc}" -std=c99 -pedantic-errors -Wall -Wextra -Werror -Wno-unknown-pragmas \
        ${CFLAGS:--O2} -ffp-contract=off -UNDEBUG -I"$root/include" \
        "$root/src/mixer/$kernel.c" "$root/tests/host/test_$kernel.c" \
        -lm -o "$build/test_$kernel"
    "$build/test_$kernel"
done
