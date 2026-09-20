class CfgPatches
{
	class TRADER_NPC
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};

class CfgMods
{
	class TRADER_NPC
	{
		dir="TRADER_NPC";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="TRADER_NPC";
		credits="";
		author="";
		authorID="0";
		version="3.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"TRADER_NPC/scripts/defines",
					"TRADER_NPC/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"TRADER_NPC/scripts/defines",
					"TRADER_NPC/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"TRADER_NPC/scripts/defines",
					"TRADER_NPC/scripts/5_Mission"
				};
			};
		};
	};
};