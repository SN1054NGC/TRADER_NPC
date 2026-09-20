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
// Голос торговца: фразы сгенерированы локально (piper -> ogg).
// Голос выбирается по имени NPC: SurvivorF_* -> female_*, SurvivorM_* -> male_*.
// ============================================================
// Голос торговца: фразы сгенерированы локально (piper -> ogg).
// Голос выбирается по имени NPC: SurvivorF_* -> female_*, SurvivorM_* -> male_*.
// Кривые/фильтры - как в ванильных наборах (baseCharacter_SoundSet).
// ============================================================
// Голос торговца: 6 голосов x варианты фраз (piper -> ogg, локально).
// ============================================================
class CfgSoundShaders
{
	class TRADER_NPC_Voice_Base
	{
		range = 25;
	};
	class TRADER_NPC_VOICE_female_1_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_sell_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_sell_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_3_sell_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_sell_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_sell_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_buy_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_buy_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_buy_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_bye_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_bye_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_bye_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_bye_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_empty_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_empty_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_empty_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_empty_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_funny_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_funny_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_funny_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_funny_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_3.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_poor_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_poor_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_poor_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_poor_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_rich_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_rich_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_rich_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_greet_rich_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_notrade_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_notrade_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_notrade_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_notrade_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_no_money_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_no_money_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_no_money_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_no_money_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_2_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_sell_2.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_3_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_3_sell_3.ogg", 1}};
		volume = 1.0;
	};
};

class CfgSoundSets
{
	class TRADER_NPC_VoiceSet_Base
	{
		sound3DProcessingType = "character3DProcessingType";
		volumeCurve = "characterAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class TRADER_NPC_VOICE_female_1_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_3_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_3_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_buy_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_buy_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_buy_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_buy_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_bye_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_bye_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_bye_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_bye_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_empty_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_empty_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_empty_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_empty_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_funny_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_funny_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_funny_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_funny_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_3_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_poor_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_poor_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_poor_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_poor_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_rich_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_rich_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_greet_rich_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_greet_rich_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_notrade_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_notrade_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_notrade_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_notrade_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_no_money_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_no_money_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_no_money_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_no_money_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_2_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_sell_2_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_3_sell_3_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_3_sell_3_SoundShader"};
		volumeFactor = 1.0;
	};
};
