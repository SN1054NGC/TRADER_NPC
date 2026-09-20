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

// ============================================================
// ДЕНЬГИ TRADER_NPC. Внешне и по поведению - ванильная бумага:
// класс наследует Paper (модель/текстуру DZ\gear\consumables\Paper.p3d),
// но складывается в пачку до 99999 штук и ничего не весит, чтобы
// деньги можно было носить. canBeSplit=1 - игрок сам делит пачку.
//
// Класс валюты и номинал задаются в профиле (TraderNpcConfig.txt):
//   <CurrencyName> #tm_ruble      - как валюта называется в интерфейсе
//   <Currency> TraderNpcMoney, 1  - Класс, Номинал (1 бумага = 1 рубль)
// Можно указать ЛЮБОЙ класс игры: предмет без varQuantityMax/count
// считается модом за 1 штуку (см. increasePlayerCurrency).
// ============================================================
class CfgVehicles
{
	class Paper;

	class TraderNpcMoney: Paper
	{
		scope=2;
		displayName="$STR_tm_money";
		descriptionShort="$STR_tm_money_description";
		weight=0;
		canBeSplit=1;
		varQuantityInit=1;
		varQuantityMin=0;
		varQuantityMax=99999;
		varQuantityDestroyOnMin=1;
	};
};

// ============================================================
// Голос торговца: фразы сгенерированы локально (piper) и лежат в sounds/voice.
// ============================================================
// Голос торговца: фразы сгенерированы локально (piper -> ogg).
// Голос выбирается по имени NPC (пол) и порядку спавна: female_1/female_2/male_1/male_2.
// ============================================================
class CfgSoundShaders
{
	class TRADER_NPC_VOICE_female_1_greet_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_greet_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_browse_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_browse", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_buy_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_buy_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_buy_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_buy_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_no_money_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_no_money", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_sell_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_empty_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_empty", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_notrade_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_notrade", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_bye_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_bye", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_greet_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_greet_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_browse_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_browse", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_buy_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_buy_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_buy_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_buy_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_no_money_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_no_money", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_sell_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_empty_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_empty", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_notrade_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_notrade", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_female_2_bye_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_bye", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_greet_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_greet_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_browse_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_browse", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_buy_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_buy_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_buy_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_buy_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_no_money_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_no_money", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_sell_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_empty_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_empty", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_notrade_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_notrade", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_1_bye_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_bye", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_greet_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_greet_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_browse_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_browse", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_buy_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_buy_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_buy_2_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_buy_2", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_no_money_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_no_money", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_sell_1", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_empty_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_empty", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_notrade_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_notrade", 1}};
		volume = 1.0;
		range = 25;
	};
	class TRADER_NPC_VOICE_male_2_bye_SoundShader
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_bye", 1}};
		volume = 1.0;
		range = 25;
	};
};

class CfgSoundSets
{
	class TRADER_NPC_VOICE_female_1_greet_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_greet_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_browse_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_browse_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_buy_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_buy_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_buy_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_buy_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_no_money_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_no_money_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_empty_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_empty_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_notrade_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_bye_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_bye_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_greet_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_greet_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_browse_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_browse_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_buy_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_buy_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_buy_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_buy_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_no_money_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_no_money_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_empty_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_empty_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_notrade_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_2_bye_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_bye_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_greet_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_greet_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_browse_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_browse_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_buy_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_buy_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_buy_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_buy_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_no_money_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_no_money_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_empty_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_empty_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_notrade_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_1_bye_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_bye_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_greet_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_greet_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_browse_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_browse_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_buy_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_buy_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_buy_2_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_buy_2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_no_money_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_no_money_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_empty_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_empty_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_notrade_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_male_2_bye_SoundSet
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_bye_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
};
