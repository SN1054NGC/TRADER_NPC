class CfgPatches
{
	class ArmorStats_CfgPatches
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class ArmorStats
	{
		dir="ArmorStats";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="ArmorStats";
		credits="";
		author="Me";
		authorID="0";
		version=1;
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World",
			"Game"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"ArmorStats/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"ArmorStats/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"ArmorStats/scripts/5_Mission"
				};
			};
		};
	};
};
