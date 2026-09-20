param([int]$MaxMinutes = 45)
$log = "D:\DAYZDISKP\@TRADER_NPC\_build\log\auto_deploy.log"
function L($s){ $t=(Get-Date).ToString("HH:mm:ss"); Add-Content $log "$t $s" }
L "start: waiting for DayZ_x64 to close (max $MaxMinutes min)"
$end = (Get-Date).AddMinutes($MaxMinutes)
while ((Get-Date) -lt $end) {
  if (-not (Get-Process -Name DayZ_x64 -ErrorAction SilentlyContinue)) { break }
  Start-Sleep -Seconds 5
}
if (Get-Process -Name DayZ_x64 -ErrorAction SilentlyContinue) { L "timeout: client still running"; exit 3 }
L "client closed -> rebuild"
Start-Sleep -Seconds 3
& "D:\DAYZDISKP\@TRADER_NPC\build_pbo.ps1" *>> $log
L "sync loose copy"
& "D:\DAYZDISKP\@TRADER_NPC\deploy_server.ps1" *>> $log
$srv = "D:\steam\steamapps\common\DayZServer\@TRADER_NPC\addons\TRADER_NPC.pbo"
$i = Get-Item $srv
L ("PBO now: " + $i.Length + " bytes, " + $i.LastWriteTime + ", md5 " + (Get-FileHash $srv -Algorithm MD5).Hash.Substring(0,8))
L "done"
