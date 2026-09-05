#!/usr/bin/env bash
set -euo pipefail
ulimit -c 0
cd -- "$(dirname -- "$0")/.."
test_dir=$(mktemp -d /tmp/quarktex-readback.XXXXXX)

readback_source() {
    awk '/^static void sync_bitmap\(/ { copying=1 }
         /^ULONG W3D_CheckIdle\(/ { copying=0 }
         copying { print }' Warp3D.library/Hardware.c
    awk '/^ULONG W3D_SetDrawRegion\(/ { copying=1 }
         /^ULONG W3D_SetDrawRegionWBM\(/ { copying=0 }
         copying { print }' Warp3D.library/Effect.c
    sed -n '1,$p' tests/readback-main.c
}

readback_source | cc -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter \
    -Igl -include tests/readback-support.h -x c - -o "$test_dir/fixed"
"$test_dir/fixed"

# Negative controls: missing synchronization, and stale destination tracking.
for regression in missing-sync stale-destination; do
    if [[ $regression == missing-sync ]]; then
        mutation='/^[[:space:]]*sync_bitmap(context);/d'
    else
        mutation='/^[[:space:]]*context->drawregion = bm;/d; /^[[:space:]]*context->yoffset = yoffset;/d'
    fi
    readback_source | sed "$mutation" | cc -std=c99 -Wall -Wextra \
        -Wno-unused-parameter -Wno-unused-function -Igl \
        -include tests/readback-support.h -x c - -o "$test_dir/$regression"
    if "$test_dir/$regression"; then
        echo "ERROR: negative control $regression did not fail" >&2
        exit 1
    fi
done
echo 'PASS: both negative controls detect regressions (assertion failures above are expected).'
echo "Test binaries retained in $test_dir"
