// ============================================================
// FILE: TraderSellRMB.c  (module 5_Mission -> client UI)
//
// RIGHT mouse button now opens the compact sell window instead of the MIDDLE
// button. Only the item icon handler is patched: every item in the inventory,
// in containers and in the vicinity list is rendered by Icon, and Icon keeps
// its item in the member m_Item, so no vanilla logic has to be re-implemented.
//
// When the item is not bought by a trader in range (or the inventory menu is
// not open) the call falls through to the untouched vanilla handler, so the
// normal right-click context menu / split behaviour still works.
// ============================================================
modded class Icon
{
	override void MouseClick( Widget w, int x, int y, int button )
	{
		if ( button == MouseState.RIGHT && TraderSellInput.Open( m_Item ) )
			return;

		super.MouseClick( w, x, y, button );
	}
};
