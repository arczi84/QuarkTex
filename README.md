# QuarkTex 0.54

QuarkTex is a 3D graphics hardware virtualization solution first released in
2003. It translates Warp3D API calls from an emulated AmigaOS into native
OpenGL calls on the Windows host.

Original author: **Robert Konrad**.
Author of the **0.54 update: Artur Jarosik**.

Released under the LGPL license; see [License.txt](License.txt).

## Changes in 0.54

This maintenance release is based on QuarkTex 0.53 and includes:

- **Window resizing fixes.** Synchronize the host drawable, OpenGL viewport,
  coordinate mapping and scissor coordinates when the Amiga window changes
  size or position. Preserve the caller's scissor rectangle across resizing.
- **Framebuffer readback.** Copy the host back buffer into the destination
  Amiga bitmap on `W3D_WaitIdle`, so rendered pixels are available in Amiga
  memory. Normal drawing and buffer swapping do not trigger this copy.
- **Independent depth-write and blending state.** Fix `W3D_ZBUFFERUPDATE`
  falling through to the blending cases. Updating the depth mask now leaves
  blending unchanged.

The existing Warp3D API and host bridge ABI are preserved.

## Validation

The host regression tests cover depth/blending state independence, framebuffer
readback and window resizing. Each suite includes negative controls that detect
the original bugs. These tests do not replace visual testing in WinUAE.

```sh
bash tests/run-state.sh
bash tests/run-readback.sh
bash tests/run-resize.sh
```
