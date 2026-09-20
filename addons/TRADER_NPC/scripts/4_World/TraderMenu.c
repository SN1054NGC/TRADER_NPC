// ============================================================
// ФАЙЛ: TraderMenu.c (ИСПРАВЛЕННЫЙ)
// ============================================================

class TraderItem
{
    string ClassName;
	int BuyValue;
	int SellValue;
	int Quantity;
	int IndexId;
};


class TraderMenu extends UIScriptedMenu
{
    MultilineTextWidget m_InfoBox;
    ButtonWidget m_BtnBuy;
	ButtonWidget m_BtnSell;
	ButtonWidget m_BtnCancel;
	TextListboxWidget m_ListboxItems;
	TextWidget m_Saldo;
	TextWidget m_SaldoValue;
	TextWidget m_TraderName;
	XComboBoxWidget m_XComboboxCategorys;
	ItemPreviewWidget m_ItemPreviewWidget;
	protected EntityAI previewItem;
	MultilineTextWidget m_ItemDescription;
	TextWidget m_ItemWeight;
	float m_UiUpdateTimer = 0;
	float m_UiSellTimer = 0;
	float m_UiBuyTimer = 0;
	float m_buySellTime = 0.3;

	bool m_active = false;
	
	int m_TraderID = -1;
	int m_traderIndex = -1;
	vector m_TraderVehicleSpawn = "0 0 0";
	vector m_TraderVehicleSpawnOrientation = "0 0 0";
	
	int m_Player_CurrencyAmount;
	int m_ColorBuyable;
	int m_ColorTooExpensive;
	int m_CategorysCurrentIndex;

	int m_LastRowIndex = -1;
	int m_LastCategoryCurrentIndex = -1;
	
	bool updateListbox = false;
	
	ref array<string> m_Categorys;
	ref array<int> m_CategorysTraderKey;
	ref array<int> m_CategorysKey;
	
	ref array<string> m_ListboxItemsClassnames;
	ref array<int> m_ListboxItemsQuantity;
	ref array<int> m_ListboxItemsBuyValue;
	ref array<int> m_ListboxItemsSellValue;

	ref array<int> m_ItemIDs;
	
	ref TStringArray m_FileContent;
	private bool                m_SellablesOnly = false;
	private int                 m_PreviewWidgetRotationX;
	private int                 m_PreviewWidgetRotationY;
	private vector              m_PreviewWidgetOrientation;	
	private int 				m_characterScaleDelta;
	TextWidget 					m_ItemQuantity;
    private string              m_SearchFilter = "";
    private string              m_OldSearchFilter = "";
    private EditBoxWidget		m_SearchBox; 
    private CheckBoxWidget		m_SellablesCheckbox; 
	ref array<ref TraderItem> m_FilteredListOfTraderItems;
	ref array<ref TraderItem> m_ListOfCategoryTraderItems;
	ref array<ref TraderItem> m_ListOfTraderItems;
	// '#' keys are resolved by the widget text translator, so they can be
	// concatenated with a value exactly like vanilla does it
	// (see scripts/5_mission/gui/ingamemenu.c: SetText("#key" + " " + value)).
	const string m_QuantString = "#tm_quantity";
	const string m_SizeString = "#tm_cargo_size";

	private PlayerBase m_Player;
    EntityAI previewItemKit;

	// ---- survival rating: discounted prices ----
	// The rating badge widget itself lives in the inventory (TraderRatingUI.c);
	// inside this window only the compact discount text is used.
	float      m_TraderRatingTimer = 0;

	// После каждой торговой операции сервер присылает сообщение: помечаем окно
	// устаревшим и в ближайшем тике пересобираем «в наличии» и наполненность.
	static bool s_NeedsRefresh = false;

	// ---- v2 buy window: discount / condition / sell bonus / amount slider ----
	TextWidget   m_DiscountText;
	TextWidget   m_ItemQuality;
	TextWidget   m_SellBonusText;
	TextWidget   m_SellAmountText;
	TextWidget   m_BuyAmountText;
	SliderWidget m_SellAmountSlider;
	ref map<string, int> m_OwnedCounts;
	int          m_SellAmount = 1;
	int          m_SellAmountMax = 1;
	int          m_SellAmountRow = -1;
	bool         m_SellAmountCustom = false;
	// last appraisal requested for the sell preview (id of the trader row + amount)
	int          m_SellAppraiseRow = -1;
	int          m_SellAppraiseAmount = -1;
	int          m_BuyUnitPrice = -1;      // цена за одну единицу (со скидкой рейтинга)
	string       m_SellUnitClass = "";      // класс выбранной строки (для имени единицы)

	void TraderMenu()
	{		
		m_Player_CurrencyAmount = 0;
		m_ColorBuyable = 0;
		m_ColorTooExpensive = 1;
	}	
	
	void ~TraderMenu()
	{
		if ( previewItem ) 
		{
			GetGame().ObjectDelete( previewItem );
		}
	}

    override Widget Init()
    {
		m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "TRADER_NPC/scripts/layouts/TraderMenu.layout" );

        m_BtnBuy = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_buy" ) );
		m_BtnSell = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_sell" ) );
		m_BtnCancel = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_cancel" ) );
		m_ListboxItems = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("txtlist_items") );
		m_Saldo = TextWidget.Cast(layoutRoot.FindAnyWidget("text_saldo") );
		m_SaldoValue = TextWidget.Cast(layoutRoot.FindAnyWidget("text_saldoValue") );
		m_TraderName = TextWidget.Cast(layoutRoot.FindAnyWidget("title_text") );
		m_XComboboxCategorys = XComboBoxWidget.Cast( layoutRoot.FindAnyWidget( "xcombobox_categorys" ) );
		m_ItemDescription = MultilineTextWidget.Cast( layoutRoot.FindAnyWidget( "ItemDescWidget" ) );
		m_ItemWeight = TextWidget.Cast(layoutRoot.FindAnyWidget("ItemWeight") );
		m_ItemQuantity = TextWidget.Cast(layoutRoot.FindAnyWidget("ItemQuantity"));
		m_SearchBox    = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "SearchBox" ) );
		m_SellablesCheckbox = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget( "SellablesCheckBox" ) );
		m_SellablesCheckbox.SetChecked(false);

		// v2 buy window widgets (see TraderMenu.layout)
		m_DiscountText     = TextWidget.Cast( layoutRoot.FindAnyWidget( "text_discount" ) );
		m_ItemQuality      = TextWidget.Cast( layoutRoot.FindAnyWidget( "ItemQuality" ) );
		m_SellBonusText    = TextWidget.Cast( layoutRoot.FindAnyWidget( "SellBonusWidget" ) );
		m_SellAmountText   = TextWidget.Cast( layoutRoot.FindAnyWidget( "SellAmountText" ) );
		m_BuyAmountText    = TextWidget.Cast( layoutRoot.FindAnyWidget( "BuyAmountText" ) );
		m_SellAmountSlider = SliderWidget.Cast( layoutRoot.FindAnyWidget( "SellQuantitySlider" ) );
		if ( m_SellAmountSlider )
			m_SellAmountSlider.SetStep( 1.0 );
		TraderMenu_ResetSellAmount();

        return layoutRoot;
    }

	void InitTraderValues()
	{
		if (!m_Player)
		{
			Close();
			return;
		}

		if (!m_Player.HasReceivedAllTraderData())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.RetryInitTraderValues, 500, false);
			return;
		}

		if (m_Player.m_Trader_TraderNames.Count() == 0 || m_Player.m_Trader_Categorys.Count() == 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.RetryInitTraderValues, 500, false);
			return;
		}

		m_Categorys = new array<string>;
		m_CategorysTraderKey = new array<int>;
		m_CategorysKey = new array<int>;
        m_ListboxItemsClassnames = new array<string>;		
        m_FilteredListOfTraderItems = new array<ref TraderItem>;
        m_ListOfCategoryTraderItems = new array<ref TraderItem>;
        m_ListOfTraderItems = new array<ref TraderItem>;
		m_ListboxItemsQuantity = new array<int>;
		m_ListboxItemsBuyValue = new array<int>;
		m_ListboxItemsSellValue = new array<int>;
		m_ItemIDs = new array<int>;
		
		LoadCategories();
		m_CategorysCurrentIndex = 0;
		
		updateItemListboxContent();		
		m_ListboxItems.SelectRow(0);

		updatePlayerCurrencyAmount();
		updateItemListboxColors();
	}

	void RetryInitTraderValues()
	{
		InitTraderValues();
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_UiSellTimer > 0)
			m_UiSellTimer -= timeslice;

		if (m_UiBuyTimer > 0)
			m_UiBuyTimer -= timeslice;

		m_TraderRatingTimer = m_TraderRatingTimer + timeslice;
		if ( m_TraderRatingTimer >= 1.0 )
		{
			m_TraderRatingTimer = 0;
			TraderMenu_RatingRefresh();
		}

		if (m_UiUpdateTimer >= 0.05)
		{			           
			m_SearchFilter = m_SearchBox.GetText();
			if(m_SearchFilter != m_OldSearchFilter)
				SearchForItems();

			updatePlayerCurrencyAmount();				
			updateItemListboxColors();
			TraderMenu_ApplyAppraisedPrice();

			if ( TraderMenu.s_NeedsRefresh )
			{
				TraderMenu.s_NeedsRefresh = false;
				TraderMenu_RefreshOwnedColumn();
				updateItemListboxColors();
				TraderMenu_RefreshSelectedCard();
			}

			local int row_index = m_ListboxItems.GetSelectedRow();
			if ((m_LastRowIndex != row_index) || (m_LastCategoryCurrentIndex != m_CategorysCurrentIndex))
			{
				m_LastRowIndex = row_index;
				m_LastCategoryCurrentIndex = m_CategorysCurrentIndex;
				ResetMenu();
				if ( row_index < 0 || row_index >= m_FilteredListOfTraderItems.Count() )
				{
					m_UiUpdateTimer = 0;
					return;
				}
				if(!m_FilteredListOfTraderItems.Get(row_index))
				{
					m_UiUpdateTimer = 0;
					return;
				}

				string itemType = m_FilteredListOfTraderItems.Get(row_index).ClassName;
				updateItemPreview(itemType);
			}

			m_UiUpdateTimer = 0;
		}
		else
		{
			m_UiUpdateTimer = m_UiUpdateTimer + timeslice;
		}
	}

	override void OnShow()
	{		
		super.OnShow();
		LockControls();
		PPEffects.SetBlurMenu(0.5);
	}
	
	override void OnHide()
	{
		super.OnHide();
		UnlockControls();
		PPEffects.SetBlurMenu(0);
		if ( previewItem ) 
		{
			GetGame().ObjectDelete( previewItem );
		}
	}
	
    override void LockControls()
    {
        // Миссия/HUD могут быть уже уничтожены (выход из игры) - проверяем указатели
        Mission mission = GetGame().GetMission();
        if ( mission )
        {
            mission.PlayerControlDisable(INPUT_EXCLUDE_ALL);
            Hud hud = mission.GetHud();
            if ( hud )
                hud.Show( false );
        }

        UIManager uiMgr = GetGame().GetUIManager();
        if ( uiMgr )
            uiMgr.ShowUICursor( true );
    }

    override void UnlockControls()
    {
        Mission mission = GetGame().GetMission();
        if ( mission )
        {
            mission.PlayerControlEnable(false);
            Hud hud = mission.GetHud();
            if ( hud )
                hud.Show( true );
        }

        Input input = GetGame().GetInput();
        if ( input )
            input.ResetGameFocus();

        UIManager uiMgr = GetGame().GetUIManager();
        if ( uiMgr )
            uiMgr.ShowUICursor( false );
    }

	override bool OnClick( Widget w, int x, int y, int button )
	{			
		if ( w == m_SellablesCheckbox )
		{
			m_SellablesCheckbox.SetChecked( !m_SellablesOnly );
			m_SellablesOnly = !m_SellablesOnly;
			SearchForItems();
		}	

		if ( w == m_BtnCancel )
		{
			Close();
			return true;
		}
		
		if (w == m_XComboboxCategorys)
		{
			if (updateListbox)
			{
				updateListbox = false;
				
				updateItemListboxContent();
				m_ListboxItems.SelectRow(0);

				updatePlayerCurrencyAmount();
				updateItemListboxColors();
			}
		}

		local int row_index = m_ListboxItems.GetSelectedRow();
		if ( row_index < 0 || row_index >= m_FilteredListOfTraderItems.Count() )
			return false;
		if ( !m_FilteredListOfTraderItems.Get( row_index ) )
			return false;

		string itemType = m_FilteredListOfTraderItems.Get(row_index).ClassName;
		int itemQuantity = m_FilteredListOfTraderItems.Get(row_index).Quantity;
		
		if ( w == m_BtnBuy )
		{
            if(!previewItem)                
			{
				TraderMessage.PlayerWhite("You cannot buy this item. Item doesn't exist.", m_Player);
				return true;
			}
			if (m_UiBuyTimer > 0)
			{
				TraderMessage.PlayerWhite("#tm_not_that_fast", m_Player);
				return true;
			}
			m_UiBuyTimer = m_buySellTime;

			// Количество берём с общего ползунка: 0 = как раньше (количество строки)
			int buyAmountParam = 0;
			if ( m_SellAmount > 1 && m_BuyUnitPrice > 0 )
				buyAmountParam = m_SellAmount;

			GetGame().RPCSingleParam(m_Player, TRPCs.RPC_BUY, new Param4<int, int, string, int>( m_traderIndex, m_FilteredListOfTraderItems.Get(row_index).IndexId, "", buyAmountParam), true);
			
			return true;
		}
		
		if ( w == m_BtnSell )
		{
			if (m_UiSellTimer > 0)
			{
				TraderMessage.PlayerWhite("#tm_not_that_fast", m_Player);
				return true;
			}
			m_UiSellTimer = m_buySellTime;

			// Stack / ammo pile: send the amount chosen with the slider.
			// (0 keeps the legacy whole-item sale, which also pays the
			// mounted / loaded extras computed on the server.)
			int sellAmountParam = 0;
			if ( m_SellAmountCustom && m_SellAmountRow == row_index && m_SellAmount > 1 )
				sellAmountParam = m_SellAmount;

			GetGame().RPCSingleParam(m_Player, TRPCs.RPC_SELL, new Param4<int, int, string, int>( m_traderIndex, m_FilteredListOfTraderItems.Get(row_index).IndexId, "", sellAmountParam), true);
			
			return true;
		}

		return false;
	}
	
	override bool OnChange( Widget w, int x, int y, bool finished )
	{
		// Amount slider: handled before the (finished) gate so the price
		// preview follows the slider while it is still being dragged.
		if ( w == m_SellAmountSlider )
		{
			int sliderAmount = Math.Round( m_SellAmountSlider.GetCurrent() );
			if ( sliderAmount != m_SellAmount )
			{
				TraderMenu_SetSellAmount( sliderAmount );
				TraderMenu_UpdateSellPreview();
			}
			return false;
		}

		super.OnChange(w, x, y, finished);
		if (!finished) return false;
		
		m_CategorysCurrentIndex = m_XComboboxCategorys.GetCurrentItem();
		updateListbox = true;
		
		return false;
	}
	
	void ResetMenu()
	{
		if(previewItemKit)
			previewItemKit.Delete();
		if(previewItem)
			previewItem.Delete();
		m_ItemWeight.SetText("");
		m_ItemQuantity.SetText("");
		m_ItemDescription.SetText("");
		if ( m_ItemQuality )
			m_ItemQuality.SetText("");
		TraderMenu_ResetSellAmount();
	}

	// ============================================================
	// v2 buy window helpers:
	//   owned-count column, condition in words, sell price breakdown
	//   and the amount slider used to sell part of a stack / ammo pile.
	// The inventory is enumerated exactly ONCE per list refresh and the
	// per-class sums are cached in m_OwnedCounts - the list can hold
	// several hundred rows and one inventory walk per row would be slow.
	// ============================================================
	void TraderMenu_BuildOwnedCounts()
	{
		m_OwnedCounts = new map<string, int>;
		if ( !m_Player )
			return;

		array<EntityAI> ownedItems = new array<EntityAI>;
		m_Player.GetInventory().EnumerateInventory( InventoryTraversalType.PREORDER, ownedItems );

		// the item in the hands is not part of the inventory traversal
		EntityAI handsItem = m_Player.GetHumanInventory().GetEntityInHands();
		if ( handsItem )
			ownedItems.Insert( handsItem );

		for ( int i = 0; i < ownedItems.Count(); i++ )
		{
			ItemBase ownedItem = ItemBase.Cast( ownedItems.Get( i ) );
			if ( !ownedItem )
				continue;
			if ( ownedItem.IsRuined() )
				continue;

			string ownedClass = ownedItem.GetType();
			ownedClass.ToLower();

			int ownedAmount = TraderSmartSellEngine.GetSellableAmount( ownedItem );
			if ( m_OwnedCounts.Contains( ownedClass ) )
				m_OwnedCounts.Set( ownedClass, m_OwnedCounts.Get( ownedClass ) + ownedAmount );
			else
				m_OwnedCounts.Set( ownedClass, ownedAmount );
		}
	}

	// How many units of this class the player owns (stack size / rounds).
	int TraderMenu_CountOwned( string itemClassname )
	{
		if ( !m_OwnedCounts || itemClassname == "" )
			return 0;

		string lower = itemClassname;
		lower.ToLower();

		if ( !m_OwnedCounts.Contains( lower ) )
			return 0;

		return m_OwnedCounts.Get( lower );
	}

	// A real, sellable instance of the class (attached / ruined excluded by
	// isInPlayerInventory), used to price the sell preview honestly.
	ItemBase TraderMenu_FindOwned( string itemClassname )
	{
		ItemBase ownedItem;
		if ( !m_Player || itemClassname == "" )
			return null;

		// isInPlayerInventory lower-cases its argument in place, so hand it a
		// copy - the caller's string (a trader row class name) must not change.
		string lower = itemClassname;
		lower.ToLower();

		if ( !m_Player.isInPlayerInventory( lower, 1, ownedItem ) )
			return null;

		return ownedItem;
	}

	// Приблизительный износ в процентах по дискретному состоянию (точный
	// процент здоровья клиент прочитать не может - GetHealth01 только на сервере).
	// Точную ЦЕНУ показывает ответ сервера (аппразер), она считается по health.
	string TraderMenu_GetWearPercent( ItemBase wearItem )
	{
		if ( !wearItem )
			return "";
		int level = wearItem.GetHealthLevel();
		if ( level == GameConstants.STATE_PRISTINE )
			return "(100%)";
		if ( level == GameConstants.STATE_WORN )
			return "(~75%)";
		if ( level == GameConstants.STATE_DAMAGED )
			return "(~50%)";
		if ( level == GameConstants.STATE_BADLY_DAMAGED )
			return "(~25%)";
		return "(0%)";
	}

	string TraderMenu_GetConditionText( ItemBase conditionItem )
	{
		if ( !conditionItem )
			return "";

		int healthLevel = conditionItem.GetHealthLevel();
		if ( healthLevel == GameConstants.STATE_PRISTINE )
			return Widget.TranslateString( "#tm_cond_pristine" );
		if ( healthLevel == GameConstants.STATE_WORN )
			return Widget.TranslateString( "#tm_cond_worn" );
		if ( healthLevel == GameConstants.STATE_DAMAGED )
			return Widget.TranslateString( "#tm_cond_damaged" );
		if ( healthLevel == GameConstants.STATE_BADLY_DAMAGED )
			return Widget.TranslateString( "#tm_cond_badly" );
		if ( healthLevel == GameConstants.STATE_RUINED )
			return Widget.TranslateString( "#tm_cond_ruined" );

		return "";
	}

	// Mirrors DayZPlayerImplement.handleSellRPC: a partial stack is priced
	// from the trader table (exact row for that amount, otherwise the
	// per-one-unit row times the amount) and conditioned by item health;
	// a whole item additionally pays the mounted / loaded extra bonus.
	int TraderMenu_EstimateSellTotal( TraderItem sellTraderItem, ItemBase sellItem, out int basePrice, out int bonusPrice )
	{
		basePrice = 0;
		bonusPrice = 0;
		if ( !sellTraderItem )
			return 0;
		if ( sellTraderItem.SellValue < 0 )
			return sellTraderItem.SellValue;

		// PlayerBase extends DayZPlayerImplement (4_world/entities/manbase.c),
		// so this is an up-cast - no Cast() needed.
		DayZPlayerImplement impl = m_Player;
		if ( !impl || !sellItem )
			return sellTraderItem.SellValue;

		if ( m_SellAmountCustom && m_SellAmount > 1 )
		{
			int stackBase = TraderSmartSellEngine.ComputeStackBasePrice( impl, m_traderIndex, sellTraderItem.ClassName, m_SellAmount, sellTraderItem.SellValue );
			basePrice = TraderSmartSellEngine.Condition( sellItem, stackBase );
			return basePrice;
		}

		basePrice = TraderSmartSellEngine.Condition( sellItem, sellTraderItem.SellValue );
		bonusPrice = TraderSmartSellEngine.ComputeExtraBonus( impl, m_traderIndex, sellItem );
		return basePrice + bonusPrice;
	}

	void TraderMenu_ResetSellAmount()
	{
		m_SellAmount = 1;
		m_SellAmountMax = 1;
		m_SellAmountRow = -1;
		m_SellAmountCustom = false;

		if ( m_SellAmountSlider )
		{
			m_SellAmountSlider.SetMinMax( 1.0, 1.0 );
			m_SellAmountSlider.SetCurrent( 1.0 );
			m_SellAmountSlider.Show( false );
		}
		if ( m_SellAmountText )
			m_SellAmountText.SetText( "" );
	}

	void TraderMenu_SetSellAmount( int amount )
	{
		if ( amount < 1 )
			amount = 1;
		if ( amount > m_SellAmountMax )
			amount = m_SellAmountMax;

		m_SellAmount = amount;

		if ( m_SellAmountSlider )
			m_SellAmountSlider.SetCurrent( amount );

		if ( m_SellAmountText )
		{
			string amountLabel = Widget.TranslateString( "#tm_sell_amount" );
			m_SellAmountText.SetText( amountLabel + " " + amount + " / " + m_SellAmountMax );
		}

		// Подпись покупки: N x цена за единицу = итог (цена уже со скидкой рейтинга)
		if ( m_BuyAmountText && m_BuyUnitPrice > 0 )
		{
			string buyLabel = Widget.TranslateString( "#tm_buy" );
			int buyTotal = m_BuyUnitPrice * amount;
			string unit = TraderMenu_UnitName();
			m_BuyAmountText.SetText( buyLabel + " " + amount + " " + unit + " x " + m_BuyUnitPrice + " = " + buyTotal );
		}
	}

	void TraderMenu_UpdateSellPreview()
	{
		if ( !m_SellBonusText || !m_Player )
			return;

		if ( m_SellAmountRow < 0 || m_SellAmountRow >= m_FilteredListOfTraderItems.Count() )
		{
			m_SellBonusText.SetText( "" );
			return;
		}

		TraderItem sellRow = m_FilteredListOfTraderItems.Get( m_SellAmountRow );
		if ( !sellRow )
			return;

		if ( sellRow.SellValue < 0 )
		{
			m_SellBonusText.SetText( Widget.TranslateString( "#tm_ragged_not_bought" ) );
			return;
		}

		ItemBase sellItem = TraderMenu_FindOwned( sellRow.ClassName );
		if ( !sellItem )
		{
			m_SellBonusText.SetText( "" );
			return;
		}

		int basePrice;
		int bonusPrice;
		int totalPrice = TraderMenu_EstimateSellTotal( sellRow, sellItem, basePrice, bonusPrice );
		if ( totalPrice < 0 )
		{
			m_SellBonusText.SetText( "" );
			return;
		}

		// the localised label already ends with ':' - do not add another one
		string paysLabel = Widget.TranslateString( "#tm_trader_pays" );
		string priceText = paysLabel + " " + totalPrice + " " + m_Player.m_Trader_CurrencyName;
		if ( bonusPrice > 0 )
		{
			string bonusLabel = Widget.TranslateString( "#tm_bonus_breakdown" );
			priceText = priceText + "  (" + basePrice + " + " + bonusPrice + " " + bonusLabel + ")";
		}

		m_SellBonusText.SetText( priceText );

		// The client cannot read the exact item health (GetHealth01 is
		// server-only), so ask the server for the authoritative figure and
		// overwrite the estimate when the reply arrives. Debounced: only when
		// the selected trader row or the chosen amount actually changes.
		int appraiseAmount = 0;
		if ( m_SellAmountCustom && m_SellAmount > 1 )
			appraiseAmount = m_SellAmount;

		if ( m_SellAppraiseRow != sellRow.IndexId || m_SellAppraiseAmount != appraiseAmount )
		{
			m_SellAppraiseRow = sellRow.IndexId;
			m_SellAppraiseAmount = appraiseAmount;
			m_Player.RequestSellAppraise( m_traderIndex, sellRow.IndexId, appraiseAmount );
		}
	}

	// Overwrite the estimate with the server figure when it matches the row and
	// amount the request was made for (see RequestSellAppraise in
	// DayZPlayerImplement + HandleSellAppraiseReply).
	void TraderMenu_ApplyAppraisedPrice()
	{
		if ( !m_SellBonusText || !m_Player )
			return;
		if ( m_SellAmountRow < 0 || m_SellAmountRow >= m_FilteredListOfTraderItems.Count() )
			return;

		TraderItem sellRow = m_FilteredListOfTraderItems.Get( m_SellAmountRow );
		if ( !sellRow || sellRow.SellValue < 0 )
			return;

		int appraiseAmount = 0;
		if ( m_SellAmountCustom && m_SellAmount > 1 )
			appraiseAmount = m_SellAmount;

		if ( m_Player.m_Trader_ApprRow != sellRow.IndexId )
			return;
		if ( m_Player.m_Trader_ApprAmount != appraiseAmount )
			return;
		if ( m_Player.m_Trader_ApprTotal < 0 )
			return;

		string paysLabel = Widget.TranslateString( "#tm_trader_pays" );
		string priceText = paysLabel + " " + m_Player.m_Trader_ApprTotal + " " + m_Player.m_Trader_CurrencyName;
		if ( m_Player.m_Trader_ApprBonus > 0 )
		{
			string bonusLabel = Widget.TranslateString( "#tm_bonus_breakdown" );
			priceText = priceText + "  (" + m_Player.m_Trader_ApprBase + " + " + m_Player.m_Trader_ApprBonus + " " + bonusLabel + ")";
		}
		m_SellBonusText.SetText( priceText );
	}

	void TraderMenu_UpdateSellInfo( EntityAI previewEntity, int rowIndex )
	{
		TraderMenu_ResetSellAmount();
		if ( m_ItemQuality )
			m_ItemQuality.SetText( "" );
		if ( m_SellBonusText )
			m_SellBonusText.SetText( "" );

		if ( rowIndex < 0 || rowIndex >= m_FilteredListOfTraderItems.Count() )
			return;

		TraderItem sellRow = m_FilteredListOfTraderItems.Get( rowIndex );
		if ( !sellRow )
			return;

		m_SellAmountRow = rowIndex;
		m_SellUnitClass = sellRow.ClassName;

		// цена за одну единицу со скидкой рейтинга + сколько можно купить
		m_BuyUnitPrice = -1;
		int buyMax = 1;
		if ( sellRow.BuyValue >= 0 )
		{
			m_BuyUnitPrice = TraderMenu_BuyPrice( sellRow.BuyValue );
			if ( m_BuyUnitPrice > 0 && m_Player_CurrencyAmount > 0 )
				buyMax = m_Player_CurrencyAmount / m_BuyUnitPrice;
			if ( buyMax < 1 )
				buyMax = 1;
			if ( m_SellAmountMax < buyMax )
				m_SellAmountMax = buyMax;
		}

		ItemBase previewBase = ItemBase.Cast( previewEntity );
		if ( m_ItemQuality && previewBase )
		{
			string qualityText = TraderMenu_GetConditionText( previewBase ) + " " + TraderMenu_GetWearPercent( previewBase );
			if ( qualityText != "" )
			{
				string qualityLabel = Widget.TranslateString( "#tm_quality" );
				m_ItemQuality.SetText( qualityLabel + ": " + qualityText );
			}
		}

		if ( sellRow.SellValue >= 0 )
		{
			int ownedAmount = TraderMenu_CountOwned( sellRow.ClassName );

			// Магазин продаётся целиком - количество выбирать нечего.
			ItemBase unitProbe = TraderMenu_FindOwned( sellRow.ClassName );
			Magazine unitMag = Magazine.Cast( unitProbe );
			if ( unitMag && !unitMag.IsAmmoPile() )
				ownedAmount = 1;

			if ( ownedAmount > m_SellAmountMax )
				m_SellAmountMax = ownedAmount;
			// количество берём с ползунка и когда предмет покупаем (owned == 0),
			// если единица торговли стакается (патроны/стаки)
			if ( ownedAmount > 0 || buyMax > 1 )
				m_SellAmountCustom = true;
			if ( m_SellAmountMax > 1 )
			{
				if ( m_SellAmountSlider )
				{
					m_SellAmountSlider.SetMinMax( 1.0, ownedAmount * 1.0 );
					m_SellAmountSlider.SetStep( 1.0 );
					m_SellAmountSlider.SetCurrent( 1.0 );
					m_SellAmountSlider.Show( true );
				}
				TraderMenu_SetSellAmount( 1 );
			}
		}

		TraderMenu_UpdateSellPreview();
	}

	// "Количество 30   Сейчас 250 / 1000" - что лежит в инвентаре прямо сейчас
	// (обновляется после каждой продажи: жидкости, стаки, патроны в пачке).
	string TraderMenu_QuantityText( string itemClassname, string baseText )
	{
		ItemBase ownedItem = TraderMenu_FindOwned( itemClassname );
		if ( !ownedItem )
			return baseText;

		int have = TraderSmartSellEngine.GetSellableAmount( ownedItem );
		int maxHave = 0;
		Magazine ownedMag = Magazine.Cast( ownedItem );
		if ( ownedMag )
			maxHave = ownedMag.GetAmmoMax();
		else
			maxHave = ownedItem.GetQuantityMax();

		if ( maxHave <= 1 && have <= 1 )
			return baseText;

		string label = Widget.TranslateString( "#tm_have_now" );
		return baseText + "   " + label + ": " + have + " / " + maxHave;
	}

	// "патрон" / "магазин" / "шт" - что именно означает одна единица строки
	string TraderMenu_UnitName()
	{
		ItemBase unitItem = TraderMenu_FindOwned( m_SellUnitClass );
		if ( !unitItem )
			return Widget.TranslateString( "#tm_unit_pc" );

		Magazine mag = Magazine.Cast( unitItem );
		if ( mag )
		{
			if ( mag.IsAmmoPile() )
				return Widget.TranslateString( "#tm_unit_round" );
			return Widget.TranslateString( "#tm_unit_mag" );
		}
		return Widget.TranslateString( "#tm_unit_pc" );
	}

	// Пересобрать колонку "в наличии" после торговой операции
	void TraderMenu_RefreshOwnedColumn()
	{
		if ( !m_ListboxItems || !m_FilteredListOfTraderItems )
			return;
		TraderMenu_BuildOwnedCounts();
		for ( int i = 0; i < m_FilteredListOfTraderItems.Count(); i++ )
		{
			TraderItem row = m_FilteredListOfTraderItems.Get( i );
			if ( !row )
				continue;
			string ownedText = "-";
			int ownedCount = TraderMenu_CountOwned( row.ClassName );
			if ( ownedCount > 0 )
				ownedText = "x" + ownedCount;
			m_ListboxItems.SetItem( i, ownedText, NULL, 3 );
		}
	}

	// Обновить карточку и ползунок для выбранной строки (после продажи/покупки)
	void TraderMenu_RefreshSelectedCard()
	{
		int rowIndex = m_ListboxItems.GetSelectedRow();
		if ( rowIndex < 0 || rowIndex >= m_FilteredListOfTraderItems.Count() )
			return;
		TraderItem row = m_FilteredListOfTraderItems.Get( rowIndex );
		if ( !row )
			return;
		TraderMenu_UpdateSellInfo( previewItem, rowIndex );
		m_ItemQuantity.SetText( TraderMenu_QuantityText( row.ClassName, m_QuantString ) );
	}

	void TraderMenu_FillListRow( int rowIndex, TraderItem rowTraderItem )
	{
		if ( !m_ListboxItems || !rowTraderItem )
			return;

		string ownedText = "-";
		int ownedAmount = TraderMenu_CountOwned( rowTraderItem.ClassName );
		if ( ownedAmount > 0 )
			ownedText = "x" + ownedAmount;

		m_ListboxItems.SetItem( rowIndex, "" + TraderMenu_BuyPrice( rowTraderItem.BuyValue ), NULL, 1 );
		m_ListboxItems.SetItem( rowIndex, "" + rowTraderItem.SellValue, NULL, 2 );
		m_ListboxItems.SetItem( rowIndex, ownedText, NULL, 3 );
	}

	void updateItemListboxContent()
	{		
		LoadItemsFromFile();	
		SearchForItems();
	}
	
	void updateItemListboxColors()
	{
		for (int i = 0; i < m_ListboxItems.GetNumItems(); i++)
		{
			int itemCosts = TraderMenu_BuyPrice( m_FilteredListOfTraderItems.Get(i).BuyValue );
			
			if (itemCosts < 0)
			{
				m_ListboxItems.SetItemColor(i, 1, ARGBF(0, 1, 1, 1) );
			}
			else if (m_Player_CurrencyAmount >= itemCosts)
			{
				m_ListboxItems.SetItemColor(i, 1, ARGBF(1, 1, 1, 1) );
			}
			else
			{
				m_ListboxItems.SetItemColor(i, 1, ARGBF(1, 1, 0, 0) );
			}
			
			string itemClassname = m_FilteredListOfTraderItems.Get(i).ClassName;
			int itemQuantity = m_FilteredListOfTraderItems.Get(i).Quantity;
			
			if (m_FilteredListOfTraderItems.Get(i).SellValue < 0)
			{
				m_ListboxItems.SetItemColor(i, 2, ARGBF(0, 1, 1, 1) );
			}
			else if (IsSellableOrInInventory(itemClassname, itemQuantity))
			{
				m_ListboxItems.SetItemColor(i, 2, ARGBF(1, 0, 1, 0) );
			}
			else
			{
				m_ListboxItems.SetItemColor(i, 2, ARGBF(1, 1, 1, 1) );
			}

			EntityAI entityInHands = m_Player.GetHumanInventory().GetEntityInHands();
			if (entityInHands)
			{
				if (IsAttached(entityInHands, itemClassname))
					m_ListboxItems.SetItemColor(i, 0, ARGBF(1, 0.4, 0.4, 1) );
				else if (IsAttachment(entityInHands, itemClassname))
					m_ListboxItems.SetItemColor(i, 0, ARGBF(1, 0.7, 0.7, 1) );
				else
					m_ListboxItems.SetItemColor(i, 0, ARGBF(1, 1, 1, 1) );
			}
		}
	}

	// ============================================================
	// ИСПРАВЛЕННЫЙ МЕТОД: IsSellableOrInInventory (УПРОЩЕН)
	// ============================================================
	bool IsSellableOrInInventory(string itemClassname, int itemQuantity)
	{
		ItemBase item;
		
		// Просто используем isInPlayerInventory - она уже содержит всю логику
		if (m_Player.isInPlayerInventory(itemClassname, itemQuantity, item))
		{
			return true;
		}
		
		// ДОПОЛНИТЕЛЬНАЯ ПРОВЕРКА ДЛЯ ГРАНАТ И МАГАЗИНОВ
		// Если isInPlayerInventory не сработала, проверяем напрямую
		if (itemQuantity == -3 || itemQuantity == -4)
		{
			// Проверяем предмет в руках
			EntityAI handsItem = m_Player.GetHumanInventory().GetEntityInHands();
			if (handsItem && handsItem.IsKindOf(itemClassname))
			{
				ItemBase handsItemBase = ItemBase.Cast(handsItem);
				if (handsItemBase && !handsItemBase.IsRuined())
				{
					return true;
				}
			}
		}
		
		return false;
	}

	void updateItemPreview(string itemType)
	{
		if ( !m_ItemPreviewWidget )
			{
				Widget preview_frame = layoutRoot.FindAnyWidget("ItemFrameWidget");

				if ( preview_frame ) 
				{
					float width;
					float height;
					preview_frame.GetSize(width, height);

					m_ItemPreviewWidget = ItemPreviewWidget.Cast( GetGame().GetWorkspace().CreateWidget(ItemPreviewWidgetTypeID, 0, 0, 1, 1, WidgetFlags.VISIBLE, ARGB(255, 255, 255, 255), 10, preview_frame) );
				}
			}

			if ( previewItem )
				GetGame().ObjectDelete( previewItem );

			string lower = itemType;
			int leng = -1;
			string itemName = itemType;
            lower.ToLower();
			if(TR_Helper.KitIgnoreArray.Find(itemType) == -1 && lower.Contains("kit_"))
            {
                itemName = itemType.Substring(4,itemType.Length());  
            }
            else if(TR_Helper.KitIgnoreArray.Find(itemType) == -1 && lower.Contains("_kit"))
            {
                leng = itemType.Length() - 4;
                itemName = itemType.Substring(0,leng);
				if(lower.Contains("md_camonetshelter"))
					itemName = "Land_" + itemName;
            }
			else if(TR_Helper.KitIgnoreArray.Find(itemType) == -1 && lower.Contains("kit"))
            {
                leng = itemType.Length() - 3;      
                itemName = itemType.Substring(0,leng);  
            }
			previewItem = EntityAI.Cast(GetGame().CreateObject( itemName, "0 0 0", true, false, true ));
			if(!previewItem)
			{
				previewItem = EntityAI.Cast(GetGame().CreateObject( itemType, "0 0 0", true, false, true ));
			}
			m_ItemPreviewWidget.SetItem( previewItem );
            
			m_ItemPreviewWidget.SetModelPosition( Vector(1.0,1.0,0.5) );
			m_ItemPreviewWidget.SetModelOrientation( Vector(0,0,0) );

			float itemx, itemy;		
			m_ItemPreviewWidget.GetPos(itemx, itemy);
			m_ItemPreviewWidget.SetSize( 1.0, 1.0 );
			m_ItemPreviewWidget.SetPos( 0, 0 );

			if (previewItem)
			{
				local int row_index = m_ListboxItems.GetSelectedRow();
				int itemQuantity = m_FilteredListOfTraderItems.Get(row_index).Quantity;
				string itemQuant = m_QuantString + " 1";				
				if(itemQuantity > 0 && !TR_Helper.HasQuantityBar(itemName))
				{
					itemQuant = m_QuantString + " " + itemQuantity.ToString();
				}
				else
				{
					int sizeCount = TR_Helper.GetItemSlotCount(itemName);
					if(sizeCount > 0)
					{
						itemQuant = m_SizeString + " " + sizeCount;
					}
				}
				m_ItemWeight.SetText(GetItemWeightText());
				m_ItemQuantity.SetText(TraderMenu_QuantityText( m_FilteredListOfTraderItems.Get(row_index).ClassName, itemQuant ));
				TraderMenu_UpdateSellInfo( previewItem, row_index );
                if(TR_Helper.KitIgnoreArray.Find(itemType) == -1 && lower.Contains("kit") && previewItemKit)
                {
				    m_ItemDescription.SetText(string.Format("This item is a kit. %1",GetEntityAITooltip(previewItemKit)));
                }
                else
                {
				    m_ItemDescription.SetText(GetEntityAITooltip(previewItem));
                }
			}
			else
			{
				m_ItemWeight.SetText("");
				m_ItemDescription.SetText("ERROR FINDING ITEM. DO NOT BUY THIS ITEM.");
				m_ItemQuantity.SetText("");
			}
	}

	string GetItemWeightText()
	{
		ItemBase item_IB = ItemBase.Cast( previewItem );
		if(item_IB)
		{
			int weight = item_IB.GetSingleInventoryItemWeight();
			
			if (weight >= 1000)
			{
				int kilos = Math.Round(weight / 1000.0);
				return  "#inv_inspect_about" + " " + kilos.ToString() + " " + "#inv_inspect_kg";
			}
			else if (weight >= 500)
			{
				return "#inv_inspect_under_1";
			} 
			else if (weight >= 250)
			{
				return "#inv_inspect_under_05";
			}
			else 
			{
				return "#inv_inspect_under_025";
			}			
		}

		return "";
	}
	
	void updatePlayerCurrencyAmount()
	{
		m_Player_CurrencyAmount = 0;
		m_Player_CurrencyAmount = m_Player.getPlayerCurrencyAmount();
		m_SaldoValue.SetText(" " + m_Player_CurrencyAmount);

		if ( m_DiscountText )
		{
			if ( m_Player.m_Trader_RatingEnabled && m_Player.m_Trader_RatingDiscount > 0 )
			{
				string discountLabel = Widget.TranslateString( "#tm_rating_discount" );
				m_DiscountText.SetText( discountLabel + " " + m_Player.m_Trader_RatingDiscount + "%" );
			}
			else
			{
				m_DiscountText.SetText( "" );
			}
		}
	}

	// ============================================================
	// Survival rating: price actually charged + badge refresh
	// ============================================================
	int TraderMenu_BuyPrice( int basePrice )
	{
		if ( !m_Player )
			return basePrice;
		return TraderRating.ApplyDiscount( basePrice, m_Player.m_Trader_RatingDiscount );
	}

	// Called once per second from Update(): keeps the compact discount text
	// next to the money in sync with the rating the server sends.
	// No separate badge is drawn in this window - every corner is taken by the
	// title, the list, the details panel, the sell bar and the buttons, so a
	// badge would overlap them. The full badge (star, progress bar and the
	// right-click hint) is drawn in the inventory by TraderRatingUI.c from
	// TRADER_NPC/scripts/layouts/TraderRating.layout.
	void TraderMenu_RatingRefresh()
	{
		if ( !m_Player )
			return;

		updatePlayerCurrencyAmount();
	}

	bool IsAttached(EntityAI parentEntity, string attachmentClassname)
	{
		for ( int i = 0; i < parentEntity.GetInventory().AttachmentCount(); i++ )
		{
			EntityAI attachment = parentEntity.GetInventory().GetAttachmentFromIndex ( i );
			if ( attachment.IsKindOf ( attachmentClassname ) )
				return true;
		}

		return false;
	}

	bool IsAttachment(EntityAI parentEntity, string attachmentClassname)
	{
		string type_name = parentEntity.GetType();
		TStringArray cfg_chamberables = new TStringArray;

		GetGame().ConfigGetTextArray(CFG_WEAPONSPATH + " " + type_name + " chamberableFrom", cfg_chamberables);

		for (int i = 0; i < cfg_chamberables.Count(); i++)
		{
			if (attachmentClassname == cfg_chamberables[i])
				return true;
		}

		TStringArray cfg_magazines = new TStringArray;

		GetGame().ConfigGetTextArray(CFG_WEAPONSPATH + " " + type_name + " magazines", cfg_magazines);

		for (i = 0; i < cfg_magazines.Count(); i++)
		{
			if (attachmentClassname == cfg_magazines[i])
				return true;
		}

		return false;
	}
	
	bool LoadCategories()
	{		
		m_TraderName.SetText(m_Player.m_Trader_TraderNames.Get(m_TraderID));
		m_Saldo.SetText(m_Player.m_Trader_CurrencyName + ": ");

		m_XComboboxCategorys.ClearAll();
		m_Categorys = new array<string>;
		m_CategorysTraderKey = new array<int>;
		m_CategorysKey = new array<int>;
		
		for ( int i = 0; i < m_Player.m_Trader_Categorys.Count(); i++ )
		{
			if (m_TraderID != m_Player.m_Trader_CategorysTraderKey.Get(i))
				continue;
			
			m_XComboboxCategorys.AddItem(m_Player.m_Trader_Categorys.Get(i));
			m_Categorys.Insert(m_Player.m_Trader_Categorys.Get(i));
			m_CategorysTraderKey.Insert(m_Player.m_Trader_CategorysTraderKey.Get(i));
			m_CategorysKey.Insert(i);
		}
		
		return true;
	}
	
	bool LoadItemsFromFile()
	{
		m_ListOfTraderItems.Clear();
		m_ListOfCategoryTraderItems.Clear();
		m_FilteredListOfTraderItems.Clear();

		for ( int i = 0; i < m_Player.m_Trader_ItemsClassnames.Count(); i++ )
		{	
			if(m_Player.m_Trader_ItemsTraderId.Get(i) != m_TraderID)
				continue;

			TraderItem item = new TraderItem;
			item.ClassName = m_Player.m_Trader_ItemsClassnames.Get(i);
			item.Quantity = m_Player.m_Trader_ItemsQuantity.Get(i);
			item.BuyValue = m_Player.m_Trader_ItemsBuyValue.Get(i);
			item.SellValue = m_Player.m_Trader_ItemsSellValue.Get(i);
			item.IndexId = i;
			m_ListOfTraderItems.Insert(item);
			if ( m_Player.m_Trader_ItemsCategoryId.Get(i) == m_CategorysKey.Get(m_CategorysCurrentIndex) )
			{		
				m_ListOfCategoryTraderItems.Insert(item);
				m_FilteredListOfTraderItems.Insert(item);
			}
		}
		
		return true;
	}
		
	void SearchForItems()
    { 
		m_ListboxItems.ClearItems();
		m_FilteredListOfTraderItems.Clear();
		TraderMenu_BuildOwnedCounts();
        string displayName = "";
		int countFilter = 0;
       
		if(m_SearchFilter && m_SearchFilter != string.Empty)
		{
			countFilter = 0;
			foreach(TraderItem traderItem : m_ListOfTraderItems)
			{                
				displayName = m_Player.getItemDisplayName(traderItem.ClassName);
				string low_DisplayName = displayName;
				low_DisplayName.ToLower();
				string low_m_SearchFilter = m_SearchFilter;
				low_m_SearchFilter.ToLower();
				if(low_DisplayName.Contains(low_m_SearchFilter))
				{
					m_FilteredListOfTraderItems.Insert(traderItem);
					m_ListboxItems.AddItem( displayName, NULL, 0 );	
					TraderMenu_FillListRow( countFilter, traderItem );
					countFilter++;
				}
			}
		}
		else if(m_SellablesOnly)
		{
			countFilter = 0;
			foreach(TraderItem sellableTraderItem : m_ListOfTraderItems)
			{                
				if(!ShouldShowInSellablesList(sellableTraderItem))
						continue;
				displayName = m_Player.getItemDisplayName(sellableTraderItem.ClassName);
				m_FilteredListOfTraderItems.Insert(sellableTraderItem);
				m_ListboxItems.AddItem( displayName, NULL, 0 );	
				TraderMenu_FillListRow( countFilter, sellableTraderItem );
				countFilter++;
			}
		}
		else
		{
			countFilter = 0;
			foreach(TraderItem catTraderItem : m_ListOfCategoryTraderItems)
			{ 
				displayName = m_Player.getItemDisplayName(catTraderItem.ClassName);    
				m_FilteredListOfTraderItems.Insert(catTraderItem);
				m_ListboxItems.AddItem( displayName, NULL, 0 );	
				TraderMenu_FillListRow( countFilter, catTraderItem );
				countFilter++;
			}
		}

        m_OldSearchFilter = m_SearchFilter;
        if(m_FilteredListOfTraderItems.Count() > 0)
        {
            m_LastRowIndex = -1;
            m_ListboxItems.SelectRow(0);
        }
        
	}

	bool ShouldShowInSellablesList(TraderItem catTraderItem)
	{
		if(!m_SellablesCheckbox.IsChecked())
			return true;			
		string itemClassname = catTraderItem.ClassName;
		int itemQuantity = catTraderItem.Quantity;
		if (catTraderItem.SellValue < 0)
			return false;
		
		return IsSellableOrInInventory(itemClassname, itemQuantity);
	}

    string GetEntityAITooltip(EntityAI item)
	{
		string temp;
		if (!item.DescriptionOverride(temp))
		{
			temp = item.ConfigGetString("descriptionShort");
		}
		return m_Player.TrimUntPrefix(temp);
	}

    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w, x, y, button);
		
		if (w == m_ItemPreviewWidget)
		{
			GetGame().GetDragQueue().Call(this, "UpdateRotation");
			GetMousePos(m_PreviewWidgetRotationX, m_PreviewWidgetRotationY);
			return true;
		}
		return false;
	}

	void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging)
	{
		vector o = m_PreviewWidgetOrientation;
		o[0] = o[0] + (m_PreviewWidgetRotationY - mouse_y);
		o[1] = o[1] - (m_PreviewWidgetRotationX - mouse_x);
		
		m_ItemPreviewWidget.SetModelOrientation( o );
		
		if (!is_dragging)
		{
			m_PreviewWidgetOrientation = o;
		}
	}

	override bool OnMouseWheel(Widget  w, int  x, int  y, int wheel)
	{
		super.OnMouseWheel(w, x, y, wheel);
		
		if ( w == m_ItemPreviewWidget )
		{
			m_characterScaleDelta = wheel;
			UpdateScale();
		}
		return false;
	}
	
	void UpdateScale()
	{
		float w, h, x, y;		
		m_ItemPreviewWidget.GetPos(x, y);
		m_ItemPreviewWidget.GetSize(w,h);
		w = w + ( m_characterScaleDelta / 4);
		h = h + ( m_characterScaleDelta / 4 );
		if ( w > 0.5 && w < 3 )
		{
			m_ItemPreviewWidget.SetSize( w, h );
	
			int screen_w, screen_h;
			GetScreenSize(screen_w, screen_h);
			float new_x = x - ( m_characterScaleDelta / 8 );
			float new_y = y - ( m_characterScaleDelta / 8 );
			m_ItemPreviewWidget.SetPos( new_x, new_y );
		}
	}	
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if ( key == KeyCode.KC_ESCAPE )
		{
			Close();
		}
		
		return super.OnKeyDown(w, x, y, key);
	}
};