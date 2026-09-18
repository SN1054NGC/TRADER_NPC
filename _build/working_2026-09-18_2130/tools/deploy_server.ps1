# ============================================================
# deploy_server.ps1 - sync the MOD SOURCES to the server deployment.
# Source of truth: D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot
# Target:          D:/steam/steamapps/common/DayZServer/@sps_client_bot
# Only this mod is touched; every other mod is read-only.
# Run this BEFORE building the pbo (build_pbo.ps1) or compiling in Workbench.
# ============================================================
$ErrorActionPreference = "Stop"

$SrcAddon = "D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot"
$SrvAddon = "D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons/sps_client_bot"
$SrvPbo   = "D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons/sps_client_bot.pbo"
$Elements = @("scripts", "languagecore", "images", "config.cpp")

Write-Host "== deploy sps_client_bot :" (Get-Date -Format "yyyy-MM-dd HH:mm:ss") -ForegroundColor Cyan

if (-not (Test-Path $SrcAddon)) { Write-Error "missing source $SrcAddon"; exit 1 }
if (-not (Test-Path $SrvAddon)) { Write-Error "missing server $SrvAddon"; exit 1 }

# 1) NOTE: do NOT delete the pbo here. Verified on this server: unpacked addon
# folders are NOT mounted even with -filePatching, so a missing pbo means the mod
# (and with it every trader) silently disappears. Refresh the pbo with
# build_pbo.ps1 (PBO Manager) or with Workbench after syncing the sources.
if (Test-Path $SrvPbo) {
  Write-Host "  [pbo] present - run build_pbo.ps1 (or Workbench) to refresh it"
} else {
  Write-Host "  [pbo] MISSING - the mod will not load! run build_pbo.ps1" -ForegroundColor Red
}

# 2) purge old artifacts, 3) copy the fresh sources
foreach ($el in $Elements) {
  $t = Join-Path $SrvAddon $el
  if (Test-Path $t) { Remove-Item $t -Recurse -Force; Write-Host "  [clear] $el" }
}
foreach ($el in $Elements) {
  $s = Join-Path $SrcAddon $el
  if (-not (Test-Path $s)) { Write-Host "  [skip] src $el"; continue }
  Copy-Item $s (Join-Path $SrvAddon $el) -Recurse -Force
  Write-Host "  [copy] $el"
}

# 4) verify (md5) the files that carry the trader logic
$fail = $false
function Assert-Match([string]$rel) {
  $s = Join-Path $SrcAddon $rel
  $d = Join-Path $SrvAddon $rel
  if ((Test-Path $s) -and (Test-Path $d)) {
    if ((Get-FileHash $s -Algorithm MD5).Hash -eq (Get-FileHash $d -Algorithm MD5).Hash) {
      Write-Host "  [ok] $rel" -ForegroundColor Green
    } else {
      Write-Host "  [BAD] $rel" -ForegroundColor Red
      $script:fail = $true
    }
  } else {
    Write-Host "  [MISSING] $rel" -ForegroundColor Red
    $script:fail = $true
  }
}
Assert-Match "scripts/4_World/TraderSmartSell.c"
Assert-Match "scripts/4_World/TraderMenu.c"
Assert-Match "scripts/4_World/Entities/DayZPlayerImplement.c"
Assert-Match "scripts/5_Mission/TraderInspectMenuExt.c"
Assert-Match "scripts/5_Mission/mission/missionServer.c"
Assert-Match "scripts/layouts/TraderSellPopup.layout"
Assert-Match "scripts/layouts/TraderRating.layout"
Assert-Match "scripts/3_Game/TraderRating.c"
Assert-Match "scripts/5_Mission/TraderRatingUI.c"
Assert-Match "languagecore/stringtable.csv"
Assert-Match "config.cpp"

if ($fail) { Write-Error "verify fail"; exit 1 }
Write-Host "== deploy OK: server addon == source ==" -ForegroundColor Green
if (Test-Path $SrvPbo) {
  Write-Host "   note: the deployed pbo was NOT rebuilt by this script - run build_pbo.ps1 if you changed scripts" -ForegroundColor Yellow
} else {
  Write-Host "   WARNING: no pbo deployed - the mod will not load until build_pbo.ps1 runs" -ForegroundColor Red
}
exit 0
