# LibXR HPM Build Tests

This repository provides headless generation and full firmware build targets for
the HPM backend in `LibXR_CppCodeGenerator`. Each top-level target directory
contains one HPM SDK project and its checked-in generator inputs.

The committed `.hpmpc` files are credential-free test fixtures. Do not replace
them with signed HPM Pinmux Tool working copies.

## Requirements

- HPM SDK with `HPM_SDK_BASE` configured
- HPM RISC-V GCC toolchain with `GNURISCV_TOOLCHAIN_PATH` configured
- CMake and Ninja on `PATH`
- `LibXR_CppCodeGenerator` 5.3.0 or newer
- initialized LibXR submodule

```sh
git submodule update --init --recursive
python -m pip install --upgrade "libxr>=5.3.0"
./test.sh
```

On Windows PowerShell:

```powershell
git submodule update --init --recursive
python -m pip install --upgrade "libxr>=5.3.0"
./test.ps1
```

Pass one or more target directory names to build only those targets:

```sh
./test.sh HPM5361EVKLite
```

Each target executes `xr_hpm_cfg generate`, configures the HPM SDK project, and
links the complete firmware image. Generated build directories and
`.config.yaml` files are intentionally ignored.

The test harness is distributed under the BSD-3-Clause license. Imported HPM
SDK board files retain their original copyright and license notices.
