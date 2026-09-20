// ============================================================
// File: DayZPlayerImplement.c (modified Trader sell logic)
// ============================================================

class TRITEM
{
    string classname;
    int quantity;
};

modded class DayZPlayerImplement
{
    static const string m_Trader_ConfigFilePath = "$profile:Trader_NPC_Prof/TraderNpcConfig.txt";
    static const string m_Trader_ObjectsFilePath = "$profile:Trader_NPC_Prof/TraderNpcObjects.txt";
    static const string m_Trader_VehiclePartsFilePath = "$profile:Trader_NPC_Prof/TraderNpcVehicleParts.txt";

    bool m_Trader_RecievedAllData = false;
    bool m_Trader_IsInSafezone = false;
    
    string m_Trader_CurrencyName;
    ref array<string> m_Trader_CurrencyClassnames;
    ref array<int> m_Trader_CurrencyValues;
    int m_Player_CurrencyAmount;

    int m_Trader_LastSelledTime = 0;
int m_Trader_LastApprTime = 0;   // лимит частоты запросов аппразера
    int m_Trader_LastBuyedTime = 0;
    
    ref array<string> m_Trader_TraderNames;
    ref array<vector> m_Trader_TraderPositions;
    ref array<int> m_Trader_TraderIDs;
    ref array<int> m_Trader_TraderSafezones;
    ref array<vector> m_Trader_TraderVehicleSpawns;
    ref array<vector> m_Trader_TraderVehicleSpawnsOrientation;
    
    ref array<string> m_Trader_Categorys;
    ref array<int> m_Trader_CategorysTraderKey;
    
    ref array<int> m_Trader_ItemsTraderId;
    ref array<int> m_Trader_ItemsCategoryId;
    ref array<string> m_Trader_ItemsClassnames;
    ref array<int> m_Trader_ItemsQuantity;
    ref array<int> m_Trader_ItemsBuyValue;
    ref array<int> m_Trader_ItemsSellValue;
    // 5-е поле строки цены для магазинов:
    //   ""      - базовый патрон (магазин продаётся полным)
    //   "empty" - продать пустым (игрок заряжает сам)
    //   иначе   - класс патрона, которым снарядить (флаг <AutoPricesMagFill>)
    ref TStringArray m_Trader_ItemsAmmo;    

    ref array<string> m_Trader_NPCDummyClasses;

    float m_Trader_BuySellTimer = 0.3;

        string itemDisplayNameClient;
    bool m_Trader_IsSelling;

    // ============================================================
    // SURVIVAL RATING -> TRADER DISCOUNT (declared here, next to the other
    // trader state, so both DayZPlayerImplement and PlayerBase can use it)
    // ============================================================
    int   m_Trader_LifeSeconds = 0;          // whole alive seconds (server auth, client synced)
    float m_Trader_LifeAccum = 0;            // server only: fractional second carry
    int   m_Trader_RatingDiscount = 0;       // 0..100 %
    float m_Trader_RatingProgress = 0;       // 0..1, drives the badge progress bar
    bool  m_Trader_RatingEnabled = false;    // server: feature enabled, client: show badge
    int   m_Trader_RatingMaxDiscount = 20;   // server tuning (TraderNpcVariables.txt)
    int   m_Trader_RatingFullHours = 100;    // server tuning
    float m_Trader_RatingCurve = 1.35;       // server tuning
    bool  m_Trader_RatingCollapsed = false;  // значок рейтинга свёрнут игроком (клиент)
    bool  m_Trader_RatingHud = true;         // показывать значок в игре (не только в инвентаре)

    // Путь и убийства тоже влияют на выживаемость (и на скидку)
    int   m_Trader_RatingDistance = 0;        // пройдено метров (сервер считает, клиент видит)
    float m_Trader_DistanceAccum = 0;         // сервер: дробный остаток метров
    int   m_Trader_RatingKills = 0;           // убито заражённых
    int   m_Trader_RatingFullDistance = 20000;// метров для полного зачёта
    int   m_Trader_RatingFullKills = 250;     // заражённых для полного зачёта
    float m_Trader_RatingWeightTime = 0.5;    // веса вкладов (нормируются)
    float m_Trader_RatingWeightDistance = 0.3;
    float m_Trader_RatingWeightKills = 0.2;
    vector m_Trader_LastPos;                  // сервер: последняя позиция
    bool  m_Trader_HasLastPos = false;

    // Звук денег: слышат все игроки в радиусе
    bool  m_Trader_SoundEnabled = true;
    float m_Trader_SoundRange = 60.0;
    int   m_Trader_KillRewardAmount = 1;      // деньги за одного заражённого

    int m_Trader_ApprTrader = -1;
    int m_Trader_ApprRow = -1;
    int m_Trader_ApprAmount = -1;
    int m_Trader_ApprBase = -1;
    int m_Trader_ApprBonus = -1;
    int m_Trader_ApprTotal = -1;

    string m_Trader_PlayerUID;
    ref array<string> m_Trader_AdminPlayerUIDs;
    
    bool HasReceivedAllTraderData()
    {
        return m_Trader_RecievedAllData;
    }

    void SetReceivedAllTraderData(bool state)
    {
        m_Trader_RecievedAllData = state;
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (GetGame().IsServer())
            handleServerRPCs(sender, rpc_type, ctx);
        else
            handleClientRPCs(sender, rpc_type, ctx);
    }

    bool CreateItemInInventory(PlayerBase player, string itemType, int amount, string ammoClass = "")
    {
        array<EntityAI> itemsArray = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
        string itemLower = itemType;
        itemLower.ToLower();

        int currentAmount = amount;
        ItemBase item;
        Ammunition_Base ammoItem;
        bool hasSomeQuant = (TR_Helper.ItemHasCount(itemType) || TR_Helper.ItemHasQuantity(itemType)) && !TR_Helper.HasQuantityBar(itemType) && amount >= 0;        
        int itemHasSpawnedOrStacked = 0;
        bool hasShownStackMessage = false;

        if (hasSomeQuant)
        {
            for (int i = 0; i < itemsArray.Count(); i++)
            {
                if (currentAmount <= 0)
                    break;
                Class.CastTo(item, itemsArray.Get(i));
                string itemPlayerClassname = "";
                if (item)
                {
                    if (item.IsRuined())
                        continue;

                    itemPlayerClassname = item.GetType();
                    itemPlayerClassname.ToLower();
                    if (itemLower == itemPlayerClassname && !item.IsFullQuantity() && !item.IsMagazine())
                    {
                        currentAmount = item.AddQuantityTR(currentAmount);
                        itemDisplayNameClient = item.GetDisplayName();
                        itemHasSpawnedOrStacked++;
                    }
                }

                Class.CastTo(ammoItem, itemsArray.Get(i));
                if (ammoItem)
                {
                    if (ammoItem.IsRuined())
                        continue;
                    itemPlayerClassname = ammoItem.GetType();
                    itemPlayerClassname.ToLower();
                    if (itemLower == itemPlayerClassname && ammoItem.IsAmmoPile())
                    {
                        currentAmount = ammoItem.AddQuantityTR(currentAmount);
                        itemDisplayNameClient = ammoItem.GetDisplayName();
                        itemHasSpawnedOrStacked++;
                    }
                }
            }
            
            if (itemHasSpawnedOrStacked > 0 && !itemType.Contains("Ruble"))
            {
                TraderMessage.PlayerWhite("#tm_some" + " " + itemDisplayNameClient + "\n" + "#tm_added_to_inventory", PlayerBase.Cast(this));
                hasShownStackMessage = true;
            }
        }
        else
        {
            currentAmount = 1;
        }

        if (currentAmount > 0 || !hasSomeQuant)
        {
            InventoryLocationType foundLocType;
            // the helper already returns EntityAI, so no Cast is needed here
            EntityAI newItem = TM_InventoryTransactions.CreateItemInPlayerInventory(itemType, this, foundLocType);
            if (!newItem)
            {
                foundLocType = InventoryLocationType.UNKNOWN;
            }

            string displayName = "";
            if (newItem)
            {
                displayName = newItem.GetDisplayName();
            }
            else
            {
                displayName = getItemDisplayName(itemType);
            }

            switch ( foundLocType )
            {
                case InventoryLocationType.CARGO:
                case InventoryLocationType.ATTACHMENT:
                    if (!hasShownStackMessage && !itemType.Contains("Ruble"))
                    {
                        TraderMessage.PlayerWhite(displayName + "\n" + "#tm_added_to_inventory", PlayerBase.Cast(this));
                    }
                    break;
                case InventoryLocationType.GROUND:
                    if (!itemType.Contains("Ruble"))
                    {
                        TraderMessage.PlayerWhite(displayName + "\n" + "#tm_was_placed_on_ground", PlayerBase.Cast(this));
                    }
                    break;
                case InventoryLocationType.UNKNOWN:
                {
                    Error("[Trader] Failed to spawn entity "+itemType+" ! Make sure the classname exists and item can be spawned");
                    return false;
                }
            }

            Magazine newMagItem = Magazine.Cast(newItem);
            Ammunition_Base newammoItem = Ammunition_Base.Cast(newItem);
            if(newMagItem && !newammoItem)                    
            {    
                // M/-3 и прочие сентинелы нельзя отдавать в ServerSetAmmoCount:
                // отрицательное значение означало бы "0 патронов". Берём максимум.
                int magRounds = amount;
                if ( magRounds < 1 )
                    magRounds = GetItemMaxQuantity( itemType );
                if ( magRounds < 0 )
                    magRounds = 0;
                if ( ammoClass == "empty" )
                    magRounds = 0;                      // пустой: игрок зарядит сам

                newMagItem.ServerSetAmmoCount(magRounds);

                // Снаряжение конкретным патроном (5-е поле строки) - экспериментально,
                // включается ключом <AutoPricesMagFill> yes.
                TraderTradeRules.Load();
                if ( ammoClass != "" && ammoClass != "empty" && TraderTradeRules.m_MagFill && magRounds > 0 )
                {
                    for ( int ci = 0; ci < magRounds; ci++ )
                    {
                        float cartDamage = 1.0;
                        string cartName = ammoClass;
                        newMagItem.SetCartridgeAtIndex( ci, cartDamage, cartName );
                    }
                }
                currentAmount = 0;
                UpdateInventoryMenu();
                return true;
            }
            if (hasSomeQuant)
            {
                if (newammoItem)
                {
                    currentAmount = newammoItem.SetQuantityTR(currentAmount);
                    UpdateInventoryMenu();
                    return true;
                }    
                ItemBase newItemBase;
                if (Class.CastTo(newItemBase, newItem))
                {
                    currentAmount = newItemBase.SetQuantityTR(currentAmount);
                }
            }
        }
        UpdateInventoryMenu();
        return true;
    }

    void increasePlayerCurrency(int currencyAmount)
    {
        if (currencyAmount == 0)
            return;

        EntityAI entity;
        ItemBase item;
        PlayerBase player = PlayerBase.Cast(this);
        // идём от старшей купюры к младшей; i >= 0 (было i < Count() - при
        // неоплачиваемом остатке цикл уходил в минус и висел)
        for (int i = m_Trader_CurrencyClassnames.Count() - 1; i >= 0; i--)
        {
            // Класс без varQuantityMax/count (обычный предмет, например Paper) -
            // это не ошибка: одна штука предмета = один номинал.
            int itemMaxAmount = GetItemMaxQuantity(m_Trader_CurrencyClassnames.Get(i));
            if(itemMaxAmount <= 0)
            {
                itemMaxAmount = 1;
            }

            while (currencyAmount / m_Trader_CurrencyValues.Get(i) > 0)
            {
                if (currencyAmount > itemMaxAmount * m_Trader_CurrencyValues.Get(i))
                {
                    CreateItemInInventory(player, m_Trader_CurrencyClassnames.Get(i), itemMaxAmount);
                    currencyAmount -= itemMaxAmount * m_Trader_CurrencyValues.Get(i);
                }
                else
                {
                    CreateItemInInventory(player, m_Trader_CurrencyClassnames.Get(i), currencyAmount / m_Trader_CurrencyValues.Get(i));
                    currencyAmount -= (currencyAmount / m_Trader_CurrencyValues.Get(i) * m_Trader_CurrencyValues.Get(i));
                }

                if (currencyAmount == 0)
                    return;
            }
        }
    }

	// ---- ИИ-торговец: вопрос из чата (!текст) ----
	void handleAiAskRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		TraderVoice.Play( PlayerBase.Cast(this), "funny" );
		#ifdef SERVER
		Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data))
			return;
		TraderAiChat.Ask(PlayerBase.Cast(this), data.param1);
		#endif
	}

	// ---- ответ модели приходит в чат ----
	void handleAiAnswerRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data))
			return;
		g_Game.Chat("Торговец: " + data.param1, "colorAction");
	}

    	void handleServerRPCs(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == TRPCs.RPC_BUY)
			handleBuyRPC(sender, rpc_type, ctx);

		if (rpc_type == TRPCs.RPC_SELL)
			handleSellRPC(sender, rpc_type, ctx);

		if (rpc_type == TRPCs.RPC_APPRAISE_SELL)
			handleSellAppraiseRPC(sender, rpc_type, ctx);
		if (rpc_type == TRPCs.RPC_AI_ASK)
			handleAiAskRPC(sender, rpc_type, ctx);
	}

    void handleBuyRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
		TraderVoice.Play( PlayerBase.Cast(this), "buy" );
        // param4 = количество единиц (0 = как раньше: количество из строки цены)
        Param4<int, int, string, int> rpb = new Param4<int, int, string, int>(-1, -1, "", 0);
        ctx.Read(rpb);

        int traderIndex = rpb.param1;
        int itemID = rpb.param2;
        itemDisplayNameClient = rpb.param3;
        int requestedAmount = rpb.param4;

        m_Trader_IsSelling = false;

        if (GetGame().GetTime() - m_Trader_LastBuyedTime < m_Trader_BuySellTimer * 1000)
            return;
        m_Trader_LastBuyedTime = GetGame().GetTime();

        if (itemID >= m_Trader_ItemsClassnames.Count() || itemID < 0 || traderIndex >= m_Trader_TraderPositions.Count() || traderIndex < 0)
            return;

        string itemType = m_Trader_ItemsClassnames.Get(itemID);
        int itemQuantity = m_Trader_ItemsQuantity.Get(itemID);
        int itemCosts = m_Trader_ItemsBuyValue.Get(itemID);

        // Количество: ползунок на клиенте присылает, сколько единиц он покупает.
        // 0 или совпадение со строкой = старое поведение (одна сделка = строка).
        int buyAmount = itemQuantity;
        bool customAmount = false;
        if ( requestedAmount > 0 && requestedAmount != itemQuantity )
        {
            buyAmount = requestedAmount;
            customAmount = true;
        }
        if ( buyAmount < 1 )
            buyAmount = 1;

        // ---- ЗАЩИТА: клиент не может купить больше, чем допускает строка ----
        // Максимум = вместимость класса (стак/пачка/магазин), не больше 1000.
        int maxBuyAmount = GetItemMaxQuantity( itemType );
        if ( maxBuyAmount < 1 )
            maxBuyAmount = 1;
        if ( maxBuyAmount > 1000 )
            maxBuyAmount = 1000;
        if ( buyAmount > maxBuyAmount )
            buyAmount = maxBuyAmount;

        // Покупать количеством законно ТОЛЬКО когда в таблице есть единичная
        // строка (Quantity = 1): тогда цена честно = цена единицы * количество.
        // Иначе разрешаем лишь количество самой строки - иначе произвольное
        // число от клиента умножало бы одну цену на что угодно.
        bool hasUnitRow = ( TraderSmartSellEngine.FindBuyRowByQuantity( this, traderIndex, itemType, 1 ) >= 0 );
        if ( !hasUnitRow && itemQuantity > 0 && buyAmount > itemQuantity )
        {
            buyAmount = itemQuantity;
            customAmount = true;
        }

        vector playerPosition = GetPosition();
        PlayerBase player = PlayerBase.Cast(this);
        vector traderPosition = m_Trader_TraderPositions.Get(traderIndex);
        float distanceToPlayer = vector.Distance(playerPosition, traderPosition);
        // Торговать можно рядом с торговцем ИЛИ из любой точки его сейф-зоны
        if (distanceToPlayer > TR_Helper.GetTraderAllowedTradeDistance() && !player.IsInSafeZone())
        {
            traderServerLog("tried to access the Trader out of Range! This could be an Hacker!");
            return;
        }

        m_Player_CurrencyAmount = getPlayerCurrencyAmount();

        if (itemCosts < 0)
        {
            TraderMessage.PlayerWhite("#tm_cant_be_bought", player);
            return;
        }

        // Цена покупки: при покупке количества - unit * amount (точная строка на
        // это количество выигрывает), иначе цена строки как раньше.
        if ( customAmount )
        {
            itemCosts = TraderSmartSellEngine.ComputeStackBaseBuyPrice( this, traderIndex, itemType, buyAmount, itemCosts );
        }

        // ---- survival rating: the discount is applied here, server side only ----
        int payCosts = TraderRating.ApplyDiscount( itemCosts, m_Trader_RatingDiscount );
        int savedCoins = itemCosts - payCosts;

        if (m_Player_CurrencyAmount < payCosts)
        {
            TraderMessage.PlayerWhite("#tm_cant_afford", player);
            TraderVoice.Play( PlayerBase.Cast(this), "no_money" );
            return;
        }

        if ( savedCoins > 0 )
        {
            traderTradesLog("bought " + getItemDisplayName(itemType) + "(" + itemType + ") for " + payCosts + " (base " + itemCosts + ", rating -" + m_Trader_RatingDiscount + "%)");
        }
        else
        {
            traderTradesLog("bought " + getItemDisplayName(itemType) + "(" + itemType + ")");
        }

        // 5-е поле строки: каким патроном снарядить магазин ("" = базовый)
        string buyAmmoClass = "";
        if ( m_Trader_ItemsAmmo && itemID >= 0 && itemID < m_Trader_ItemsAmmo.Count() )
            buyAmmoClass = m_Trader_ItemsAmmo.Get( itemID );

        deductPlayerCurrency(payCosts);
        TraderPlaySoundForAll("pickUpPaper_SoundSet", GetPosition());
        CreateItemInInventory(player, itemType, buyAmount, buyAmmoClass);
    }

    void handleSellRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
		TraderVoice.Play( PlayerBase.Cast(this), "sell" );
        Param4<int, int, string, int> rps = new Param4<int, int, string, int>( -1, -1, "", 0 );
        ctx.Read(rps);

        int traderIndex = rps.param1;
        int itemID = rps.param2;
        itemDisplayNameClient = rps.param3;
        int requestedAmount = rps.param4;

        m_Trader_IsSelling = true;

        if (GetGame().GetTime() - m_Trader_LastSelledTime < m_Trader_BuySellTimer * 1000)
            return;
        m_Trader_LastSelledTime = GetGame().GetTime();

        if (itemID >= m_Trader_ItemsClassnames.Count() || itemID < 0 || traderIndex >= m_Trader_TraderPositions.Count() || traderIndex < 0)
            return;

        string itemType = m_Trader_ItemsClassnames.Get(itemID);
        int itemQuantity = m_Trader_ItemsQuantity.Get(itemID);
        int itemSellValue = m_Trader_ItemsSellValue.Get(itemID);
        int itemBuyValue = m_Trader_ItemsBuyValue.Get(itemID);

        // ---- ЗАЩИТА: продажа дороже покупки = бесконечные деньги ----
        if ( itemBuyValue > 0 && itemSellValue > itemBuyValue )
        {
            traderServerLog("row sell > buy, capped: " + itemType + " sell " + itemSellValue + " -> " + itemBuyValue);
            itemSellValue = itemBuyValue;
        }

        vector playerPosition = GetPosition();    
        PlayerBase player = PlayerBase.Cast(this);
        if (vector.Distance(playerPosition, m_Trader_TraderPositions.Get(traderIndex)) > TR_Helper.GetTraderSellAllowedDistance() && !player.IsInSafeZone())
        {
            traderServerLog("tried to access the Trader out of Range! This could be an Hacker!");
            return;
        }

        // Stack sale: the client may ask for fewer units than the price-row
        // quantity (ammo piles / plain stacks). amount 0 or == row quantity
        // keeps the legacy whole-item behaviour.
        int sellAmount = itemQuantity;
        bool customAmount = false;
        if ( requestedAmount > 0 && requestedAmount != itemQuantity )
        {
            sellAmount = requestedAmount;
            customAmount = true;
        }

        if (itemSellValue < 0)
        {
            TraderMessage.PlayerWhite("#tm_cant_be_sold", player);
            return;
        }

        bool sold = false;
        int bonus = 0;
        int basePrice = 0;
        string persistentID;
        int b1;
        int b2;
        int b3;
        int b4;

        ItemBase sellableItem;
        if ( !isInPlayerInventory( itemType, sellAmount, sellableItem ) )
        {
            // Уничтоженный предмет торговец принимает как ХЛАМ: забирает и
            // удаляет (утилизация), платит TraderTradeRules.m_RuinedPrice
            // (0 = бесплатно, но мусор исчезает). Цена настраивается в
            // TraderNpcVariables.txt (<AutoPricesRuinedPrice>).
            TraderTradeRules.Load();
            ItemBase scrapItem;
            if ( isInPlayerInventory( itemType, sellAmount, scrapItem, true ) && scrapItem && scrapItem.IsRuined() )
            {
                string scrapName = getItemDisplayName( scrapItem.GetType() );
                deleteItem( scrapItem );
                if ( TraderTradeRules.m_RuinedPrice > 0 )
                    increasePlayerCurrency( TraderTradeRules.m_RuinedPrice );
        TraderPlaySoundForAll("Paper_Tear_SoundSet", GetPosition());
                TraderMessage.PlayerWhite( "" + scrapName + "\n#tm_ruined_recycled", player );
                return;
            }

            TraderMessage.PlayerWhite( "#tm_you_cant_sell", player );
            return;
        }
                        if ( sellableItem && sellableItem.IsRuined() )
        {
            // Fully destroyed items are NOT buyable/tradeable at all.
            TraderMessage.PlayerWhite( "" + getItemDisplayName( sellableItem.GetType() ) + "\n#tm_you_cant_sell", player );
            return;
        }
        if ( sellableItem )
        {
            // Магазин (НЕ патронная пачка) продаётся только целиком: у него
            // "количество" - это патроны внутри, и продажа по количеству удаляла
            // бы по одному патрону вместо магазина.
            Magazine sellMagUnit = Magazine.Cast( sellableItem );
            if ( sellMagUnit && !sellMagUnit.IsAmmoPile() )
            {
                customAmount = false;
                sellAmount = itemQuantity;          // родной сентинел строки (M = -3)
            }

            // Для стаков/пачек/патронов - фактическое количество, для предметов
            // целиком (оружие, магазин) - 1. Иначе RemoveItem уходит в
            // SetItemAmount(item, -1), а -1 там означает "заполнить до максимума".
            // Клампим ТОЛЬКО положительное количество (стаки/патроны). Отрицательные
            // значения - это сентинелы строки (-3 магазин, -4 оружие, -5 стейк): по ним
            // RemoveItem удаляет предмет ЦЕЛИКОМ, и подменять их на 1 нельзя, иначе
            // RemoveItem уходит в SetItemAmount(item, -1) = "заполнить до максимума".
            if ( sellAmount > 1 )
            {
                int allowedSellAmount = TraderSmartSellEngine.GetSellableAmount( sellableItem );
                if ( allowedSellAmount < 1 )
                    allowedSellAmount = 1;
                if ( sellAmount > allowedSellAmount )
                    sellAmount = allowedSellAmount;
            }

            if ( customAmount )
            {
                // Partial stack sale: price the requested number of units from
                // the trader table (an exact row for that amount wins, otherwise
                // the per-one-unit row multiplied by the amount), then condition
                // it by the item's own health. Accessory bonus does not apply.
                int stackBase = TraderSmartSellEngine.ComputeStackBasePrice( this, traderIndex, itemType, sellAmount, itemSellValue );
                basePrice = TraderSmartSellEngine.Condition( sellableItem, stackBase );
            }
            else
            {
                // Цена строки конфига = цена ПОЛНОЙ единицы с нетронутым износом
                // (максимум). Дальше она уменьшается: износ (Condition) и, для
                // магазинов, реально вложенные патроны (AddMagazineBonus:
                // корпус по своей строке + каждый патрон по строке патрона).
                // Полный магазин = максимальная цена, полупустой платит меньше.
                // Пустая ёмкость (слитая фляга, канистра, пустой стак с
                // количеством) НЕ должна продаваться как полная: платим цену
                // утилизации и забираем предмет. Проверяем только предметы,
                // у которых вообще есть полоса количества.
                // "Пусто" = у предмета есть количество (полоса количества ИЛИ
                // максимум > 1) и оно нулевое. Так покрываются фляги, канистры,
                // газ, баллоны, аптечки, еда порциями - любые Quantity-предметы,
                // а не только те, что рисуют полосу (quantityBar).
                bool isEmptyItem = ( TR_Helper.HasQuantityBar( itemType ) || sellableItem.GetQuantityMax() > 1 );
                if ( !sellableItem.IsMagazine() && isEmptyItem && sellableItem.GetQuantity() <= 0 )
                {
                    string emptyName = getItemDisplayName( itemType );
                    deleteItem( sellableItem );
                    TraderTradeRules.Load();
                    if ( TraderTradeRules.m_RuinedPrice > 0 )
                        increasePlayerCurrency( TraderTradeRules.m_RuinedPrice );
        TraderPlaySoundForAll("Paper_Tear_SoundSet", GetPosition());
                    TraderMessage.PlayerWhite( "" + emptyName + "\n#tm_ruined_recycled", player );
                    return;
                }

                Magazine soldMag = Magazine.Cast( sellableItem );
                if ( soldMag && !soldMag.IsAmmoPile() )
                {
                    int magTotal = 0;
                    TraderSmartSellEngine.AddMagazineBonus( magTotal, this, traderIndex, sellableItem );
                    if ( magTotal > 0 )
                    {
                        basePrice = magTotal;
                        sellableItem.GetPersistentID( b1, b2, b3, b4 );
                        persistentID = itemType + "[" + b1 + " " + b2 + " " + b3 + " " + b4 + "]";
                        deleteItem( sellableItem );   // магазин удаляем целиком, а не по патрону
                        increasePlayerCurrency( basePrice );
        TraderPlaySoundForAll("pickUpPaper_SoundSet", GetPosition());
                        traderTradesLog( "sold magazine " + itemType + " (" + persistentID + ")" + " for " + basePrice );
                        TraderMessage.PlayerWhite( "" + getItemDisplayName( itemType ) + "\n#tm_was_sold", player );
                        return;
                    }
                }

                // Base price of the sold item is conditioned by its own health so it
                // matches the rule TraderSmartSell already applies to every accessory:
                // undamaged -> full row price; worn -> proportional (the worse, the
                // cheaper, min 1). Fully ruined items can NOT be sold at all (RemoveItem
                // rejects them), so only damaged-but-alive items reach this line.
                basePrice = TraderSmartSellEngine.Condition( sellableItem, itemSellValue );

                // Extra money for everything mounted on / loaded into the head
                // that the trader will buy (attachments priced by sell-row and
                // full magazines priced by their rows + a per-1-round fee of the
                // loaded cartridge). Computed BEFORE removal so items still exist.
                bonus = TraderSmartSellEngine.ComputeExtraBonus( this, traderIndex, sellableItem );
            }

            // стадия еды: ROTTEN -> фиксированная цена, BURNED/RAW -> скидка
            TraderTradeRules.Load();
            basePrice = TraderSmartSellEngine.ApplyStage( sellableItem, basePrice );

            sellableItem.GetPersistentID( b1, b2, b3, b4 );
            persistentID = itemType + "[" + b1 + " " + b2 + " " + b3 + " " + b4 + "]";
            sold = RemoveItem( sellableItem, itemType, sellAmount );
        }

        if ( sold )
        {
            string itemAmount = "";
            if ( sellAmount > 0 )
                itemAmount = sellAmount.ToString() + "x ";

            int totalValue = basePrice + bonus;

            traderTradesLog( "sold" + " " + itemAmount + getItemDisplayName( itemType ) + " (" + persistentID + ")" + " for " + totalValue );
            TraderMessage.ServerLog( "[TraderPacked] sold " + itemType + " total=" + totalValue + " (base=" + basePrice + " bonus=" + bonus + ")" );

            string soldDisplay = getItemDisplayName( itemType );
            if ( customAmount )
                soldDisplay = soldDisplay + " x" + sellAmount;

            if ( bonus > 0 )
            {
                TraderMessage.PlayerWhite( "" + soldDisplay + "\n#tm_was_sold" + "\n#tm_sold_accessory_bonus" + ": +" + bonus.ToString() + " " + m_Trader_CurrencyName, player );
            }
            else
            {
                TraderMessage.PlayerWhite( "" + soldDisplay + "\n#tm_was_sold", player );
            }
            increasePlayerCurrency( totalValue );
        TraderPlaySoundForAll("pickUpPaper_SoundSet", GetPosition());
        }
    }
void SendAppraiseReply(int traderIndex, int itemID, int basePrice, int bonus, int total)
{
    if (!GetGame() || !GetIdentity())
        return;
    GetGame().RPCSingleParam(this, TRPCs.RPC_APPRAISE_SELL_REPLY,
        new Param5<int, int, int, int, int>(traderIndex, itemID, basePrice, bonus, total),
        false, GetIdentity());
}

void handleSellAppraiseRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
{
		TraderVoice.Play( PlayerBase.Cast(this), "greet" );
    // ---- ЗАЩИТА: не чаще 4 раз в секунду ----
    if ( GetGame().GetTime() - m_Trader_LastApprTime < 250 )
        return;
    m_Trader_LastApprTime = GetGame().GetTime();

    Param3<int, int, int> rpa = new Param3<int, int, int>(-1, -1, 0);
    ctx.Read(rpa);

    int traderIndex = rpa.param1;
    int itemID = rpa.param2;
    int requestedAmount = rpa.param3;

    if (!GetGame().IsServer())
        return;

    if (itemID >= m_Trader_ItemsClassnames.Count() || itemID < 0 || traderIndex >= m_Trader_TraderPositions.Count() || traderIndex < 0)
        return;

    vector playerPosition = GetPosition();
    if (vector.Distance(playerPosition, m_Trader_TraderPositions.Get(traderIndex)) > TR_Helper.GetTraderSellAllowedDistance() && !PlayerBase.Cast(this).IsInSafeZone())
        return;

    string itemType = m_Trader_ItemsClassnames.Get(itemID);
    int itemQuantity = m_Trader_ItemsQuantity.Get(itemID);
    int rowSellValue = m_Trader_ItemsSellValue.Get(itemID);
    if (rowSellValue < 0)
    {
        SendAppraiseReply(traderIndex, itemID, 0, 0, -1);
        return;
    }

    int sellAmount = itemQuantity;
    bool customAmount = false;
    if ( requestedAmount > 0 && requestedAmount != itemQuantity )
    {
        sellAmount = requestedAmount;
        customAmount = true;
    }

    ItemBase inst;
    if (!isInPlayerInventory(itemType, sellAmount, inst) || !inst)
    {
        SendAppraiseReply(traderIndex, itemID, 0, 0, -1);
        return;
    }

    int basePrice = 0;
    int bonus = 0;
    if ( customAmount )
    {
        int stackBase = TraderSmartSellEngine.ComputeStackBasePrice(this, traderIndex, itemType, sellAmount, rowSellValue);
        basePrice = TraderSmartSellEngine.Condition(inst, stackBase);
    }
    else
    {
        basePrice = TraderSmartSellEngine.Condition(inst, rowSellValue);
        bonus = TraderSmartSellEngine.ComputeExtraBonus(this, traderIndex, inst);
    }
    int total = basePrice + bonus;

    SendAppraiseReply(traderIndex, itemID, basePrice, bonus, total);
}

void handleClientRPCs(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
{
		if (rpc_type == TRPCs.RPC_AI_ANSWER)
		{
			handleAiAnswerRPC(sender, rpc_type, ctx);
			return;
		}
                if (rpc_type == TRPCs.RPC_APPRAISE_SELL_REPLY)
            HandleSellAppraiseReply(sender, rpc_type, ctx);
        if (rpc_type == TRPCs.RPC_SEND_NOTIFICATION || rpc_type == TRPCs.RPC_DELETE_SAFEZONE_MESSAGES)
        {
            // Любая торговая операция заканчивается сообщением - помечаем окно
            // торговца устаревшим, чтобы обновились "в наличии" и наполненность.
            TraderMenu.s_NeedsRefresh = true;
        }
        switch(rpc_type)
        {
            case TRPCs.RPC_SEND_TRADER_CURRENCYNAME_ENTRY:
                handleSendTraderCurrencyNameEntryRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_CURRENCY_ENTRY:
                handleSendTraderCurrencyEntryRPC(sender, rpc_type, ctx);
            break;
            
            case TRPCs.RPC_SEND_TRADER_NAME_ENTRY:
                handleSendTraderNameEntryRPC(sender, rpc_type, ctx);
            break;
            
            case TRPCs.RPC_SEND_TRADER_CATEGORY_ENTRY:
                handleSendTraderCategoryEntryRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_NPCDUMMY_ENTRY:
                handleSendTraderNPCDummyEntryRPC(sender, rpc_type, ctx);
            break;
            
            case TRPCs.RPC_SEND_TRADER_ITEM_ENTRY:
                handleSendTraderItemEntryRPC(sender, rpc_type, ctx);
            break;
            
            case TRPCs.RPC_SEND_TRADER_MARKER_ENTRY:
                handleSendTraderMarkerEntryRPC(sender, rpc_type, ctx);
            break;
            
            case TRPCs.RPC_SEND_TRADER_CLEAR:
                handleSendTraderClearRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_MENU_BACK:
                handleSendMenuBackRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_NOTIFICATION:
                HandleSendNotificationRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_PLAY_TRADER_SOUND:
                HandlePlayTraderSoundRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SYNC_OBJECT_ORIENTATION:
                handleSyncObjectOrientationRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_DELETE_SAFEZONE_MESSAGES:    
                PersonalNotifications().DeleteAllMessages();
            break;

            case TRPCs.RPC_SEND_TRADER_VARIABLES_ENTRY:
                handleSendTraderNpcVariablesEntryRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_PLAYERUID:
                handleSendTraderPlayerUIDRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_ADMINS_ENTRY:
                handleSendTraderNpcAdminsEntryRPC(sender, rpc_type, ctx);
            break;
        }
    }

    void handleSyncObjectOrientationRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param2<Object, vector> syncObject_rp = new Param2<Object, vector>( NULL, "0 0 0" );
        ctx.Read( syncObject_rp );
        
        Object objectToSync = syncObject_rp.param1;
        vector objectToSyncOrientation  = syncObject_rp.param2;

        objectToSync.SetOrientation(objectToSyncOrientation);
    }
    
    void handleSendTraderCurrencyNameEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<string> currencyName_rp = new Param1<string>( "" );
        ctx.Read( currencyName_rp );
        
        m_Trader_CurrencyName = currencyName_rp.param1;
    }

    void handleSendTraderCurrencyEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param2<string, int> currency_rp = new Param2<string, int>( "", -1 );
        ctx.Read( currency_rp );
        
        m_Trader_CurrencyClassnames.Insert(currency_rp.param1);
        m_Trader_CurrencyValues.Insert(currency_rp.param2);
    }
    
    void handleSendTraderNameEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<string> tradername_rp = new Param1<string>( "" );
        ctx.Read( tradername_rp );
        
        m_Trader_TraderNames.Insert(tradername_rp.param1);
    }
    
    void handleSendTraderCategoryEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param2<string, int> category_rp = new Param2<string, int>( "", 0 );
        ctx.Read( category_rp );                    
        
        m_Trader_Categorys.Insert(category_rp.param1);
        m_Trader_CategorysTraderKey.Insert(category_rp.param2);
    }

    void handleSendTraderNPCDummyEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<string> npcDummy_rp = new Param1<string>( "" );
        ctx.Read( npcDummy_rp );                    
        
        m_Trader_NPCDummyClasses.Insert(npcDummy_rp.param1);
    }
    
    void handleSendTraderItemEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param7<int, int, string, int, int, int, string> itemEntry_rp = new Param7<int, int, string, int, int, int, string>( 0, 0, "", 0, 0, 0, "");
        ctx.Read( itemEntry_rp );
        
        m_Trader_ItemsTraderId.Insert(itemEntry_rp.param1);
        m_Trader_ItemsCategoryId.Insert(itemEntry_rp.param2);
        m_Trader_ItemsClassnames.Insert(itemEntry_rp.param3);
        m_Trader_ItemsQuantity.Insert(itemEntry_rp.param4);
        m_Trader_ItemsBuyValue.Insert(itemEntry_rp.param5);
        m_Trader_ItemsSellValue.Insert(itemEntry_rp.param6);
        m_Trader_ItemsAmmo.Insert(itemEntry_rp.param7);
    }
    
    void handleSendTraderMarkerEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param5<int, vector, int, vector, vector> markerEntry = new Param5<int, vector, int, vector, vector>( 0, "0 0 0", 0, "0 0 0", "0 0 0" );
        ctx.Read( markerEntry );                    
        
        m_Trader_TraderIDs.Insert(markerEntry.param1);
        m_Trader_TraderPositions.Insert(markerEntry.param2);
        m_Trader_TraderSafezones.Insert(markerEntry.param3);
        m_Trader_TraderVehicleSpawns.Insert(markerEntry.param4);
        m_Trader_TraderVehicleSpawnsOrientation.Insert(markerEntry.param5);
    }
    
    void handleSendTraderClearRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        SetReceivedAllTraderData(false);
        m_Trader_CurrencyName = "";
        m_Trader_CurrencyClassnames = new array<string>;
        m_Trader_CurrencyValues = new array<int>;
        m_Trader_TraderNames = new array<string>;
        m_Trader_TraderPositions = new array<vector>;
        m_Trader_TraderIDs = new array<int>;
        m_Trader_TraderSafezones = new array<int>;
        m_Trader_TraderVehicleSpawns = new array<vector>;
        m_Trader_TraderVehicleSpawnsOrientation = new array<vector>;
        m_Trader_Categorys = new array<string>;
        m_Trader_CategorysTraderKey = new array<int>;
        m_Trader_ItemsTraderId = new array<int>;
        m_Trader_ItemsCategoryId = new array<int>;
        m_Trader_ItemsClassnames = new array<string>;
        m_Trader_ItemsQuantity = new array<int>;
        m_Trader_ItemsBuyValue = new array<int>;
        m_Trader_ItemsSellValue = new array<int>;
        m_Trader_ItemsAmmo = new TStringArray;
        m_Trader_PlayerUID = "";
        m_Trader_AdminPlayerUIDs = new array<string>;
        m_Trader_NPCDummyClasses = new array<string>;
    }

    void handleSendMenuBackRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        GetGame().GetUIManager().Back();
    }

    void HandleSendNotificationRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param4<string, float, int, bool> msg = new Param4<string, float, int, bool>( "", 0, 0, false);
        ctx.Read( msg );
        if(msg.param4)
        {
            PersonalNotifications().ShowExitSafezoneMessage(msg.param2);
        }
        else
        {
            PersonalNotifications().ShowMessage(msg.param1, msg.param2, msg.param3);
        }
    }
    
    void showTraderMessage(string message, float time, int color = 0)
    {
        PersonalNotifications().ShowMessage(message, time, color);
    }

    // Звук денег "для всех": сервер рассылает игрокам в радиусе, каждый играет локально
    void TraderPlaySoundForAll(string soundSet, vector pos)
    {
#ifdef SERVER
        if (!m_Trader_SoundEnabled || soundSet == "")
            return;

        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase p = PlayerBase.Cast(players.Get(i));
            if (!p || !p.GetIdentity())
                continue;
            if (vector.Distance(p.GetPosition(), pos) > p.m_Trader_SoundRange)
                continue;

            GetGame().RPCSingleParam(p, TRPCs.RPC_PLAY_TRADER_SOUND, new Param4<string, float, float, float>(soundSet, pos[0], pos[1], pos[2]), false, p.GetIdentity());
        }
#endif
    }

    void HandlePlayTraderSoundRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param4<string, float, float, float> rp = new Param4<string, float, float, float>("", 0, 0, 0);
        ctx.Read(rp);
        if (rp.param1 == "")
            return;

        SEffectManager.PlaySound(rp.param1, Vector(rp.param2, rp.param3, rp.param4), 0.05, 0.1);
    }

    void handleSendTraderNpcVariablesEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<float> traderVariables_rp = new Param1<float>( 0 );
        ctx.Read( traderVariables_rp );
        
        m_Trader_BuySellTimer = traderVariables_rp.param1;
    }

    void handleSendTraderPlayerUIDRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<string> traderPlayerUID_rp = new Param1<string>( "" );
        ctx.Read( traderPlayerUID_rp );
        
        m_Trader_PlayerUID = traderPlayerUID_rp.param1;
    }

    void handleSendTraderNpcAdminsEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param1<string> traderAdmins_rp = new Param1<string>( "" );
        ctx.Read( traderAdmins_rp );
        
        m_Trader_AdminPlayerUIDs.Insert(traderAdmins_rp.param1);
    }

    void traderServerLog(string message)
    {
        TraderMessage.ServerLog("[TRADER] Player: (" + GetIdentity().GetName() + ") " + GetIdentity().GetId() + " " + message);
    }

    void traderTradesLog(string message)
    {
        TraderMessage.TradesLog("[TRADER] Player: (" + GetIdentity().GetName() + ") " + GetIdentity().GetId() + " " + message);
    }

    TraderNotifications PersonalNotifications()
    {
        PlayerBase p = PlayerBase.Cast(this);
        if (p)
            return p.GetTraderNotifications();
        return null;
    }

    string TrimUntPrefix(string str)
    {
        str.Replace("$UNT$", "");
        return str;
    }

    string getItemDisplayName(string itemClassname)
    {
        TStringArray itemInfos = new TStringArray;
        
        string cfg = "CfgVehicles " + itemClassname + " displayName";
        string displayName;
        GetGame().ConfigGetText(cfg, displayName);
    
        if (displayName == "")
        {
            cfg = "CfgAmmo " + itemClassname + " displayName";
            GetGame().ConfigGetText(cfg, displayName);
        }
        
        if (displayName == "")
        {
            cfg = "CfgMagazines " + itemClassname + " displayName";
            GetGame().ConfigGetText(cfg, displayName);
        }
        
        if (displayName == "")
        {
            cfg = "cfgWeapons " + itemClassname + " displayName";
            GetGame().ConfigGetText(cfg, displayName);
        }
    
        if (displayName == "")
        {
            cfg = "CfgNonAIVehicles " + itemClassname + " displayName";
            GetGame().ConfigGetText(cfg, displayName);
        }
        
        if (displayName != "")
            return TrimUntPrefix(displayName);
        else
            return itemClassname;
    }

    int GetItemMaxQuantity(string itemClassname)
    {
        TStringArray searching_in = new TStringArray;
        searching_in.Insert( CFG_MAGAZINESPATH  + " " + itemClassname + " count");
        searching_in.Insert( CFG_VEHICLESPATH + " " + itemClassname + " varQuantityMax");

        for ( int s = 0; s < searching_in.Count(); ++s )
        {
            string path = searching_in.Get( s );

            if ( GetGame().ConfigIsExisting( path ) )
            {
                return GetGame().ConfigGetInt( path );
            }
        }

        return 0;
    }

    int getItemAmount(ItemBase item)
    {
        Magazine mgzn = Magazine.Cast(item);
                
        int itemAmount = 0;
        if( item.IsMagazine() )
        {
            itemAmount = mgzn.GetAmmoCount();
        }
        else
        {
            itemAmount = item.GetQuantity();
        }
        
        return itemAmount;
    }

    bool SetItemAmount(ItemBase item, int amount)
    {
        if (!item)
            return false;

        if (amount == -1)
            amount = GetItemMaxQuantity(item.GetType());

        if (amount == -3)
            amount = 0;

        if (amount == -4)
            amount = 0;

        if (amount == -5)
            amount = Math.RandomIntInclusive(GetItemMaxQuantity(item.GetType()) * 0.5, GetItemMaxQuantity(item.GetType()));

        Magazine mgzn = Magazine.Cast(item);
                
        if( item.IsMagazine() )
        {
            if (!mgzn)
                return false;

            mgzn.ServerSetAmmoCount(amount);
        }
        else
        {
            item.SetQuantity(amount);
        }

        return true;
    }

    // ============================================================
    // ???????????????????????? ??????????: isInPlayerInventory
    // ???????????????????????? ?????????????? ???????? ??????????????????
    // ============================================================
    bool isInPlayerInventory(string itemClassname, int amount, out ItemBase item, bool allowRuined = false)
    {
        itemClassname.ToLower();

        ItemBase item_in_hands = ItemBase.Cast(GetHumanInventory().GetEntityInHands());
        if (item_in_hands)
        {            
            if(DoItemSellChecks(item_in_hands, itemClassname, amount))
            {
                item = item_in_hands;
                return true;
            }
        }

        bool isWeapon = false;
        if (amount == -4) isWeapon = true;
        array<EntityAI> itemsArray = new array<EntityAI>;        
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

        for (int i = 0; i < itemsArray.Count(); i++)
        {
            Class.CastTo(item, itemsArray.Get(i));

            if (!item) continue;
            if (item.IsRuined() && !allowRuined) continue;
            if (isAttached(item)) continue;

            if(item.GetInventory())
            {
                if (!item.GetInventory().CanRemoveEntity()) continue;
            }

            if(DoItemSellChecks(item, itemClassname, amount))
            {
                return true;
            }
        }
        
        return false;
    }

    protected bool DoItemSellChecks(ItemBase item, string itemClassname, int amount)
    {
        string itemPlayerClassname = item.GetType();
        itemPlayerClassname.ToLower();    
        
        bool isMagazine = false;
        if (amount == -3) isMagazine = true;
        bool isWeapon = false;
        if (amount == -4) isWeapon = true;
        bool isSteak = false;
        if (amount == -5) isSteak = true;
        bool isGrenade = item.IsInherited(Grenade_Base);

        if(itemPlayerClassname == itemClassname)
        {
            float steakProcentage = 0.5;
            if(isSteak)
            {
                Edible_Base edible = Edible_Base.Cast(item);
                if(edible)
                {
                    if (edible.GetFoodStage())
                    {
                        if(edible.GetFoodStageType() == FoodStageType.ROTTEN)
                        {
                            return false;
                        }
                    }
                }    
                int steakAmount = getItemAmount(item);
                int maxSteakAmount = GetItemMaxQuantity(itemPlayerClassname);
                int MaxPercentage = maxSteakAmount * steakProcentage;
                if(steakAmount >= MaxPercentage)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            if(isMagazine || isWeapon || isGrenade)
            {
                return true;
            }
            if(getItemAmount(item) >= amount)
            {
                return true;
            }
        }
        return false;
    }

    array<ItemBase> getMergeableItemFromPlayerInventory(string itemType, int amount, bool absolute = false)
    {
        array<ItemBase> mergableItems = new array<ItemBase>;

        array<EntityAI> itemsArray = new array<EntityAI>;        
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

        ItemBase itemToCombine = ItemBase.Cast(GetGame().CreateObject(itemType, "0 0 0"));

        if (!itemToCombine)
            return new array<ItemBase>;

        SetItemAmount(itemToCombine, amount);

        ItemBase item;        
        for (int i = 0; i < itemsArray.Count(); i++)
        {
            Class.CastTo(item, itemsArray.Get(i));

            if (!item)
                continue;

            if (item.IsRuined())
                continue;

            if (itemToCombine.GetType() != item.GetType())
                continue;

            if (item.CanBeCombined(itemToCombine) && getItemAmount(item) < GetItemMaxQuantity(item.GetType()))
            {
                amount -= GetItemMaxQuantity(item.GetType()) - getItemAmount(item);
                SetItemAmount(itemToCombine, amount);
                mergableItems.Insert(item);
            }
        }

        GetGame().ObjectDelete(itemToCombine);

        if (absolute && amount > 0)
            return new array<ItemBase>;

        return mergableItems;
    }

    bool canCreateItemInPlayerInventory(string itemType, int amount)
    {
        array<ItemBase> mergeableItems = getMergeableItemFromPlayerInventory(itemType, amount);
        if (mergeableItems.Count() > 0)
            return true;

        EntityAI item = EntityAI.Cast(GetGame().CreateObject(itemType, "0 0 0"));
        if (!item)
            return false;

        SetItemAmount(ItemBase.Cast(item), amount);
        if(GetInventory().CanAddEntityToInventory(item))
        {
            GetGame().ObjectDelete(item);
            return true;
        }
        GetGame().ObjectDelete(item);

        EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
        if (!entityInHands)
            return true;

        return false;            
    }

    bool RemoveItem(ItemBase item, string itemClassname, int amount)
    {        
        if (item.IsRuined()) return false;
        if (isAttached(item)) return false;

        itemClassname.ToLower();

        bool isMagazine = false;
        if (amount == -3) isMagazine = true;
        bool isWeapon = false;
        if (amount == -4) isWeapon = true;
        bool isSteak = false;
        if (amount == -5) isSteak = true;
        bool isGrenade = item.IsInherited(Grenade_Base);

        string itemPlayerClassname = item.GetType();
        itemPlayerClassname.ToLower();
        if(itemPlayerClassname == itemClassname)
        {
            int itemAmount = getItemAmount(item);
            int maxAmount = GetItemMaxQuantity(itemPlayerClassname);
            float steakProcentage = 0.5;
            bool shouldDel = false;
            if(isSteak)
            {
                int MaxPercentage = maxAmount * steakProcentage;
                if(itemAmount >= MaxPercentage)
                {
                    shouldDel = true;
                }
            }
            if(isMagazine || isWeapon || isGrenade)
            {
                shouldDel = true;
            }
            if(itemAmount == amount)
            {
                shouldDel = true;
            }
            if(shouldDel)
            {
                deleteItem(item);
                return true;
            }
            else
            {
                SetItemAmount(item, itemAmount - amount);
                return true;
            }
        }
        return false;
    }

    void DeleteItemAndOfferRewardsForAllAttachments(ItemBase item)
    {
        int totalSellValue = 0;
        for ( int i = 0; i < item.GetInventory().GetAttachmentSlotsCount(); i++ )
        {
            int slotId = item.GetInventory().GetAttachmentSlotId(i);
            EntityAI attachment = item.GetInventory().FindAttachment(slotId);
            if (attachment)
            {
                int itemID = m_Trader_ItemsClassnames.Find(attachment.GetType());
                if ( itemID != -1 )
                {
                    int itemQuantity = m_Trader_ItemsQuantity.Get(itemID);
                    int itemSellValue = m_Trader_ItemsSellValue.Get(itemID);
                    if (itemSellValue < 0)
                        continue;
                    totalSellValue += itemSellValue;
                }
            }
        }
        increasePlayerCurrency(totalSellValue);
        TraderPlaySoundForAll("pickUpPaper_SoundSet", GetPosition());
        item.Delete();
    }

    void deleteItem(ItemBase item)
    {
        if (item)
        {
            InventoryLocation src = new InventoryLocation();
            if (item.GetInventory() && item.GetInventory().GetCurrentInventoryLocation(src))
            {
                if(src.GetType() == InventoryLocationType.HANDS)
                {
                    LocalDestroyEntityInHands();
                    return;
                }
            }
            GetGame().ObjectDelete(item);
        }
    }

    // ============================================================
    // ???????????????????????? ??????????: isAttached
    // ?????????????????? ?????????????? ?????????????????? ?? ???????????????? ???????? ?? ?????????????????????????? ??????????????????
    // ============================================================
    bool isAttached(ItemBase item)
    {
        EntityAI parent = item.GetHierarchyParent();

        if (!parent)
            return false;

        // ???????????????? ?? ?????????????? ?????????? ?????????????????? ???????? ???????? ?????? ??????????????????????
        if (item.IsMagazine() || item.IsAmmoPile())
            return false;

        // ?????????????????????????? ???????????????? (??????????????, ?????????????????? ?? ??.??.) ??? ???? ??????????????????
        if (item.GetInventory().IsAttachment())
            return true;

        // ???????? ?????????????? ???? ?????????????? ?? ???? ??????????????, ???? ???????????????????? ?? ????????????/???????????????? ??? ???? ??????????????????
        if (parent.IsWeapon() || parent.IsMagazine())
            return true;

        return false;
    }

    int getPlayerCurrencyAmount()
    {
        PlayerBase m_Player = PlayerBase.Cast(this);
        
        int currencyAmount = 0;
        
        array<EntityAI> itemsArray = new array<EntityAI>;
        m_Player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

        ItemBase item;
        
        for (int i = 0; i < itemsArray.Count(); i++)
        {
            Class.CastTo(item, itemsArray.Get(i));

            if (!item)
                continue;

            for (int j = 0; j < m_Player.m_Trader_CurrencyClassnames.Count(); j++)
            {
                if(item.GetType() == m_Player.m_Trader_CurrencyClassnames.Get(j))
                {
                    int itemAmount = getItemAmount(item);

                    if (itemAmount <= 0)
                        itemAmount = 1;      // предмет без количества = 1 штука

                    currencyAmount += itemAmount * m_Player.m_Trader_CurrencyValues.Get(j);
                }
            }
        }
        
        return currencyAmount;
    }

    void deductPlayerCurrency(int currencyAmount)
    {        
        if (currencyAmount == 0)
            return;

        array<EntityAI> itemsArray = new array<EntityAI>;
        ItemBase item;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
        
        for (int i = 0; i < m_Trader_CurrencyClassnames.Count(); i++)
        {
            for (int j = 0; j < itemsArray.Count(); j++)
            {
                Class.CastTo(item, itemsArray.Get(j));
                
                if (!item)
                    continue;

                if(item.GetType() == m_Trader_CurrencyClassnames.Get(i))
                {
                    int itemAmount = getItemAmount(item);

                    if (itemAmount <= 0)
                        itemAmount = 1;      // предмет без количества = 1 штука

                    if(itemAmount * m_Trader_CurrencyValues.Get(i) > currencyAmount)
                    {
                        if (currencyAmount >= m_Trader_CurrencyValues.Get(i))
                        {
                            SetItemAmount(item, itemAmount - (currencyAmount / m_Trader_CurrencyValues.Get(i)));

                            UpdateInventoryMenu();
                            
                            currencyAmount -= (currencyAmount / m_Trader_CurrencyValues.Get(i)) * m_Trader_CurrencyValues.Get(i);
                        }

                        if (currencyAmount < m_Trader_CurrencyValues.Get(i))
                        {
                            exchangeCurrency(item, currencyAmount, m_Trader_CurrencyValues.Get(i));

                            return;
                        }
                    }
                    else
                    {
                        GetGame().ObjectDelete(itemsArray.Get(j));
                        
                        UpdateInventoryMenu();
                        
                        currencyAmount -= itemAmount * m_Trader_CurrencyValues.Get(i);
                    }
                }
            }
        }
    }

    void exchangeCurrency(ItemBase item, int currencyAmount, int currencyValue)
    {
        if (!item)
            return;

        if (currencyAmount == 0)
            return;

        int itemAmount = getItemAmount(item);

        if (itemAmount <= 1)
            deleteItem(item);
        else
            SetItemAmount(item, itemAmount - 1);

        increasePlayerCurrency(currencyValue - currencyAmount);
    }

        void deleteObject(Object obj)
    {
        if (obj)
            GetGame().ObjectDelete(obj);
    }

    void RequestSellAppraise(int traderIndex, int row, int amount)
    {
        if (!GetGame().IsClient())
            return;
        m_Trader_ApprTrader = traderIndex;
        m_Trader_ApprRow = row;
        m_Trader_ApprAmount = amount;
        m_Trader_ApprBase = -1;
        m_Trader_ApprBonus = -1;
        m_Trader_ApprTotal = -1;
        GetGame().RPCSingleParam(this, TRPCs.RPC_APPRAISE_SELL, new Param3<int, int, int>(traderIndex, row, amount), true);
    }

    void HandleSellAppraiseReply(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param5<int, int, int, int, int> rp = new Param5<int, int, int, int, int>(-1, -1, -1, -1, -1);
        ctx.Read(rp);
        m_Trader_ApprTrader = rp.param1;
        m_Trader_ApprRow = rp.param2;
        m_Trader_ApprBase = rp.param3;
        m_Trader_ApprBonus = rp.param4;
        m_Trader_ApprTotal = rp.param5;
    }
}
