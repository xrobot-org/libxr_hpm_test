param(
    [Parameter(Position = 0, ValueFromRemainingArguments = $true)]
    [string[]]$Targets
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

if (-not $env:HPM_SDK_BASE) {
    throw 'HPM_SDK_BASE is not set.'
}

if (-not (Get-Command xr_hpm_cfg -ErrorAction SilentlyContinue)) {
    throw 'xr_hpm_cfg is not installed or not on PATH.'
}

if (-not (Test-Path -LiteralPath "$RepoRoot/libxr/CMakeLists.txt")) {
    throw 'LibXR submodule is missing. Run: git submodule update --init --recursive'
}

if (-not $Targets) {
    $Targets = Get-ChildItem -LiteralPath $RepoRoot -Directory |
        Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName 'hpm_peripherals.yaml') } |
        ForEach-Object Name
}

if (-not $Targets) {
    throw 'No HPM test targets found.'
}

foreach ($Target in $Targets) {
    $TargetDirectory = if ([System.IO.Path]::IsPathRooted($Target)) {
        [System.IO.Path]::GetFullPath($Target)
    } else {
        [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $Target))
    }

    if (-not (Test-Path -LiteralPath (Join-Path $TargetDirectory 'hpm_peripherals.yaml'))) {
        throw "Target directory not found: $Target"
    }

    Write-Host "==> Generating $([System.IO.Path]::GetFileName($TargetDirectory))"
    & xr_hpm_cfg generate `
        -d $TargetDirectory `
        --peripheral-config hpm_peripherals.yaml `
        --libxr-config User/libxr_config.yaml `
        --config-output .config.yaml `
        -o User/app_main.cpp `
        --hw-cntr `
        --format json
    if ($LASTEXITCODE -ne 0) {
        throw "xr_hpm_cfg failed with exit code $LASTEXITCODE"
    }

    $BuildDirectory = Join-Path $TargetDirectory 'build'
    Write-Host "==> Building $([System.IO.Path]::GetFileName($TargetDirectory))"
    & cmake -S $TargetDirectory -B $BuildDirectory -G Ninja `
        -DHPM_BUILD_TYPE=flash_xip `
        -DCMAKE_BUILD_TYPE=Debug `
        "-DLIBXR_DIR=$RepoRoot/libxr"
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed with exit code $LASTEXITCODE"
    }
    & cmake --build $BuildDirectory
    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed with exit code $LASTEXITCODE"
    }
}
