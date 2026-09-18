# HANDOFF — DayZ Trader «продаётся только стоимость оружия, бонус за навесное не начисляется»

## 1. Исходная проблема (запрос пользователя)
При продаже оружия с установленными аксессуарами (прицел, магазин с патронами, хэндгард и т.д.) **платится только цена ствола (головы)**, «бонус» за вложенные предметы равен 0.
Логика бонуса реализована на сервере в `handleSellRPC`, но на практике тест не дал ожидаемого бонуса.

---

## 2. Критически важные системные правила (НЕ НАРУШАТЬ)

### 2.1. Исходники и развёртка
- **Оригинал, где правится код**: `C:/aila/@sps_client_bot/addons/sps_client_bot` (папки `scripts/…`, `config.cpp`, `languagecore`, `images`).
- **Рабочая КОПИЯ (read-only, только для сверки)**: `C:/aila/dayz_mods/sps_client_bot` — **НЕ редактировать**, не создавать там.
- **Развёртываемый мод на сервере (DayZServer, SPS-хостинг)**: `D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons/sps_client_bot` + сборка `D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons/sps_client_bot.pbo`.
- Пути внутри мода фиксированы и вида `sps_client_bot/scripts/…` / `…/scripts/layouts/…` — новую топологию не придумывать.
- **Протокол деплоя перед пересборкой**: см. `deploy_server.ps1` ниже (п. 3.1). Всегда удалять устаревший `.pbo`, вычищать артефакты (scripts/languagecore/images/config.cpp) в развёртке, копировать свежие исходники, сверять MD5. Потом Workbench пересобирает мод из исходников.

### 2.2. Enforce Script (Enfusion) запомни
- НЕТ тернарника `?:` — только if/else.
- НЕЛЬЗЯ использовать void-метод как условие/операнд/аргумент-выражение (`if (obj.Method())` запрещено). Нужно вызывать разрешающий предикат (CanAdd…/IsInherited…), а void-серверный вызов — отдельной строкой.
- Классы-сущности расширять через `modded class …`; виртуальные переопределять `override`, при необходимости звать `super.X()`.
- Синтаксис см. «Enforce Script (Enfusion DayZ) финальный свод» в системном правиле.
- Правила политики продажи Trader: **«S3 + поузловой state»**, «Решение по продаже Trader», применимы как ориентир.

### 2.3. PowerShell в run_terminal_command (правило dayz-sps-terminal-powershell)
- Символы `$` в командах ПОДАВЛЯЮТСЯ инструментом: переменные `$f/$_/$x` превращаются в пустоту → битые присваивания и циклы. Кириллица в консоли выводится как мусор (CP866).
- НЕ писать переменные PowerShell вообще.
- НО: готовый шаблон БЕЗ переменных работает:
  ```
  Get-Item 'C:/path' | Select-Object FullName,LastWriteTime,Length | Format-Table -AutoSize | Out-String -Width 250
  ```
  и поиск строки:
  ```
  Select-String -Path '<абс.путь>' -Pattern 'a','b' | Select-Object LineNumber,Line | Format-Table -AutoSize | Out-String -Width 250
  ```
- Файл фрагментом читать через встроенный `read_file`, а не срезом `(Get-Content …)[n..m]` (нужна переменная).
- grep_search / file_glob_search **НЕ ограничиваются переданным путём**: ищут по всему workspace и находят чужие файлы (pdf_ollama и пр.). Предпочитать `read_file` напрямую по файлу.

### 2.4. Файлы с суффиксами `.bak*` — рабочие резервные копии (`.bak`, `.bak1`, `.bak_exp`, …). Их НЕ трогать и НЕ компилировать как живые; игнорировать при правках.

---

## 3. deploy_server.ps1 и как пересобрать

### 3.1 Протокол переноса исходники → сервер (core)
1. В `D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons` удалить **дочерние** `images, languagecore, scripts` и `config.cpp` (рекурсивно) — артефакты старой сборки.
2. Скопировать из `C:/aila/@sps_client_bot/addons/sps_client_bot` эти же элементы (проверяемые: обычно `scripts`, `config.cpp`; при необходимости `languagecore`/`images`).
3. Удалить `D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons/sps_client_bot.pbo`, чтобы Workbench пересобрал из исходников. Правило двустороннее — всегда синхронизируй с сервером перед каждой компиляцией.
4. Запускать хелпер (создан ранее):
   `powershell -ExecutionPolicy Bypass -File C:/aila/@sps_client_bot/deploy_server.ps1`
   Скрипт: удаляет pbo (с retry при блокировке), чистит 4 элемента, копирует их, сверяет MD5 4 файлов и падает ненулевым кодом при расхождении/заблокированном pbo:
   - `scripts/4_World/TraderSmartSell.c`
   - `scripts/4_World/Entities/DayZPlayerImplement.c`
   - `scripts/5_Mission/TraderInspectMenuExt.c`
   - `languagecore/stringtable.csv`

### 3.2 Факт на момент хэндоффа (проверено датами)
- `C:/aila/….TraderSmartSell.c` — LastWrite 06.09.2026 13:09:54 (8306 байт).
- `C:/aila/….DayZPlayerImplement.c` — 13:22:18 (40338 байт).
- Серверный `.pbo` `D:/steam/…/addons/sps_client_bot.pbo` — **существует**, 06.09.2026 13:35:05 (565763 байт).
**Вывод**: PBO новее правок → серверная сборка УЖЕ содержит бонусную логику. Значит проблема не в устаревшем pbo, а в поведении выполнения. Проверять дальше по логу (п. 6).

---

## 4. Архитектура продажи, как есть (по коду)

### 4.1 Два клиентских пути продажи — оба сходятся на сервере в `handleSellRPC`
1. **Главное меню торговца** — `.../scripts/4_World/TraderMenu.c`, класс `TraderMenu extends UIScriptedMenu`:
   - По клику `m_BtnSell` шлёт:
     `GetGame().RPCSingleParam(m_Player, TRPCs.RPC_SELL, new Param3<int,int,string>(m_traderIndex, m_FilteredListOfTraderItems.Get(row_index).IndexId, ""), true);`
   - `IndexId` // = глобальный индекс строки в `m_Trader_ItemsClassnames`.
2. **Инспекция предмета** — `.../scripts/5_Mission/TraderInspectMenuExt.c`, `modded class InspectMenuNew`:
   - Кнопка SellButton; найдя ближайшего торговца и строку прайса только по его trader (`TraderUI_FindPriceRow` требует `m_Trader_ItemsTraderId.Get(i)==traderIndex`), шлёт:
     `GetGame().RPCSingleParam(player, TRPCs.RPC_SELL, new Param3<int,int,string>(m_sellTraderIndex, m_sellPriceRow, ""), true);`
   - Отображение цены НЕ глобальное (только у этого торговца), а серверный бонус — глобальный.

Оба пути → `handleServerRPCs` (DayZPlayerImplement) → при `RPC_SELL` вызывает `handleSellRPC`.

### 4.2 Серверный обработчик (файл `scripts/4_World/Entities/DayZPlayerImplement.c`)
`modded class DayZPlayerImplement`, метод `void handleSellRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)`:
- читает `Param3<int,int,string>(traderIndex, itemID, displayName)`.
- анти-спам `GetTime()`/`m_Trader_BuySellTimer`.
- `itemType/m_Trader_ItemsClassnames.Get(itemID)`, `itemQuantity/…Quantity`, `itemSellValue/…SellValue`.
- проверки диапазона itemID/traderIndex, дистанции к торговцу, `itemSellValue>=0`.
- `if (!isInPlayerInventory(itemType,itemQuantity,sellableItem)) { PlayerWhite("#tm_you_cant_sell"); return; }`
- если нашелся:
  `bonus = TraderSmartSellEngine.ComputeExtraBonus(this, traderIndex, sellableItem);`
  `sold = RemoveItem(sellableItem, itemType, itemQuantity);`
- если sold:
  `traderTradesLog("sold "+…);`
  `TraderMessage.ServerLog("[TraderPacked] sold " + itemType + " total=" + (itemSellValue+bonus) + " (base=" + itemSellValue + " bonus=" + bonus + ")");`
  если bonus>0: `PlayerWhite(display + "\n#tm_was_sold\n#tm_sold_accessory_bonus: N <валюта>")` ; иначе `"…\n#tm_was_sold"`.
  `increasePlayerCurrency(itemSellValue + bonus);`
> Строки в текущем файле: ~306 начало метода, ~362 вызов ComputeExtraBonus. Файл ~1133 строки, скобки сбалансированы (памятка 135/135). Важно: `RemoveItem`/`deleteItem` при sold==true удаляет голову со всем содержимым; деньги за навесное игрок получает бонусом ДО удаления.

Сопутствующие методы (там же):
- `isInPlayerInventory(itemClassname, amount, out ItemBase item)` — ищет предмет (сперва в руках `GetHumanInventory().GetEntityInHands()`, затем EnumerateInventory PREORDER, пропуская isRuined/isAttached/не CanRemoveEntity).
- `DoItemSellChecks(item, classname, amount)` — `amount==-3`=>magazine, `-4`=>weapon, `-5`=>steak, else по count. Магазин/оружие сразу true.
- `RemoveItem(item, classname, amount)` -> shouldDel => `deleteItem` true; иначе `SetItemAmount(...)`.
- `deleteItem` — HANDS: `LocalDestroyEntityInHands()` иначе `GetGame().ObjectDelete`.
- `increasePlayerCurrency(currencyAmount)` — по `m_Trader_CurrencyClassnames/…Values`.
- Коды TRPCs (`RPC_BUY`,`RPC_SELL`) в `scripts/3_Game/Enums/TRPCs.c`.

### 4.3 Бонусный движок (файл `scripts/4_World/TraderSmartSell.c`)
Класс `TraderSmartSellEngine { }`. Принципы в шапке файла. Статики:
- `FindRowByQuantity(impl, traderIndex, cls, requestedQty)` — глобально по всей `m_Trader_ItemsClassnames`, Sell>=0; предпочтение строки same trader, иначе любой trader.
- `FindSellableRow(impl, traderIndex, cls)` — глобально по имени, Sell>=0 (аналог).
- `Condition(ItemBase ib, int baseSell)` — hp=`ib.GetHealth01("","Health")`; ruined||hp<=0.01 => 1; иначе `val=Math.Round(baseSell*hp)`, мин. 1.
- `AddAccessory(inout total, impl, traderIndex, ItemBase acc)` — строка по `acc.GetType()`; если есть: `total += Condition(acc,row.Sell)`.
- `AddMagazineBonus(inout total, impl, traderIndex, ItemBase magBase)`:
  1) корпус-магазин через AddAccessory;
  2) патроны: `GetAmmoCount()`; если>0: `mag.GetCartridgeAtIndex(0, tmpDamage, firstCart)`; `FindRowByQuantity(firstCart,1)` => row Sell; `perRound = Condition(magBase,perRoundSell)`; `total += perRound * ammoCount`.
- `ComputeExtraBonus(impl, traderIndex, packedHead)`:
  EnumerateInventory PREORDER по `packedHead.GetInventory()`; для каждого ребёнка: magazine => AddMagazineBonus, иначе AddAccessory. Сайд-эффектов нет.

---

## 5. Активный прайс на сервере (Profiles/Trader/TraderConfig.txt)
Путь: `D:/steam/steamapps/common/DayZServer/Profiles/Trader/TraderConfig.txt` (шаблон также `@Trader/extras/Trader/TraderConfig.txt`).
- Формат строки: `Classname, Quantity, BuyPrice, SellPrice`; комментарии `//`.
- Quantity: `*` — единичка/обычный; `W` — оружие; `M` — магазин; `S` — стек/стейк-тип; иначе положительное число (в т.ч. патроны разными кратностями по отдельной строке).
- **TRADER 2 (Weapon Trader)**: оружие (category …), пример `M4A1, W, 10000, 1000`.
- **TRADER 3 (Weapon Supplies)**: патроны `Ammo_556x45, 1, 100, 10` (и кратности 10/30), магазины `Mag_STANAG_30Rnd, M, 400, 40`, аксессуары `M4_CarryHandleOptic, *, 500, 50`; `ACOGOptic, *, 1200, 120`; `M4_PlasticHndgrd, *, 150, 15`; `M4_Suppressor, *, 8780, 878` (все Sell>=0).
- Валюта `<CurrencyName> #tm_ruble`, `<Currency> SPS_Money5000, 1`.
**Следствие**: глобальный поиск строк навесного из Trader3 при продаже винтовки из Trader2 корректен. Конфиг не блокирует бонус.

---

## 6. Диагностическая зацепка (следующий шаг НЕ сделан)
Сервер пишет лог: `D:/steam/steamapps/common/DayZServer/Profiles/TM_GeneralLogs_<дата>.log` (PluginTraderServerLog.m_LogName="TM_GeneralLogs_"; то же через PluginTraderTradesLog "TM_Trades_").
Искать строку: `[TraderPacked] sold <type> total=NN (base=1000 bonus=NN)`. 
- Если bonus>0 при продаже оружия с навесным → бонус работает; тогда разбираться, почему игрок/тест увидел только базу (клиентское сообщение/выдача, по-другому продавал, ошибка окна).
- Если bonus==0 при навесном — причины кандидаты: (а) sellableItem — не «живое» оружие игрока с навесным (продажа «пустого»/из иного контекста); (б) класс/имя cartridge магазина не совпадает со строкой Qty==1 прайса (что внутри магазина); (в) IsMagazine()/GetCartridgeAtIndex дают другое; (г) TraderMenu выбирал не строку головы.
- Если строка лога отсутствует (при продаже оружия) — handleSellRPC не исполнялся этим путём (старый клиент/другое меню) либо лог отключён `TRADER_HIDE_SERVER_LOGS` (в исходнике такого дефайна не использовано; серверные `#ifdef SERVER` на месте).

**Уточнить у пользователя**:
- Через какую меню продавал (главное меню торговца vs Inspect/инспекция предмета с кнопкой SELL).
- Какое оружие/навесное/магазин/патроны держал в руках; какое именно сообщение получил (с бонусом или нет) и численно сколько заплатили.
- Задача возникала до или после сборки pbo 13:35 (т.е. тестировался ли уже актуальный билд).

---

## 7. Локализация (тексты) — уже в наличии
`languagecore/stringtable.csv` (UTF-8/BOM; кириллица искажается только в консоли, сам файл корректен). Ключи в работе: `#tm_was_sold`, `#tm_cant_be_sold`, `#tm_you_cant_sell`, `#tm_added_to_inventory`, `#tm_was_placed_on_ground`, `#tm_not_that_fast`, `#tm_some`, `#tm_ruble`, `#tm_sold_accessory_bonus` (= "Bonus for carried accessories & rounds"); RU «Бонус за вложенные предметы и патроны»; zh/zht/zhs. Используется как: `…\n#tm_was_sold\n#tm_sold_accessory_bonus: <N> <валюта>`.

---

## 8. Контекст/история (не потерять)
- Ранее стойкий баг «weapon не продаётся / VM-краш из-за нуль-пойнтера»; починено null-guard'ом и чисткой устаревшего pbo. Введён flat-sell путь B1 + бонусный computed. Пользователь пока не дал фидбэка о продаже оружия на актуальной сборке pbo 13:35 (новый тест, вероятно, не проводился).
- Существуют файлы `.bak*` (в т.ч. `.bak_b1`, `.bak_exp`, `.bak1`): резервные, НЕ трогать/не сверять по дифу/не удалять. Живые: `TraderSmartSell.c`, `DayZPlayerImplement.c`, `TraderMenu.c`, `TraderInspectMenuExt.c`, `TraderMessage.c`, `TraderNotifications.c`, `InventoryTransactions.c`, plugin-файлы TraderLog.
- `TraderSellDebugHook.c` существует — проверить, что не содержит активного конфликтующего override для продажи (иначе кандидат причины).
- Работа в файлах должна быть на путях `addons/sps_client_bot/scripts/…` и `addons/sps_client_bot/5_Mission/layouts`, НЕ в `C:/aila/dayz_mods` (копия!).

## 9. Рабочие команды-примеры (run_terminal_command; без переменных)
- Список addons сервера:
  `Get-ChildItem 'D:/steam/steamapps/common/DayZServer/@sps_client_bot/addons' | Select-Object Name,Length,LastWriteTime | Format-Table -AutoSize | Out-String -Width 250`
- Даты/длины:
  `Get-Item '<path Src>' , '<path Pbo>' | Select-Object FullName,LastWriteTime,Length | Format-Table -AutoSize | Out-String -Width 250`
- Найти свежие Trader-профильные логи:
  `Get-ChildItem 'D:/steam/steamapps/common/DayZServer/Profiles' -Recurse -Filter 'TM_*.log' | Sort-Object LastWriteTime -Descending | Select-Object FullName,LastWriteTime | Format-Table -AutoSize | Out-String -Width 250`
- Номер строки по паттерну:
  `Select-String -Path '<абс.путь>' -Pattern 'handleSellRPC','ComputeExtraBonus' | Select-Object LineNumber,Line | Format-Table -AutoSize | Out-String -Width 250`
- Деплой перед пересборкой: п. 3.1 / `deploy_server.ps1`.

---

## 10. Отчёт сессии (что сделано 06.09.2026 после pbo 13:35; ошибки и почему)

### 10.1 Выполнено
1. **Удалены все `*.bak*`** из исходников `C:/aila/@sps_client_bot/addons/sps_client_bot/scripts` (9 файлов: `TraderSmartSell.c.bak*`, `DayZPlayerImplement.c.bak*`) и из серверной развёртки `D:/steam/…/DayZServer/@sps_client_bot/...`. 
   ⚠️ УТОЧНЕНИЕ к п.2.4/п.8 этого файла: резервные `.bak*` **были удалены по явному запросу пользователя** («убери файлы *.c.bak* и не добавляй их в мод при деплое»). Теперь это НЕ трогать в обратную сторону: в мод `.bak`-файлы не добавлять. Поскольку `deploy_server.ps1` чистит папку `scripts` целиком и копирует из источника, `.bak` из очищенного источника не попадают в развёртку.
2. **Диагностика по серверным логам** `D:/steam/…/Profiles/TM_GeneralLogs_*.log`:
   - сборка 06:49: `[TraderPacked] PrepareSellPackedItem … FindTraderRow: m_Trader_ItemsTraderId is NULL` → `HEAD ROW NOT FOUND for <type> -> blocked` (тогда «голова» не находилась вообще).
   - сборка pbo 13:35: `[TraderPacked] sold M4A1 total=1000 (base=1000 bonus=0)` ×4 → продажа головы проходит, но **`bonus` всегда 0**. Значит движок вызывается, но `ComputeExtraBonus` на продаваемом инстансе возвращает 0.
   → В `handleSellRPC` путь продажи идёт по прайс-строке головы; `ComputeExtraBonus` вызывается; вопрос — почему на реальном навесном не находится/не учитывается (нет ли обвесов на тестовых M4A1, либо нет найденной sell-строки/патронной строки Qty==1). Требуется тест на **упакованном** предмете.
3. **Добавлена подробная серверная диагностика** в `TraderSmartSell.c` (раздел 10 ниже не удалять):
   - в `AddMagazineBonus`: `TraderMessage.ServerLog("[TraderPacked] DBG mag type=… ammoCount=… firstCart=… cartRow=… perRoundSell=… perRound=…")` — находит ли движок картридж и строку Qty==1 и сколько даёт за патрон.
   - в `ComputeExtraBonus`: лог по каждому дочернему узлу `"[TraderPacked] DBG child[<i>] type=… isMag=Y/N row=… rowSell=… contrib=…"` — что видит EnumInventorpy и по какой цене (rowSell=-1 => нет sell-строки для этого обвеса; contrib — посчитанный вклад узла).
   Эти логи печатаются ДО удаления предмета (предмет ещё существует).
4. **Деплой перед пересборкой**: запущен `C:/aila/@sps_client_bot/deploy_server.ps1` (несколько раз по мере правок). Финально: source↔server MD5 `[ok]` по 4 файлам, `.pbo` удалён → Workbench пересоберёт начисто. Исполнено после последней правки (логика/скобки сбалансированы, тернарники убраны).
5. Работа велась строго в оригинале `C:/aila/@sps_client_bot` (НЕ в `dayz_mods`). Чистка `.bak` также не затронула `addons/Trader` профильные файлы.

### 10.2 Ошибки при этой работе и их исправления (важно для будущего)
**a. Компиляция World: `tradersmartsell.c(210): Invalid statement ')'`**
Причина: временно добавленный **многострочный** `TraderMessage.ServerLog(...)` (конкатенация с переносом строк внутри вызова) ломал парсер на закрывающей `)`.
Исправление: переписать такие `ServerLog` в **одну физическую строку**. В Enforce Script при внесении отладочного лога не размещать конкатенацию по нескольким строкам аргумента — собрать локальную `string` или писать в одну строку.

**b. Компиляция World: `tradersmartsell.c(243): Broken expression (missing ';'?)`**
Причина: использованы **тернарники `?:`** в строке лога: `( dChild.IsMagazine() ? "Y" : "N" )` и `( dRow >= 0 ? impl.m_Trader_ItemsSellValue.Get(dRow) : -1 )`. Enforce Script **НЕ поддерживает** `?:` (см. правило «НЕТ ТЕРНАРНИКА»).
Исправление: заменить на `if/else` + локальные переменные (`string dMag; int dRowSell`). После правки весь файл проверен: `?` осталась только в комментарии; скобки сбалансированы (16/16).

**c. Правило про `.ToString()` на int в конкатенации**
В отладочном логе сперва использовал `int.ToString()` в конкатенации — переписано на авто-конкатенацию `"..." + intValue` (движок сам конвертирует int), чтобы не рисковать. В Enforce предпочитают `"..."+${int}` как в `handleSellRPC` (уже компилировалось).

**d. PowerShell: `ParserError: TerminatorExpectedAtEndOfString`**
При попытке `Select-String` с большим списком паттернов, частично содержащих спецсимволы/перенос внутри кавычек (`map<K<'…`) — команда падала у оболочки.
Исправление/урок: в `run_terminal_command` использовать только готовые безопасные однострочные шаблоны без `$` и без спецсимволов/разрывов. Для паттернов с `[ ] < > ( )` — пользоваться `read_file`/`grep_search`, а не терминалом. (Записано отдельным правилом про quoting и `$`.)

**e. Замечание по grep_search/file_glob_search**
Эти инструменты НЕ ограничиваются переданным путём и шарят по всему workspace (находят чужие `pdf_ollama`, лицензии и т.п.), флудят результат. Для контроля использовать `read_file` напрямую либо `Select-String` по точному файлу.

### 10.3 Текущее состояние и следующий шаг (НЕ выполнен)
- Код `TraderSmartSell.c` синтаксически чист (нет тернарников, однострочные `ServerLog`, скобки в норме), источник ↔ сервер синхронны, pbo удалён.
- **Нужна** пересборка мода в Workbench из `D:/steam/…/DayZServer/@sps_client_bot`, запуск сервера и **тест продажи упакованного** оружия (напр. M4A1 с прицелом + глушителем + заряженным магазином).
- Затем снять строки `DBG child[…]/DBG mag …` и `sold … bonus=…` из `TM_GeneralLogs_*.log` и определить причину bonus==0: (a) на тестовом экземпляре реально не было навесного; (b) для обвеса нет sell-строки (row=-1 / rowSell=-1); (c) движок не находит патронную строку Quantity==1 (cartRow=-1) — тогда правка в поиске строк/кофиге; (d) магазин вообще не попадает в EnumInventorpy (проверить по `DBG child`).
