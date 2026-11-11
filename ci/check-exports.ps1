<#
.SYNOPSIS
  Checks that a DLL exports only plain, unmangled C names from an allowed list.

.DESCRIPTION
  A mangled C++ name (starting with '?') or any unexpected export means the
  public ABI changed by accident. Fails with exit code 1 and lists the
  offending names.

.EXAMPLE
  ./ci/check-exports.ps1 -Dll lib/x64/dynamic/JSR-SDK.dll
#>
param(
  [Parameter(Mandatory)] [string] $Dll,
  # Exact names that may be exported.
  [string[]] $Allowed = @('CreateJSRSDKManager', 'DestroyJSRSDKManager'),
  # Name patterns that may be exported, such as the C API's 'JSR_*'.
  [string[]] $AllowedPatterns = @()
)

$ErrorActionPreference = 'Stop'

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$dumpbin = Get-ChildItem (Join-Path $vs 'VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe') |
  Select-Object -Last 1 -ExpandProperty FullName
if (-not $dumpbin) { throw 'dumpbin.exe not found' }

$output = & $dumpbin /nologo /exports $Dll
if ($LASTEXITCODE -ne 0) { throw "dumpbin failed for $Dll" }

# Export rows look like: "ordinal hint RVA name [= decorated]".
$names = $output |
  Where-Object { $_ -match '^\s+\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]{8}\s+(\S+)' } |
  ForEach-Object { $Matches[1] }

if (-not $names) {
  Write-Error "No exports found in $Dll"
  exit 1
}

$bad = $names | Where-Object {
  $name = $_
  -not ($Allowed -contains $name) -and
  -not ($AllowedPatterns | Where-Object { $name -like $_ })
}

Write-Host "Exports of ${Dll}:"
$names | ForEach-Object { Write-Host "  $_" }

if ($bad) {
  Write-Host ''
  Write-Host 'Unexpected exports:'
  $bad | ForEach-Object { Write-Host "  $_" }
  exit 1
}
Write-Host 'All exports are expected.'
