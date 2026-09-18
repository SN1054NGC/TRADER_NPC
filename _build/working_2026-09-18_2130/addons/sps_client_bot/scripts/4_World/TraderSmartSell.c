// ============================================================
// FILE: TraderSmartSell.c
// Accessory / computed bonus for selling a PACKED head
// (weapon / magazine etc) and stack pricing helpers.
//
// This is a PURE COMPUTATION helper: it never deletes, returns
// or modifies any item and never touches the inventory state.
//
// Behavior:
//  * The head (weapon) itself is priced by the caller from its
//    own trader price-row (see DayZPlayerImplement.handleSellRPC).
//  * THIS file computes the EXTRA money worth of things mounted on
//    (or loaded into) that head, to ADD on top of the head price:
//      - any direct accessory (optic/SILS, handguard, ..., magazine)
//        that has its own trader sell-row (SellValue>=0) is priced by
//        that row, conditioned by the accessory's own health
//        (hp = GetHealth01, minimum 1 for ruined / hp<=0.01).
//      - when an accessory is a MAGAZINE it is paid twice the safe way:
//          (m)agazine body  = mag sell-row price  * mag hp
//          (b)ullets inside = price of a SINGLE round of its cartridge
//             wired one time per actually present round
//             (round count read via Magazine.GetCartridgeAtIndex,
//             cartType from config; price per round = trader's sell row
//             that has Quantity==1 for that ammo class).
//      - items that have NO sellable trader row (SellValue<0 / absent)
//        contribute 0 and do not block anything.
//  * Stack helpers (this revision) let the client sell only PART of a
//    stack / ammo pile with a slider:
//      - GetSellableAmount()      how many units can be sold at once
//      - ComputeStackBasePrice()  base price for an arbitrary amount
//  Nothing here has side effects (no Game.ObjectDelete/local take/etc).
//
// Enforce Script notes: no ternary operator, no void-as-expression,
// no multi-line concatenation inside call arguments.
// ============================================================

class TraderAccessoryBonus
{
	int 	Bonus;
	bool 	HasHeadRow;   // true only when caller should add head row price too
	int 	HeadRowIndex;

	void TraderAccessoryBonus()
	{
		Bonus = 0;
		HasHeadRow = false;
		HeadRowIndex = -1;
	}
};

class TraderSmartSellEngine
{
	// ------------------------------------------------------------------
	// Find the trader row index whose class-name == cls and whose
	// Quantity field == requestedQty (used to locate "per ONE unit"
	// rows, e.g. ammo rows that are priced for a single round).
	// Returns -1 when absent.
	// ------------------------------------------------------------------
	static int FindRowByQuantity( DayZPlayerImplement impl, int traderIndex, string cls, int requestedQty )
	{
		if ( !impl )
			return -1;
		if ( !impl.m_Trader_ItemsClassnames || !impl.m_Trader_ItemsTraderId )
			return -1;
		if ( !impl.m_Trader_ItemsQuantity || !impl.m_Trader_ItemsSellValue )
			return -1;
		if ( impl.m_Trader_ItemsClassnames.Count() <= 0 )
			return -1;

		string lower = cls;
		lower.ToLower();

		// Expansion-style: search the GLOBAL trader price list (any trader),
		// not only the head's own trader. Accessory rows (mag/mount/ammo) may
		// be listed under a different trader block than the weapon itself.
		int any = -1;
		for ( int i = 0; i < impl.m_Trader_ItemsClassnames.Count(); i++ )
		{
			if ( impl.m_Trader_ItemsQuantity.Get( i ) != requestedQty )
				continue;
			string row = impl.m_Trader_ItemsClassnames.Get( i );
			row.ToLower();
			if ( row != lower )
				continue;
			if ( impl.m_Trader_ItemsSellValue.Get( i ) < 0 )
				continue;
			if ( impl.m_Trader_ItemsTraderId.Get( i ) == traderIndex )
				return i;          // preferred: same trader
			if ( any == -1 )
				any = i;           // fallback: any trader with this class
		}
		return any;
	}

	// ------------------------------------------------------------------
	// Find ANY trader row for the cls (any quantity) with SellValue >= 0.
	// ------------------------------------------------------------------
	static int FindSellableRow( DayZPlayerImplement impl, int traderIndex, string cls )
	{
		if ( !impl )
			return -1;
		if ( cls == "" )
			return -1;
		if ( !impl.m_Trader_ItemsClassnames || !impl.m_Trader_ItemsTraderId || !impl.m_Trader_ItemsSellValue )
			return -1;
		if ( impl.m_Trader_ItemsClassnames.Count() <= 0 )
			return -1;

		string lower = cls;
		lower.ToLower();

		// Expansion-style global search (any trader) as robust fallback.
		int any = -1;
		for ( int i = 0; i < impl.m_Trader_ItemsClassnames.Count(); i++ )
		{
			string row = impl.m_Trader_ItemsClassnames.Get( i );
			row.ToLower();
			if ( row != lower )
				continue;
			if ( impl.m_Trader_ItemsSellValue.Get( i ) < 0 )
				continue;
			if ( impl.m_Trader_ItemsTraderId.Get( i ) == traderIndex )
				return i;          // preferred: same trader
			if ( any == -1 )
				any = i;           // fallback: any trader
		}
		return any;
	}

	// ------------------------------------------------------------------
	// Sell value of ONE unit of cls = the row whose Quantity is exactly 1.
	// Returns -1 when the price table has no such row.
	// ------------------------------------------------------------------
	static int FindUnitSellValue( DayZPlayerImplement impl, int traderIndex, string cls )
	{
		int row = FindRowByQuantity( impl, traderIndex, cls, 1 );
		if ( row < 0 )
			return -1;

		return impl.m_Trader_ItemsSellValue.Get( row );
	}

	// ------------------------------------------------------------------
	// Base (not health-conditioned) price for selling 'amount' units of cls.
	//   1) a price row whose Quantity is exactly 'amount' wins (tier price)
	//   2) otherwise the per-one-unit row is multiplied by 'amount'
	//   3) otherwise the caller supplied fallback row value is used as is
	// amount <= 1 keeps the plain row price.
	// ------------------------------------------------------------------
	static int ComputeStackBasePrice( DayZPlayerImplement impl, int traderIndex, string cls, int amount, int fallbackRowSellValue )
	{
		if ( amount <= 1 )
			return fallbackRowSellValue;

		int exact = FindRowByQuantity( impl, traderIndex, cls, amount );
		if ( exact >= 0 )
		{
			int exactSell = impl.m_Trader_ItemsSellValue.Get( exact );
			if ( exactSell > 0 )
				return exactSell;
		}

		int unit = FindUnitSellValue( impl, traderIndex, cls );
		if ( unit > 0 )
			return unit * amount;

		return fallbackRowSellValue;
	}

	// ------------------------------------------------------------------
	// How many units of this item the trader can take in one go.
	// Ammo piles and plain stacks return their current count; anything
	// that is sold as a whole (weapon, weapon magazine, single item)
	// returns 1 so the caller hides the amount slider.
	// ------------------------------------------------------------------
	static int GetSellableAmount( ItemBase ib )
	{
		if ( !ib )
			return 1;

		if ( ib.IsMagazine() )
		{
			Magazine mag = Magazine.Cast( ib );
			if ( !mag )
				return 1;

			if ( mag.IsAmmoPile() )
			{
				int ammo = mag.GetAmmoCount();
				if ( ammo > 1 )
					return ammo;
			}
			return 1;
		}

		int qty = ib.GetQuantity();
		if ( qty > 1 )
			return qty;

		return 1;
	}

	// ------------------------------------------------------------------
	// Condition a base sell value by the item's own health.
	//
	// IMPORTANT: Object.GetHealth01() is a SERVER-only read. Calling it on a
	// client raises a VM exception ("Object::GetHealth01 cannot be called on
	// client", spamming the client RPT and aborting the UI update that called
	// us). Clients therefore use the synced discrete damage state through
	// Object.GetHealthLevel() - the same source the vanilla client UI uses -
	// and the middle of the matching damage band. The server always computes
	// the exact figure, and every client display is corrected by the
	// RPC_APPRAISE_SELL_REPLY value as soon as it arrives.
	// ------------------------------------------------------------------
	static float HealthFactorFromLevel( int level )
	{
		// vanilla healthLabels bands: pristine >= 0.9, worn 0.6..0.9,
		// damaged 0.4..0.6, badly damaged 0.2..0.4, ruined ~0
		if ( level == GameConstants.STATE_PRISTINE )
			return 0.95;
		if ( level == GameConstants.STATE_WORN )
			return 0.75;
		if ( level == GameConstants.STATE_DAMAGED )
			return 0.5;
		if ( level == GameConstants.STATE_BADLY_DAMAGED )
			return 0.3;
		if ( level == GameConstants.STATE_RUINED )
			return 0.0;
		return 1.0;
	}

	#ifdef SERVER
	static int Condition( ItemBase ib, int baseSell )
	{
		if ( !ib )
			return 0;
		if ( baseSell <= 0 )
			return 0;

		float hp = ib.GetHealth01( "", "Health" );
		if ( ib.IsRuined() || hp <= 0.01 )
		{
			return 1;
		}

		int val = Math.Round( baseSell * hp );
		if ( val < 1 )
			val = 1;
		return val;
	}
	#else
	static int Condition( ItemBase ib, int baseSell )
	{
		if ( !ib )
			return 0;
		if ( baseSell <= 0 )
			return 0;

		int level = ib.GetHealthLevel();
		if ( level == GameConstants.STATE_RUINED )
			return 1;

		float hp = HealthFactorFromLevel( level );
		if ( hp <= 0.01 )
			return 1;

		int val = Math.Round( baseSell * hp );
		if ( val < 1 )
			val = 1;
		return val;
	}
	#endif

	// ------------------------------------------------------------------
	// One ordinary (non-magazine) attached accessory:
	// sell-row present -> + conditioned row, else 0.
	// ------------------------------------------------------------------
	static void AddAccessory( inout int total, DayZPlayerImplement impl, int traderIndex, ItemBase acc )
	{
		if ( !acc )
			return;

		int row = FindSellableRow( impl, traderIndex, acc.GetType() );
		if ( row == -1 )
			return;

		int raw = impl.m_Trader_ItemsSellValue.Get( row );
		total = total + Condition( acc, raw );
	}

	// ------------------------------------------------------------------
	// A magazine accessory: pay its body (conditioned) then pay every
	// currently loaded round once, at the trader "per single round" price
	// gathered from the row whose quantity is exactly 1.
	// ------------------------------------------------------------------
	static void AddMagazineBonus( inout int total, DayZPlayerImplement impl, int traderIndex, ItemBase magBase )
	{
		if ( !magBase )
			return;

		Magazine mag = Magazine.Cast( magBase );
		if ( !mag )
			return;

		// 1) the magazine shell itself (its own trader sell-row)
		AddAccessory( total, impl, traderIndex, magBase );

		// 2) loaded rounds
		int ammoCount = mag.GetAmmoCount();
		if ( ammoCount <= 0 )
			return;

		// Determine the round class-name by reading the first cartridge so
		// we can look up its single-round trader row.
		// (Magazines usually hold a single ammo type.)
		// Magazine.GetCartridgeAtIndex() is a SERVER-only read (it is absent
		// from every vanilla client script), so on the client only the
		// magazine body is estimated - the exact total still comes from the
		// server appraisal.
		#ifdef SERVER
		float tmpDamage;
		string firstCart = "";
		bool got = mag.GetCartridgeAtIndex( 0, tmpDamage, firstCart );
		if ( !got || firstCart == "" )
			return; // no price info -> treat loaded rounds as included in mag / no extra

		int perRoundRow = FindRowByQuantity( impl, traderIndex, firstCart, 1 );
		if ( perRoundRow == -1 )
			return;

		int perRoundSell = impl.m_Trader_ItemsSellValue.Get( perRoundRow );
		if ( perRoundSell <= 0 )
			return;

		// price per round = perRoundSell (healthCondition applied once to
		// the round collective - effectively to the magazine health).
		int perRound = Condition( magBase, perRoundSell );

		total = total + ( perRound * ammoCount );
		#endif
	}

	// ------------------------------------------------------------------
	// Main entry: compute EXTRA bonus for the packed head only.
	// Returns bonus to add on top of the head's own row price.
	// No side effects anywhere.
	// ------------------------------------------------------------------
	static int ComputeExtraBonus( DayZPlayerImplement impl, int traderIndex, ItemBase packedHead )
	{
		if ( !impl || !packedHead )
			return 0;

		if ( !packedHead.GetInventory() )
			return 0;

		int bonus = 0;
		array<EntityAI> content = new array<EntityAI>;
		packedHead.GetInventory().EnumerateInventory( InventoryTraversalType.PREORDER, content );

		foreach ( EntityAI ent : content )
		{
			if ( ent == packedHead )
				continue;

			ItemBase child = ItemBase.Cast( ent );
			if ( !child )
				continue;

			if ( child.IsMagazine() )
			{
				AddMagazineBonus( bonus, impl, traderIndex, child );
			}
			else
			{
				AddAccessory( bonus, impl, traderIndex, child );
			}
		}

		return bonus;
	}
};
