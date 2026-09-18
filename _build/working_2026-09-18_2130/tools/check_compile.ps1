# ============================================================
# check_compile.ps1 - verify the mod WITHOUT touching other mods.
#
#   1) PBO STRUCTURE  : unpack @sps_client_bot/addons/sps_client_bot.pbo and make
#                       sure config.cpp sits at the ADDON ROOT. Without that the
#                       engine mounts the package but loads no addon at all
#                       (this is what made every trader disappear).
#   2) SCRIPT COMPILE : start DayZServer, wait for "Module: Mission" in
#                       Profiles/script_<stamp>.log. Lines that mention
#                       sps_client_bot are split into errors and warnings.
#                       PASS  = Mission reached and no sps_client_bot ERROR
#                       (warnings such as unsafe down-casting do not fail).
#   3) TRADER BOOT    : with -TraderCheckSeconds N the server keeps running N
#                       seconds, is stopped gracefully (so the mod's log file is
#                       flushed) and the newest TM_GeneralLogs_*.log is checked
#                       for "[TRADER] LOADING TRADER CONFIG" and spawned traders.
#
# Report: _build/log/compile_report.txt   Exit: 0 PASS / 1 FAIL / 3 server running
# ============================================================
param(
  [int]$TimeoutSec = 240,
  [int]$TraderCheckSeconds = 0,
  [switch]$KeepRunning
)
$ErrorActionPreference = "Stop"

$Srv      = "D:\steam\steamapps\common\DayZServer"
$Profiles = Join-Path $Srv "Profiles"
$Exe      = Join-Path $Srv "DayZServer_x64.exe"
$SrvPbo   = Join-Path $Srv "@sps_client_bot\addons\sps_client_bot.pbo"
$Root     = "D:\DAYZDISKP\@sps_client_bot\_build"
$LogDir   = Join-Path $Root "log"
$Report   = Join-Path $LogDir "compile_report.txt"
$PboConsole = "C:\Program Files\PBO Manager v.1.4 beta\PBOConsole.exe"
$ServerArgs = '-config=serverDZ.cfg -port=2302 -cpuCount=1 -exThreads=2 -maxMem=32768 -dologs -adminlog -netlog -freezecheck -filePatching "-servermod=@AntifreeZe;@sps_zmb_01;" -profiles=Profiles "-mod=@CF;@Community-Online-Tools;@LuxRedux;@Dabs Framework;@DayZ Editor Loader;@sps_client_bot;@sps_item;"'

New-Item -ItemType Directory -Force -Path $LogDir | Out-Null
$Lines = New-Object System.Collections.ArrayList
function Add-Line([string]$s) { [void]$Lines.Add($s); Write-Host $s }

Add-Line ("# sps_client_bot check  " + (Get-Date -Format "yyyy-MM-dd HH:mm:ss"))

# ---------------- 1) pbo structure ----------------
$pboOk = $false
if (-not (Test-Path $SrvPbo)) {
  Add-Line "PBO            = MISSING (the mod cannot load without it)"
} else {
  $chk = Join-Path $LogDir "pbo_check"
  if (Test-Path $chk) { Remove-Item $chk -Recurse -Force }
  Start-Process -FilePath $PboConsole -ArgumentList "-unpack", $SrvPbo, $chk -Wait -RedirectStandardOutput (Join-Path $LogDir "pbo_check.log")
  if (Test-Path (Join-Path $chk "config.cpp")) { $pboOk = $true }
  $nested = Test-Path (Join-Path $chk "sps_client_bot")
  Add-Line ("PBO structure  = " + $(if ($pboOk) { "OK (config.cpp at addon root)" } else { "BAD (config.cpp missing at root, nested folder=" + $nested + ")" }))
  Add-Line ("PBO size       = " + (Get-Item $SrvPbo).Length + " bytes")
}
if (-not $pboOk) {
  Add-Line "RESULT = FAIL (fix the pbo: run build_pbo.ps1 - it writes the $PREFIX$ marker)"
  $Lines | Set-Content $Report
  exit 1
}

# ---------------- 2) compile ----------------
if (Get-Process -Name "DayZServer_x64" -ErrorAction SilentlyContinue) {
  Write-Error "DayZServer_x64 is already running - stop it before the check"
  exit 3
}

$beforeScripts = @(Get-ChildItem $Profiles -Filter "script_*.log" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Name)
$beforeTm      = @(Get-ChildItem $Profiles -Filter "TM_GeneralLogs_*.log" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Name)

$p = Start-Process -FilePath $Exe -ArgumentList $ServerArgs -WorkingDirectory $Srv -PassThru
Add-Line ("server_pid     = " + $p.Id)
Write-Host "[check] server started, waiting for Module: Mission ..."

$deadline = (Get-Date).AddSeconds($TimeoutSec)
$log = $null
$reachedMission = $false
$exited = $false
while ((Get-Date) -lt $deadline) {
  Start-Sleep -Seconds 3
  if (-not (Get-Process -Id $p.Id -ErrorAction SilentlyContinue)) { $exited = $true; break }
  $cand = Get-ChildItem $Profiles -Filter "script_*.log" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($cand -and ($beforeScripts -notcontains $cand.Name)) {
    $log = $cand
    if (Select-String -Path $log.FullName -Pattern "Module: Mission" -Quiet) { $reachedMission = $true; break }
  }
}
Add-Line ("reached_mission = " + $reachedMission)

if ($TraderCheckSeconds -gt 0 -and $reachedMission) {
  # MissionServer.OnUpdate/LoadServerConfigs starts after every other mod finished
  # its init, so poll for the mod's own log file instead of a fixed sleep.
  Write-Host ("[check] waiting up to " + $TraderCheckSeconds + "s for the trader boot ...")
  $limit = (Get-Date).AddSeconds($TraderCheckSeconds)
  while ((Get-Date) -lt $limit) {
    Start-Sleep -Seconds 3
    $tmNow = Get-ChildItem $Profiles -Filter "TM_GeneralLogs_*.log" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($tmNow -and ($beforeTm -notcontains $tmNow.Name)) { break }
    if (-not (Get-Process -Id $p.Id -ErrorAction SilentlyContinue)) { break }
  }
}

# graceful stop first so the mod flushes its TM log
$exitedEarly = -not [bool](Get-Process -Id $p.Id -ErrorAction SilentlyContinue)
if (-not $KeepRunning) {
  if ($exitedEarly) {
    Write-Host "[check] server already exited before the stop (crash or early exit)"
  } else {
    try { cmd /c "taskkill /PID $($p.Id)" | Out-Null } catch { }
    Start-Sleep -Seconds 20
    if (Get-Process -Id $p.Id -ErrorAction SilentlyContinue) {
      Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue
      Start-Sleep -Seconds 2
      Write-Host "[check] server force-stopped"
    } else {
      Write-Host "[check] server stopped gracefully"
    }
  }
}
Add-Line ("server_exited_early = " + $exitedEarly)

$mods = @()
$ownErr = @()
$ownWarn = @()
$allErr = @()
if ($log) {
  Add-Line ("script_log      = " + $log.FullName)
  foreach ($m in (Select-String -Path $log.FullName -Pattern "Module:")) { $mods += $m.Line.Trim() }
  foreach ($m in (Select-String -Path $log.FullName -Pattern "sps_client_bot")) {
    $line = $m.Line.Trim()
    $isErr = (($line -like "*(E)*") -or ($line -match ".c(d+):"))
    if ($isErr) { $ownErr += ("  " + $line) } else { $ownWarn += ("  " + $line) }
  }
  foreach ($m in (Select-String -Path $log.FullName -Pattern '\(E\)')) { $allErr += ("  " + $m.Line.Trim()) }
}
Add-Line "--- modules ---"
foreach ($m in $mods) { Add-Line $m }
Add-Line "--- sps_client_bot ERRORS (must be empty) ---"
if ($ownErr.Count -eq 0) { Add-Line "  (none)" } else { foreach ($l in $ownErr) { Add-Line $l } }
Add-Line "--- sps_client_bot warnings (informational) ---"
if ($ownWarn.Count -eq 0) { Add-Line "  (none)" } else { foreach ($l in $ownWarn) { Add-Line $l } }
Add-Line "--- all (E) lines from every mod (informational) ---"
if ($allErr.Count -eq 0) { Add-Line "  (none)" } else { foreach ($l in $allErr) { Add-Line $l } }

# ---------------- 3) trader boot ----------------
$traderOk = $null
if ($TraderCheckSeconds -gt 0) {
  $tm = Get-ChildItem $Profiles -Filter "TM_GeneralLogs_*.log" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($tm -and ($beforeTm -notcontains $tm.Name)) {
    $boot = Select-String -Path $tm.FullName -Pattern "LOADING TRADER CONFIG" -Quiet
    $spawned = (Select-String -Path $tm.FullName -Pattern "OBJECT TYPE ENTRY" -ErrorAction SilentlyContinue).Count
    $markers = (Select-String -Path $tm.FullName -Pattern "TRADER MARKER" -ErrorAction SilentlyContinue).Count
    Add-Line ("--- trader boot: " + $tm.Name + " (" + $tm.Length + " bytes) ---")
    Add-Line ("  loading_config = " + $boot + " ; spawned_objects = " + $spawned + " ; marker_lines = " + $markers)
    $traderOk = ($boot -and $spawned -gt 0)
    Add-Line ("  traders = " + $(if ($traderOk) { "OK" } else { "MISSING" }))
  } else {
    Add-Line "--- trader boot: NO new TM_GeneralLogs file (server exited before flushing / config load did not run) ---"
  }
  if ($exitedEarly) { Add-Line "  note: the engine exited early (see Profiles/crash_*.log + *.mdmp); trader check may be inconclusive" }
}

# compile verdict; the trader check only fails when the server was still alive
$pass = $reachedMission -and ($ownErr.Count -eq 0)
if ($TraderCheckSeconds -gt 0 -and -not $exitedEarly) { $pass = $pass -and ($traderOk -eq $true) }
Add-Line ("RESULT = " + $(if ($pass) { "PASS" } else { "FAIL" }))
$Lines | Set-Content $Report
Write-Host ("[check] report: " + $Report) -ForegroundColor Cyan

if ($pass) { Write-Host "CHECK: PASS" -ForegroundColor Green; exit 0 }
Write-Host "CHECK: FAIL" -ForegroundColor Red
exit 1
