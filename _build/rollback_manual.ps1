
$ErrorActionPreference = 'Continue'
$dir = 'D:\steam\steamapps\common\DayZServer\Profiles\Trader_NPC_Prof'
$utf8 = New-Object System.Text.UTF8Encoding($false)
$enc  = [System.Text.Encoding]::UTF8

Write-Output '--- 1) возвращаем ручной конфиг с разделением торговцев ---'
$src = Join-Path $dir '_old\TraderNpcConfig_manual_backup_20260921_0752.txt'
Copy-Item $src (Join-Path $dir 'TraderNpcConfig.txt') -Force
'  восстановлен ' + [math]::Round((Get-Item (Join-Path $dir 'TraderNpcConfig.txt')).Length/1KB,1) + ' KB'

Write-Output '--- 2) чистим закомментированные <Currency> ---'
$cfg = Join-Path $dir 'TraderNpcConfig.txt'
$lines = [System.IO.File]::ReadAllText($cfg) -split "\r?\n"
$out = @(); $removed = 0
foreach ($ln in $lines) {
  if ($ln.Trim().StartsWith('//') -and $ln -match '<Currency') { $removed++; continue }
  $out += $ln
}
[System.IO.File]::WriteAllText($cfg, ($out -join [Environment]::NewLine), $utf8)
'  удалено закомментированных строк валюты: ' + $removed

Write-Output '--- 3) отключаем авто-применение ассортимента ---'
$vars = Join-Path $dir 'TraderNpcVariables.txt'
$vt = [System.IO.File]::ReadAllText($vars)
$vt = $vt -replace '<AutoPricesApply>\s*yes', '<AutoPricesApply> no'
[System.IO.File]::WriteAllText($vars, $vt, $utf8)
Select-String -Path $vars -Pattern '^<AutoPrices>|^<AutoPricesApply>|^<AutoPricesOnlyUnlisted>' -Encoding UTF8 | ForEach-Object { '  ' + $_.Line.Trim() }

Write-Output '--- 4) контрольный запуск ---'
$o = & 'D:\DAYZDISKP\@TRADER_NPC\check_compile.ps1' -TraderCheckSeconds 50
$o | Select-String 'RESULT|traders =|loading_config|CHECK:' | ForEach-Object { '  ' + $_.Line.Trim() }
$t = Get-ChildItem 'D:\steam\steamapps\common\DayZServer\Profiles' -Filter 'TM_GeneralLogs_*.log' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
Write-Output ('--- лог ' + $t.Name + ' ---')
Write-Output '  торговцы:'
Select-String -Path $t.FullName -Pattern 'READING TRADER ENTRY' -Encoding UTF8 | ForEach-Object { '    ' + $_.Line.Trim() }
Write-Output '  валюта:'
Select-String -Path $t.FullName -Pattern 'CURRENCY' -Encoding UTF8 | ForEach-Object { '    ' + $_.Line.Trim() }
Write-Output '  отброшенные позиции (без модели):'
$sk = Select-String -Path $t.FullName -Pattern 'skipped \(no model' -Encoding UTF8
'    всего: ' + ($sk | Measure-Object).Count
$sk | Select-Object -First 15 | ForEach-Object { '    ' + $_.Line.Trim() }
