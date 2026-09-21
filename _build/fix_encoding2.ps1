
$ErrorActionPreference = 'Continue'
$dir = 'D:\steam\steamapps\common\DayZServer\Profiles\Trader_NPC_Prof'
$cp1251 = [System.Text.Encoding]::GetEncoding(1251)
$utf8 = New-Object System.Text.UTF8Encoding($false)
Write-Output '--- исправление мохабры ---'
foreach ($f in Get-ChildItem $dir -Filter '*.txt' -File) {
  $text = [System.Text.Encoding]::UTF8.GetString([System.IO.File]::ReadAllBytes($f.FullName))
  $lines = $text -split "\r?\n"
  $fixedLines = New-Object System.Collections.Generic.List[string]
  $n = 0
  foreach ($ln in $lines) {
    $done = $false
    if ($ln -match 'Р|С') {
      try {
        $cand = [System.Text.Encoding]::UTF8.GetString($cp1251.GetBytes($ln))
        if ($cand -notmatch [char]0xFFFD -and $cand -match '[А-Яа-я]') {
          $back = $cp1251.GetString([System.Text.Encoding]::UTF8.GetBytes($cand))
          if ($back -eq $ln) { $fixedLines.Add($cand); $n++; $done = $true }
        }
      } catch {}
    }
    if (-not $done) { $fixedLines.Add($ln) }
  }
  if ($n -gt 0) {
    [System.IO.File]::WriteAllText($f.FullName, ($fixedLines -join [Environment]::NewLine), $utf8)
    Write-Output ('  ' + $f.Name + ': исправлено строк ' + $n)
  } else { Write-Output ('  ' + $f.Name + ': чисто') }
}
Write-Output '--- пример строки после исправления ---'
$p = Join-Path $dir 'TraderNpcConfig.txt'
Get-Content $p -Encoding UTF8 | Where-Object { $_ -match 'РјРѕР¶РЅРѕ|можно' } | Select-Object -First 2 | ForEach-Object { '   ' + $_.Trim().Substring(0, [Math]::Min(100, $_.Trim().Length)) }
Write-Output '--- структура TraderNpcConfig.txt ---'
Write-Output ('  строк ' + (Get-Content $p | Measure-Object).Count + ', ' + [math]::Round((Get-Item $p).Length/1KB,1) + ' KB')
foreach ($key in '<CurrencyName>','<Currency>','<Trader>','<Category>','<FileEnd>','<OpenFile>') {
  $c = (Select-String -Path $p -Pattern ([regex]::Escape($key)) -Encoding UTF8 | Measure-Object).Count
  Write-Output ('    ' + $key.PadRight(16) + ': ' + $c)
}
Write-Output '--- значимые строки конфига ---'
Get-Content $p -Encoding UTF8 | Where-Object { $_ -notmatch '^\s*//' -and $_.Trim() -ne '' } | Select-Object -First 15 | ForEach-Object { '    ' + $_.Trim().Substring(0, [Math]::Min(100, $_.Trim().Length)) }
