# Задача: одно компактное окно продажи в стиле ArmorStats (MMB)

Статус: РЕАЛИЗОВАНО В ИСХОДНИКАХ (`C:/aila/@sps_client_bot/addons/sps_client_bot`).
Дальше требуется: деплой на D:/ (deploy_server.ps1) + визуальная проверка в
Workbench / на сервере (подстройка геометрии окон под разрешение при необходимости).

## Итоговое решение (вариант A)
По MMB на предмете в инвентаре всегда открывается **одно компактное окно продажи**
в стиле ArmorStats (ванильная центрированная панель), НЕ большой инспектор.
Показывает только:
- название предмета,
- качество/состояние (pristine…ruined, виджет `ItemDamageWidget`),
- описание (`ItemDescWidget`),
- реальную цену продажи + красную кнопку «Продать», **либо** текст «нет продавца/цены».

ПКМ остаётся нетронутым. Механика открытия/закрытия/слоя — штатный
`MENU_INSPECT` -> `InspectMenuNew` (движок поверх, сам закрывает), не off-screen-хак.

## Файлы, которые правятся/правлены
- `scripts/layouts/TraderSellPopup.layout` — компактный центрированный макет (rootFrame
  full => BgVignette + BackPanel ~43% по центру; name/quality/description + sell-область).
  Виджеты: `ItemNameWidget`, `ItemDamageWidgetBackground`/`ItemDamageWidget`,
  `ItemDescWidget`, `ItemFrameWidget` (обязателен, иначе null в vanilla SetItem),
  `SellRow` (внутри `SellPriceWidget` + `SellButton` userID 90012), `NoPriceWidget`.
- `scripts/5_Mission/TraderInspectMenuExt.c` — `modded InspectMenuNew` единственный
  владелец. `Init()` грузит layout, создаёт ссылки на виджеты.
  Логика в одну точку (`TraderUI_ResolveBuyEntry/ HasBuyRow/ UpdateSellPriceView /
  ApplyView`): если по близлежащему торговцу есть buy-строка (row sell>=0) и предмет
  не сломан => показать SellRow (цена локальная через TraderSmartSellEngine +
  серверная сверка через `RequestSellAppraise`/`ApplyAppraisedSellPrice`), клик по
  кнопке шлёт `RPC_SELL`. Иначе — скрыть SellRow, показать `NoPriceWidget`
  с локал-ключом `#tm_no_trader_nearby`. RUINED => «RUINED - not sellable».

## Ключевые решения / правки против прошлой версии
- Убрана «большая/старая» версия (панель 550×250 без центрирования у верхнего левого
  угла, SellRow никогда не показывался). Теперь центрировано, footer управляемый.
- Кнопка/цена корректно прячутся, если нет цены/продавца (не «SELL» на пустом месте).
- Удалён дублирующий источник `modded class InspectMenuNew` (в 5_Mission лежит один .c,
  нет `.bak`), чтобы не было конфликта двух переопределений.

## Принятые допущения (для визуальной правки)
- Геометрия собрана «на глаз» (скриншот не проверить из IDE): окно фикс 0..1 фракций
  центрировано, текст через "exact text size". Визуально (масштаб окна/видимость текста
  и кнопок) проверить в игре; при необходимости подогнать числа в layout.

## Правило локализации
- Fallback-текст использует существующий ключ stringtable `#tm_no_trader_nearby`.
  НЕ ссылаться на `#tm_no_trader_price` (ключа нет в csv).
