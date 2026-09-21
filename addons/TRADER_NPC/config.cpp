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
	class RedCaviar;

	// Монеты TRADER_NPC: ванильная банка икры (модель red_caviar.p3d), но стакается счетом
	// и имеет вес. 1 монета = weight грамм, пачка до 99999 штук.
	class TraderNpcCoin: RedCaviar
	{
		scope=2;
		displayName="$STR_tm_coin";
		descriptionShort="$STR_tm_coin_desc";
		weight=5;
		stackedUnit="pc.";
		varQuantityInit=1;
		varQuantityMin=0;
		varQuantityMax=99999;
		varQuantityDestroyOnMin=1;
		canBeSplit=1;
		quantityBar=0;
	};
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
class CfgSoundShaders
{
	class TRADER_NPC_Voice_Base
	{
		range = 25;
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
	class TRADER_NPC_VOICE_female_1_browse_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_browse.ogg", 1}};
		volume = 1.0;
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
	class TRADER_NPC_VOICE_female_1_no_money_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_no_money.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_empty.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_notrade.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_1_bye.ogg", 1}};
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
	class TRADER_NPC_VOICE_female_2_browse_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_browse.ogg", 1}};
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
	class TRADER_NPC_VOICE_female_2_no_money_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_no_money.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_empty.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_notrade.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\female_2_bye.ogg", 1}};
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
	class TRADER_NPC_VOICE_male_1_browse_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_browse.ogg", 1}};
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
	class TRADER_NPC_VOICE_male_1_no_money_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_no_money.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_empty.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_notrade.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_1_bye.ogg", 1}};
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
	class TRADER_NPC_VOICE_male_2_browse_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_browse.ogg", 1}};
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
	class TRADER_NPC_VOICE_male_2_no_money_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_no_money.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_sell_1.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_empty.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_notrade.ogg", 1}};
		volume = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_SoundShader: TRADER_NPC_Voice_Base
	{
		samples[] = {{"TRADER_NPC\sounds\voice\male_2_bye.ogg", 1}};
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
	class TRADER_NPC_VOICE_female_1_browse_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_browse_SoundShader"};
		volumeFactor = 1.0;
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
	class TRADER_NPC_VOICE_female_1_no_money_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_no_money_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_empty_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_empty_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_notrade_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_notrade_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_1_bye_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_1_bye_SoundShader"};
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
	class TRADER_NPC_VOICE_female_2_browse_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_browse_SoundShader"};
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
	class TRADER_NPC_VOICE_female_2_no_money_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_no_money_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_empty_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_empty_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_notrade_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_notrade_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_female_2_bye_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_female_2_bye_SoundShader"};
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
	class TRADER_NPC_VOICE_male_1_browse_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_browse_SoundShader"};
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
	class TRADER_NPC_VOICE_male_1_no_money_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_no_money_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_empty_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_empty_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_notrade_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_notrade_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_1_bye_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_1_bye_SoundShader"};
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
	class TRADER_NPC_VOICE_male_2_browse_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_browse_SoundShader"};
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
	class TRADER_NPC_VOICE_male_2_no_money_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_no_money_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_sell_1_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_sell_1_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_empty_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_empty_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_notrade_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_notrade_SoundShader"};
		volumeFactor = 1.0;
	};
	class TRADER_NPC_VOICE_male_2_bye_SoundSet: TRADER_NPC_VoiceSet_Base
	{
		soundShaders[] = {"TRADER_NPC_VOICE_male_2_bye_SoundShader"};
		volumeFactor = 1.0;
	};
};
