# Профильные файлы TRADER_NPC (примеры)

В этой папке лежат **шаблоны**, а не рабочие конфиги сервера. Реальные файлы живут в профиле сервера:

    <DayZ Server>/Profiles/Trader_NPC_Prof/
      TraderNpcVariables.txt        настройки (ключи <Feature…>, рейтинг, звук, авто-цены)
      TraderNpcObjects.txt          торговцы: маркеры, позиции, сейф-зоны, объект и одежда
      TraderNpcConfig.txt           ассортимент и ВАЛЮТА: Classname, Quantity, BuyPrice[, SellPrice[, Ammo]]
      TraderNpcConfig_auto.txt      ГЕНЕРИРУЕТСЯ модом из db/types.xml (не редактировать)
      TraderNpcConfig_Vehicles.txt / TraderNpcConfig_Boats.txt
      TraderNpcVehicleParts.txt / TraderNpcVehicleParts_Boats.txt
      TraderNpcAdmins.txt           UID администраторов торговца (по одному в строке)

## Валюта
Деньги — предмет `TraderNpcMoney`: ванильная бумага (`Paper`), которая складывается в пачку
до **99999** штук и ничего не весит. Задаётся в `TraderNpcConfig.txt` **до первого `<TraderName>`**:

    <CurrencyName> #tm_ruble      // как валюта называется в интерфейсе
    <Currency> TraderNpcMoney, 1  // Класс, Номинал (1 бумага = 1 рубль)

* Строк `<Currency>` может быть несколько — они читаются от старшей купюры к младшей.
* Вместо бумаги можно указать **любой класс игры** (`Paper`, `Nail`, `AK101`, …). Если у предмета
  нет `varQuantityMax`/`count`, мод считает одну штуку за один номинал.
* Цены в ассортименте — в тех же единицах, что и номинал (по умолчанию 1 бумага = 1 рубль).
* Название предмета в инвентаре — `$STR_tm_money` из `languagecore/stringtable.csv`.

## Почему здесь только примеры
Цены, состав ассортимента и UID администраторов — это **экономика конкретного сервера**.
Публиковать рабочие файлы не нужно: скопируйте пример, переименуйте в нужное имя и заполните своими значениями.

## Порядок установки
1. Скопировать нужные файлы из этой папки в `<Профиль>/Trader_NPC_Prof/`.
2. В `TraderNpcVariables.txt` выставить переключатели функций (`<Feature…> yes|no`).
3. В `TraderNpcObjects.txt` задать своих торговцев (позиции, радиус сейф-зоны, объект).
4. В `TraderNpcConfig.txt` — валюту (`<Currency>`) и свой ассортимент (или включить `<AutoPrices> yes`
   и получить черновик цен в `TraderNpcConfig_auto.txt`).
5. `TraderNpcAdmins.txt` — UID админов; в шаблоне стоит заглушка `PUT_YOUR_ID_HERE`.

> Файлы читаются парсером TRADER_NPC: комментарии — только одиночные `//` (многострочные `/* */` ломают загрузку),
> `<FileEnd>` обязателен в конце файла.