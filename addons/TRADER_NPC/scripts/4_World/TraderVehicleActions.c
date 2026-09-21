// ============================================================
// FILE: TraderVehicleActions.c   (module 4_World)
//
// Замок купленной машины на действиях ванилы:
//   * ActionGetInTransport - нельзя сесть в чужую запертую машину;
//   * ActionStartEngine    - нельзя завести чужую запертую машину.
//
// Проверка владельца идёт на СЕРВЕРЕ (у клиента нет списка допущенных),
// поэтому клиент показывает действие, а сервер его отклоняет и объясняет
// игроку причину сообщением.
// ============================================================

class TraderVehicleNotify
{
	static void Locked(PlayerBase player, CarScript car)
	{
		if (!player)
			return;

		string owner = car.TraderVehicleGetOwnerName();
		if (owner == "")
			owner = "неизвестен";

		TraderMessage.PlayerRed("Машина заперта на владельца: " + owner, player);
	}
};

modded class ActionGetInTransport
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

#ifdef SERVER
		if (!target)
			return true;

		CarScript car;
		if (!Class.CastTo(car, target.GetObject()))
			return true;

		if (TraderVehicleLock.CanUse(car, player))
			return true;

		TraderVehicleNotify.Locked(player, car);
		return false;
#else
		return true;
#endif
	}
};

modded class ActionStartEngine
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

#ifdef SERVER
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (!vehCommand)
			return true;

		CarScript car;
		if (!Class.CastTo(car, vehCommand.GetTransport()))
			return true;

		if (TraderVehicleLock.CanUse(car, player))
			return true;

		TraderVehicleNotify.Locked(player, car);
		return false;
#else
		return true;
#endif
	}
};
