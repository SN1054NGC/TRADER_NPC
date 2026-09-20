# Профильные файлы TRADER_NPC (примеры)

В этой папке лежат **шаблоны**, а не рабочие конфиги сервера. Реальные файлы живут в профиле сервера:

    <DayZ Server>/Profiles/Trader_NPC_Prof/
      TraderVariables.txt        настройки (ключи <Feature…>, рейтинг, звук, авто-цены)
      TraderObjects.txt          торговцы: маркеры, позиции, сейф-зоны, объект и одежда
      TraderConfig.txt           ассортимент: Classname, Quantity, BuyPrice[, SellPrice[, Ammo]]
      TraderConfig_auto.txt      ГЕНЕРИРУЕТСЯ модом из db/types.xml (не редактировать)
      TraderConfig_Vehicles.txt / TraderConfig_Boats.txt
      TraderVehicleParts.txt / TraderVehicleParts_Boats.txt
      TraderAdmins.txt           UID администраторов торговца (по одному в строке)

## Почему здесь только примеры
Цены, состав ассортимента и UID администраторов — это **экономика конкретного сервера**.
Публиковать рабочие файлы не нужно: скопируйте пример, переименуйте в нужное имя и заполните своими значениями.

## Порядок установки
1. Скопировать нужные файлы из этой папки в `<Профиль>/Trader/`.
2. В `TraderVariables.txt` выставить переключатели функций (`<Feature…> yes|no`).
3. В `TraderObjects.txt` задать своих торговцев (позиции, радиус сейф-зоны, объект).
4. В `TraderConfig.txt` — свой ассортимент (или включить `<AutoPrices> yes` и получить черновик цен в `TraderConfig_auto.txt`).
5. `TraderAdmins.txt` — UID админов; в шаблоне стоит заглушка `PUT_YOUR_ID_HERE`.

> Файлы читаются парсером TRADER_NPC: комментарии — только одиночные `//` (многострочные `/* */` ломают загрузку),
> `<FileEnd>` обязателен в конце файла.
