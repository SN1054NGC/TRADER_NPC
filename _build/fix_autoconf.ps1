
$ErrorActionPreference = 'Continue'
$dir = 'D:\steam\steamapps\common\DayZServer\Profiles\Trader_NPC_Prof'
$vars = Join-Path $dir 'TraderNpcVariables.txt'
$utf8 = New-Object System.Text.UTF8Encoding($false)

Write-Output '--- что сейчас в TraderNpcVariables.txt по авто-ценам и модулям ---'
Select-String -Path $vars -Pattern 'AutoPrices|VoiceEnabled|AiEnabled|AiUrl|SellCoef' -Encoding UTF8 | ForEach-Object { '  ' + $_.Line.Trim() }

$text = [System.IO.File]::ReadAllText($vars)
$need = [ordered]@{
  '<AutoPrices> yes'       = '<AutoPrices> yes'
  '<AutoPricesApply> yes'  = '<AutoPricesApply> yes'
  '<VoiceEnabled> yes'     = '<VoiceEnabled> yes'
}
$lines = $text -split "\r?\n"
$added = @()
foreach ($k in $need.Keys) {
  $key = $k.Split(' ')[0]
  if (-not ($lines | Where-Object { $_.Trim().StartsWith($key) })) {
    $idx = ($lines | Select-String -Pattern '^<FileEnd>' | Select-Object -First 1).LineNumber
    if (-not $idx) { $idx = $lines.Count }
    $new = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt $lines.Count; $i++) {
      if ($i -eq ($idx - 1)) { $new.Add($need[$k]) }
      $new.Add($lines[$i])
    }
    $lines = $new.ToArray(); $added += $k
  }
}
if ($added.Count -gt 0) {
  [System.IO.File]::WriteAllText($vars, ($lines -join [Environment]::NewLine), $utf8)
  Write-Output ('--- добавлено: ' + ($added -join ', '))
} else { Write-Output '--- все ключи уже были на месте' }

Write-Output '--- трактовка после правки ---'
Select-String -Path $vars -Pattern 'AutoPrices|VoiceEnabled' -Encoding UTF8 | ForEach-Object { '  ' + $_.Line.Trim() }

Write-Output '--- контрольный запуск сервера ---'
$out = & 'D:\DAYZDISKP\@TRADER_NPC\check_compile.ps1' -TraderCheckSeconds 45
$out | Select-String 'RESULT|traders =|loading_config|CHECK:' | ForEach-Object { '  ' + $_.Line.Trim() }
$t = Get-ChildItem 'D:\steam\steamapps\common\DayZServer\Profiles' -Filter 'TM_GeneralLogs_*.log' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
Write-Output '  --- авто-цены и загрузка ---'
Select-String -Path $t.FullName -Pattern 'AutoPrices|READING TRADER|CURRENCY ENTRY|applied' -Encoding UTF8 | Select-Object -First 8 | ForEach-Object { '    ' + $_.Line.Trim() }
