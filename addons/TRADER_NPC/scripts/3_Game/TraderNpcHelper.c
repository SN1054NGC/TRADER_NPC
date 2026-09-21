static const int TRADERMENU_UI  = 665001;


class TR_Helper
{
    static float GetTraderAllowedTradeDistance()
    {
        return 3.0;
    }

    static float GetTraderSellAllowedDistance()
    {
    	// <SellAnywhere> yes: продавать можно из любого места карты (скупщик работает везде)
    	if ( TraderNpcFeatures.s_SellAnywhere )
    		return 999999.0;
    	return 12.0;
    }

    static ref TStringArray KitIgnoreArray = 
    {
        "BloodTestKit",
        "StartKitIV",
        "FirstAidKit",
        "MSFC_FirstAidKit",
        "WG_ITS_Medkit",
        "WG_AI2_Medkit",
        "SewingKit",
        "LeatherSewingKit",
        "WeaponCleaningKit",
        "KitchenKnife",
        "ElectronicRepairKit",
        "TireRepairKit",
        "gunwall_kit_mung",
        "gunwall_metal_kit_mung"
    };
};