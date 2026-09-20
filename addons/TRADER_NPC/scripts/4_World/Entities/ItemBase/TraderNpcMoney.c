// ============================================================
// Деньги TRADER_NPC - ванильная бумага, которая складывается в пачку
// (config.cpp: CfgVehicles TraderNpcMoney, varQuantityMax=99999).
//
// Здесь только поведение: у денег не должно быть действий бумаги
// "разжечь костёр/печь", иначе пачку денег можно было бы сжечь.
// ============================================================
class TraderNpcMoney extends Paper
{
	override void SetActions()
	{
		super.SetActions();

		RemoveAction(ActionCreateIndoorFireplace);
		RemoveAction(ActionCreateIndoorOven);
	}
}