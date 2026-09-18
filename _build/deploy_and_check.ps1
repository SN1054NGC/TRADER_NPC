cd "D:\DAYZDISKP\@sps_client_bot"
"=== BUILD ==="
& ".\build_pbo.ps1" | Select-String "pbo\]|verify|deploy|locked"
"=== SYNC LOOSE ==="
& ".\deploy_server.ps1" | Select-String "deploy OK" | Select-Object -First 1
"=== COMPILE + TRADER BOOT ==="
$ok = $false
for ($n = 1; $n -le 2; $n++) {
  $out = & ".\check_compile.ps1" -TraderCheckSeconds 55
  $out | Select-String "PBO size|reached_mission|ERRORS|SCRIPT.*sps_client_bot|RESULT|traders =|loading_config|marker_lines"
  if ($out -match "traders = OK") { $ok = $true; break }
  "--- attempt " + $n + ": no trader log (known Lux crash), retrying ---"
}
"=== AUTO CONFIG ==="
$auto = "D:\steam\steamapps\common\DayZServer\Profiles\Trader\TraderConfig_auto.txt"
if (Test-Path $auto) {
  $fi = Get-Item $auto
  "  exists: " + $fi.Length + " bytes, " + (Get-Content $auto).Count + " lines"
  Get-Content $auto -TotalCount 6 | ForEach-Object { "    " + $_ }
  "    ..."
  Get-Content $auto -TotalCount 40 | Select-Object -Last 3 | ForEach-Object { "    " + $_ }
} else { "  NOT created (server did not reach readTraderData)" }
"=== MD5 dev vs server-loose ==="
foreach ($f in @("scripts\4_World\TraderSmartSell.c","scripts\4_World\Entities\DayZPlayerImplement.c","scripts\5_Mission\mission\missionServer.c","scripts\4_World\TraderTradeRules.c","scripts\5_Mission\TraderAutoPrices.c")) {
  $a = "D:\DAYZDISKP\@sps_client_bot\addons\sps_client_bot\" + $f
  $b = "D:\steam\steamapps\common\DayZServer\@sps_client_bot\addons\sps_client_bot\" + $f
  $ha = (Get-FileHash $a -Algorithm MD5).Hash.Substring(0,8)
  $hb = (Get-FileHash $b -Algorithm MD5).Hash.Substring(0,8)
  $st = "OK"
  if ($ha -ne $hb) { $st = "MISMATCH" }
  "  {0,-52} {1} {2} {3}" -f $f, $ha, $hb, $st
}
$p = "D:\steam\steamapps\common\DayZServer\@sps_client_bot\addons\sps_client_bot.pbo"
$i = Get-Item $p
"=== PBO: " + $i.Length + " bytes, " + $i.LastWriteTime.ToString("HH:mm:ss") + ", md5 " + (Get-FileHash $p -Algorithm MD5).Hash.Substring(0,8) + " ==="
