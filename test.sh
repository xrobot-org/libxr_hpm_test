#!/bin/sh

set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ -z "${HPM_SDK_BASE:-}" ]; then
    echo "HPM_SDK_BASE is not set." >&2
    exit 1
fi

if ! command -v xr_hpm_cfg >/dev/null 2>&1; then
    echo "xr_hpm_cfg is not installed or not on PATH." >&2
    exit 1
fi

if [ ! -f "$repo_root/libxr/CMakeLists.txt" ]; then
    echo "LibXR submodule is missing. Run: git submodule update --init --recursive" >&2
    exit 1
fi

if [ "$#" -gt 0 ]; then
    targets="$*"
else
    targets=$(find "$repo_root" -mindepth 2 -maxdepth 2 -name hpm_peripherals.yaml \
        -exec dirname {} \; | sort)
fi

if [ -z "$targets" ]; then
    echo "No HPM test targets found." >&2
    exit 1
fi

for target in $targets; do
    case "$target" in
        /*) target_dir="$target" ;;
        *) target_dir="$repo_root/$target" ;;
    esac

    if [ ! -f "$target_dir/hpm_peripherals.yaml" ]; then
        echo "Target directory not found: $target" >&2
        exit 1
    fi

    echo "==> Generating $(basename "$target_dir")"
    xr_hpm_cfg generate \
        -d "$target_dir" \
        --peripheral-config hpm_peripherals.yaml \
        --libxr-config User/libxr_config.yaml \
        --config-output .config.yaml \
        -o User/app_main.cpp \
        --hw-cntr \
        --format json

    echo "==> Building $(basename "$target_dir")"
    cmake -S "$target_dir" -B "$target_dir/build" -G Ninja \
        -DHPM_BUILD_TYPE=flash_xip \
        -DCMAKE_BUILD_TYPE=Debug \
        -DLIBXR_DIR="$repo_root/libxr"
    cmake --build "$target_dir/build"
done
