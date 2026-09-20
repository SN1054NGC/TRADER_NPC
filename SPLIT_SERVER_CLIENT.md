# Разделение TRADER_NPC на публичный клиент и серверный мод (защита серверной части)

Статус: план (обсуждается). Записано 2026-09-20.

## 1. Чего хотим
- **Публичный клиентский мод** `@TRADER_NPC` — всё, что нужно игроку: интерфейс, layout, клиентские обработчики.
- **Серверный мод** `@TRADER_NPC_Server` (в `-servermod=`) — вся логика, которая «делает» торговца: конфиги и цены, спавн, сейф-зоны, RPC-обработчики, мосты к сайту и базе.
- **Без серверного мода торговля не работает** ни у кого — клиент ждёт подтверждения от сервера.
- Публичный репозиторий (GitHub) содержит **только клиентскую часть**.

## 2. Что это даёт и чего НЕ даёт (честно)
**Даёт:** серверную логику и данные нельзя скопировать из публичного репозитория — их там просто нет. Публичный клиент сам по себе бесполезен как «рабочий мод».
**Не даёт:** шифрования. Скрипты Enforce внутри PBO **не шифруются** — PBO вскрывается (PBO Manager/PBOConsole). Обфускация (например, pbo.tools) только затрудняет чтение. Реальная защита = держать ценное (логику, цены, БД) на серверной стороне и не публиковать.

## 3. Механизм движка
- `-mod=` — аддон грузится и на сервере, и на клиенте.
- `-servermod=` — аддон грузится **только на сервере**, клиентам он не нужен.

Значит: клиентская часть идёт в `-mod=`, серверная — в `-servermod=`. Чтобы клиент умел говорить с сервером, обе части должны совпадать по:
- enum RPC (`TRPCs`) — одинаковые значения;
- именам синхронизируемых переменных (`RegisterNetSyncVariable*`) — клиент обязан знать имена, поэтому «тонкие» заглушки полей и регистрации остаются в клиентском моде;
- путям layout и именам виджетов (клиентские).

Хендшейк уже есть в моде: клиент отправляет `RPC_TRADER_MOD_IS_LOADED`, сервер отвечает `RPC_TRADER_MOD_IS_LOADED_CONFIRM`.

## 4. Что куда переезжает
| Клиент (`@TRADER_NPC`, публичный) | Сервер (`@TRADER_NPC_Server`, `-servermod=`) |
|---|---|
| `scripts/layouts/*` (все окна) | `mission/missionServer.c` — чтение конфигов, спавн торговцев, сейф-зоны |
| `4_World/TraderMenu.c`, `TraderInspectMenuExt.c`, `TraderSellInput.c`, `TraderSellRMB.c` | `5_Mission/TraderAutoPrices.c` (генератор цен) |
| `5_Mission/TraderRatingUI.c`, `TraderRatingHud.c`, `TraderNpcMapGps.c` | серверные ветки `DayZPlayerImplement.c` (handleBuy/Sell/Appraise, валюта, стаки) |
| `4_World/TraderNotifications.c`, `TraderMessage.c` (показ) | `4_World/TraderSmartSell.c`, `TraderTradeRules.c` (оценка) |
| `3_Game/Enums/TraderNpcRPCs.c` (общий, дублируется) | `4_World/TraderKillReward.c` (хук убийства) |
| `3_Game/TraderRating.c` (математика, общая) | мост к сайту/БД (коины, хранилище) |
| «тонкие» заглушки полей игрока + `RegisterNetSyncVariable*` | всё под `#ifdef SERVER`, доступ к файлам профиля |

Патчи ванильных классов (сейф-зона, `WeaponManager`, user actions, гранаты) — **серверные**, но часть из них срабатывает и на клиенте; их место определяется по факту (`#ifdef SERVER` уже расставлены).

## 5. Как «не работает без серверного мода»
1. Клиент при входе ждёт `RPC_TRADER_MOD_IS_LOADED_CONFIRM`; нет ответа → окно торговца не открывается, показывается сообщение «Серверная часть TRADER_NPC не установлена».
2. Все клиентские запросы (покупка/продажа/оценка/коины) уходят серверу; без серверной части они никем не обрабатываются → эффекта нет.
3. Серверный мод — единственное место, где читаются `TraderConfig.txt` / `TraderVariables.txt` / `TraderObjects.txt` и создаются торговцы; без него на сервере нет ни NPC, ни цен.

## 6. Лицензия (важно)
Наш мод — производная от **GPL-3.0** (апстрим PBO-Tools/Dr-Jones-Trader-Rework, лицензия заявлена в его README).
- Код мода (и клиентский, и серверный) остаётся **GPL-3.0**; исходники должны быть доступны **получателям** сборки.
- Поэтому: публичный клиент публикуем **с исходниками** (как сейчас в репозитории), а серверный PBO отдаём **владельцу сервера** — он получает исходники как получатель (это GPL не нарушает), но в публичный репозиторий серверную часть не выкладываем.
- Данные (цены, конфиги, сайт/БД, ленджер коинов) — это наши данные/сервис, не код апстрима; их публиковать не нужно.
- **Нельзя** закрывать исходники того, что мы распространяем как PBO: это прямое требование GPL-3.0.

## 7. Этапы работ
1. Вынести `TRADER_NPC_Server` как отдельный аддон: `addons/TRADER_NPC_Server/{config.cpp, scripts/...}`; перевести сборку на два PBO.
2. Перенести серверные файлы (таблица §4), поправив `#ifdef` и убрав серверные вызовы из клиентских файлов.
3. Оставить в клиентском моде: layouts, UI, enum RPC, математика рейтинга, заглушки sync-полей, ожидание хендшейка.
4. `config.cpp` серверного мода: свой `CfgMods`, `hideName=1`, `author/credits`, `requiredAddons` (в т.ч. клиентский аддон).
5. Сборка: `build_pbo.ps1` → два PBO; серверный кладём в `@TRADER_NPC_Server`, `start.bat`: `-servermod=...;@TRADER_NPC_Server`.
6. Хендшейк-гейт в клиентском UI (жёсткий отказ или предупреждение — решить).
7. По желанию: обфускация серверного PBO (pbo.tools) — «затруднить», не «защитить».

## 8. Что решить
1. Имя серверного аддона: `TRADER_NPC_Server` (или `@TRADER_NPC_S`)?
2. Гейт жёсткий (без серверного мода торговли нет вообще) или мягкий (предупреждение + неработающие кнопки)?
3. Раздавать серверный мод только владельцам серверов вместе с исходниками (GPL)? Обфусцировать или нет?
4. Коины/сайт: серверный мост к БД оставляем только в серверном моде (в публичном клиенте — ничего)?
5. Что именно из «патчей ванилы» (сейф-зона, `WeaponManager` и т.п.) обязано быть на клиенте, а что можно унести на сервер?

---

## Результат анализа зависимостей (2026-09-20)

Проверено скриптом `_split_analysis.js` по всем 67 `.c` файлам аддона:
файл считается серверным, если он объявляет серверный класс (`MissionServer`,
`TraderAutoPrices`, `TraderSmartSell`, `TraderTradeRules`, `TraderKillReward`,
`TraderNpcProfile`, `PluginTrader*`) или пишет в `$profile:` и при этом не строит виджеты;
клиентским — если объявляет клиентский класс/строит виджеты (`CreateWidgets`).

```
=== ТОЛЬКО СЕРВЕР (в приватный аддон): 9 ===
  1565 строк  4_World/Entities/DayZPlayerImplement.c  [#ifdef SERVER]  [профиль]
  1380 строк  5_Mission/mission/missionServer.c  [#ifdef SERVER]  [профиль]
   655 строк  5_Mission/TraderAutoPrices.c  [профиль]
   519 строк  4_World/TraderSmartSell.c  [#ifdef SERVER]
   116 строк  4_World/Plugins/PluginTraderLogBase.c  [#ifdef SERVER]  [профиль]
   106 строк  4_World/TraderTradeRules.c  [профиль]
    84 строк  5_Mission/TraderNpcProfile.c  [профиль]
     9 строк  4_World/Plugins/PluginTraderServerLog.c  [#ifdef SERVER]
     9 строк  4_World/Plugins/PluginTraderTradesLog.c  [#ifdef SERVER]
=== ТОЛЬКО КЛИЕНТ (в публичный аддон): 7 ===
  1415 строк  4_World/TraderMenu.c  [виджеты]
   548 строк  5_Mission/TraderInspectMenuExt.c  [виджеты]
   216 строк  5_Mission/TraderRatingUI.c  [виджеты]
   190 строк  5_Mission/TraderRatingHud.c  [виджеты]
   168 строк  4_World/TraderNotifications.c  [виджеты]
   118 строк  4_World/TraderMessage.c  [#ifdef SERVER]
   104 строк  5_Mission/TraderSellInput.c
=== СМЕШАННЫЕ (нужна работа с #ifdef): 52 ===
   298 строк  4_World/Entities/ManBase/PlayerBase.c  [#ifdef SERVER]
   204 строк  4_World/Classes/SafeZone/SafeZone.c  [#ifdef SERVER]
   157 строк  4_World/Classes/UserActionsComponent/Actions/Trader/ActionTrade.c
   136 строк  3_Game/TraderRating.c
   102 строк  3_Game/TraderNpcFeatures.c
    92 строк  3_Game/TraderNpcText.c
    87 строк  3_Game/TraderNpcHelper.c
    74 строк  4_World/InventoryTransactions.c  [#ifdef SERVER]
    61 строк  4_World/Entities/ItemBase/ChemGas_Grenade.c
    48 строк  4_World/Entities/Ammunition_Base.c
    46 строк  5_Mission/TraderNpcMapGps.c
    45 строк  3_Game/VONManager.c
    44 строк  3_Game/DayZGame.c
    43 строк  4_World/Entities/ItemBase.c
    36 строк  4_World/Plugins/PluginBase/PluginTransmissionAgents.c
    36 строк  4_World/TraderKillReward.c
    34 строк  3_Game/Enums/TraderNpcRPCs.c
    23 строк  5_Mission/TraderSellRMB.c
    20 строк  5_Mission/mission/missionBase.c
    18 строк  4_World/Entities/ItemBase/Grenade_Base.c
    17 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/ActionRestrainTarget.c
    17 строк  4_World/Entities/ItemBase/TraderNpcMoney.c
    16 строк  4_World/Classes/ContaminatedArea/ContaminatedArea_Local.c
    16 строк  4_World/Plugins/PluginManager.c  [#ifdef SERVER]
    16 строк  5_Mission/mission/missionGameplay.c
    15 строк  4_World/Classes/PlayerModifiers/Modifiers/Conditions/AreaExposure.c
    14 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionCheckPulseTarget.c
    14 строк  4_World/Entities/ItemBase/TrapBase.c
    14 строк  4_World/Entities/ManBase/DayZPlayer/DayZPlayerMeleeFightLogic_LightHeavy.c
    13 строк  4_World/Animations.c
    12 строк  4_World/Classes/TransmissionAgents/Agents/InfluenzaAgent.c
    11 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/DeployActions/ActionDeployObject.c
    11 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionCheckPulse.c
    11 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionCollectBloodTarget.c
    10 строк  4_World/Classes/UserActionsComponent/ActionConstructor.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/ActionForceConsume.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/ActionForceFeedCan.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/ActionLockDoors.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/ActionRestrainSelf.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionBurnSewTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionDefibrilateTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionGiveBloodTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionGiveSalineTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/Continuous/Medical/ActionSewTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/SingleUse/ActionForceConsumeSingle.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/SingleUse/ActionUnpin.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/SingleUse/Medical/ActionDisinfectTarget.c
    10 строк  4_World/Classes/UserActionsComponent/Actions/SingleUse/Medical/ActionInjectTarget.c
    10 строк  4_World/Classes/Weapons/WeaponManager.c
     8 строк  4_World/Entities/BuildingBase.c
     7 строк  4_World/Classes/UserActionsComponent/Actions/Interact/ActionDetach.c
     7 строк  defines/traderDefines.c
```

### Вывод: чистого разделения по файлам нет

* **9 файлов только серверные** — их можно целиком унести в приватный аддон.
* **7 файлов только клиентские** — остаются в публичном.
* **51 файл смешанный.** Главные из них:
  * `scripts/4_World/Entities/DayZPlayerImplement.c` (1556 строк) — в одном классе и клиентский
    `OnRPC`/чат-уведомления, и серверные `handleBuyRPC`/`handleSellRPC`/`increasePlayerCurrency`;
  * `scripts/4_World/Entities/ManBase/PlayerBase.c` — сейф-зона (сервер) + рейтинг/HUD (клиент);
  * `scripts/4_World/TraderMenu.c` (клиент) **вызывает** методы из серверного файла
    (`getPlayerCurrencyAmount`), поэтому просто выкинуть серверный файл из клиентского PBO нельзя.
* Разделение вида «выкинуть серверные файлы из клиентского PBO» ломает компиляцию клиента.

### Как разделить правильно (архитектура-заготовка)

1. Публичный `@TRADER_NPC` (клиент): UI/layouts/сеть + **пустые виртуальные заглушки**
   (`class TraderNpcBackend { void HandleBuy(...) {} ... }`) и весь RPC-плумбинг под `#ifdef SERVER`,
   который только вызывает эти заглушки.
2. Приватный `@TRADER_NPC_Server` (servermod): `modded class TraderNpcBackend` с `override` —
   там остаётся вся закрытая логика (цены, скупка, награды, профиль, авто-цены).
   Enforce разрешает `modded class` + `override` для класса, объявленного в другом аддоне,
   поэтому серверный код не попадает в публичный PBO.
3. Проверка: `check_compile.ps1` (сервер компилирует оба модуля) + отдельная сборка двух PBO.

> Пока пункты 1-3 не сделаны, публиковать `DayZPlayerImplement.c` нельзя — в нём лежит серверная логика.
