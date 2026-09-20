class ActionTrade: ActionInteractBase
{
    private int m_traderID = -1;
    private int m_traderIndex = -1;
    private PlayerBase m_Player;
    private float m_Trader_AllowedTradeDistance = 3.0;

    void ActionTrade()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_HUDCursorIcon = CursorIcons.CloseHood;
        m_Text = "Trade";
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(10);
        m_ConditionItem = new CCINone;
    }

    override string GetText()
    {
        return m_Text;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (GetGame().IsServer())
            return true;

        if (!target || !target.GetObject() || !player)
            return false;

        if (player && !player.HasReceivedAllTraderData())
        {
            return false;
        }

        float distance = vector.Distance(player.GetPosition(), target.GetObject().GetPosition());
        if (distance > player.m_Trader_TradingDistance)
        {
            return false;
        }

        PlayerBase ntarget = PlayerBase.Cast(target.GetObject());
        if (!ntarget)
            return false;

        if (!ntarget.IsTrader())
            return false;

        return CanOpenTrader(player, target.GetObject());
    }

    override void OnStartClient(ActionData action_data)
    {
        m_Player = action_data.m_Player;
        if (g_Game.GetUIManager().GetMenu() == NULL)
        {
            if (m_Player.HasReceivedAllTraderData() && m_Player.m_Trader_Categorys.Count() > 0)
            {
                initializeTraderMenu();
            }
            else
            {
                GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.RetryInitializeTraderMenu, 500, false);
            }
        }
    }

    void RetryInitializeTraderMenu()
    {
        if (g_Game.GetUIManager().GetMenu() == NULL)
        {
            if (m_Player.HasReceivedAllTraderData() && m_Player.m_Trader_Categorys.Count() > 0)
            {
                initializeTraderMenu();
            }
            else
            {
                GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.RetryInitializeTraderMenu, 500, false);
            }
        }
    }

    bool CanOpenTrader(PlayerBase player, Object target)
    {
        m_Player = player;
        vector playerPosition = player.GetPosition();
        m_Trader_AllowedTradeDistance = player.m_Trader_TradingDistance;

        PlayerBase playerTrader = PlayerBase.Cast(target);
        if (playerTrader && playerTrader.m_Trader_TraderIndex > -1)
        {
            m_traderIndex = playerTrader.m_Trader_TraderIndex;
        }
        else
        {
            m_traderIndex = getNearbyTraderUID(playerPosition);
        }

        m_traderID = getTraderID();
        if (m_traderIndex != -1)
        {
            if (player.m_Trader_TraderNames)
            {
                string traderName = player.m_Trader_TraderNames.Get(getTraderID());
                m_Text = "Trade [" + traderName + "]";
            }
            return true;
        }
        return false;
    }

    int getNearbyTraderUID(vector position)
    {
        for (int traderIndex = 0; traderIndex < m_Player.m_Trader_TraderPositions.Count(); traderIndex++)
        {
            if (getIsTraderNearby(position, traderIndex))
                return traderIndex;
        }
        return -1;
    }

    int getTraderID()
    {
        return m_Player.m_Trader_TraderIDs.Get(m_traderIndex);
    }

    float getDistanceToTrader(vector position, int traderIndex)
    {
        vector traderPosition = m_Player.m_Trader_TraderPositions.Get(traderIndex);
        return vector.Distance(position, traderPosition);
    }

    bool getIsTraderNearby(vector position, int traderIndex)
    {
        float distanceToTrader = getDistanceToTrader(position, traderIndex);
        return distanceToTrader <= m_Trader_AllowedTradeDistance;
    }

    void initializeTraderMenu()
    {
        if (GetGame().GetUIManager().GetMenu() == NULL)
        {
            m_Player.m_TraderMenu = TraderMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(TRADERMENU_UI, null));
            if (m_Player.m_TraderMenu)
            {
                m_Player.m_TraderMenu.m_TraderID = m_traderID;
                m_Player.m_TraderMenu.m_traderIndex = m_traderIndex;
                m_Player.m_TraderMenu.m_buySellTime = m_Player.m_Trader_BuySellTimer;
                m_Player.m_TraderMenu.InitTraderValues();
            }
        }
    }
}