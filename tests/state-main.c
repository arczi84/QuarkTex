/* This test runs the actual W3D_SetState body extracted from Context.c. */
int main(void)
{
    int blend, initial_depth, target_depth, failures = 0, cases = 0;
    for (blend = 0; blend <= 1; ++blend) {
        for (initial_depth = 0; initial_depth <= 1; ++initial_depth) {
            for (target_depth = 0; target_depth <= 1; ++target_depth) {
                W3D_Context context = { initial_depth ? W3D_ZBUFFERUPDATE : 0 };
                host_depth_write = initial_depth;
                W3D_SetState(&context, W3D_BLENDING, blend ? W3D_ENABLE : W3D_DISABLE);
                blend_calls = 0;
                ++cases;
                if (W3D_SetState(&context, W3D_ZBUFFERUPDATE,
                        target_depth ? W3D_ENABLE : W3D_DISABLE) != W3D_SUCCESS ||
                    host_blend != blend || blend_calls != 0 ||
                    host_depth_write != target_depth ||
                    !!(context.state & W3D_BLENDING) != blend ||
                    !!(context.state & W3D_ZBUFFERUPDATE) != target_depth) {
                    ++failures;
                    fprintf(stderr, "FAIL blend=%d depth=%d->%d: host blend=%d depth=%d blend calls=%d\n",
                        blend, initial_depth, target_depth, host_blend, host_depth_write, blend_calls);
                }
            }
        }
    }
    printf("W3D_SetState: %d cases, %d failures\n", cases, failures);
    return failures ? 1 : 0;
}
