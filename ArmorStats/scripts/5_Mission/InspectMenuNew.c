modded class InspectMenuNew
{
	protected ref TStringArray m_widgets = {"bullet_bar", "shock_bar", "bullet_bar_percent", "shock_bar_percent"};

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("ArmorStats/gui/layouts/day_z_inventory_new_inspect.layout");
		
		return layoutRoot;
	}
	
	override void SetItem(EntityAI item)
	{
		super.SetItem(item);
		
		Widget armor_wrapper = layoutRoot.FindAnyWidget("armor_wrapper");

		Clothing clothing;
		if (Class.CastTo(clothing, item) && clothing.DisplayStatGUIOnThis())
		{
			ArmorGUI.ShutUpAndColor(armor_wrapper, m_widgets, item.GetType());
			armor_wrapper.Show(true);	
		}
		else
		{
			armor_wrapper.Show(false);
		}
	}
}