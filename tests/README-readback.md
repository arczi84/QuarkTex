# Destination bitmap readback regression

Run `bash tests/run-readback.sh` with a native C compiler (`cc`) and awk/sed.
No Amiga SDK, MiniGL checkout, emulator or GPU is required.

The test extracts the real `sync_bitmap`, `W3D_WaitIdle` and
`W3D_SetDrawRegion` functions. Mock host GL and Picasso96 operations verify:

- Completion precedes readback; guest pointers receive the host translation.
- RGBA bytes, including alpha, are preserved and rows become top-down.
- Destination changes and vertical offsets are honored without writing guard rows.
- Pixel-pack alignment, row length and skips are set and restored.
- Invalid geometry, missing bitmaps and failed allocations do not copy pixels.
- Fullscreen buffer swapping in `W3D_SetDrawRegion` is preserved.

Two negative controls remove synchronization or destination tracking. Both must
fail; their assertion messages are expected. Temporary test binaries are retained.

These tests do not exercise context-creation tags, the real host GL bridge,
Picasso96 clipping/conversion or the original StormC toolchain. The readback
path was tested as part of a combined local GCC/MiniGL build in WinUAE with
Jazz2; that is not equivalent to testing this isolated PR with StormC.
