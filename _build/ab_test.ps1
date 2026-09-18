# A/B: падает ли сервер БЕЗ нашего мода
param([int]$Seconds = 75, [int]$Runs = 2, [switch]$WithoutOurs)
$ErrorActionPreference = "Stop"
$Srv = "D:\steam\steamapps\common\DayZServer"
$Exe = Join-Path $Srv "DayZServer_x64.exe"
$Profiles = Join-Path $Srv "Profiles"
$modsWith    = "@CF;@Community-Online-Tools;@LuxRedux;@Dabs Framework;@DayZ Editor Loader;@sps_client_bot;@sps_item;"
$modsWithout = "@CF;@Community-Online-Tools;@LuxRedux;@Dabs Framework;@DayZ Editor Loader;@sps_item;"
$Out = Join-Path $Profiles "AB_result.txt"
$variant = if ($WithoutOurs) { "BEZ @sps_client_bot" } else { "S @sps_client_bot" }
$mods = if ($WithoutOurs) { $modsWithout } else { $modsWith }
Add-Content $Out ("#### " + (Get-Date -Format "dd.MM HH:mm:ss") + " variant: " + $variant + " runs: " + $Runs + " x " + $Seconds + "s")
if (Get-Process -Name DayZServer_x64 -ErrorAction SilentlyContinue) { Add-Content $Out "CANCEL: server already running"; Write-Host "ABORT"; exit 9 }
$argList = @("-config=serverDZ.cfg","-port=2302","-cpuCount=1","-exThreads=2","-maxMem=32768","-dologs","-adminlog","-netlog","-freezecheck","-filePatching","-servermod=@AntifreeZe;@sps_zmb_01;","-profiles=Profiles",("-mod=" + $mods))
for ($i = 1; $i -le $Runs; $i++) {
  $before = @(Get-ChildItem $Profiles -Filter "crash_*.log" | Select-Object -ExpandProperty Name)
  $p = Start-Process -FilePath $Exe -WorkingDirectory $Srv -PassThru -ArgumentList $argList
  $start = Get-Date
  $alive = $true
  for ($t = 0; $t -lt $Seconds; $t++) {
    Start-Sleep -Seconds 1
    if ($p.HasExited) { $alive = $false; break }
  }
  $secs = [int]((Get-Date) - $start).TotalSeconds
  $sig = ""
  $newCrash = @(Get-ChildItem $Profiles -Filter "crash_*.log" | Where-Object { $before -notcontains $_.Name })
  if ($newCrash.Count -gt 0) {
    $f = $newCrash | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    $c = Get-Content $f.FullName -TotalCount 16
    $r = ($c | Where-Object { $_ -match "^Reason:" } | Select-Object -First 1)
    $fn = ($c | Where-Object { $_ -match "Function:" } | Select-Object -First 1)
    $sig = $f.Name + " :: " + $r + " " + $fn
  }
  if ($alive) {
    Add-Content $Out ("  run " + $i + ": ALIVE after " + $secs + "s -> graceful stop")
    if (-not $p.HasExited) { & taskkill /PID $p.Id /T /F | Out-Null }
    Start-Sleep -Seconds 8
  } else {
    Add-Content $Out ("  run " + $i + ": CRASHED after " + $secs + "s   " + $sig)
  }
  Start-Sleep -Seconds 3
}
Add-Content $Out "#### end"
Get-Content $Out | Select-Object -Last 10
