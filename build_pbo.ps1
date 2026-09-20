# ============================================================
# build_pbo.ps1 - pack the mod into TRADER_NPC.pbo with PBO Manager.
#
# WHY THE $PREFIX$ FILE IS CRITICAL (this was the "traders disappeared" bug):
#   The engine mounts '@TRADER_NPC\addons\TRADER_NPC.pbo' and looks for
#   config.cpp (CfgPatches/CfgMods) AT THE ADDON ROOT. A pbo built by PBOConsole
#   WITHOUT a prefix contains a real 'TRADER_NPC' sub-folder instead, so the
#   config is one level too deep: the engine mounts the package but loads no
#   addon at all - no scripts, no plugins, no traders.
#   PBO Manager sets the pbo property 'prefix' from a file named $PREFIX$ that
#   sits in the folder being packed; its content must be the mod prefix.
#   Result: prefix = TRADER_NPC\  AND config.cpp at the addon root.
#
#  * no binarization at all (PBOConsole only stores files)
#  * removes stray config .bin (we keep config.cpp only)
#  * verifies the structure by unpacking the fresh pbo
#  * copies the result to the server deployment, keeping a timestamped backup
# ============================================================
param(
  [switch]$NoDeploy
)
$ErrorActionPreference = "Stop"

$SrcAddon = "D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC"
$Root     = "D:/DAYZDISKP/@TRADER_NPC/_build"
$Stage    = Join-Path $Root "stage"
$LogDir   = Join-Path $Root "log"
$OutPbo   = Join-Path $Root "TRADER_NPC.pbo"
$SrvAddon = "D:/steam/steamapps/common/DayZServer/@TRADER_NPC/addons/TRADER_NPC"
$SrvPbo   = "D:/steam/steamapps/common/DayZServer/@TRADER_NPC/addons/TRADER_NPC.pbo"
$Backup   = Join-Path $Root "backup"
$PboConsole = "C:/Program Files/PBO Manager v.1.4 beta/PBOConsole.exe"
$Prefix   = "TRADER_NPC"
# -NoDeploy : build the pbo only (the server may hold the deployed file open)

if (-not (Test-Path $PboConsole)) { Write-Error "PBOConsole not found: $PboConsole"; exit 1 }
if (-not (Test-Path $SrcAddon))   { Write-Error "missing source $SrcAddon"; exit 1 }

Write-Host "== build_pbo TRADER_NPC :" (Get-Date -Format "yyyy-MM-dd HH:mm:ss") -ForegroundColor Cyan

if (Test-Path $Stage) { Remove-Item $Stage -Recurse -Force }
New-Item -ItemType Directory -Force -Path (Join-Path $Stage $Prefix) | Out-Null
New-Item -ItemType Directory -Force -Path $LogDir | Out-Null

foreach ($el in @("scripts", "languagecore", "images", "config.cpp")) {
  $s = Join-Path $SrcAddon $el
  if (-not (Test-Path $s)) { Write-Host "  [skip] $el"; continue }
  Copy-Item $s (Join-Path (Join-Path $Stage $Prefix) $el) -Recurse -Force
  Write-Host "  [stage] $el"
}

# ---- the addon prefix marker: makes PBO Manager write pbo property 'prefix' ----
[System.IO.File]::WriteAllText((Join-Path (Join-Path $Stage $Prefix) '$PREFIX$'), $Prefix)
Write-Host "  [prefix] $PREFIX$ = $Prefix"

# never ship binarized configs - keep config.cpp only
Get-ChildItem (Join-Path $Stage $Prefix) -Recurse -Include "*.bin" -ErrorAction SilentlyContinue | ForEach-Object {
  Write-Host "  [purge bin] $($_.FullName)"; Remove-Item $_.FullName -Force
}

# pack the ADDON folder itself (its contents become the pbo root)
if (Test-Path $OutPbo) { Remove-Item $OutPbo -Force }
Start-Process -FilePath $PboConsole -ArgumentList "-pack", (Join-Path $Stage $Prefix), $OutPbo -Wait -RedirectStandardOutput (Join-Path $LogDir "pack.log") -RedirectStandardError (Join-Path $LogDir "pack.err")
if (-not (Test-Path $OutPbo)) { Write-Error "pack failed - see $LogDir/pack.log"; exit 1 }
Write-Host ("  [pbo] " + (Get-Item $OutPbo).Length + " bytes")

# verify the structure: config.cpp must be at the ROOT of the archive
$check = Join-Path $LogDir "check"
if (Test-Path $check) { Remove-Item $check -Recurse -Force }
Start-Process -FilePath $PboConsole -ArgumentList "-unpack", $OutPbo, $check -Wait -RedirectStandardOutput (Join-Path $LogDir "unpack.log")
if (-not (Test-Path (Join-Path $check "config.cpp"))) {
  Write-Error "BAD pbo: config.cpp is not at the addon root (the engine will not load the mod)"
  exit 1
}
if (Test-Path (Join-Path $check $Prefix)) {
  Write-Error "BAD pbo: found a nested '$Prefix' folder - the addon prefix is wrong"
  exit 1
}
$files = (Get-ChildItem $check -Recurse -File | Measure-Object).Count
$layouts = (Get-ChildItem (Join-Path $check "scripts/layouts") -File -ErrorAction SilentlyContinue | Measure-Object).Count
Write-Host ("  [verify] addon root OK, " + $files + " files, " + $layouts + " layouts") -ForegroundColor Green

if ($NoDeploy) {
  Write-Host ("  [no-deploy] built only: " + $OutPbo) -ForegroundColor Yellow
  Write-Host "== build_pbo OK (not deployed) ==" -ForegroundColor Green
  exit 0
}

# deploy
New-Item -ItemType Directory -Force -Path $Backup | Out-Null
if (Test-Path $SrvPbo) {
  $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
  Copy-Item $SrvPbo (Join-Path $Backup "TRADER_NPC.pbo.$stamp") -Force
  for ($i = 0; $i -lt 8; $i++) {
    try { Remove-Item $SrvPbo -Force -ErrorAction Stop; break }
    catch { Write-Host "  [pbo] locked, retry $($i + 1)..."; Start-Sleep -Milliseconds 700 }
  }
  if (Test-Path $SrvPbo) { Write-Error "cannot replace locked pbo: $SrvPbo"; exit 1 }
  Write-Host "  [backup] previous pbo kept in $Backup"
}
Copy-Item $OutPbo $SrvPbo -Force
Write-Host ("  [deploy] " + $SrvPbo) -ForegroundColor Green
Write-Host "== build_pbo OK ==" -ForegroundColor Green
exit 0
