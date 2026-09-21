
$ErrorActionPreference = 'Continue'
$dir = 'D:\steam\steamapps\common\DayZServer\Profiles\Trader_NPC_Prof'
$cp1251 = [System.Text.Encoding]::GetEncoding(1251)
$utf8 = New-Object System.Text.UTF8Encoding($false)
Write-Output '--- исправление русской кодировки ---'
foreach ($f in Get-ChildItem $dir -Filter '*.txt' -File) {
  $text = [System.Text.Encoding]::UTF8.GetString([System.IO.File]::ReadAllBytes($f.FullName))
  $lines = $text -split "\r?\n"
  $fixed = New-Object System.Collections.Generic.List[string]
  $n = 0; $first = ''
  foreach ($ln in $lines) {
    $done = $false
    if ($ln -match 'Р|С') {
      try {
        $cand = [System.Text.Encoding]::UTF8.GetString($cp1251.GetBytes($ln))
        if ($cand -notmatch [char]0xFFFD -and $cand -match '[А-Яа-я]') {
          $back = $cp1251.GetString([System.Text.Encoding]::UTF8.GetBytes($cand))
          if ($back -eq $ln) {
            if ($first -eq '' -and $ln.Trim().Length -gt 15) { $first = $cand.Trim() }
            $fixed.Add($cand); $n++; $done = $true
          }
        }
      } catch {}
    }
    if (-not $done) { $fixed.Add($ln) }
  }
  if ($n -gt 0) {
    [System.IO.File]::WriteAllText($f.FullName, ($fixed -join [Environment]::NewLine), $utf8)
    Write-Output ('  ' + $f.Name + ': исправлено строк ' + $n)
    Write-Output ('     пример: ' + $first.Substring(0, [Math]::Min(95, $first.Length)))
  } else { Write-Output ('  ' + $f.Name + ': чисто') }
}
Write-Output '--- проверка целостности TraderNpcConfig.txt ---'
$p = Join-Path $dir 'TraderNpcConfig.txt'
'  строк ' + (Get-Content $p | Measure-Object).Count + ', ' + [math]::Round((Get-Item $p).Length/1KB,1) + ' KB'
foreach ($k in '<CurrencyName>','<Currency>','<Trader>','<Category>','<FileEnd>') {
  '    ' + $k.PadRight(16) + ': ' + (Select-String -Path $p -Pattern ([regex]::Escape($k)) -Encoding UTF8 | Measure-Object).Count
}
Write-Output '--- русские комментарии теперь читаются (пример) ---'
Select-String -Path $p -Pattern 'ВАЛЮТ|НАЗВАНИЕ|Рубл|рубл' -Encoding UTF8 | Select-Object -First 4 | ForEach-Object { '    ' + $_.Line.Trim().Substring(0, [Math]::Min(95, $_.Line.Trim().Length)) }
