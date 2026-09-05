# Depth-write / blending regression

Run `bash tests/run-state.sh` with a native C compiler (`cc`) and awk/sed.
No Amiga SDK, emulator or GPU is required.

The test extracts the actual `W3D_SetState` function and checks all eight
combinations of initial blending, initial depth-write and requested depth-write.
Changing `W3D_ZBUFFERUPDATE` must update the host depth mask without changing
host blending or the context's blending flag.

The fixed function passes. A negative control restores the two original
commented-out cases and must fail (the printed failures are intentional).
This is a host-side logic regression, not a full StormC build or GL driver test.
