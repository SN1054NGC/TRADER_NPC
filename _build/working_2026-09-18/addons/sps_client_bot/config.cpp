class CfgPatches
{
	class sps_client_bot
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
	class sps_client_bot
	{
		dir="sps_client_bot";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="sps_client_bot";
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
					"sps_client_bot/scripts/defines",
					"sps_client_bot/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"sps_client_bot/scripts/defines",
					"sps_client_bot/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"sps_client_bot/scripts/defines",
					"sps_client_bot/scripts/5_Mission"
				};
			};
		};
	};
};