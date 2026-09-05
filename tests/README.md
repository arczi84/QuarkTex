# Window resize regression

Run `bash tests/run-resize.sh` with a C99 host compiler available as `cc`.
The test extracts the actual `sync_window` body and mocks the host window/GL
operations. It covers fixed-position growth and shrinkage, position-only moves,
unchanged and empty windows, fullscreen, coordinate mapping, and scissor Y.
A negative control restores the original position-only check and must fail.

The Amiga-side correction was cross-built with GCC16 RC7 in a local GCC port.
The user confirmed the combined QuarkTex and MiniGL corrections in WinUAE
with Jazz2, including a 640x480 to 800x600 windowed resolution change.
That local build also contains independent GCC/state/readback changes which
are not included here. The original StormC project was not rebuilt.

MiniGL separately needed its GLViewport Y origin to use the current inner
window height instead of the saved scissor height. That change is outside
this repository and is not part of this patch. QuarkTex's Windows DLL is
unchanged; the existing moveWindow ABI resizes the child window, while the
Amiga GL bridge updates its viewport and pixel-coordinate modelview.
