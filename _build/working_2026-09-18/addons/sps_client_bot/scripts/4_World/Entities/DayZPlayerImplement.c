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
    static const string m_Trader_ConfigFilePath = "$profile:Trader/TraderConfig.txt";
    static const string m_Trader_ObjectsFilePath = "$profile:Trader/TraderObjects.txt";
    static const string m_Trader_VehiclePartsFilePath = "$profile:Trader/TraderVehicleParts.txt";

    bool m_Trader_RecievedAllData = false;
    bool m_Trader_IsInSafezone = false;
    
    string m_Trader_CurrencyName;
    ref array<string> m_Trader_CurrencyClassnames;
    ref array<int> m_Trader_CurrencyValues;
    int m_Player_CurrencyAmount;

    int m_Trader_LastSelledTime = 0;
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
    int   m_Trader_RatingMaxDiscount = 20;   // server tuning (TraderVariables.txt)
    int   m_Trader_RatingFullHours = 100;    // server tuning
    float m_Trader_RatingCurve = 1.35;       // server tuning

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

    bool CreateItemInInventory(PlayerBase player, string itemType, int amount)
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
            EntityAI newItem = EntityAI.Cast(TM_InventoryTransactions.CreateItemInPlayerInventory(itemType, this, foundLocType));
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
                newMagItem.ServerSetAmmoCount(amount);
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
        for (int i = m_Trader_CurrencyClassnames.Count() - 1; i < m_Trader_CurrencyClassnames.Count(); i--)
        {
            int itemMaxAmount = GetItemMaxQuantity(m_Trader_CurrencyClassnames.Get(i));
            if(itemMaxAmount == 0)
            {
                Error("[Trader] Currency "+ m_Trader_CurrencyClassnames.Get(i) +" has max quantity 0 which might mean this class doesn't exist."); 
                continue;
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

    	void handleServerRPCs(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == TRPCs.RPC_BUY)
			handleBuyRPC(sender, rpc_type, ctx);

		if (rpc_type == TRPCs.RPC_SELL)
			handleSellRPC(sender, rpc_type, ctx);

		if (rpc_type == TRPCs.RPC_APPRAISE_SELL)
			handleSellAppraiseRPC(sender, rpc_type, ctx);
	}

    void handleBuyRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        Param3<int, int, string> rpb = new Param3<int, int, string>(-1, -1, "");
        ctx.Read(rpb);

        int traderIndex = rpb.param1;
        int itemID = rpb.param2;
        itemDisplayNameClient = rpb.param3;

        m_Trader_IsSelling = false;

        if (GetGame().GetTime() - m_Trader_LastBuyedTime < m_Trader_BuySellTimer * 1000)
            return;
        m_Trader_LastBuyedTime = GetGame().GetTime();

        if (itemID >= m_Trader_ItemsClassnames.Count() || itemID < 0 || traderIndex >= m_Trader_TraderPositions.Count() || traderIndex < 0)
            return;

        string itemType = m_Trader_ItemsClassnames.Get(itemID);
        int itemQuantity = m_Trader_ItemsQuantity.Get(itemID);
        int itemCosts = m_Trader_ItemsBuyValue.Get(itemID);

        vector playerPosition = GetPosition();
        PlayerBase player = PlayerBase.Cast(this);
        vector traderPosition = m_Trader_TraderPositions.Get(traderIndex);
        float distanceToPlayer = vector.Distance(playerPosition, traderPosition);
        if (distanceToPlayer > TR_Helper.GetTraderAllowedTradeDistance())
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

        // ---- survival rating: the discount is applied here, server side only ----
        int payCosts = TraderRating.ApplyDiscount( itemCosts, m_Trader_RatingDiscount );
        int savedCoins = itemCosts - payCosts;

        if (m_Player_CurrencyAmount < payCosts)
        {
            TraderMessage.PlayerWhite("#tm_cant_afford", player);
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

        deductPlayerCurrency(payCosts);
        CreateItemInInventory(player, itemType, itemQuantity);
    }

    void handleSellRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
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

        vector playerPosition = GetPosition();    
        PlayerBase player = PlayerBase.Cast(this);
        if (vector.Distance(playerPosition, m_Trader_TraderPositions.Get(traderIndex)) > TR_Helper.GetTraderSellAllowedDistance())
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
    if (vector.Distance(playerPosition, m_Trader_TraderPositions.Get(traderIndex)) > TR_Helper.GetTraderSellAllowedDistance())
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
                if (rpc_type == TRPCs.RPC_APPRAISE_SELL_REPLY)
            HandleSellAppraiseReply(sender, rpc_type, ctx);
        if (rpc_type == TRPCs.RPC_SEND_NOTIFICATION || rpc_type == TRPCs.RPC_DELETE_SAFEZONE_MESSAGES)
            Print("[Ntf:D] handleClientRPCs code=" + rpc_type);
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

            case TRPCs.RPC_SYNC_OBJECT_ORIENTATION:
                handleSyncObjectOrientationRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_DELETE_SAFEZONE_MESSAGES:    
                PersonalNotifications().DeleteAllMessages();
            break;

            case TRPCs.RPC_SEND_TRADER_VARIABLES_ENTRY:
                handleSendTraderVariablesEntryRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_PLAYERUID:
                handleSendTraderPlayerUIDRPC(sender, rpc_type, ctx);
            break;

            case TRPCs.RPC_SEND_TRADER_ADMINS_ENTRY:
                handleSendTraderAdminsEntryRPC(sender, rpc_type, ctx);
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
        Param6<int, int, string, int, int, int> itemEntry_rp = new Param6<int, int, string, int, int, int>( 0, 0, "", 0, 0, 0);
        ctx.Read( itemEntry_rp );
        
        m_Trader_ItemsTraderId.Insert(itemEntry_rp.param1);
        m_Trader_ItemsCategoryId.Insert(itemEntry_rp.param2);
        m_Trader_ItemsClassnames.Insert(itemEntry_rp.param3);
        m_Trader_ItemsQuantity.Insert(itemEntry_rp.param4);
        m_Trader_ItemsBuyValue.Insert(itemEntry_rp.param5);
        m_Trader_ItemsSellValue.Insert(itemEntry_rp.param6);
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

    void handleSendTraderVariablesEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
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

    void handleSendTraderAdminsEntryRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
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
    bool isInPlayerInventory(string itemClassname, int amount, out ItemBase item)
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
            if (item.IsRuined()) continue;
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
                    currencyAmount += getItemAmount(item) * m_Player.m_Trader_CurrencyValues.Get(j);
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
