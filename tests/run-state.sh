#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")/.."
test_dir=$(mktemp -d /tmp/quarktex-state.XXXXXX)

state_source() {
    # Preserve the original flag values, excluding trailing/multiline comments.
    awk '$1 == "#define" && ($3 ~ /^\(1<</ || $2 ~ /^W3D_(SUCCESS|UNSUPPORTEDSTATE|ENABLE|DISABLE)$/) {
        print $1, $2, $3
    }' Warp3D.library/Warp3D.h
    awk '/^ULONG W3D_SetState\(/ { copying=1 }
         /^ULONG W3D_Hint\(/ { copying=0 }
         copying { print }' Warp3D.library/Context.c
    sed -n '1,$p' tests/state-main.c
}

state_source | cc -std=c99 -Wall -Wextra -Werror -Igl -include tests/state-support.h \
    -x c - -o "$test_dir/fixed"
"$test_dir/fixed"

# Negative control: put back precisely the two commented-out cases. The same
# executable test must detect the original bug, not merely approve the patch.
state_source | sed \
    -e 's/_glDepthMask(GL_TRUE); break;/\/\* original missing depth-mask operation and break *\//' \
    -e 's/_glDepthMask(GL_FALSE); break;/\/\* original missing depth-mask operation and break *\//' \
    | cc -std=c99 -Wall -Wextra -Wno-unused-function -Igl -include tests/state-support.h \
        -x c - -o "$test_dir/original"
if "$test_dir/original"; then
    echo 'ERROR: negative control did not detect the original bug' >&2
    exit 1
fi
echo 'PASS: fixed implementation passes; original implementation fails.'
echo "Test binaries retained in $test_dir"
