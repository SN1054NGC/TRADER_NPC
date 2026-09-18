// ============================================================
// ФАЙЛ: TraderMessage.c (ИСПРАВЛЕННЫЙ)
// ============================================================

class TraderMessage
{
	static void ServerLog(string str)
	{
#ifdef SERVER
#ifndef TRADER_HIDE_SERVER_LOGS        
        PluginTraderServerLog m_Logger = PluginTraderServerLog.Cast(GetPlugin(PluginTraderServerLog));
        if(m_Logger)
        {
            m_Logger.Log(str);
        }
        else
        {
            Print(str);
        }
#endif
#endif
    }

	static void TradesLog(string str)
	{
#ifdef SERVER
#ifndef TRADER_HIDE_SERVER_LOGS        
        PluginTraderTradesLog m_Logger = PluginTraderTradesLog.Cast(GetPlugin(PluginTraderTradesLog));
        if(m_Logger)
        {
            m_Logger.Log(str);
        }
        else
        {
            Print(str);
        }
#endif
#endif
    }

    static void PlayerWhite(string message, PlayerBase player, float time = 10)
    {
        if (!player)
            return;
        SendNotification(message, player, time, 0);
    }

    static void PlayerRed(string message, PlayerBase player, float time = 10)
    {
        if (!player)
            return;
        SendNotification(message, player, time, COLOR_RED);
    }

    static void PlayerGreen(string message, PlayerBase player, float time = 10)
    {
        if (!player)
            return;
        SendNotification(message, player, time, COLOR_GREEN);
    }

    static void SafezoneExit(PlayerBase player, float time)
    {
        if (!player)
            return;

        SendNotification("", player, time, COLOR_RED, true);
    }

    static void DeleteSafezoneMessages(PlayerBase player)
    {
        if (!player)
            return;

        if (GetGame().IsServer())
        {
            GetGame().RPCSingleParam(player, TRPCs.RPC_DELETE_SAFEZONE_MESSAGES, new Param1<bool>( true ), false, player.GetIdentity());
        }
        else
        {
            // ============================================================
            // ИСПРАВЛЕНИЕ: НА КЛИЕНТЕ ТОЖЕ УДАЛЯЕМ СООБЩЕНИЯ
            // ============================================================
            player.GetTraderNotifications().DeleteAllMessages();
        }
    }

        static void SendNotification(string message, PlayerBase player, float time, int color, bool isExitSafezoneMsg = false)
    {        
        Print("[Ntf:D] SendNotification isServer=" + (GetGame().IsServer()) + " exit=" + isExitSafezoneMsg + " msg=[" + message + "] color=" + color + " time=" + time);
        if (GetGame().IsServer())
        {
            if (!player || !player.GetIdentity())
            {
                Print("[Ntf:D] SendNotification SERVER bail no identity");
                return;
            }
            GetGame().RPCSingleParam(player, TRPCs.RPC_SEND_NOTIFICATION, new Param4<string, float, int, bool>( message, time, color, isExitSafezoneMsg), false, player.GetIdentity());
            Print("[Ntf:D] SendNotification SERVER RPC sent");
        }
        else
        {
            // ============================================================
            // ИСПРАВЛЕНИЕ: НА КЛИЕНТЕ ПОКАЗЫВАЕМ СООБЩЕНИЕ НЕПОСРЕДСТВЕННО
            // ============================================================
            if(isExitSafezoneMsg)
            {
                Print("[Ntf:D] SendNotification CLIENT show exit-safezone");
                player.GetTraderNotifications().ShowExitSafezoneMessage(time);
            }
            else
            {
                Print("[Ntf:D] SendNotification CLIENT show message");
                player.GetTraderNotifications().ShowMessage(message, time, color);
            }
        }
    }
}