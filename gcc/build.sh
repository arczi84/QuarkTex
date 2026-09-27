#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")/.."
toolchain=${QUARKTEX_GCC:-/opt/amiga-gcc16-rc11/bin/m68k-amigaos}
math_flag=${QUARKTEX_MATH_FLAG:--ffast-math}
mkdir -p build-gcc
flags=(-std=gnu99 -fgnu89-inline -mcpu=68040 -mhard-float -mcrt=clib2
       -O2 "$math_flag" -fno-strict-aliasing -fno-builtin -fcommon -I gcc -include gcc/compat.h)
for src in Warp3D.library/*.c; do
    "$toolchain-gcc" "${flags[@]}" -c "$src" -o "build-gcc/$(basename "${src%.c}").o"
done
perl gcc/generate.pl bridge > build-gcc/gl_bridge.s
perl gcc/generate.pl symbols > build-gcc/gl_symbols.inc
perl gcc/generate.pl vectors > build-gcc/vectors.inc
for unit in library runtime gl_bridge; do
    "$toolchain-gcc" "${flags[@]}" -Ibuild-gcc -c "gcc/$unit.c" -o "build-gcc/gcc_$unit.o"
done
"$toolchain-gcc" -mcpu=68040 -mhard-float -c build-gcc/gl_bridge.s -o build-gcc/gl_stubs.o
"$toolchain-gcc" -nostdlib -nostartfiles -mcpu=68040 -mhard-float -mcrt=clib2 \
    -o build-gcc/Warp3D.library build-gcc/*.o -lgcc
