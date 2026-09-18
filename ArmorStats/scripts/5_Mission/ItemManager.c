modded class ItemManager
{
    protected ref TStringArray m_widgets = {"bullet_bar", "shock_bar", "bullet_bar_percent", "shock_bar_percent"};

    void ItemManager(Widget root)
    {
        m_TooltipWidget = GetGame().GetWorkspace().CreateWidgets("ArmorStats/gui/layouts/day_z_inventory_new_tooltip.layout", root);
        m_TooltipSlotWidget	= GetGame().GetWorkspace().CreateWidgets("ArmorStats/gui/layouts/day_z_inventory_new_tooltip_slot.layout", root);

        m_TooltipWidget.Show(false);
		m_TooltipSlotWidget.Show(false);
    }

    override void PrepareTooltip(EntityAI item, int x = 0, int y = 0)
    {
        super.PrepareTooltip(item, x, y);

        Widget rootWidget = m_TooltipWidget.FindAnyWidget("Stats_FW");

        Clothing clothing;
        if (Class.CastTo(clothing, item) && clothing.DisplayStatGUIOnThis())
        {
            ArmorGUI.ShutUpAndColor(rootWidget, m_widgets, item.GetType());
            rootWidget.Show(true);
        }
        else
        {
            rootWidget.Show(false);
        }
    }
}