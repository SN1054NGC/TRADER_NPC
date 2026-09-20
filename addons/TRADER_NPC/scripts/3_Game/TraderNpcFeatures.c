// ============================================================
// ФАЙЛ: TraderNpcFeatures.c   (модуль 3_Game — общий)
//
// ЕДИНЫЙ РЕЕСТР ПЕРЕКЛЮЧАТЕЛЕЙ ФУНКЦИЙ TRADER_NPC.
//
// Зачем: (1) владелец может включать/выключать функции конфигом;
//        (2) если функция конфликтует с другим модом — её выключают, и наш
//            `modded class` просто вызывает super (ванильное поведение),
//            то есть патч становится "прозрачным".
//
// Ключи конфига (TraderVariables.txt), по умолчанию все ВКЛЮЧЕНЫ,
// кроме AutoPrices (генерирует файл цен):
//   <FeatureRating>          рейтинг выживания и скидка
//   <FeatureSound>           звук денег/бумаги
//   <FeatureSafezoneTrade>   торговля из сейф-зоны и запреты в сейф-зоне
//   <FeatureMapTools>        будильник/GPS: время и координаты
//   <FeatureHud>             панель рейтинга в самой игре (HUD)
//   <FeatureKillReward>      выплата за убитого заражённого
//   <FeatureAutoPrices>      авто-цены из db/types.xml
//   <FeatureNotifications>   всплывающие уведомления торговца
//
// ВАЖНО: выключение НЕ снимает override (движок не умеет "отпатчивать" уже
// скомпилированный класс), поэтому каждый патч обязан начинаться с проверки
// флага и вызова super - см. примеры в TraderPlaySoundForAll / WeaponManager.
// ============================================================
class TraderNpcFeatures
{
	// серверные флаги (читаются из TraderVariables.txt)
	static bool  s_Rating = true;
	static bool  s_Sound = true;
	static bool  s_SafezoneTrade = true;
	static bool  s_MapTools = true;
	static bool  s_Hud = true;
	static bool  s_KillReward = true;
	static bool  s_AutoPrices = true;
	static bool  s_Notifications = true;

	// Применяет строку вида "<FeatureRating> yes" (ключ уже очищен от комментария)
	static bool Apply(string lineContent)
	{
		if (!lineContent.Contains("<Feature"))
			return false;

		string lower = lineContent;
		lower.ToLower();

		bool on = (lower.Contains("yes") || lower.Contains("true") || lower.Contains("1"));

		if (lineContent.Contains("<FeatureRating>"))        { s_Rating = on;        return true; }
		if (lineContent.Contains("<FeatureSound>"))         { s_Sound = on;         return true; }
		if (lineContent.Contains("<FeatureSafezoneTrade>")) { s_SafezoneTrade = on; return true; }
		if (lineContent.Contains("<FeatureMapTools>"))      { s_MapTools = on;      return true; }
		if (lineContent.Contains("<FeatureHud>"))           { s_Hud = on;           return true; }
		if (lineContent.Contains("<FeatureKillReward>"))    { s_KillReward = on;    return true; }
		if (lineContent.Contains("<FeatureAutoPrices>"))    { s_AutoPrices = on;    return true; }
		if (lineContent.Contains("<FeatureNotifications>")) { s_Notifications = on; return true; }

		return false;
	}

	// Авто-отключение при обнаружении известных конфликтующих модов.
	// Возвращает строку с отчётом (для лога), либо "".
	static string AutoDetectConflicts()
	{
		string report = "";
		if (GetGame().ConfigIsExisting("CfgPatches ExpansionTrader"))
		{
			s_SafezoneTrade = false;
			report = report + " ExpansionTrader";
		}
		if (GetGame().ConfigIsExisting("CfgPatches TraderX"))
		{
			s_KillReward = false;
			report = report + " TraderX";
		}
		return report;
	}

	static string Report()
	{
		string r = "";
		r = r + " rating=" + Bool(Safe(s_Rating));
		r = r + " sound=" + Bool(Safe(s_Sound));
		r = r + " safezoneTrade=" + Bool(Safe(s_SafezoneTrade));
		r = r + " mapTools=" + Bool(Safe(s_MapTools));
		r = r + " hud=" + Bool(Safe(s_Hud));
		r = r + " killReward=" + Bool(Safe(s_KillReward));
		r = r + " autoPrices=" + Bool(Safe(s_AutoPrices));
		r = r + " notifications=" + Bool(Safe(s_Notifications));
		return r;
	}

	static bool Safe(bool v) { return v; }

	static string Bool(bool v)
	{
		if (v)
			return "on";
		return "off";
	}
};
