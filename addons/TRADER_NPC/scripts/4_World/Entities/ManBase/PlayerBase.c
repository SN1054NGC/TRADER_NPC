// ============================================================
// ФАЙЛ: PlayerBase.c (ИСПРАВЛЕННЫЙ)
// ============================================================

modded class PlayerBase
{ 
    protected bool m_IsInSafeZone = false;
    bool m_Trader_IsTrader = false;
    bool m_Trader_SafezoneShowDebugShapes = false;
    float m_Trader_TradingDistance = 3.0;
    int m_Trader_TraderIndex = -1;
    protected int m_SafeZoneCount = 0;
    protected int m_PrevSafeZoneCount = 0;
    ref TraderMenu m_TraderMenu;
    ref TraderNotifications m_Trader_TraderNotifications;

    // NOTE: the survival-rating fields (m_Trader_LifeSeconds, m_Trader_RatingDiscount,
    // m_Trader_RatingProgress, m_Trader_RatingEnabled, m_Trader_Rating{MaxDiscount,
    // FullHours,Curve}, m_Trader_LifeAccum) are declared in DayZPlayerImplement next to
    // the other trader state: PlayerBase inherits from it, so they are reachable from
    // both classes (handleBuyRPC lives in DayZPlayerImplement and needs the discount).
    
    // ============================================================
    // ИСПРАВЛЕНИЕ: УДАЛЯЕМ ДУБЛИРУЮЩУЮСЯ ПЕРЕМЕННУЮ
    // Переменная m_Trader_TraderNotifications перенесена в DayZPlayerImplement
    // ============================================================
    
    void PlayerBase()
    {
        RegisterNetSyncVariableBool("m_Trader_IsTrader");
        RegisterNetSyncVariableBool("m_IsInSafeZone");
        RegisterNetSyncVariableBool("m_Trader_SafezoneShowDebugShapes");
        RegisterNetSyncVariableBool("m_Trader_RecievedAllData");
        RegisterNetSyncVariableFloat("m_Trader_TradingDistance", 0.0, 3.0, 1);
        RegisterNetSyncVariableInt("m_Trader_TraderIndex", -1, 1000);
        RegisterNetSyncVariableInt("m_Trader_LifeSeconds", 0, 100000000);
        RegisterNetSyncVariableInt("m_Trader_RatingDiscount", 0, 100);
	RegisterNetSyncVariableInt("m_Trader_RatingDistance", 0, 100000000);
	RegisterNetSyncVariableInt("m_Trader_RatingKills", 0, 1000000);
        RegisterNetSyncVariableFloat("m_Trader_RatingProgress", 0.0, 1.0, 0.01);
        RegisterNetSyncVariableBool("m_Trader_RatingEnabled");
    }

    bool IsInSafeZone()
    {
        return m_IsInSafeZone;
    }

    bool IsTrader()
    {
        return m_Trader_IsTrader;
    }

    void AddSafeZoneTrigger()
    {
        if (!GetGame().IsServer())
            return;
        m_PrevSafeZoneCount = m_SafeZoneCount;
        m_SafeZoneCount++;
        
        if (m_SafeZoneCount > 0)
        {
            if (!m_IsInSafeZone)
            {
                SetAllowDamage(false);
                SetInSafeZone(true);
            }
            if(m_PrevSafeZoneCount == 0)
            {
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(OnExitSafeZoneCountdownComplete);
                TraderMessage.DeleteSafezoneMessages(this);
                TraderMessage.PlayerGreen("#tm_entered_safezone", this);
            }
        }
    }

    void RemoveSafeZoneTrigger(int exitTimer)
    {
        if (!GetGame().IsServer())
            return;

        if (m_SafeZoneCount > 0)
        {
            m_SafeZoneCount--;
        }
        
        if (m_IsInSafeZone && m_SafeZoneCount == 0)
        {
            TraderMessage.SafezoneExit(this, exitTimer);            
            if(exitTimer > 0)
            {
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnExitSafeZoneCountdownComplete, exitTimer * 1000, false);
            }
            else
            {
                OnExitSafeZoneCountdownComplete();
            }
        }
    }

    protected void OnExitSafeZoneCountdownComplete()
    {
        TraderMessage.DeleteSafezoneMessages(this);
        TraderMessage.PlayerRed("#tm_left_safezone", this);
        
        bool godModeVPP, godModeCOT;
        EnScript.GetClassVar(this, "hasGodmode", 0, godModeVPP);
        EnScript.GetClassVar(this, "m_COT_GodMode", 0, godModeCOT);

        if (!godModeVPP && !godModeCOT)
            SetAllowDamage(true);

        SetInSafeZone(false);
    }
    
    void SetInSafeZone(bool state)
    {
        m_IsInSafeZone = state;
        SetSynchDirty();
    }
    
    bool Trader_IsAdmin()
    {
        for (int i = 0; i < m_Trader_AdminPlayerUIDs.Count(); i++)
        {
            if (m_Trader_AdminPlayerUIDs.Get(i) == m_Trader_PlayerUID)
                return true;
        }

        return false;
    }

    override void SetSuicide(bool state)
    {
        super.SetSuicide(state);

        if (state && IsInSafeZone() && GetGame().IsServer())
            SetAllowDamage(true);
    }

    override bool CanBeTargetedByAI(EntityAI ai)
    {
        if(IsTrader() || IsInSafeZone())
        {
            return false;
        }
        
        return super.CanBeTargetedByAI(ai);
    }
    
    #ifdef EXPANSIONMODCORE
    override bool Expansion_IsInSafeZone()
    {
        if(IsTrader() || IsInSafeZone())
        {
            return true;
        }

        return super.Expansion_IsInSafeZone();
    }
    #endif

    override void SetActions(out TInputActionMap InputActionMap)
    {
        super.SetActions(InputActionMap);

        AddAction(ActionTrade, InputActionMap);
    }
    
    // ============================================================
    // МЕТОД ПОЛУЧАЕТ НОТИФИКАЦИИ ИЗ DayZPlayerImplement
    // ============================================================
    TraderNotifications GetTraderNotifications()
    {
        if (!m_Trader_TraderNotifications)
        {
            m_Trader_TraderNotifications = new TraderNotifications();
            m_Trader_TraderNotifications.Init();
        }
        return m_Trader_TraderNotifications;
    }

    // ============================================================
    // Survival rating: alive time -> discount (0..MaxDiscount %)
    // ============================================================
    void TraderRating_Reset()
    {
        m_Trader_LifeSeconds = 0;
        m_Trader_LifeAccum = 0;
        m_Trader_RatingDistance = 0;
        m_Trader_DistanceAccum = 0;
        m_Trader_RatingKills = 0;
        m_Trader_HasLastPos = false;
        m_Trader_RatingProgress = 0;
        m_Trader_RatingDiscount = 0;
        SetSynchDirty();
    }

    void TraderRating_Recalc()
    {
        // Выживаемость = время + пройденный путь + убитые заражённые
        float timeRatio = 0;
        if ( m_Trader_RatingFullHours > 0 )
            timeRatio = m_Trader_LifeSeconds / ( m_Trader_RatingFullHours * 3600.0 );

        float distRatio = 0;
        if ( m_Trader_RatingFullDistance > 0 )
            distRatio = m_Trader_RatingDistance / ( m_Trader_RatingFullDistance * 1.0 );

        float killRatio = 0;
        if ( m_Trader_RatingFullKills > 0 )
            killRatio = m_Trader_RatingKills / ( m_Trader_RatingFullKills * 1.0 );

        float progress = TraderRating.ComputeProgressMixed( timeRatio, distRatio, killRatio, m_Trader_RatingWeightTime, m_Trader_RatingWeightDistance, m_Trader_RatingWeightKills, m_Trader_RatingCurve );
        int discount = TraderRating.ComputeDiscount( progress, m_Trader_RatingMaxDiscount );

        m_Trader_RatingProgress = progress;
        m_Trader_RatingDiscount = discount;

        // called once per second by MissionServer.OnUpdate, so a plain dirty
        // flag is enough to keep alive time, progress and discount in sync.
        SetSynchDirty();
    }

    // Убийство заражённого: +1 к рейтингу выживания и небольшая выплата
    // (цена 1 шт. самого слабого патрона), чтобы новичок накопил на первый магазин.
    void TraderKillReward()
    {
        #ifdef SERVER
        m_Trader_RatingKills = m_Trader_RatingKills + 1;
        TraderRating_Recalc();

        if ( m_Trader_KillRewardAmount > 0 )
            increasePlayerCurrency( m_Trader_KillRewardAmount );
        #endif
    }

    // Persistence: written AFTER the vanilla block (and read after it), which
    // keeps the save/load order symmetric for every modded override.
    override void OnStoreSave( ParamsWriteContext ctx )
    {
        super.OnStoreSave( ctx );

        #ifdef SERVER
        ctx.Write( TraderRating.SAVE_VERSION );
        ctx.Write( m_Trader_LifeSeconds );
        ctx.Write( m_Trader_LifeAccum );
        ctx.Write( m_Trader_RatingDistance );
        ctx.Write( m_Trader_RatingKills );
        #endif
    }

    override bool OnStoreLoad( ParamsReadContext ctx, int version )
    {
        if ( !super.OnStoreLoad( ctx, version ) )
            return false;

        #ifdef SERVER
        int ratingVersion = 0;
        if ( ctx.Read( ratingVersion ) )
        {
            if ( ratingVersion >= 1 )
            {
                int loadedSeconds = 0;
                if ( ctx.Read( loadedSeconds ) )
                    m_Trader_LifeSeconds = loadedSeconds;

                float loadedAccum = 0;
                if ( ctx.Read( loadedAccum ) )
                    m_Trader_LifeAccum = loadedAccum;

                int loadedDistance = 0;
                if ( ctx.Read( loadedDistance ) )
                    m_Trader_RatingDistance = loadedDistance;

                int loadedKills = 0;
                if ( ctx.Read( loadedKills ) )
                    m_Trader_RatingKills = loadedKills;
            }
        }
        // no block present = first character save after installing the feature
        #endif

        return true;
    }

    override void EEKilled( Object killer )
    {
        super.EEKilled( killer );

        #ifdef SERVER
        // a dead character loses its survival rating; the respawned one starts at 0
        TraderRating_Reset();
        #endif
    }
}