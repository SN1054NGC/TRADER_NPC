modded class MissionServer
{
    static const string m_Trader_ConfigFilePath = "$profile:Trader_NPC_Prof/TraderConfig.txt";
    static const string m_Trader_ObjectsFilePath = "$profile:Trader_NPC_Prof/TraderObjects.txt";
    static const string m_Trader_VehiclePartsFilePath = "$profile:Trader_NPC_Prof/TraderVehicleParts.txt";
    static const string m_Trader_VariableFilePath = "$profile:Trader_NPC_Prof/TraderVariables.txt";
    static const string m_Trader_AdminsFilePath = "$profile:Trader_NPC_Prof/TraderAdmins.txt";

    float m_Trader_SafezoneTimeout = 30;
    bool m_Trader_SafezoneRemoveAnimals = false;
    bool m_Trader_SafezoneRemoveInfected = false;
    bool m_Trader_SafezoneRemoveEAI = false;
    bool m_Trader_SafezoneShowDebugShapes = false;
    float m_Trader_TradingDistance = 3.0;

    bool m_Trader_ReadAllTraderData = false;

    string m_Trader_CurrencyName;
    ref array<string> m_Trader_CurrencyClassnames;
    ref array<int> m_Trader_CurrencyValues;
    
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
    ref TStringArray m_Trader_ItemsAmmo;      // 5-е поле строки: патрон для магазина

    ref array<string> m_Trader_AdminPlayerUIDs;
    ref array<string> m_Trader_NPCDummyClasses;

    float m_Trader_BuySellTimer = 0.3;

    // ---- survival rating (trader discount), tuned in TraderVariables.txt ----
    bool  m_Trader_RatingEnabled = true;
    int   m_Trader_RatingMaxDiscount = 20;
    int   m_Trader_RatingFullHours = 100;
    float m_Trader_RatingCurve = 1.35;
    float m_Trader_RatingTick = 0;
int   m_Trader_RatingFullDistance = 20000;
int   m_Trader_RatingFullKills = 250;
float m_Trader_RatingWeightTime = 0.5;
float m_Trader_RatingWeightDistance = 0.3;
float m_Trader_RatingWeightKills = 0.2;
bool  m_Trader_SoundEnabled = true;
float m_Trader_SoundRange = 60.0;
int   m_Trader_KillReward = 0;            // 0 = авто: цена 1 шт. самого слабого патрона

    ref array<PlayerBase> m_Trader_SpawnedTraderCharacters;

    ref array<Object> m_Trader_ObjectsList;
    
    override void OnInit()
    {        
        super.OnInit();         
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.LoadServerConfigs, 1000, false);
    }

    void LoadServerConfigs()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.LoadServerConfigs);
        TraderMessage.ServerLog("[TRADER] LOADING TRADER CONFIG");
        SpawnTraderObjects();
        readTraderVariables();
        readTraderData();
        readTraderAdmins();

        #ifdef SERVER
        // Auto-prices: builds $profile:Trader_NPC_Prof/TraderConfig_auto.txt from the
        // mission types.xml for every tradeable class the manual config does
        // not list. Disabled by default - see <AutoPrices> in TraderVariables.txt.
        // Nothing is applied automatically: the file is only offered, and the
        // owner includes it with <OpenFile>TraderConfig_auto.txt.
        TraderAutoPrices.Run( m_Trader_ItemsClassnames );
        #endif
        
        // ============================================================
        // ИСПРАВЛЕНИЕ: ПРОВЕРКА g_Game
        // ============================================================
        DayZGame game = DayZGame.Cast(GetGame());
        if (game)
        {
            game.SetTraderPositionsAndSafezones(m_Trader_TraderPositions, m_Trader_TraderSafezones);
        }
        
        TraderMessage.ServerLog("[TRADER] FINISHED LOADING TRADER CONFIG");
    }

    override void HandleBody(PlayerBase player)
    {
        if (player.IsUnconscious() || player.IsRestrained())
        {
            if(player.IsInSafeZone())
            {
                player.SetAllowDamage(true);
            }
        }

        super.HandleBody(player);
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // ---- survival rating: add the elapsed second to every living character ----
        if ( !m_Trader_RatingEnabled )
            return;

        m_Trader_RatingTick = m_Trader_RatingTick + timeslice;
        if ( m_Trader_RatingTick < 1.0 )
            return;

        float elapsed = m_Trader_RatingTick;
        m_Trader_RatingTick = 0;

        array<Man> ratingPlayers = new array<Man>;
        GetGame().GetPlayers( ratingPlayers );

        for ( int i = 0; i < ratingPlayers.Count(); i++ )
        {
            PlayerBase ratingPlayer = PlayerBase.Cast( ratingPlayers.Get( i ) );
            if ( !ratingPlayer )
                continue;

            ratingPlayer.m_Trader_RatingEnabled = true;
            ratingPlayer.m_Trader_RatingMaxDiscount = m_Trader_RatingMaxDiscount;
            ratingPlayer.m_Trader_RatingFullHours = m_Trader_RatingFullHours;
            ratingPlayer.m_Trader_RatingCurve = m_Trader_RatingCurve;
            ratingPlayer.m_Trader_RatingFullDistance = m_Trader_RatingFullDistance;
            ratingPlayer.m_Trader_RatingFullKills = m_Trader_RatingFullKills;
            ratingPlayer.m_Trader_RatingWeightTime = m_Trader_RatingWeightTime;
            ratingPlayer.m_Trader_RatingWeightDistance = m_Trader_RatingWeightDistance;
            ratingPlayer.m_Trader_RatingWeightKills = m_Trader_RatingWeightKills;
            ratingPlayer.m_Trader_SoundEnabled = m_Trader_SoundEnabled;
            ratingPlayer.m_Trader_SoundRange = m_Trader_SoundRange;
            ratingPlayer.m_Trader_KillRewardAmount = TraderComputeKillReward();

            if ( ratingPlayer.IsAlive() )
            {
                ratingPlayer.m_Trader_LifeAccum = ratingPlayer.m_Trader_LifeAccum + elapsed;
                int wholeSeconds = Math.Floor( ratingPlayer.m_Trader_LifeAccum );
                if ( wholeSeconds > 0 )
                {
                    ratingPlayer.m_Trader_LifeAccum = ratingPlayer.m_Trader_LifeAccum - wholeSeconds;
                    ratingPlayer.m_Trader_LifeSeconds = ratingPlayer.m_Trader_LifeSeconds + wholeSeconds;
                }

                // Пройденный путь. Телепорты (больше 50 м за тик) не считаем.
                vector nowPos = ratingPlayer.GetPosition();
                if ( ratingPlayer.m_Trader_HasLastPos )
                {
                    float step = vector.Distance( ratingPlayer.m_Trader_LastPos, nowPos );
                    if ( step > 0.01 && step < 50.0 )
                        ratingPlayer.m_Trader_DistanceAccum = ratingPlayer.m_Trader_DistanceAccum + step;
                }
                ratingPlayer.m_Trader_LastPos = nowPos;
                ratingPlayer.m_Trader_HasLastPos = true;

                int wholeMeters = Math.Floor( ratingPlayer.m_Trader_DistanceAccum );
                if ( wholeMeters > 0 )
                {
                    ratingPlayer.m_Trader_DistanceAccum = ratingPlayer.m_Trader_DistanceAccum - wholeMeters;
                    ratingPlayer.m_Trader_RatingDistance = ratingPlayer.m_Trader_RatingDistance + wholeMeters;
                }
            }

            ratingPlayer.TraderRating_Recalc();
        }
    }
    
    void SpawnTraderObjects()
    {
        m_Trader_NPCDummyClasses = new array<string>;
        m_Trader_ObjectsList = new array<Object>;
        TraderMessage.ServerLog("[TRADER] READING TRADER OBJECTS FILE");
        FileHandle file_index = OpenFile(m_Trader_ObjectsFilePath, FileMode.READ);
                
        if ( file_index == 0 )
        {
            TraderMessage.ServerLog( "[TRADER] FOUND NO TRADEROBJECTS FILE!" );
            return;
        }
        
        int markerCounter = 0;
        bool skipDirEntry = false;
        
        string line_content = "";
        while ( markerCounter <= 5000 && line_content.Contains("<FileEnd>") == false)
        {
            if (skipDirEntry)
                skipDirEntry = false;
            else
                line_content = TraderNpcText.NextTerm(file_index, "<Object>", "<FileEnd>");
            
            if (!line_content.Contains("<Object>"))
                continue;
            
            line_content.Replace("<Object>", "");
            line_content = TraderNpcText.Clean(line_content);
            line_content = TraderNpcText.Tidy(line_content);
            
            string traderObjectType = line_content;
            TraderMessage.ServerLog("[TRADER] OBJECT TYPE ENTRY " + line_content);
            
            line_content = TraderNpcText.NextTerm(file_index, "<ObjectPosition>", "<FileEnd>");
            
            line_content.Replace("<ObjectPosition>", "");
            line_content = TraderNpcText.Clean(line_content);
            
            TStringArray strso = new TStringArray;
            line_content.Split( ",", strso );
            
            string traderObjectPosX = strso.Get(0);
            traderObjectPosX = TraderNpcText.Tidy(traderObjectPosX);
            
            string traderObjectPosY = strso.Get(1);
            traderObjectPosY = TraderNpcText.Tidy(traderObjectPosY);
            
            string traderObjectPosZ = strso.Get(2);
            traderObjectPosZ = TraderNpcText.Tidy(traderObjectPosZ);
            
            vector objectPosition = "0 0 0";
            objectPosition[0] = traderObjectPosX.ToFloat();
            objectPosition[1] = traderObjectPosY.ToFloat();
            objectPosition[2] = traderObjectPosZ.ToFloat();

            TraderMessage.ServerLog("[TRADER] OBJECT POSITION = '" + objectPosition + "'");
            
            line_content = TraderNpcText.NextTerm(file_index, "<ObjectOrientation>", "<FileEnd>");
            
            line_content.Replace("<ObjectOrientation>", "");
            line_content = TraderNpcText.Clean(line_content);

            TStringArray strsod = new TStringArray;
            line_content.Split( ",", strsod );
            
            string traderObjectOriX = strsod.Get(0);
            traderObjectOriX = TraderNpcText.Tidy(traderObjectOriX);
            
            string traderObjectOriY = strsod.Get(1);
            traderObjectOriY = TraderNpcText.Tidy(traderObjectOriY);
            
            string traderObjectOriZ = strsod.Get(2);
            traderObjectOriZ = TraderNpcText.Tidy(traderObjectOriZ);
            
            vector objectOrientation = vector.Zero;
            objectOrientation[0] = traderObjectOriX.ToFloat();
            objectOrientation[1] = traderObjectOriY.ToFloat();
            objectOrientation[2] = traderObjectOriZ.ToFloat();

            TraderMessage.ServerLog("[TRADER] OBJECT ORIENTATION = '" + objectOrientation + "'");

            array<Object> persistanceCheck_nearby_objects = new array<Object>;
            GetGame().GetObjectsAtPosition(objectPosition, 2, persistanceCheck_nearby_objects, null);
            bool foundItem = false;
            Object objectNearby ;
            
            for (int i = 0; i < persistanceCheck_nearby_objects.Count(); i++)
            {
                objectNearby = persistanceCheck_nearby_objects.Get(i);
                if(objectNearby && objectNearby.IsKindOf(traderObjectType))
                {                        
                    foundItem = true;
                    TraderMessage.ServerLog("TraderObject: " + traderObjectType + " was found already at " + objectPosition + ". Persistent item won't be spawned again.");
                    m_Trader_ObjectsList.Insert(objectNearby);
                    break;
                }
            }
            Object newtraderObj;
            bool isTrader = false;    
            PlayerBase man;
            if(!foundItem)
            {
                newtraderObj = GetGame().CreateObjectEx(traderObjectType, objectPosition, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOPERSISTENCY_WORLD);
                if (newtraderObj)
                {            
                    m_Trader_ObjectsList.Insert(newtraderObj);
                    newtraderObj.SetPosition(objectPosition);
                    newtraderObj.SetOrientation(objectOrientation);
                    EntityAI entity = EntityAI.Cast(newtraderObj);
                    if(entity)
                    {
                        entity.SetLifetime(316224000);
                        entity.SetLifetimeMax(316224000);
                    }
                    TraderMessage.ServerLog("TraderObject: " + traderObjectType + " spawned at " + objectPosition);
                    
                    if (Class.CastTo(man, newtraderObj))
                    {
                        TraderMessage.ServerLog("[TRADER] Object was a Man..");
                        isTrader = true;
                        man.SetAllowDamage(false);
                    }
                }
                else
                {
                    TraderMessage.ServerLog("TraderObject: " + traderObjectType + " could NOT be spawned at " + objectPosition + ". Please check class name is correct.");
                }               
            }            

            int attachmentCounter = 0;
            while ( attachmentCounter <= 1000 && line_content.Contains("<Object>") == false)
            {
                line_content = TraderNpcText.NextTerms(file_index, {"<ObjectAttachment>", "<OpenFile>"}, "<Object>");

                if (line_content == string.Empty)    
                {
                    line_content = "<FileEnd>";
                    break;
                }

                if (line_content.Contains("<OpenFile>"))
                {
                    if (OpenNewFileForReading(line_content, file_index))
                        continue;
                    else
                        return;
                }

                if (line_content.Contains("<Object>"))
                {
                    skipDirEntry = true;
                    markerCounter++;
                    break;
                }

                line_content.Replace("<ObjectAttachment>", "");
                line_content = TraderNpcText.Clean(line_content);

                if (isTrader)
                {
                    man.GetInventory().CreateInInventory(line_content);
                    TraderMessage.ServerLog("[TRADER] '" + line_content + "' WAS ATTACHED");
                }
                else
                {
                    if (line_content == "NPC_DUMMY")
                    {
                        if(foundItem)
                        {
                            RegisterDummy(objectNearby);
                        }
                        else
                        {                            
                            RegisterDummy(newtraderObj);
                        }
                    }
                    else
                    {    
                        TraderMessage.ServerLog("[TRADER] OBJECT TO ATTACH WAS INVALID!");
                    }
                }

                attachmentCounter++;
            }
        }
        
        CloseFile(file_index);
    }

    void RegisterDummy(Object traderObj)
    {
        if (traderObj)
        {
            m_Trader_NPCDummyClasses.Insert(traderObj.GetType());
            TraderMessage.ServerLog("[TRADER] NPC DUMMY WAS REGISTERED!");
        }
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        if(!m_Trader_ReadAllTraderData)
        {
            TraderMessage.ServerLog( "[TRADER] Trader data was not ready!" );
        }
        if (!player.HasReceivedAllTraderData())
        {    
            sendTraderDataToPlayer(player);
        }
    }

    void sendTraderDataToPlayer(PlayerBase player)
    {
        Param1<bool> crpClr = new Param1<bool>( true );
        GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_CLEAR, crpClr, true, player.GetIdentity());

        player.m_Trader_CurrencyName = m_Trader_CurrencyName;
        Param1<string> crp0 = new Param1<string>( m_Trader_CurrencyName );
        GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_CURRENCYNAME_ENTRY, crp0, true, player.GetIdentity());

        int i = 0;
        player.m_Trader_CurrencyClassnames = new array<string>;
        player.m_Trader_CurrencyValues = new array<int>;
        for ( i = 0; i < m_Trader_CurrencyClassnames.Count(); i++ )
        {
            player.m_Trader_CurrencyClassnames.Insert(m_Trader_CurrencyClassnames.Get(i));
            player.m_Trader_CurrencyValues.Insert(m_Trader_CurrencyValues.Get(i));

            Param2<string, int> crp1 = new Param2<string, int>( m_Trader_CurrencyClassnames.Get(i), m_Trader_CurrencyValues.Get(i) );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_CURRENCY_ENTRY, crp1, true, player.GetIdentity());
        }
        
        for ( i = 0; i < m_Trader_TraderNames.Count(); i++ )
        {
            Param1<string> crp2 = new Param1<string>( m_Trader_TraderNames.Get(i) );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_NAME_ENTRY, crp2, true, player.GetIdentity());
        }
        
        for ( i = 0; i < m_Trader_Categorys.Count(); i++ )
        {
            Param2<string, int> crp3 = new Param2<string, int>( m_Trader_Categorys.Get(i), m_Trader_CategorysTraderKey.Get(i) );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_CATEGORY_ENTRY, crp3, true, player.GetIdentity());
        }

        for ( i = 0; i < m_Trader_NPCDummyClasses.Count(); i++ )
        {
            Param<string> crp6 = new Param1<string>( m_Trader_NPCDummyClasses.Get(i) );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_NPCDUMMY_ENTRY, crp6, true, player.GetIdentity());
        }
        
                        
        player.m_Trader_ItemsClassnames = new array<string>;
        player.m_Trader_ItemsQuantity = new array<int>;
        player.m_Trader_ItemsBuyValue = new array<int>;
        player.m_Trader_ItemsSellValue = new array<int>;
        player.m_Trader_ItemsAmmo = new TStringArray;
        player.m_Trader_ItemsTraderId = new array<int>;
        player.m_Trader_ItemsCategoryId = new array<int>;
        for ( i = 0; i < m_Trader_ItemsClassnames.Count(); i++ )
        {
            player.m_Trader_ItemsClassnames.Insert(m_Trader_ItemsClassnames.Get(i));
            player.m_Trader_ItemsQuantity.Insert(m_Trader_ItemsQuantity.Get(i));
            player.m_Trader_ItemsBuyValue.Insert(m_Trader_ItemsBuyValue.Get(i));
            player.m_Trader_ItemsSellValue.Insert(m_Trader_ItemsSellValue.Get(i));
            if ( m_Trader_ItemsAmmo && i < m_Trader_ItemsAmmo.Count() )
                player.m_Trader_ItemsAmmo.Insert(m_Trader_ItemsAmmo.Get(i));
            else
                player.m_Trader_ItemsAmmo.Insert("");
            player.m_Trader_ItemsTraderId.Insert(m_Trader_ItemsTraderId.Get(i));
            player.m_Trader_ItemsCategoryId.Insert(m_Trader_ItemsCategoryId.Get(i));

            string rowAmmo = "";
            if ( m_Trader_ItemsAmmo && i < m_Trader_ItemsAmmo.Count() )
                rowAmmo = m_Trader_ItemsAmmo.Get(i);
            Param7<int, int, string, int, int, int, string> crp4 = new Param7<int, int, string, int, int, int, string>( m_Trader_ItemsTraderId.Get(i), m_Trader_ItemsCategoryId.Get(i), m_Trader_ItemsClassnames.Get(i), m_Trader_ItemsQuantity.Get(i), m_Trader_ItemsBuyValue.Get(i), m_Trader_ItemsSellValue.Get(i), rowAmmo );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_ITEM_ENTRY, crp4, true, player.GetIdentity());
        }
        
        player.m_Trader_TraderPositions = new array<vector>;
        player.m_Trader_TraderVehicleSpawns = new array<vector>;
        player.m_Trader_TraderVehicleSpawnsOrientation = new array<vector>;
        player.m_Trader_TraderSafezones = new array<int>;
        for ( i = 0; i < m_Trader_TraderPositions.Count(); i++ )
        {
            player.m_Trader_TraderPositions.Insert(m_Trader_TraderPositions.Get(i));
            player.m_Trader_TraderVehicleSpawns.Insert(m_Trader_TraderVehicleSpawns.Get(i));
            player.m_Trader_TraderVehicleSpawnsOrientation.Insert(m_Trader_TraderVehicleSpawnsOrientation.Get(i));
            player.m_Trader_TraderSafezones.Insert(m_Trader_TraderSafezones.Get(i));

            Param5<int, vector, int, vector, vector> crp5 = new Param5<int, vector, int, vector, vector>( m_Trader_TraderIDs.Get(i), m_Trader_TraderPositions.Get(i), m_Trader_TraderSafezones.Get(i), m_Trader_TraderVehicleSpawns.Get(i), m_Trader_TraderVehicleSpawnsOrientation.Get(i) );
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_MARKER_ENTRY, crp5, true, player.GetIdentity());
        }

        player.m_Trader_BuySellTimer = m_Trader_BuySellTimer;
        GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_VARIABLES_ENTRY, new Param1<float>(m_Trader_BuySellTimer), true, player.GetIdentity());

        player.m_Trader_PlayerUID = player.GetIdentity().GetPlainId();
        GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_PLAYERUID, new Param1<string>(player.GetIdentity().GetPlainId()), true, player.GetIdentity());

        player.m_Trader_AdminPlayerUIDs = new array<string>;
        for ( i = 0; i < m_Trader_AdminPlayerUIDs.Count(); i++ )
        {
            player.m_Trader_AdminPlayerUIDs.Insert(m_Trader_AdminPlayerUIDs.Get(i));
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_TRADER_ADMINS_ENTRY, new Param1<string>(m_Trader_AdminPlayerUIDs.Get(i)), true, player.GetIdentity());
        }
                
        player.m_Trader_RatingEnabled = m_Trader_RatingEnabled;
        player.m_Trader_RatingMaxDiscount = m_Trader_RatingMaxDiscount;
        player.m_Trader_RatingFullHours = m_Trader_RatingFullHours;
        player.m_Trader_RatingCurve = m_Trader_RatingCurve;
        player.m_Trader_RatingFullDistance = m_Trader_RatingFullDistance;
        player.m_Trader_RatingFullKills = m_Trader_RatingFullKills;
        player.m_Trader_RatingWeightTime = m_Trader_RatingWeightTime;
        player.m_Trader_RatingWeightDistance = m_Trader_RatingWeightDistance;
        player.m_Trader_RatingWeightKills = m_Trader_RatingWeightKills;
        player.m_Trader_SoundEnabled = m_Trader_SoundEnabled;
        player.m_Trader_SoundRange = m_Trader_SoundRange;
        player.m_Trader_KillRewardAmount = TraderComputeKillReward();
        player.TraderRating_Recalc();

        player.SetReceivedAllTraderData(true);
        player.m_Trader_SafezoneShowDebugShapes = m_Trader_SafezoneShowDebugShapes;
        player.m_Trader_TradingDistance = m_Trader_TradingDistance;
        player.SetSynchDirty();
    }

    // Награда за заражённого: цена 1 шт. самого слабого патрона из конфига
    // (либо явное значение <KillReward>). Минимум 1.
    int TraderComputeKillReward()
    {
        if ( m_Trader_KillReward > 0 )
            return m_Trader_KillReward;

        if ( !m_Trader_ItemsClassnames || !m_Trader_ItemsQuantity || !m_Trader_ItemsSellValue )
            return 1;

        int best = 0;
        for ( int i = 0; i < m_Trader_ItemsClassnames.Count(); i++ )
        {
            string cls = m_Trader_ItemsClassnames.Get( i );
            if ( !cls.Contains( "Ammo_" ) )
                continue;

            if ( m_Trader_ItemsQuantity.Get( i ) != 1 )
                continue;

            int sell = m_Trader_ItemsSellValue.Get( i );
            if ( sell <= 0 )
                continue;

            if ( best == 0 || sell < best )
                best = sell;
        }

        if ( best <= 0 )
        {
            for ( int j = 0; j < m_Trader_ItemsSellValue.Count(); j++ )
            {
                int sell2 = m_Trader_ItemsSellValue.Get( j );
                if ( sell2 <= 0 )
                    continue;
                if ( best == 0 || sell2 < best )
                    best = sell2;
            }
        }

        if ( best < 1 )
            best = 1;

        return best;
    }

    void readTraderVariables()
    {

        // Если профиля нет - создаём его (папка Trader_NPC_Prof) с дефолтами
        TraderNpcProfile.EnsureDefaults();
        TraderMessage.ServerLog("[TRADER] READING TRADER VARIABLES FILE");

        FileHandle file_index = OpenFile(m_Trader_VariableFilePath, FileMode.READ);
        
        if ( file_index == 0 )
        {
            TraderMessage.ServerLog("[TRADER] FOUND NO TRADERVARIABLE FILE!");
            return;
        }

        int variableCounter = 0;
        
        string line_content = "";
        while (variableCounter <= 500 && !line_content.Contains("<FileEnd>"))
        {
            bool validEntry = false;

            line_content = "";
            int char_count = FGets( file_index,  line_content );
            line_content = TraderNpcText.Clean(line_content);

            if (line_content.Contains("<BuySellTimer>"))
            {
                line_content.Replace("<BuySellTimer>", "");
                line_content = TraderNpcText.Clean(line_content);

                m_Trader_BuySellTimer = line_content.ToFloat();
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] BuySellTimer = " + line_content);
            }

            // Единый реестр переключателей функций: <FeatureXxx> yes|no
            if ( TraderNpcFeatures.Apply( line_content ) )
            {
                validEntry = true;

                string autoOff = TraderNpcFeatures.AutoDetectConflicts();
                if ( autoOff != "" )
                    TraderMessage.ServerLog( "[TRADER] auto-disabled for:" + autoOff );

                TraderMessage.ServerLog( "[TRADER] FEATURES:" + TraderNpcFeatures.Report() + " (changed by " + line_content + ")" );
            }

            if (line_content.Contains("<SafezoneTimeout>"))
            {
                line_content.Replace("<SafezoneTimeout>", "");
                line_content = TraderNpcText.Clean(line_content);

                m_Trader_SafezoneTimeout = line_content.ToFloat();
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SafezoneTimeout = " + line_content);
            }
            string lowerLine;
            if (line_content.Contains("<SafezoneRemoveAnimals>"))
            {
                line_content.Replace("<SafezoneRemoveAnimals>", "");
                line_content = TraderNpcText.Clean(line_content);
                lowerLine = line_content;
                lowerLine.ToLower();
                if(lowerLine.Contains("yes"))
                {
                    m_Trader_SafezoneRemoveAnimals = true;
                }
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SafezoneRemoveAnimals = " + line_content);
            }
            
            if (line_content.Contains("<SafezoneRemoveInfected>"))
            {
                line_content.Replace("<SafezoneRemoveInfected>", "");
                line_content = TraderNpcText.Clean(line_content);
                lowerLine = line_content;
                lowerLine.ToLower();
                if(lowerLine.Contains("yes"))
                {
                    m_Trader_SafezoneRemoveInfected = true;
                }
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SafezoneRemoveInfected = " + line_content);
            }
            
            if (line_content.Contains("<SafezoneRemoveEAI>"))
            {
                line_content.Replace("<SafezoneRemoveEAI>", "");
                line_content = TraderNpcText.Clean(line_content);
                lowerLine = line_content;
                lowerLine.ToLower();
                if(lowerLine.Contains("yes"))
                {
                    m_Trader_SafezoneRemoveEAI = true;
                }
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SafezoneRemoveEAI = " + line_content);
            }
            if (line_content.Contains("<SafezoneShowDebugShape>"))
            {
                line_content.Replace("<SafezoneShowDebugShape>", "");
                line_content = TraderNpcText.Clean(line_content);
                lowerLine = line_content;
                lowerLine.ToLower();
                if(lowerLine.Contains("yes"))
                {
                    m_Trader_SafezoneShowDebugShapes = true;
                }
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SafezoneShowDebugShape = " + line_content);
            }
            if (line_content.Contains("<TradingDistance>"))
            {
                line_content.Replace("<TradingDistance>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_TradingDistance = line_content.ToFloat();
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] TradingDistance = " + line_content);
            }

            // ---- survival rating -> trader discount ----
            if (line_content.Contains("<RatingEnabled>"))
            {
                line_content.Replace("<RatingEnabled>", "");
                line_content = TraderNpcText.Clean(line_content);
                lowerLine = line_content;
                lowerLine.ToLower();
                m_Trader_RatingEnabled = lowerLine.Contains("yes");
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingEnabled = " + line_content);
            }

            if (line_content.Contains("<RatingMaxDiscount>"))
            {
                line_content.Replace("<RatingMaxDiscount>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingMaxDiscount = line_content.ToInt();
                if ( m_Trader_RatingMaxDiscount < 0 ) m_Trader_RatingMaxDiscount = 0;
                if ( m_Trader_RatingMaxDiscount > 100 ) m_Trader_RatingMaxDiscount = 100;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingMaxDiscount = " + line_content);
            }

            if (line_content.Contains("<RatingFullHours>"))
            {
                line_content.Replace("<RatingFullHours>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingFullHours = line_content.ToInt();
                if ( m_Trader_RatingFullHours < 1 ) m_Trader_RatingFullHours = 1;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingFullHours = " + line_content);
            }

            if (line_content.Contains("<RatingCurve>"))
            {
                line_content.Replace("<RatingCurve>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingCurve = line_content.ToFloat();
                if ( m_Trader_RatingCurve < 0.1 ) m_Trader_RatingCurve = 0.1;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingCurve = " + line_content);
            }

            if (line_content.Contains("<RatingFullDistance>"))
            {
                line_content.Replace("<RatingFullDistance>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingFullDistance = line_content.ToInt();
                if ( m_Trader_RatingFullDistance < 1 ) m_Trader_RatingFullDistance = 1;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingFullDistance = " + line_content);
            }

            if (line_content.Contains("<RatingFullKills>"))
            {
                line_content.Replace("<RatingFullKills>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingFullKills = line_content.ToInt();
                if ( m_Trader_RatingFullKills < 1 ) m_Trader_RatingFullKills = 1;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] RatingFullKills = " + line_content);
            }

            if (line_content.Contains("<RatingWeightTime>"))
            {
                line_content.Replace("<RatingWeightTime>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingWeightTime = line_content.ToFloat();
                if ( m_Trader_RatingWeightTime < 0 ) m_Trader_RatingWeightTime = 0;
                validEntry = true;
            }

            if (line_content.Contains("<RatingWeightDistance>"))
            {
                line_content.Replace("<RatingWeightDistance>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingWeightDistance = line_content.ToFloat();
                if ( m_Trader_RatingWeightDistance < 0 ) m_Trader_RatingWeightDistance = 0;
                validEntry = true;
            }

            if (line_content.Contains("<RatingWeightKills>"))
            {
                line_content.Replace("<RatingWeightKills>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_RatingWeightKills = line_content.ToFloat();
                if ( m_Trader_RatingWeightKills < 0 ) m_Trader_RatingWeightKills = 0;
                validEntry = true;
            }

            if (line_content.Contains("<SoundEnabled>"))
            {
                m_Trader_SoundEnabled = ( line_content.Contains("yes") || line_content.Contains("YES") );
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] SoundEnabled = " + line_content);
            }

            if (line_content.Contains("<SoundRange>"))
            {
                line_content.Replace("<SoundRange>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_SoundRange = line_content.ToFloat();
                if ( m_Trader_SoundRange < 1 ) m_Trader_SoundRange = 1;
                validEntry = true;
            }

            if (line_content.Contains("<KillReward>"))
            {
                line_content.Replace("<KillReward>", "");
                line_content = TraderNpcText.Clean(line_content);
                m_Trader_KillReward = line_content.ToInt();
                if ( m_Trader_KillReward < 0 ) m_Trader_KillReward = 0;
                validEntry = true;

                TraderMessage.ServerLog("[TRADER] KillReward = " + line_content);
            }

            if (validEntry)
                variableCounter++;
        }

        CloseFile(file_index);

        TraderMessage.ServerLog("[TRADER] DONE END!");
    }

    void readTraderAdmins()
    {
        TraderMessage.ServerLog("[TRADER] READING TRADER ADMINS FILE");
        m_Trader_AdminPlayerUIDs = new array<string>;    

        FileHandle file_index = OpenFile(m_Trader_AdminsFilePath, FileMode.READ);
        
        if ( file_index == 0 )
        {
            TraderMessage.ServerLog("[TRADER] FOUND NO TRADERADMINS FILE!");
            return;
        }

        int adminsCounter = 0;
        
        string line_content = "";
        while (adminsCounter <= 500 && !line_content.Contains("<FileEnd>"))
        {
            line_content = "";
            int char_count = FGets( file_index,  line_content );
            line_content = TraderNpcText.Clean(line_content);

            if (line_content.Contains("<FileEnd>") || line_content.Length() < 16)
                continue;

            TraderMessage.ServerLog("[TRADER] ADMIN PLAYER UID ENTRY " + line_content);
            m_Trader_AdminPlayerUIDs.Insert(line_content);

            adminsCounter++;
        }

        CloseFile(file_index);
    }

    bool OpenNewFileForReading(string line_content, out FileHandle file_index)
    {
        line_content.Replace("<OpenFile>", "");
        line_content = TraderNpcText.Clean(line_content);

        CloseFile(file_index);
        file_index = OpenFile("$profile:Trader_NPC_Prof/" + line_content, FileMode.READ);

        if ( file_index == 0 )
        {
            TraderMessage.ServerLog("[TRADER] CANT FIND LINKED FILE " + "$profile:Trader_NPC_Prof/" + line_content + "!");
            return false;
        }
        
        return true;
    }

    Object FindTraderObjectAtPosition(vector position)
    {
        foreach(Object traderObj : m_Trader_ObjectsList)
        {
            if(traderObj)
            {
                float distance = vector.Distance(position, traderObj.GetPosition());
                if(distance < 0.99)
                {
                    return traderObj;
                }
            }
        }
        return null;
    }

    void readTraderData()
    {        
        m_Trader_ReadAllTraderData = false;    
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
        
        FileHandle file_index = OpenFile(m_Trader_ConfigFilePath, FileMode.READ);
        
        TraderMessage.ServerLog("[TRADER] READING TRADER CURRENCY, PRICES AND CATEGORIES FILES");
        if ( file_index == 0 )
        {
            TraderMessage.ServerLog("[TRADER] FOUND NO TRADERCONFIG FILE!");
            return;
        }
        
        string line_content = "";
        
        line_content = TraderNpcText.NextTerm(file_index, "<CurrencyName>", "");
        line_content.Replace("<CurrencyName>", "");
        line_content = TraderNpcText.Clean(line_content);
        m_Trader_CurrencyName = line_content;
        TraderMessage.ServerLog("[TRADER] CURRENCY NAME ENTRY " + m_Trader_CurrencyName);

        int currencyCounter = 0;

        line_content = "";
        while (currencyCounter <= 500 && !line_content.Contains("<Trader>"))
        {
            line_content = TraderNpcText.NextTerm(file_index, "<Currency>", "<Trader>");
            line_content.Replace("<Currency>", "");
            line_content = TraderNpcText.Clean(line_content);

            if (line_content.Contains("<Trader>"))
                break;

            TStringArray crys = new TStringArray;
            line_content.Split( ",", crys );

            string currencyClassname = crys.Get(0);
            currencyClassname = TraderNpcText.Tidy(currencyClassname);
            
            string currencyValue = crys.Get(1);
            currencyValue = TraderNpcText.Tidy(currencyValue);

            m_Trader_CurrencyClassnames.Insert(currencyClassname);
            m_Trader_CurrencyValues.Insert(currencyValue.ToInt());

            TraderMessage.ServerLog("[TRADER] CURRENCY ENTRY " + currencyClassname + " value: " + currencyValue);

            currencyCounter++;
        }

        bool traderInstanceDone = true;
        int traderCounter = 0;
        
        while (traderCounter <= 5000 && line_content != "<FileEnd>")
        {            
            if (traderInstanceDone == false)
                line_content = TraderNpcText.NextTerms(file_index, {"<Trader>", "<OpenFile>"}, "");
            else
                traderInstanceDone = false;
            
            if (line_content.Contains("<OpenFile>"))
            {
                if (OpenNewFileForReading(line_content, file_index))
                    continue;
                else
                    return;
            }

            line_content.Replace("<Trader>", "");
            line_content = TraderNpcText.Clean(line_content);
            
            TraderMessage.ServerLog("[TRADER] READING TRADER ENTRY " + line_content);
            m_Trader_TraderNames.Insert(line_content);
                
            int categoryCounter = 0;
            
            line_content = "";
            while (categoryCounter <= 5000 && line_content != "<FileEnd>")
            {
                line_content = TraderNpcText.Clean(TraderNpcText.NextTerms(file_index, {"<Category>", "<OpenFile>"}, "<Trader>"));
                
                if (line_content.Contains("<OpenFile>"))
                {
                    if (OpenNewFileForReading(line_content, file_index))
                        continue;
                    else
                        return;
                }

                if (line_content.Contains("<Trader>"))
                {
                    traderInstanceDone = true;
                    break;
                }
                
                if (line_content == string.Empty)
                {
                    line_content = "<FileEnd>";
                    break;
                }
                
                line_content.Replace("<Category>", "");
                string category = TraderNpcText.Clean(line_content);
                m_Trader_Categorys.Insert(category);
                m_Trader_CategorysTraderKey.Insert(traderCounter);
                TraderMessage.ServerLog("[TRADER] READING CATEGORY ENTRY " + category);
                
                categoryCounter++;
            }
            
            traderCounter++;
        }
        
        CloseFile(file_index);
        
        //------------------------------------------------------------------------------------
        
        file_index = OpenFile(m_Trader_ConfigFilePath, FileMode.READ);
        
        int itemCounter = 0;
        int itemCounterTotal = 0;
        int char_count = 0;
        int traderID = -1;
        int categoryId = -1;

        // ---- price defaults (см. <SellCoef> в TraderConfig.txt) ----
        // Строку можно писать в 3 поля: "Class, Qty, Buy" - цена продажи тогда
        // считается как Buy * SellCoef. SellCoef задаётся на трейдера и, при
        // желании, переопределяется в отдельной категории. Явные 4 поля всегда
        // имеют приоритет и не масштабируются - поведение старых конфигов не меняется.
        float sellCoefTrader = 0.10;
        float sellCoefCategory = -1.0;
        bool inCategoryBlock = false;
        
        line_content = "";
        while ( itemCounter <= 10000 && char_count != -1 && line_content.Contains("<FileEnd>") == false)
        {
            char_count = FGets( file_index,  line_content );
            
            line_content = TraderNpcText.Clean(line_content);

            if (line_content.Contains("<OpenFile>"))
            {
                if (OpenNewFileForReading(line_content, file_index))
                    continue;
                else
                    return;
            }

            if (line_content.Contains("<Trader>"))
            {
                traderID++;
                itemCounter = 0;
                sellCoefTrader = 0.10;
                sellCoefCategory = -1.0;
                inCategoryBlock = false;

                continue;
            }
            
            if (line_content.Contains("<Category>"))
            {
                categoryId++;
                itemCounter = 0;
                inCategoryBlock = true;

                continue;
            }
        
            if (line_content.Contains("<SellCoef>"))
            {
                float newCoef = TraderNpcText.Tidy(TraderAutoPrices.TagValue(line_content)).ToFloat();
                if (newCoef > 0.0 && newCoef <= 1.0)
                {
                    if (inCategoryBlock)
                        sellCoefCategory = newCoef;
                    else
                        sellCoefTrader = newCoef;
                }
                continue;
            }

            if (!line_content.Contains(","))
                continue;

            if (line_content.Contains("<Currency"))
                continue;

            TStringArray strs = new TStringArray;
            line_content.Split( ",", strs );
            
            string itemStr = strs.Get(0);
            itemStr = TraderNpcText.Tidy(itemStr);
            
            string qntStr = strs.Get(1);
            qntStr = TraderNpcText.Tidy(qntStr);
            
            if (qntStr.Contains("*") || qntStr.Contains("-1"))
            {
                qntStr = GetItemMaxQuantity(itemStr).ToString();
            }

            if (qntStr == "M" || qntStr == "m")
                qntStr = "-3";

            if (qntStr == "W" || qntStr == "w")
                qntStr = "-4";

            if (qntStr == "S" || qntStr == "s")
                qntStr = "-5";
            
            if (strs.Count() < 3)
            {
                TraderMessage.ServerLog("[TRADER] SKIPPED ROW (need at least Class, Qty, Buy): " + line_content);
                continue;
            }

            string buyStr = strs.Get(2);
            buyStr = TraderNpcText.Tidy(buyStr);

            // 4th field is optional: "Class, Qty, Buy" -> sell = Buy * SellCoef,
            // and "*" in the sell field does the same for an explicit buy price.
            string sellStr = "*";
            if (strs.Count() >= 4)
                sellStr = TraderNpcText.Tidy(strs.Get(3));

            // понятные слова вместо "-1": nobuy = торговец не продаёт, nosell = не покупает
            if ( buyStr == "nobuy" || buyStr == "NOBUY" )
                buyStr = "-1";
            if ( sellStr == "nosell" || sellStr == "NOSELL" )
                sellStr = "-1";

            int buyValue = buyStr.ToInt();
            int sellValue = sellStr.ToInt();
            if (sellStr == "*" || sellStr == "" || sellStr == "AUTO")
            {
                float coef = sellCoefTrader;
                if (sellCoefCategory >= 0.0)
                    coef = sellCoefCategory;
                if (buyValue > 0)
                    sellValue = Math.Round(buyValue * coef);
                else
                    sellValue = buyValue;
            }

            m_Trader_ItemsTraderId.Insert(traderID);
            m_Trader_ItemsCategoryId.Insert(categoryId);
            m_Trader_ItemsClassnames.Insert(itemStr);
            m_Trader_ItemsQuantity.Insert(qntStr.ToInt());
            m_Trader_ItemsBuyValue.Insert(buyValue);
            m_Trader_ItemsSellValue.Insert(sellValue);

            // 5-е поле (необязательное): патрон для магазина, "" = базовый,
            // "empty" = пустой, иначе класс патрона.
            string ammoStr = "";
            if ( strs.Count() >= 5 )
                ammoStr = TraderNpcText.Tidy( strs.Get( 4 ) );
            m_Trader_ItemsAmmo.Insert( ammoStr );
            
            itemCounter++;
        }
        
        CloseFile(file_index);
        
        //------------------------------------------------------------------------------------
        
        file_index = OpenFile(m_Trader_ObjectsFilePath, FileMode.READ);
        
        if ( file_index == 0 )
        {
            TraderMessage.ServerLog("[TRADER] FOUND NO TRADEROBJECTS FILE!");
            return;
        }
        
        bool skipLine = false;
        int markerCounter = 0;                

        line_content = "";
        int currentTraderID = 0;        
        while ( markerCounter <= 5000 && line_content.Contains("<FileEnd>") == false)
        {
            if (!skipLine)
                line_content = TraderNpcText.NextTerms(file_index, {"<TraderMarker>", "<OpenFile>"}, "<FileEnd>");
            else
                skipLine = false;    

            if (line_content.Contains("<OpenFile>"))
            {
                if (!OpenNewFileForReading(line_content, file_index))
                    return;
            }                
            
            if (!line_content.Contains("<TraderMarker>"))
                continue;
            
            line_content.Replace("<TraderMarker>", "");
            line_content = TraderNpcText.Clean(line_content);
            line_content = TraderNpcText.Tidy(line_content);
            
            TraderMessage.ServerLog("[TRADER] MARKER ID ENTRY " + line_content);
            currentTraderID = line_content.ToInt();
            m_Trader_TraderIDs.Insert(currentTraderID);
            
            line_content = TraderNpcText.NextTerm(file_index, "<TraderMarkerPosition>", "<FileEnd>");
            
            line_content.Replace("<TraderMarkerPosition>", "");
            line_content = TraderNpcText.Clean(line_content);
            
            TStringArray strsm = new TStringArray;
            line_content.Split( ",", strsm );
            
            string traderMarkerPosX = strsm.Get(0);
            traderMarkerPosX = TraderNpcText.Tidy(traderMarkerPosX);
            
            string traderMarkerPosY = strsm.Get(1);
            traderMarkerPosY = TraderNpcText.Tidy(traderMarkerPosY);
            
            string traderMarkerPosZ = strsm.Get(2);
            traderMarkerPosZ = TraderNpcText.Tidy(traderMarkerPosZ);
            
            vector markerPosition = "0 0 0";
            markerPosition[0] = traderMarkerPosX.ToFloat();
            markerPosition[1] = traderMarkerPosY.ToFloat();
            markerPosition[2] = traderMarkerPosZ.ToFloat();
            
            m_Trader_TraderPositions.Insert(markerPosition);
            Object traderAtPos = FindTraderObjectAtPosition(markerPosition);            
            if(traderAtPos)
            {
                BuildingBase buildingItem = BuildingBase.Cast(traderAtPos);
                if(buildingItem)
                {
                    buildingItem.m_Trader_TraderIndex = m_Trader_TraderIDs.Count() - 1;
                    buildingItem.SetSynchDirty();
                    TraderMessage.ServerLog("[TRADER] TRADER MARKER Building Trader " + buildingItem);
                }
                PlayerBase playerTrader = PlayerBase.Cast(traderAtPos);
                if(playerTrader)
                {
                    playerTrader.m_Trader_IsTrader = true;
                    playerTrader.m_Trader_TraderIndex = m_Trader_TraderIDs.Count() - 1;
                    playerTrader.SetSynchDirty();
                    TraderMessage.ServerLog("[TRADER] TRADER MARKER Human Trader " + playerTrader);
                }            
                TraderMessage.ServerLog("[TRADER] TRADER MARKER POSITION ENTRY " + markerPosition);
            }
            else
            {
                TraderMessage.ServerLog("[TRADER][ERROR] Marker couldn't find an object at position " + markerPosition);
            }
            
            line_content = TraderNpcText.NextTerm(file_index, "<TraderMarkerSafezone>", "<FileEnd>");
            
            line_content.Replace("<TraderMarkerSafezone>", "");
            line_content = TraderNpcText.Clean(line_content);
            line_content = TraderNpcText.Tidy(line_content);    
            
            m_Trader_TraderSafezones.Insert(line_content.ToInt());

            int triggerRadius = line_content.ToInt();
            if(triggerRadius > 0)
            {
                SafeZoneTrigger newTrigger;
                if (Class.CastTo(newTrigger, GetGame().CreateObjectEx("SafeZoneTrigger", markerPosition, ECE_NONE)))
                {
                    vector triggerPosition = markerPosition;
                    int halfheight = 100;
                    triggerPosition[1] = triggerPosition[1] - halfheight;
                    newTrigger.SetPosition(triggerPosition);
                    newTrigger.SetCollisionCylinder( triggerRadius, halfheight * 2 );
                    newTrigger.InitSafeZone(m_Trader_SafezoneTimeout, m_Trader_SafezoneRemoveAnimals, m_Trader_SafezoneRemoveInfected , m_Trader_SafezoneRemoveEAI);
                    
                    TraderMessage.ServerLog("[TRADER] SPAWNED SAFEZONE AT " + triggerPosition + " with radius " + triggerRadius);
                }
            }
            
            line_content = TraderNpcText.NextTerm(file_index, "<VehicleSpawn>", "<TraderMarker>");

            if(line_content == string.Empty)
                break;

            if (line_content.Contains("<TraderMarker>"))
            {
                skipLine = true;
                m_Trader_TraderVehicleSpawns.Insert("0 0 0");
                m_Trader_TraderVehicleSpawnsOrientation.Insert("0 0 0");
                continue;
            }

            line_content.Replace("<VehicleSpawn>", "");
            line_content = TraderNpcText.Clean(line_content);

            TStringArray strtmv = new TStringArray;
            line_content.Split( ",", strtmv );
            
            string traderMarkerVehiclePosX = strtmv.Get(0);
            traderMarkerVehiclePosX = TraderNpcText.Tidy(traderMarkerVehiclePosX);
            
            string traderMarkerVehiclePosY = strtmv.Get(1);
            traderMarkerVehiclePosY = TraderNpcText.Tidy(traderMarkerVehiclePosY);
            
            string traderMarkerVehiclePosZ = strtmv.Get(2);
            traderMarkerVehiclePosZ = TraderNpcText.Tidy(traderMarkerVehiclePosZ);
            
            vector markerVehiclePosition = "0 0 0";
            markerVehiclePosition[0] = traderMarkerVehiclePosX.ToFloat();
            markerVehiclePosition[1] = traderMarkerVehiclePosY.ToFloat();
            markerVehiclePosition[2] = traderMarkerVehiclePosZ.ToFloat();

            TraderMessage.ServerLog("[TRADER] TRADER MARKER VEHICLE ENTRY " + markerVehiclePosition);

            m_Trader_TraderVehicleSpawns.Insert(markerVehiclePosition);

            line_content = TraderNpcText.NextTerm(file_index, "<VehicleSpawnOri>", "<TraderMarker>");

            if(line_content == string.Empty)
                break;

            if (line_content.Contains("<TraderMarker>"))
            {
                skipLine = true;
                m_Trader_TraderVehicleSpawnsOrientation.Insert("0 0 0");
                continue;
            }

            line_content.Replace("<VehicleSpawnOri>", "");
            line_content = TraderNpcText.Clean(line_content);

            TStringArray strtmvd = new TStringArray;
            line_content.Split( ",", strtmvd );
            
            string traderMarkerVehicleOriX = strtmvd.Get(0);
            traderMarkerVehicleOriX = TraderNpcText.Tidy(traderMarkerVehicleOriX);
            
            string traderMarkerVehicleOriY = strtmvd.Get(1);
            traderMarkerVehicleOriY = TraderNpcText.Tidy(traderMarkerVehicleOriY);
            
            string traderMarkerVehicleOriZ = strtmvd.Get(2);
            traderMarkerVehicleOriZ = TraderNpcText.Tidy(traderMarkerVehicleOriZ);
            
            vector markerVehicleOrientation = "0 0 0";
            markerVehicleOrientation[0] = traderMarkerVehicleOriX.ToFloat();
            markerVehicleOrientation[1] = traderMarkerVehicleOriY.ToFloat();
            markerVehicleOrientation[2] = traderMarkerVehicleOriZ.ToFloat();

            m_Trader_TraderVehicleSpawnsOrientation.Insert(markerVehicleOrientation);

            markerCounter++;
        }
        
        CloseFile(file_index);
        
        //------------------------------------------------------------------------------------
        
        m_Trader_ObjectsList.Clear();
        m_Trader_ReadAllTraderData = true;
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
                return g_Game.ConfigGetInt( path );
            }
        }

        return 0;
    }

    void SetPlayerVehicleIsInSafezone( PlayerBase player, bool isInSafezone )
    {
        Print("A mod is using SetPlayerVehicleIsInSafezone from Trader mod. Function has been deprecated.");
    }
}