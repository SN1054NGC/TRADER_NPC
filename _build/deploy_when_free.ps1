param([int]$MaxMinutes = 60)
$log = "D:\DAYZDISKP\@TRADER_NPC\_build\log\deploy_when_free.log"
function L($s){ $t=(Get-Date).ToString("HH:mm:ss"); Add-Content $log "$t $s" }
L "waiting for DayZ processes to exit (max $MaxMinutes min)"
$end = (Get-Date).AddMinutes($MaxMinutes)
while ((Get-Date) -lt $end) {
  $p = Get-Process -Name DayZ_x64,DayZServer_x64 -ErrorAction SilentlyContinue
  if (-not $p) { break }
  Start-Sleep -Seconds 5
}
if (Get-Process -Name DayZ_x64,DayZServer_x64 -ErrorAction SilentlyContinue) { L "timeout: still running"; exit 3 }
L "free -> build + deploy + check"
Start-Sleep -Seconds 3
cd "D:\DAYZDISKP\@TRADER_NPC"
& ".\build_pbo.ps1" *>> $log
& ".\deploy_server.ps1" *>> $log
$out = & ".\check_compile.ps1" -TraderCheckSeconds 45
$out | Select-String "ERRORS|RESULT|traders =" | Add-Content $log
$pbo = "D:\steam\steamapps\common\DayZServer\@TRADER_NPC\addons\TRADER_NPC.pbo"
$i = Get-Item $pbo
L ("PBO now: " + $i.Length + " bytes, md5 " + (Get-FileHash $pbo -Algorithm MD5).Hash.Substring(0,8))
$cli = "D:\steam\steamapps\common\DayZ\!Workshop\@TRADER_NPC\addons"
if (Test-Path $cli) { Copy-Item $pbo $cli -Force; L "client pbo updated" }
L "done"
