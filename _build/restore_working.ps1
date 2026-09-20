<#
.SYNOPSIS
  Откат сервера DayZ к проверенной версии TRADER_NPC.

.DESCRIPTION
  Два режима:
    (по умолчанию) -Fast    : взять готовый PBO и исходники из бэкапа и разложить на сервер
                              и клиент + вернуть профильные конфиги. Быстро, байт-в-байт.
    -Rebuild                : git reset --hard на тег, чистая сборка, деплой, проверка.

  После раскладки запускается check_compile.ps1 (кроме -NoCheck).

.EXAMPLE
  .\restore_working.ps1
  .\restore_working.ps1 -Rebuild
  .\restore_working.ps1 -Tag working-before-split -Backup D:\DAYZDISKP\_backup_working_2026-09-20
#>
param(
  [string]$Tag    = "working-2.0-auto",
  [string]$Backup = "D:\DAYZDISKP\_backup_trader_working_2026-09-20_v2",
  [switch]$Rebuild,
  [switch]$NoCheck
)

$ErrorActionPreference = "Stop"
$Repo     = "D:\DAYZDISKP\@TRADER_NPC"
$Srv      = "D:\steam\steamapps\common\DayZServer"
$SrvAddon = $Srv + "\@TRADER_NPC\addons"
$CliAddon = "D:\steam\steamapps\common\DayZ\!Workshop\@TRADER_NPC\addons"
$Prof     = $Srv + "\Profiles\Trader_NPC_Prof"
$Pbo      = "TRADER_NPC.pbo"

Write-Host "=== ОТКАТ TRADER_NPC -> $Tag (режим: $(if ($Rebuild) {'REBUILD'} else {'FAST'})) ===" -ForegroundColor Cyan

if ($Rebuild) {
  if (-not (Test-Path $Repo)) { Write-Error "нет репозитория $Repo"; exit 1 }
  Write-Host "--- git reset --hard $Tag ---"
  git -C $Repo reset --hard $Tag
  git -C $Repo clean -fd addons Profiles
  Write-Host "--- сборка и деплой ---"
  & (Join-Path $Repo "build_pbo.ps1")
  & (Join-Path $Repo "deploy_server.ps1")
} else {
  if (-not (Test-Path (Join-Path $Backup "pbo\$Pbo"))) { Write-Error "нет $Backup\pbo\$Pbo"; exit 1 }
  Write-Host "--- PBO на сервер ---"
  New-Item -ItemType Directory -Force -Path $SrvAddon | Out-Null
  Copy-Item (Join-Path $Backup "pbo\$Pbo") (Join-Path $SrvAddon $Pbo) -Force
  Write-Host "--- исходники (loose, для -filePatching) ---"
  if (Test-Path (Join-Path $SrvAddon "TRADER_NPC")) { Remove-Item (Join-Path $SrvAddon "TRADER_NPC") -Recurse -Force }
  Copy-Item (Join-Path $Backup "src\addons\TRADER_NPC") (Join-Path $SrvAddon "TRADER_NPC") -Recurse -Force
  Write-Host "--- PBO на клиент ---"
  New-Item -ItemType Directory -Force -Path $CliAddon | Out-Null
  Copy-Item (Join-Path $Backup "pbo\$Pbo") (Join-Path $CliAddon $Pbo) -Force
}

if (Test-Path $Backup) {
  Write-Host "--- профильные конфиги ---"
  $n = 0
  Get-ChildItem (Join-Path $Backup "profile") -Filter "*.txt" -ErrorAction SilentlyContinue | ForEach-Object {
    Copy-Item $_.FullName (Join-Path $Prof $_.Name) -Force
    $n++
  }
  Write-Host ("  восстановлено файлов профиля: " + $n)
} else {
  Write-Host "  (бэкапа профиля нет - профиль не тронут)" -ForegroundColor Yellow
}

Write-Host "--- контрольные суммы ---"
$srv = Join-Path $SrvAddon $Pbo
if (Test-Path $srv) {
  "  сервер: " + (Get-FileHash $srv -Algorithm MD5).Hash + "  " + (Get-Item $srv).Length + " байт"
}
$cli = Join-Path $CliAddon $Pbo
if (Test-Path $cli) {
  "  клиент: " + (Get-FileHash $cli -Algorithm MD5).Hash + "  " + (Get-Item $cli).Length + " байт"
}
$bpbo = Join-Path $Backup ("pbo\" + $Pbo)
if (Test-Path $bpbo) {
  "  бэкап: " + (Get-FileHash $bpbo -Algorithm MD5).Hash + "  " + (Get-Item $bpbo).Length + " байт"
}

if (-not $NoCheck) {
  Write-Host "--- проверка (check_compile) ---"
  $out = & (Join-Path $Repo "check_compile.ps1") -TraderCheckSeconds 55
  $out | Select-String "PBO size|RESULT|traders =|loading_config|marker_lines|ERRORS|CHECK:"
  if ($out -match "traders = OK") { Write-Host "=== ОТКАТ OK: торговцы работают ===" -ForegroundColor Green }
  else { Write-Host "=== ОТКАТ: торговцы НЕ подтверждены (см. вывод выше) ===" -ForegroundColor Red }
}
