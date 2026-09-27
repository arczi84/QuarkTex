# Building QuarkTex 0.54

Build the Amiga Warp3D.library with Amiga GCC 16 RC11:

```sh
bash gcc/build.sh
```

Default compiler: `/opt/amiga-gcc16-rc11/bin/m68k-amigaos-gcc`.
Set `QUARKTEX_GCC` to another toolchain prefix, without `-gcc`.
Output: `build-gcc/Warp3D.library`. Target: 68040, hardware FPU, O2, fast-math.

The GCC support supplies resident-library startup, Exec-backed allocation,
Warp3D vectors and assembly wrappers for the existing Windows host bridge.
The unused duplicate `envs` definition in V4Array.c is removed for GCC linking.
The Windows host components must already be installed.

Host regression tests pass; the release binary has not yet been runtime-tested
in WinUAE. Original author: Robert Konrad. Update author: Artur Jarosik.
