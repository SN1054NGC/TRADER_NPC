
$ErrorActionPreference = 'Continue'
$dir = 'D:\steam\steamapps\common\DayZServer\Profiles\Trader_NPC_Prof'
$enc1251 = [System.Text.Encoding]::GetEncoding(1251)
$utf8 = New-Object System.Text.UTF8Encoding($false)
Write-Output '--- исправление кодировки (UTF-8 -> CP1251 -> UTF-8) ---'
foreach ($f in Get-ChildItem $dir -Filter '*.txt' -File) {
  $bytes = [System.IO.File]::ReadAllBytes($f.FullName)
  $text = [System.Text.Encoding]::UTF8.GetString($bytes)
  $lines = $text -split "\r?\n"
  $out = New-Object System.Collections.Generic.List[string]
  $changed = 0
  foreach ($ln in $lines) {
    if ($ln -match '[А-Яа-яІ-ѿ]' -and $ln -match 'Р[А-Яа-я]|С[А-Яа-я]') {
      try {
        $fixed = [System.Text.Encoding]::UTF8.GetString($enc1251.GetBytes($ln))
        if ($fixed -notmatch [char]0xFFFD) { $out.Add($fixed); $changed++; continue }
      } catch {}
    }
    $out.Add($ln)
  }
  if ($changed -gt 0) {
    [System.IO.File]::WriteAllText($f.FullName, ($out -join [Environment]::NewLine), $utf8)
    '  ' + $f.Name + ': исправлено строк ' + $changed
  } else {
    '  ' + $f.Name + ': чисто'
  }
}
Write-Output '--- проверка: как теперь выглядит TraderNpcConfig.txt ---'
$p = Join-Path $dir 'TraderNpcConfig.txt'
'  строк: ' + (Get-Content $p | Measure-Object).Count + ', размер ' + [math]::Round((Get-Item $p).Length/1KB,1) + ' KB'
Get-Content $p -TotalCount 25 -Encoding UTF8 | ForEach-Object { '   ' + $_.Substring(0, [Math]::Min(110, $_.Length)) }
Write-Output '--- структура (валюта / торговец / категория / FileEnd) ---'
foreach ($key in '<CurrencyName>','<Currency>','<Trader>','<Category>','<FileEnd>') {
  $c = (Select-String -Path $p -Pattern ([regex]::Escape($key)) -Encoding UTF8 | Measure-Object).Count
  '  ' + $key.PadRight(16) + ': ' + $c
}
Write-Output '--- переменные: ключи модулей ---'
Select-String -Path (Join-Path $dir 'TraderNpcVariables.txt') -Pattern 'AutoPrices|VoiceEnabled|AiEnabled|AiUrl' -Encoding UTF8 | ForEach-Object { '  ' + $_.Line.Trim() }
