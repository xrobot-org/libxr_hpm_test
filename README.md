# LibXR HPM Build Tests

This repository provides headless generation and full firmware build targets for
the HPM backend in `LibXR_CppCodeGenerator`. Each top-level target directory
contains one HPM SDK project and its checked-in generator inputs.

The committed `.hpmpc` files are credential-free test fixtures. Do not replace
them with signed HPM Pinmux Tool working copies.

## Dependency scope

The committed LibXR submodule pins the mainline `dev` adapters for HPM UART,
SPI and MCAN. Initialize that exact gitlink; an older fork is not a substitute
for this dependency.

The HPM generator is installed from an exact source revision until that command
is included in a published release. Use a dedicated virtual environment rather
than replacing a working STM32 generator installation.

## Requirements

- HPM SDK 1.11.0 with `HPM_SDK_BASE` configured
- HPM RISC-V GCC toolchain with `GNURISCV_TOOLCHAIN_PATH` configured
- CMake and Ninja on `PATH`
- The HPM-capable generator revision pinned below (not the STM32-only PyPI 5.2.x release)
- initialized LibXR submodule

```sh
git submodule update --init --recursive
python -m pip install -r "$HPM_SDK_BASE/scripts/requirements.txt"
python -m pip install "libxr @ git+https://github.com/CaFeZn/LibXR_CppCodeGenerator.git@6b35a454cccc0aef8d9221718b91b6b151149a42"
./test.sh
```

On Windows PowerShell:

```powershell
git submodule update --init --recursive
python -m pip install -r "$env:HPM_SDK_BASE/scripts/requirements.txt"
python -m pip install "libxr @ git+https://github.com/CaFeZn/LibXR_CppCodeGenerator.git@6b35a454cccc0aef8d9221718b91b6b151149a42"
./test.ps1
```

Pass one or more target directory names to build only those targets:

```sh
./test.sh HPM5361EVKLite
```

The project enables real compiler feature probes before the SDK first checks
the toolchain, so the SDK installation does not need a local CMake patch.

Each target executes `xr_hpm_cfg generate`, configures the HPM SDK project, and
links the complete firmware image. Generated build directories and
`.config.yaml` files are intentionally ignored.

The test harness is distributed under the BSD-3-Clause license. Imported HPM
SDK board files retain their original copyright and license notices.

## HPM5361 runtime entry and console

`main()` performs board initialization and then enters generated `app_main()`.
The GPIO/button polling, UART byte report and heartbeat run after the LibXR
peripherals are constructed. MCAN interrupt entries are supplied by the HPM
adapter and dispatch to the registered classic-CAN or CAN-FD owner.

The runtime console uses **UART3 PB15 TX / PB14 RX, 115200 8N1** through LibXR.
The SDK UART0 console is disabled: PA00/PA01 are already assigned to MCAN0 in
this configuration. Do not attach the console adapter to the MCAN pins.
GPIO behavior is retained: PA10 active-low LED and PA03 active-high button.
