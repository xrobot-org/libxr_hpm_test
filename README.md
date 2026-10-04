# LibXR HPM Build Tests

This repository provides full firmware build targets for the HPM driver of
LibXR. Each top-level target directory contains one HPM SDK project whose
`User/app_main.cpp` constructs LibXR HPM peripherals.

The committed `.hpmpc` files are the HPM Pinmux Tool projects of the board pin
configuration and are stored without signing credentials.

## Dependency scope

The LibXR submodule (`https://github.com/xrobot-org/libxr.git`) is pinned to the
commit recorded in this repository. The HPM driver of that revision provides
the timebase, I2C, GPIO and PWM adapters; the HPM5361EVKLite target constructs
`HPMTimebase` and `HPMI2C` (I2C3, 100 kHz).

## Requirements

- HPM SDK 1.11.0 with `HPM_SDK_BASE` configured
- HPM RISC-V GCC toolchain with `GNURISCV_TOOLCHAIN_PATH` configured
- CMake and Ninja on `PATH`
- initialized LibXR submodule

```sh
git submodule update --init --recursive
python -m pip install -r "$HPM_SDK_BASE/scripts/requirements.txt"
./test.sh
```

On Windows PowerShell:

```powershell
git submodule update --init --recursive
python -m pip install -r "$env:HPM_SDK_BASE/scripts/requirements.txt"
./test.ps1
```

The image `ghcr.io/xrobot-org/docker-image-hpm:main` provides CMake, Ninja and
the toolchain, and records the toolchain directory in `XR_HPM_TOOLCHAIN_ROOT`.
The HPM SDK is mounted into the container:

```sh
git submodule update --init --recursive
docker run --rm -v "$PWD:/work" -v "$HPM_SDK_BASE:/opt/hpm_sdk:ro" -w /work \
  -e HPM_SDK_BASE=/opt/hpm_sdk ghcr.io/xrobot-org/docker-image-hpm:main \
  bash -c 'export GNURISCV_TOOLCHAIN_PATH="$XR_HPM_TOOLCHAIN_ROOT" && ./test.sh'
```

Pass one or more target directory names to build only those targets:

```sh
./test.sh HPM5361EVKLite
```

The project enables real compiler feature probes before the SDK first checks
the toolchain, so the build uses an unmodified SDK installation.

For each target the script configures the HPM SDK project (`flash_xip`, Debug)
in `<target>/build` and links the complete firmware image. Build directories
are ignored by Git.

The test harness is distributed under the BSD-3-Clause license. Imported HPM
SDK board files retain their original copyright and license notices.

## HPM5361 runtime entry and console

`main()` performs board initialization and then enters `app_main()` in
`User/app_main.cpp`. The GPIO/button polling, UART byte report and heartbeat
run after the LibXR timebase and I2C3 are constructed.

The runtime console is the SDK console on UART3 PB15 TX / PB14 RX, 115200 8N1.
`CMakeLists.txt` moves it there from the board default UART0 through the
`BOARD_CONSOLE_UART_*` definitions. The GPIO demo uses the PA10 active-low LED
and the PA03 active-high button.
