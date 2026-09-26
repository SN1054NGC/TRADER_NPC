<#
  audit_assortment.ps1 - сверка ассортимента торговцев с оборудованием мира.
  Пишет TraderNpcAudit.txt рядом с конфигами: типы мира по категориям, оборудование
  мира НЕ в торговле, классы торговли НЕ в types.xml, выброшенное сервером, сверка.
#>
param(
  [string]$Server = 'D:\steam\steamapps\common\DayZServer',
  [string]$Profiles = 'Trader_NPC_Prof',
  [string]$Types = 'mpmissions\dayzOffline.Lux\db\types.xml'
)

$enc  = New-Object System.Text.UTF8Encoding($false)
$prof = Join-Path (Join-Path $Server 'Profiles') $Profiles
$cfg  = Join-Path $prof 'TraderNpcConfig.txt'
$auto = Join-Path $prof 'TraderNpcConfig_auto.txt'
$types= Join-Path $Server $Types
$logs = Get-ChildItem (Join-Path $Server 'Profiles') -Recurse -File -Filter 'TM_GeneralLogs_*.log' | Sort-Object LastWriteTime -Descending
$log  = $logs[0].FullName
$out  = Join-Path $prof 'TraderNpcAudit.txt'

function CfgClasses($path) {
  if (-not (Test-Path $path)) { return @() }
  $res = Get-Content $path | ForEach-Object {
    $t = $_.Trim()
    if ($t -eq '' -or $t.StartsWith('//') -or $t.StartsWith('<')) { return }
    if ($t.IndexOf(',') -lt 0) { return }
    $t.Split(",")[0].Trim()
  } | Where-Object { $_ -ne "" }
  return $res | Sort-Object -Unique
}
$cfgCls  = @(CfgClasses $cfg)
$autoCls = @(CfgClasses $auto)

$txt = [System.IO.File]::ReadAllText($types)
$typesMap = @{}
foreach ($m in [regex]::Matches($txt, '<type name="([^"]+)"[\s\S]*?</type>')) {
  $name = $m.Groups[1].Value
  if ($typesMap.ContainsKey($name)) { continue }
  $cm = [regex]::Match($m.Value, '<category name="([^"]+)"')
  $cat = ''
  if ($cm.Success) { $cat = $cm.Groups[1].Value }
  $typesMap[$name] = $cat
}
$dropped = @(Select-String -Path $log -Pattern 'skipped \(no model in config\): (.+)$' | ForEach-Object { $_.Matches[0].Groups[1].Value.Trim() } | Sort-Object -Unique)

$cfgAll = @($cfgCls + $autoCls | Sort-Object -Unique)
$equipCat = @('weapons','magazines','ammo','explosives')
$equip = @()
foreach ($k in $typesMap.Keys) {
  $cat = $typesMap[$k]
  $isEq = ($equipCat -contains $cat) -or ($k -like 'Mag_*') -or ($k -like 'Ammo_*') -or ($k -like 'AmmoBox_*') -or ($k -like '*Optic*') -or ($k -like '*Suppressor*')
  if ($isEq) { $equip += $k }
}
$equip = $equip | Sort-Object -Unique
$missEq  = @($equip  | Where-Object { $cfgAll -notcontains $_ })
$missCfg = @($cfgAll | Where-Object { -not $typesMap.ContainsKey($_) })

$L = New-Object System.Collections.Generic.List[string]
$L.Add('=== AUDIT: trader assortment vs world equipment ===')
$L.Add('date: ' + (Get-Date).ToString('yyyy-MM-dd HH:mm'))
$L.Add('sources:')
$L.Add('  TraderNpcConfig.txt      : ' + $cfgCls.Count + ' classes')
$L.Add('  TraderNpcConfig_auto.txt : ' + $autoCls.Count + ' classes')
$L.Add('  types.xml (Lux)          : ' + $typesMap.Count + ' types')
$L.Add('  server log               : ' + $dropped.Count + ' dropped, ' + (Split-Path $log -Leaf))
$L.Add('')
$L.Add('--- 1. world types by category (top 15) ---')
foreach ($g in ($typesMap.Values | Group-Object | Sort-Object Count -Descending | Select-Object -First 15)) {
  $n = $g.Name; if ($n -eq '') { $n = '(none)' }
  $L.Add(('  {0,-16} {1}' -f $n, $g.Count))
}
$L.Add('')
$L.Add('--- 2. equipment in world: ' + $equip.Count + '; NOT in trade: ' + $missEq.Count + ' ---')
foreach ($c in $missEq) { $L.Add('  ' + $c + '   [' + $typesMap[$c] + ']') }
$L.Add('')
$L.Add('--- 3. trader classes NOT in types.xml: ' + $missCfg.Count + ' ---')
foreach ($c in $missCfg) { $L.Add('  ' + $c) }
$L.Add('')
$L.Add('--- 4. dropped by server (no model in any config): ' + $dropped.Count + ' ---')
foreach ($c in $dropped) { $L.Add('  ' + $c) }
$L.Add('')
$L.Add('--- 5. cross-check of key equipment ---')
foreach ($c in @('M4A1','AKM','SVD','AK101','AK74','AKS74U','Colt1911','B95','Aug','ASVAL','Mag_STANAG_30Rnd','Mag_AKM_30Rnd','Mag_CMAG_30Rnd','Ammo_556x45','Ammo_762x39','AmmoBox_556x45_20Rnd','AmmoBox_762x39_20Rnd','PistolSuppressor','HuntingOptic')) {
  $inT = 'no'; if ($typesMap.ContainsKey($c)) { $inT = 'yes:' + $typesMap[$c] }
  $inC = 'no'; if ($cfgAll -contains $c) { $inC = 'yes' }
  $dr  = 'no'; if ($dropped -contains $c) { $dr = 'YES' }
  $L.Add(('  {0,-24} types={1,-18} config={2,-4} dropped={3}' -f $c, $inT, $inC, $dr))
}
[System.IO.File]::WriteAllLines($out, $L, $enc)
Write-Host ('audit: ' + $out + '   lines: ' + $L.Count)
Write-Host ('  configs ' + $cfgAll.Count + ' | world equipment ' + $equip.Count + ' | not in trade ' + $missEq.Count + ' | dropped ' + $dropped.Count)