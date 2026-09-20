# ИНТЕРФЕЙС И LAYOUT — КАК УСТРОЕНО И КАК ПОЗИЦИОНИРУЕТСЯ

Документ для @TRADER_NPC (DayZ 1.29). Всё измерено на реальных файлах игры и нашего мода.
Инструменты, которыми это проверяется: `_build/validate_layouts.js`, `_build/audit_my_fonts.js`, `layout_viewer.html`.

## 1. ГДЕ ЛЕЖИТ И КАК ЗАГРУЖАЕТСЯ

Файлы: `addons/TRADER_NPC/scripts/layouts/*.layout` (внутри PBO путь `TRADER_NPC/scripts/layouts/...`).

| Файл | Строк | Назначение |
|---|---|---|
| `TraderMenu.layout` | 767 | главное окно торговца (корень 1300x990) |
| `TraderSellPopup.layout` | 1027 | окно продажи по ПКМ на предмете в инвентаре (корень 1920x1080) |
| `TraderRating.layout` | 265 | значок рейтинга выживания в инвентаре |
| `TraderNotificationsContainer.layout` | 24 | контейнер уведомлений (позиция 772,60) |
| `TraderNotification.layout` | 57 | одна строка уведомления (шаблон, создаётся на каждое сообщение) |
| `TraderItemInspectSell.layout` | 105 | вспомогательная карточка предмета |

Загрузка (Enforce Script):

    Widget root = GetGame().GetWorkspace().CreateWidgets("TRADER_NPC/scripts/layouts/TraderMenu.layout");
    TextWidget t = TextWidget.Cast(root.FindAnyWidget("text_saldo"));

Путь = префикс PBO (`TRADER_NPC`, задан в `build_pbo.ps1` как `$PREFIX$`) + путь внутри аддона.
Один и тот же layout можно создать много раз — получится много независимых экземпляров (так работает строка уведомления).

## 2. СИНТАКСИС DSL

    КлассВиджета ИмяВиджета {
     свойство значение          // атомарные свойства
     "свойство с пробелом" 1
     {                          // вложенные виджеты (дети)
      TextWidgetClass Подпись { text "..."; font "..."; }
     }
    }

Комментарии: `//`. Имя виджета — то, по чему его находит код (`FindAnyWidget("Имя")`). Если имя не нужно коду,
можно писать произвольно, но лучше осмысленно: по нему строится диагностика валидатора.

## 3. КЛАССЫ ВИДЖЕТОВ, КОТОРЫЕ МЫ ИСПОЛЬЗУЕМ

| Класс | Что это | Особенности |
|---|---|---|
| `FrameWidgetClass` | базовая рамка/контейнер | универсальный контейнер, часто корень |
| `PanelWidgetClass` | панель с фоном и `style` | фон задаётся `color` + `style` (например `rover_sim_black`) |
| `TextWidgetClass` | однострочный текст | `text`, `font`, `"exact text"`, `"text color"`, `"bold text"` |
| `MultilineTextWidgetClass` | многострочный текст | для длинных описаний |
| `RichTextWidgetClass` | текст с разметкой и переносом | `wrap`, `"strip newlines"` |
| `ImageWidgetClass` | картинка/иконка | `image0`/`imageset`/`stretch` |
| `ButtonWidgetClass` | кнопка | НИКОГДА не ставить `"exact text"` |
| `CheckBoxWidgetClass` | чекбокс с подписью | `text`, `font`; без `"exact text"` |
| `SliderWidgetClass` | ползунок | `minimum/maximum/step`, `"no focus"`, `"listen to input"`, `"fill in"` |
| `XComboBoxWidgetClass` | выпадающий список | `clear()`, `AddItem()` из кода |
| `TextListboxWidgetClass` | таблица/список | `lines N`, `colums "..."`, `"title visible" 0`, `"highlight row" 1` |
| `GridSpacerWidgetClass` | сетка | задаёт ряды компонентов |
| `WrapSpacerWidgetClass` | поток | `"Size To Content V" 1`, `content_halign` |
| `SimpleProgressBarWidgetClass` | полоса прогресса | `SetCurrent(0..100)` |
| `WindowWidgetClass` | окно с заголовком | используется как корень ПКМ-окна |

## 4. СВОЙСТВА (ЧТО ЗНАЧИТ КАЖДОЕ)

### 4.1 Видимость и ввод
- `visible 0|1` — видимость (код может переключать `Show(true/false)`).
- `disabled 0|1` — заблокирован ли виджет.
- `ignorepointer 0|1` — `1` = виджет НЕ ловит мышь (фон, декор, подписи).
- `"no focus" 0|1`, `"listen to input" 0|1`, `"fill in" 0|1` — для слайдеров/полей ввода; без `"listen to input" 1` ползунок не тянется.
- `clipchildren 0|1` — обрезать ли детей по границе. Если `0`, текст может выезжать за пределы окна (наш случай с зелёной ценой — исправлено на `1`).
- `keepsafezone`, `inheritalpha` — служебные для HUD.

### 4.2 Цвет и стиль
- `color R G B A` — цвет заливки/текста (0..1).
- `"text color" R G B A` — цвет именно текста.
- `style Имя` — ванильный стиль (`Default`, `Bold`, `Normal`, `rover_sim_black`, `MenuDefault`...).
- `priority N` — порядок отрисовки (больше = выше).

### 4.3 ПОЗИЦИОНИРОВАНИЕ — главное

Две системы координат, переключаются парой флагов:

- `hexactpos 0`, `vexactpos 0` (по умолчанию): `position` — ДОЛЯ родителя (0.030 = 3 %), `halign/valign` задаёт точку привязки.
- `hexactpos 1`, `vexactpos 1`: `position` — ПИКСЕЛИ от точки привязки.
- Аналогично для размера: `hexactsize 0` → `size` это доли родителя, `hexactsize 1` → пиксели.

Точки привязки `halign`: `left_ref`, `center_ref`, `right_ref`; `valign`: `top_ref`, `center_ref`, `bottom_ref`.

**Важно (проверено на 517 ванильных случаях):** для `right_ref`/`bottom_ref` смещение отсчитывается ВНУТРЬ,
то есть `right_ref` + `position 0.03` отступит 3 % ОТ правого края влево, а не наружу. Ошибка знака —
классическая причина «окно уехало за экран».

Примеры из нашего кода:

    // контейнер уведомлений: пиксели, привязка влево-вверх
    FrameWidgetClass Container { position 772 60; size 520 600; halign left_ref; valign top_ref; hexactpos 1; vexactpos 1; hexactsize 1; vexactsize 1; }

    // значок рейтинга: доли, привязка влево-вверх (было bottom_ref — уводило в занятую зону)
    FrameWidgetClass RatingRoot { position 0.012 0.02; size 0.280 0.105; halign left_ref; valign top_ref; }

    // цена в ПКМ-окне: 58 % ширины строки, выравнивание влево
    TextWidgetClass SellPriceWidget { position 0.030 0.02; size 0.580 0.24; halign left_ref; valign top_ref; }

Порядок расчёта валидатора: `left = parentLeft + (hexactpos ? position : position * parentWidth)` с поправкой на якорь.
Пересчитать и посмотреть глазами: `node _build/validate_layouts.js` и `layout_viewer.html`.

### 4.4 Текст
- `text "..."` — содержимое (`#ключ` = строка из `languagecore/stringtable.csv`).
- `font "gui/fonts/..."` — семейство шрифта.
- `"exact text" 0|1` — `1` = фиксированный размер шрифта; `0` на текстовом виджете = МАСШТАБИРОВАТЬ текст под размер виджета (почти всегда баг: получаются гигантские буквы).
- `"exact text size" N` — размер в пикселях (только вместе с `"exact text" 1`).
- `"bold text" 1`, `"size to text h/v" 1` — жирный и «подгонять размер под текст».
- `"text halign/valign"` — выравнивание текста внутри виджета.
- `wrap 1`, `"strip newlines" 0` — перенос строк (для RichText).

ПРАВИЛО: `"exact text"` и `"exact text size"` допустимы ТОЛЬКО у `TextWidget`/`MultilineText`/`RichText`/`EditBox`.
На `TextListboxWidget`, `ButtonWidget`, `CheckBoxWidget`, `XComboBoxWidget` их не бывает ни в одном ванильном layout — движок начинает тянуть текст под высоту виджета.

### 4.5 Список (TextListboxWidget)
- `lines N` — сколько ВИДИМЫХ строк (ваниль: 6..60; `lines 1` ломает заголовок и растягивает его на всё окно).
- `colums "#tm_item;62;#tm_price_buy;13;#tm_price_sell;13;#tm_owned;12"` — заголовок и относительные ширины столбцов.
- `"title visible" 0` + 4 обычных `TextWidget` с названиями колонок — рабочий паттерн вместо встроенного заголовка.
- `"highlight row" 1` — подсветка выбранной строки.

## 5. ШРИФТЫ

Используем только ванильные семейства (проверено: все пары «шрифт + размер» встречаются в 216 ванильных layout, неванильных — 0).

- TraderSellPopup.layout: "gui/fonts/sdf_MetronBook24" @ 24, "gui/fonts/sdf_MetronLight24" @ 18, "gui/fonts/sdf_MetronBook24" @ 20, "gui/fonts/sdf_MetronLight24" @ 20
- TraderRating.layout: "gui/fonts/sdf_MetronLight24" @ 18, "gui/fonts/sdf_MetronBook24" @ 24
- TraderNotification.layout: "gui/fonts/sdf_MetronLight24" @ 16
- TraderItemInspectSell.layout: "gui/fonts/sdf_MetronBook24", "gui/fonts/sdf_MetronLight24"
- TraderMenu.layout: "gui/fonts/sdf_MetronBook72" @ 32, "gui/fonts/Metron22" @ 18, "gui/fonts/sdf_MetronLight24" @ 18, "gui/fonts/sdf_MetronBook24" @ 22, "gui/fonts/sdf_MetronBook24" @ 18, "gui/fonts/sdf_MetronBook24"

Дополнительно: `gui/fonts/Metron22` — bitmap-шрифт, ваниль его использует в поиске сервер-браузера с локализованным текстом, значит кириллица есть.
Все `sdf_Metron*` — SDF-шрифты, масштабируются и содержат кириллицу (игра полностью локализована).
Проверка после правок: `node _build/audit_my_fonts.js` (должно быть «НЕванильных: 0»).

## 6. ГДЕ ЧТО СТОИТ: КАРТА ВАНИЛЬНОГО HUD И НАШИ ОКНА

Замерено по ванильным layout (1920x1080) — занятые зоны:

| Зона | Координаты | Что там |
|---|---|---|
| Верх-справа | 1430..1810 x 64..278 | `WeaponStats` (патроны/оружие) |
| Низ | y 880..1080 | хотбар `QuickbarGrid` (96..1824 x 960..1026), панель игрока, список действий, иконки статусов |
| Лево-центр | 412..762 x 119..316 | подсказки действий (`ActionTargetsCursorWidget`) |
| Центр | ~960 x 540 | прицел/взаимодействие |

Свободные зоны, которые мы используем:
- **сверху по центру** `x 772..1292, y 60..660` — контейнер уведомлений;
- **верхний левый угол** `x 23..560, y 21..134` — значок рейтинга;
- окно торговца и ПКМ-окно центрируются и перекрывают HUD осознанно (они модальные, курсор включён).

Как воспроизвести замер: `node _build/probe_free_space.js`.

## 7. КАК КОД УПРАВЛЯЕТ ИНТЕРФЕЙСОМ

| Действие | Код |
|---|---|
| создать окно | `GetGame().GetWorkspace().CreateWidgets(путь)` |
| найти виджет | `root.FindAnyWidget(имя)` + `TextWidget.Cast(...)` / `SimpleProgressBarWidget.Cast(...)` |
| показать/скрыть | `Show(true/false)`, `SetText(...)`, `SetColor(...)`, `Enable(...)` |
| прогресс | `SimpleProgressBarWidget.SetCurrent(0..100)` |
| курсор и управление | `GetGame().GetUIManager().ShowUICursor(true)`, `mission.PlayerControlDisable(INPUT_EXCLUDE_ALL)` |
| обновление по событию | флаг `TraderMenu.s_NeedsRefresh` выставляется из обработчика RPC и читается в `Update()` |
| закрыть окно | `TraderMessage`/`RPC_SEND_MENU_BACK`, не забыть вернуть управление и курсор (у нас есть null-guard на выход из игры) |

## 8. ПРАВИЛА И ЧАСТЫЕ ОШИБКИ

1. `"exact text" 0` на тексте = масштабирование текста под виджет → гигантские буквы. Нужен `1` + `"exact text size"`.
2. `lines 1` у листбокса → заголовок растягивается на всё окно. Нужны честные `lines` (у нас 17).
3. `right_ref`/`bottom_ref` считаются ВНУТРЬ; ошибка знака уводит элемент за экран.
4. Длинный текст без `clipchildren 1` рисуется за границей окна (зелёная цена, тестовый текст автора в уведомлениях).
5. Не ставить `"exact text"` на listbox/button/checkbox/combobox.
6. Проверять, что длинная локализованная строка влезает: считаем ~0.5 от размера шрифта на символ (20 px ≈ 10 px/символ).
7. Новые виджеты проверять `validate_layouts.js`: `OUTSIDE` (вышел за родителя) и `containment` (должно быть N/N).
8. `OVERLAP` с фоном (`Background x ...`) — норма: фон лежит под содержимым. Реальная проблема — перекрытие интерактивных элементов.
9. Стиль — ванильный: `style`/`color` как у игры, никаких «своих» ярких цветов.
10. Любая правка layout → `build_pbo.ps1` → `deploy_server.ps1` → `check_compile.ps1` (иначе клиент продолжит видеть старую версию из PBO).

## 9. ТЕКУЩИЕ ЗАМЕЧАНИЯ ВАЛИДАТОРА (ожидаемые)

- `OUTSIDE ItemFrameWidget` на 12 px (в обоих окнах) — 3D-превью предмета намеренно чуть выступает за панель.
- `OVERLAP Background x <элементы>` в `TraderMenu` — фон окна, так и задумано.
- `OVERLAP SellRow x NoPriceWidget` — заглушка «нет цены» показывается вместо строки продажи (взаимоисключающие состояния).
- `OVERLAP RatingPanel x RatingRows` — подложка значка.
- `vignette` выходит за `rootFrame` — ванильный полноэкранный слой.

