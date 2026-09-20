// ============================================================
// ФАЙЛ: TraderNotifications.c (ИСПРАВЛЕННЫЙ)
// ============================================================

class TraderNotification
{
	private Widget m_Parent;
	private Widget layoutRoot;
    RichTextWidget m_Message;
    float m_Timer = 0;
    bool m_isActive = true;
    bool m_IsExitSafezoneMsg = false;

        Widget Init(Widget root_widget, string message, float time, bool isExitSafezoneMsg = false)
    {
        m_Parent = root_widget;
        // ============================================================
        // ИСПРАВЛЕНИЕ: ПРАВИЛЬНЫЙ ПУТЬ К LAYOUT
        // ============================================================
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("TRADER_NPC/scripts/layouts/TraderNotification.layout", m_Parent);
        m_Message = RichTextWidget.Cast(layoutRoot.FindAnyWidget("text_message") );
        m_Timer = time;
        //Print("[Ntf:D] TraderNotification.Init layoutRoot=" + layoutRoot + " msgWidget=" + m_Message);
        if (isExitSafezoneMsg)
        {
            message = GetExitSafeZoneMsg();
            m_IsExitSafezoneMsg = true;
        }
        m_Message.SetText(message);
        layoutRoot.Show(true);
        return layoutRoot;
    }

    void Update()
	{
        PlayerBase currentPlayer = PlayerBase.Cast(GetGame().GetPlayer());
        if(currentPlayer && !currentPlayer.IsAlive())
        {
            RemoveNotification();
            return;
        }
        
        m_Timer -= 1;

        if (m_IsExitSafezoneMsg)
            m_Message.SetText(GetExitSafeZoneMsg());

        if (m_Timer <= 0)
        {
            RemoveNotification();
        }
    }

    void RemoveNotification()
    {        
        m_isActive = false;
        layoutRoot.Show(false);
    }

    void SetTextColor(int color)
    {
        m_Message.SetColor(color);
    }

    string GetExitSafeZoneMsg()
    {
        return "#tm_leaving_the_safezone_in" + " " + (Math.Floor(m_Timer) + 1) + " " + "#tm_seconds" + "!";
    }
}

class TraderNotifications : Managed
{
    ref array<ref TraderNotification> m_Messages;
    WrapSpacerWidget m_Container;            // окно операций (справа)
    WrapSpacerWidget m_SafezoneContainer;    // индикатор сейф-зоны (центр сверху)
	private ref Timer m_TimeoutTimer;

    void Init()
    {
        // ============================================================
        // ИСПРАВЛЕНИЕ: ПРАВИЛЬНЫЙ ПУТЬ К LAYOUT
        // ============================================================
                Widget layoutRoot = GetGame().GetWorkspace().CreateWidgets("TRADER_NPC/scripts/layouts/TraderNotificationsContainer.layout");
        m_Container = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("Wrapper"));

        // Индикатор сейф-зоны — отдельный контейнер по центру сверху, чтобы
        // не смешивался с сообщениями об операциях справа.
        Widget safezoneLayout = GetGame().GetWorkspace().CreateWidgets("TRADER_NPC/scripts/layouts/TraderSafezoneNotice.layout");
        if (safezoneLayout)
            m_SafezoneContainer = WrapSpacerWidget.Cast(safezoneLayout.FindAnyWidget("Wrapper"));
        m_Messages = new array<ref TraderNotification>;
		m_TimeoutTimer = new Timer(CALL_CATEGORY_GUI);
    }

    void Update()
	{
		for (int i = 0; i < m_Messages.Count(); i++)
        {
            TraderNotification notification = m_Messages.Get(i);
            notification.Update();

            if (!notification.m_isActive)
            {
                delete notification;
                m_Messages.RemoveOrdered(i);
                i--;
            }
        }
        if(m_Messages.Count() == 0 && m_TimeoutTimer.IsRunning())
        {
           m_TimeoutTimer.Stop();
        }
    }

        void ShowMessage(string message, float time, int color)
    {
        //Print("[Ntf:D] ShowMessage('" + message + "', time=" + time + ", color=" + color + ") container=" + m_Container);
        TraderNotification notification = new TraderNotification();
        notification.Init(m_Container, message, time);

        if (color != 0)
            notification.SetTextColor(color);
        InsertMessage(notification);
    }

    void ShowExitSafezoneMessage(float time)
    {
        DeleteAllMessages();

        WrapSpacerWidget target = m_SafezoneContainer;
        if (!target)
            target = m_Container;

        TraderNotification notification = new TraderNotification();
        notification.Init(target, "", time, true);

        notification.SetTextColor(COLOR_RED);
        InsertMessage(notification);
    }

    void InsertMessage(TraderNotification notification)
    {        
        if(m_Messages.Count() >= 7)
        {
            TraderNotification remnotification = m_Messages.Get(0);
            remnotification.RemoveNotification();
            delete remnotification;
            m_Messages.RemoveOrdered(0);
        }
        m_Messages.Insert(notification);
        if(m_Messages.Count() > 0 && !m_TimeoutTimer.IsRunning())
        {
            m_TimeoutTimer.Run(1, this, "Update", null, true);
        }
    }

    void DeleteAllMessages()
    {
        for (int i = 0; i < m_Messages.Count(); i++)
        {
            TraderNotification notification = m_Messages.Get(i);
            notification.RemoveNotification();
            delete notification;
            m_Messages.RemoveOrdered(i);
            i--;
        }
    }
}