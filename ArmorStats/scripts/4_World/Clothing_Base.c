modded class Clothing
{
	bool HasGlobalArmor()
	{ 	//Dont show gui for anything that doesn't offer bullet protection;
		return GetGame().ConfigIsExisting("CfgVehicles " + GetType() + " DamageSystem GlobalArmor Projectile");
	}

	bool ShittyConfigCheck() //inventorySlot is not an array in config.cpp like it should be;
	{
		return GetGame().ConfigGetType("CfgVehicles " + GetType() + " inventorySlot") != CT_ARRAY;
	}

    bool DisplayStatGUIOnThis()
	{ 	//We don't care about shirts and pants and shit;
		if (HasGlobalArmor())
		{
			string cfgPath = "CfgVehicles " + GetType() + " inventorySlot";
			if (ShittyConfigCheck())
			{
				string item = "";
				GetGame().ConfigGetText(cfgPath, item);
				return item == "Vest" || item == "Headgear";
			}

			TStringArray inventorySlots = new TStringArray;
			GetGame().ConfigGetTextArray(cfgPath, inventorySlots);
			return inventorySlots.Find("Vest") != -1 || inventorySlots.Find("Headgear") != -1;
		}
		return false;
	}
}