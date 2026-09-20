# PROVENANCE — происхождение кода TRADER_NPC (бывший TRADER_NPC)

Документ отвечает на два вопроса: **насколько наш код отличается от оригинала** и **почему из этого следуют GPL-3.0 и обязательные кредиты**.

Метод: построчное сравнение (LCS), побайтовое сравнение (MD5), множества непустых строк. База «ванили» — распакованные исходники DayZ (scripts/ + gui/layouts): 502 929 строк в 3 026 файлах.

---

## 0. Резюме в трёх пунктах

1. Ядро трейдера переписано: 55–99 % строк ключевых файлов отличаются от оригинала.
2. Но **20 наших файлов — это их код с точностью до 1–4 строк**, а scripts/4_World/FileReadHelper.c — **побайтово идентичен их файлу** (MD5 e35b67dc0d46f88ae64be2a815452753, 1916 байт).
3. Значит производность доказана не «по смыслу», а буквально по содержимому файлов → весь наш мод обязан распространяться под **GPL-3.0** с сохранением их копирайта и пометкой изменений.

---

## 1. Объёмы

| | файлов кода | строк (непустых) |
|---|---|---|
| Ваниль DayZ (референс) | 3 026 | 502 929 |
| PBO-Tools/Dr-Jones-Trader-Rework (их) | 51 | 6 015 |
| TRADER_NPC / TRADER_NPC (наш) | 66 | 7 180 |

Из них у них: 31 файл — правки ванильных классов, 20 файлов — собственная логика. У нас: 37 правок ванильных классов, 29 собственных файлов.

Уникальность к ванили: у них 55 % строк не встречается в ванили, у нас 50 %. Уникальность наших неванильных строк относительно их кода: **61 % (2 199 строк) не встречается ни в ванили, ни у них** — это наша работа; остальные ~1 400 строк — унаследованный их трейдерный код.

---

## 2. Категория A — ИХ КОД, ОСТАВШИЙСЯ ПОЧТИ ДОСЛОВНО (20 файлов)

Распределение по числу различающихся строк (из 40 сопоставленных файлов):

| различие | файлов | что это |
|---|---|---|
| **0 строк (байт-в-байт)** | **1** | FileReadHelper.c |
| 2–3 строки | **18** | 17 × ActionXxx.c + TrapBase.c (по 2), DayZPlayerMeleeFightLogic_LightHeavy.c (3) |
| 4–8 строк | **4** | ActionRestrainTarget.c (4), traderDefines.c (4), ActionConstructor.c (5), Ammunition_Base.c (6) |
| 9–20 строк | 4 | ActionDetach.c (9), stringtable.csv (10), TRPCs.c (14), ItemBase.c (16) |
| 21–60 строк | 1 | Grenade_Base.c |
| 61+ строк | 12 | ядро трейдера (переписано) |

### 2.1 FileReadHelper.c — побайтовое совпадение

~~~
наш: scripts/4_World/FileReadHelper.c   md5 e35b67dc0d46f88ae64be2a815452753   1916 байт
их:  scripts/4_World/FileReadHelper.c   md5 e35b67dc0d46f88ae64be2a815452753   1916 байт
равны побайтово: true     строк: 85 / 85
~~~

Это не «похожий файл», а их файл целиком. Именно им пользуется вся наша серверная загрузка прайса: **FileReadHelper.TrimComment**, **FileReadHelper.TrimSpaces**, **FileReadHelper.SearchForNextTermInFile**, **SearchForNextTermsInFile** — вызовы стоят в нашем missionServer.c (парсер TraderConfig.txt).

### 2.2 Восемнадцать файлов, отличающихся ОДНОЙ строкой

Самый весомый аргумент: совпадает весь текст, включая отступы, а отличается ровно одно выражение — переименованное поле:

| наш вариант | их вариант | файлы |
|---|---|---|
| player.IsInSafeZone() | player.m_Trader_IsInSafezone | ActionDeployObject, ActionForceConsume, ActionForceFeedCan, ActionLockDoors, ActionRestrainSelf, ActionBurnSewTarget, ActionCollectBloodTarget, ActionDefibrilateTarget, ActionGiveBloodTarget, ActionGiveSalineTarget, ActionSewTarget, ActionForceConsumeSingle, ActionUnpin, ActionDisinfectTarget, ActionInjectTarget, TrapBase, ActionRestrainTarget, DayZPlayerMeleeFightLogic_LightHeavy |
| ntarget && ntarget.IsTrader() | ntarget.m_Trader_IsTrader | ActionCheckPulse |

То есть мы взяли их патчи и заменили прямое обращение к полю на геттер, добавив в одном месте null-check. Всё остальное — их код.

### 2.3 Остальные близкие файлы

- scripts/defines/traderDefines.c — 6 против 9 строк, те же дефайны (TRADER);
- .../ActionConstructor.c — их регистрация ActionTrade (10 против 13);
- .../Ammunition_Base.c — их AddQuantityTR/SetQuantityTR (48 против 50, различий 6 строк);
- scripts/3_Game/Enums/TRPCs.c — тот же enum-протокол (29 против 30, различий 14; мы добавили RPC_APPRAISE_SELL/_REPLY);
- .../ItemBase.c — тот же AddQuantityTR/SetQuantityTR (43 против 59);
- languagecore/stringtable.csv — 51 против 41, различий 10: у нас ОСТАЛИСЬ их ключи STR_tm_1_ruble_note … STR_tm_100_ruble_note, STR_tm_vehicle_key, STR_tm_ruble_description — прямое наследие (рубли и ключи от машин).

---

## 3. Категория B — ИХ КОД, ПЕРЕРАБОТАННЫЙ (ядро трейдера)

| файл | наш / их строк | различий |
|---|---|---|
| Entities/DayZPlayerImplement.c | 1306 / 1532 | 85 % |
| mission/missionServer.c | 1123 / 1390 | 81 % |
| ManBase/PlayerBase.c | 254 / 38 | 84 % |
| 3_Game/Globals.c (их TraderFix/Globals.c + KitsToIgnore.c) | 87 / 36 | 89 % |
| config.cpp | 68 / 203 | 85 % |
| TraderMenu.c | 853 / 1053 | 58 % |
| TraderMessage.c | 118 / 61 | 59 % |
| TraderNotifications.c | 158 / 186 | 55 % |
| missionGameplay.c | 16 / 72 | 75 % |
| Grenade_Base.c | 18 / 68 | 67 % |

Важно: «переписано» здесь означает «мы правили их файл», а не «написали с нуля». Сохранились их имена методов (handleServerRPCs, handleBuyRPC, handleSellRPC), их структура данных игрока (m_Trader_Items*, m_Trader_TraderPositions, m_Trader_Currency*), их схема RPC и их формат прайса.

---

## 4. Категория C — то, что есть только у нас (29 файлов)

Крупнейшие: TraderSellPopup.layout 1011, TraderMenu.c 730, TraderInspectMenuExt.c 453, TraderSmartSell.c 285, TraderMenu.layout 254, TraderRating.layout 254, SafeZone.c 186, TraderNotifications.c 138, ActionTrade.c 137, TraderMessage.c 108, TraderItemInspectSell.layout 105, PluginTraderLogBase.c 100, TraderRating.c 91, TraderSellInput.c 90, Globals.c 76, TraderRatingUI.c 72, InventoryTransactions.c 64, TraderNotification.layout 57, ChemGas_Grenade.c 56, Ammunition_Base.c 45, TRPCs.c 29, TraderNotificationsContainer.layout 23, TraderSellRMB.c 21, Animations.c 12, PluginTraderServerLog.c 9, PluginTraderTradesLog.c 9, BuildingBase.c 8, traderDefines.c 6 (плюс сам FileReadHelper.c — он в категории A).

Оговорка: не все они «наши» в смысле авторства — часть (SafeZone.c, плагины логов, InventoryTransactions.c, DayZGame.c, VONManager.c, BuildingBase.c, ChemGas_Grenade.c) происходит из более широкой линии Trader/WSP, которой в фикс-репозитории нет вовсе. В кредитах их тоже нельзя приписывать только нам.

---

## 5. Следы их мода, оставшиеся в нашем коде

1. FileReadHelper.c — байт-в-байт.
2. **Формат прайса и парсер**: TraderConfig.txt, TraderObjects.txt, TraderVehicleParts.txt, TraderVariables.txt, TraderAdmins.txt; теги <CurrencyName>, <Currency>, <Trader>, <Category>, <OpenFile>, <TraderMarker>, <VehicleSpawn>, <FileEnd>; строка Classname, Quantity, BuyPrice, SellPrice с кодами W/M/S. Весь наш readTraderData() построен на их схеме — мы лишь добавили теги <Rating*>.
3. Enum TRPCs — тот же протокол RPC (RPC_BUY, RPC_SELL, RPC_SEND_TRADER_*).
4. Имена полей и методов: m_Trader_*, handleSellRPC, HandleSendNotificationRPC.
5. Локализация: ключи #tm_* и их остатки STR_tm_*_ruble_note, STR_tm_vehicle_key.
6. Макеты: TraderMenu.layout и TraderNotification.layout — их макеты, правленые; в TraderMenu.layout осталась их маска ассетов Mask "{AD4618E43F128FE1}WillowGlade/WG_TraderFix/imagesets/search.edds" и стили rover_sim_colorable.
7. config.cpp: структура CfgMods / CfgPatches и dependencies {Game, World, Mission} — их шаблон (мы убрали объявления предметов).

---

## 6. Почему производность бесспорна (техническая сторона)

Правовая конструкция «производная работа» (GPL-3.0 §0) срабатывает не только при дословном копировании. Здесь есть оба основания:

1. **Дословное воспроизведение.** FileReadHelper.c совпадает побайтово, 20 файлов совпадают с точностью до одной переименованной конструкции. Это прямое копирование исходного текста.
2. **Переработка исходного текста.** Ещё 20+ файлов — их файлы, изменённые нами: сохранены их структура, имена, алгоритмы. Переработка чужого кода — классическая производная работа.
3. **Заимствование архитектуры и форматов.** Протокол RPC, схема данных игрока, формат конфигов и парсер, локализационные ключи — их дизайн, а не наш.

Достаточно любого одного пункта; здесь выполняются все три.

---

## 7. Что из этого следует по GPL-3.0

Апстрим объявляет GPL-3.0 в README (файла LICENSE в репозитории при этом нет — ссылка в README ведёт на отсутствующий файл). Трактyем лицензию как GPL-3.0 и соблюдаем её.

1. **Сохранить уведомления** (§4, §5): имя автора, ссылку на апстрим, текст лицензии. Файл LICENSE (полный текст GPL-3.0) обязан лежать в нашем репозитории.
2. **Пометить изменения** (§5a): prominent notices stating that you modified it, and giving a relevant date. Практически — раздел «Changes since upstream» в README плюс строка в шапке каждого изменённого файла: // Modified 2026-09-18 by SN1054NGC (TRADER_NPC): <кратко>.
3. **Лицензировать всё произведение под GPL-3.0** (§5c): нельзя закрыть, нельзя перелицензировать в MIT/проприетарную, нельзя продавать как закрытый продукт.
4. **Давать Corresponding Source** (§6) каждому, кому распространяем мод: исходники в репозитории плюс ссылка на репозиторий в описании Steam Workshop.
5. **Не добавлять дополнительных ограничений** (§10): формулировки «нельзя использовать на коммерческих серверах», «нельзя форкать», «запрещено перераспространять» несовместимы с GPL-3.0.
6. **Никаких гарантий** (§11) — покрыто текстом лицензии.
7. Публикация производной уже является распространением, обязанности возникают даже если мод бесплатный и продаж нет.

### 7.1 Нюансы

- **Файла LICENSE в апстриме нет.** Декларация в README — заявление автора о намерении. Для строгой чистоты: сохранить доказательство (ссылка/скриншот README), положить наш LICENSE (GPL-3.0) и уведомить автора письмом (это и вежливость, и фиксация отсутствия возражений).
- **Ассеты Bohemia Interactive** (ванильные текстуры, модели, шрифты) регулируются правилами BI для DayZ-модов: можно использовать в бесплатных модах, нельзя продавать. GPL-3.0 этому не противоречит, но ассеты остаются BI.
- **Ассеты из других модов** (например валюта SPS_Money* из sps_item) — у них своя лицензия; перечислить отдельно.
- **Их контент** (ruble/keyLada/Road_Cone/hoodie + VehicleKey.c, ActionLockUnlockVehicle.c, CarScript.c, DayzPlayer.c) мы не используем; если когда-нибудь возьмём — он тоже под GPL-3.0 как часть их репозитория.
- **Steam Workshop**: BI требует, чтобы автор имел права на контент; GPL-декларация не мешает, но описание должно содержать кредиты и ссылку на исходники.

---

## 8. Готовые тексты

### 8.1 Блок в README (Credits & License)

~~~markdown
## Credits & License

This project is a derivative work based on Dr Jones Trader ReWork
(https://github.com/PBO-Tools/Dr-Jones-Trader-Rework) by PBO-Tools / Nick,
a community fix for the DayZ trader mod originally created by Dr_J0nes
(the author field of the upstream config.cpp). The upstream repository is
archived; its last code change was 2023-07-30.

The upstream project declares the GNU General Public License v3.0; this fork is
distributed under the same license - see LICENSE. Original copyright (C) 2023
PBO-Tools / Dr_J0nes. Authorship of the original trader logic, the
TraderConfig.txt format and its parser, the RPC protocol and the FileReadHelper
utilities remains with the upstream authors.

Thanks also to the authors of the WSP / WillowGlade trader layouts (the asset
mask WillowGlade/WG_TraderFix is still referenced by TraderMenu.layout).

### Changes since upstream (GPL-3.0 section 5a)

- 20 files are kept almost verbatim from upstream. scripts/4_World/FileReadHelper.c
  is byte-identical (md5 e35b67dc0d46f88ae64be2a815452753); the ActionXxx,
  ItemBase and TrapBase safezone patches differ by a single renamed field
  (player.IsInSafeZone() instead of player.m_Trader_IsInSafezone).
- The trader core was reworked: DayZPlayerImplement.c (85% of lines changed),
  missionServer.c (81%), PlayerBase.c (84%), TraderMenu.c (58%), TraderMessage.c (59%),
  TraderNotifications.c (55%), config.cpp (85%).
- Removed upstream content we do not use: ruble currency items, vehicle keys,
  road cone, hoodies, vehicle actions and CarScript.
- Added by us (not present upstream): TraderSmartSell.c, TraderInspectMenuExt.c,
  TraderSellInput.c, TraderSellRMB.c, TraderRating.c, TraderRatingUI.c,
  TraderSellPopup.layout, TraderRating.layout, TraderItemInspectSell.layout,
  TraderNotificationsContainer.layout and the survival-rating trader discount.
~~~

### 8.2 Шапка изменённого файла

~~~c
// Based on Dr Jones Trader ReWork (PBO-Tools/Nick, GPL-3.0), itself a fix for the
// DayZ trader mod by Dr_J0nes. See LICENSE and README.
// Modified 2026-09-18 by SN1054NGC (TRADER_NPC): <что именно изменено>
~~~

### 8.3 Письмо автору (EN)

~~~
Hi Nick,

Thanks for Dr Jones Trader ReWork - it fixed exactly the problems we were
hitting (items not appearing in the inventory, NULL pointer crashes, unsafe
down-casting). We are continuing that work as an open fork called TRADER_NPC:
<link>.

We keep the GPL-3.0 license, your copyright notice and the full upstream
history; the README documents every change we made, and your FileReadHelper.c
is left byte-identical. If you would rather we did not use it this way, just
tell us and we will adjust immediately.

Thanks again for publishing the fix.
~~~

---

## 9. Чек-лист публикации

- [ ] LICENSE = полный текст GPL-3.0
- [ ] README: Credits & License + Changes since upstream (раздел 8.1)
- [ ] CREDITS.md: PBO-Tools/Nick, Dr_J0nes, WSP/WillowGlade + сторонние ассеты
- [ ] Шапки Modified ... в изменённых файлах (раздел 8.2)
- [ ] config.cpp: author / credits / version
- [ ] meta.cpp: name = TRADER_NPC (publishedid не менять, если мод уже в Workshop)
- [ ] Steam Workshop: кредиты + ссылка на исходники + текст лицензии
- [ ] История git от апстрима (форк или импорт), ветка rework
- [ ] Письмо автору отправлено (раздел 8.3)
