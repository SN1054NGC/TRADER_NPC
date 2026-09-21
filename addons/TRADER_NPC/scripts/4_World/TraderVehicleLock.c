// ============================================================
// FILE: TraderVehicleLock.c   (module 4_World)
//
// Замок купленной машины БЕЗ ключа-предмета:
//   * владелец хранится по ID игрока (GetPlainId), а не в предмете;
//   * владелец может выдать доступ другим игрокам по нику;
//   * сесть за руль и завести машину может только владелец и допущенные;
//   * данные живут на самой машине и сохраняются движком
//     (OnStoreSave/OnStoreLoad - штатный хук центральной экономики).
//
// Всё на ванильных классах: CarScript, Transport, PlayerIdentity, Serializer.
// ============================================================

class TraderVehicleLock
{
	static const int   STORE_VERSION = 1;
	static const float FIND_RADIUS   = 30.0;

	static bool GetPlayerId(PlayerBase player, out string uid, out string pname)
	{
		uid = "";
		pname = "";
		if (!player)
			return false;
		PlayerIdentity ident = player.GetIdentity();
		if (!ident)
			return false;
		uid = ident.GetPlainId();
		pname = ident.GetName();
		if (uid == "")
			return false;
		return true;
	}

	// может ли этот игрок пользоваться машиной
	static bool CanUse(CarScript car, PlayerBase player)
	{
		if (!car)
			return false;
		if (!car.TraderVehicleIsLocked())
			return true;

		string uid;
		string pname;
		if (!GetPlayerId(player, uid, pname))
			return false;
		if (car.TraderVehicleGetOwnerId() == uid)
			return true;
		return car.TraderVehicleHasAccess(uid);
	}

	// машина, в которой сидит игрок, иначе ближайшая рядом
	static CarScript FindPlayerVehicle(PlayerBase player)
	{
		if (!player)
			return null;

		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			CarScript inside;
			if (Class.CastTo(inside, vehCommand.GetTransport()))
				return inside;
		}

		array<Object> objects = new array<Object>;
		g_Game.GetObjectsAtPosition(player.GetPosition(), FIND_RADIUS, objects, null);

		CarScript nearest = null;
		float best = FIND_RADIUS + 1.0;
		for (int i = 0; i < objects.Count(); i++)
		{
			CarScript car;
			if (!Class.CastTo(car, objects.Get(i)))
				continue;
			float d = vector.Distance(player.GetPosition(), car.GetPosition());
			if (d < best)
			{
				best = d;
				nearest = car;
			}
		}
		return nearest;
	}
};

modded class CarScript
{
	string m_TRV_OwnerId;
	string m_TRV_OwnerName;
	ref array<string> m_TRV_AccessIds;
	ref array<string> m_TRV_AccessNames;
	bool m_TRV_Locked;

	void TraderVehicleEnsureAccess()
	{
		if (!m_TRV_AccessIds)
			m_TRV_AccessIds = new array<string>;
		if (!m_TRV_AccessNames)
			m_TRV_AccessNames = new array<string>;
	}

	bool TraderVehicleIsLocked()
	{
		if (m_TRV_OwnerId == "")
			return false;
		return m_TRV_Locked;
	}

	string TraderVehicleGetOwnerId()
	{
		return m_TRV_OwnerId;
	}

	string TraderVehicleGetOwnerName()
	{
		return m_TRV_OwnerName;
	}

	bool TraderVehicleHasAccess(string uid)
	{
		if (!m_TRV_AccessIds)
			return false;
		if (uid == "")
			return false;
		for (int i = 0; i < m_TRV_AccessIds.Count(); i++)
		{
			if (m_TRV_AccessIds.Get(i) == uid)
				return true;
		}
		return false;
	}

	void TraderVehicleSetOwner(string uid, string pname)
	{
		TraderVehicleEnsureAccess();
		m_TRV_OwnerId = uid;
		m_TRV_OwnerName = pname;
		m_TRV_Locked = true;
	}

	void TraderVehicleSetLocked(bool locked)
	{
		m_TRV_Locked = locked;
	}

	bool TraderVehicleAddAccess(string uid, string pname)
	{
		TraderVehicleEnsureAccess();
		if (uid == "")
			return false;
		if (TraderVehicleHasAccess(uid))
			return false;
		m_TRV_AccessIds.Insert(uid);
		m_TRV_AccessNames.Insert(pname);
		return true;
	}

	bool TraderVehicleRemoveAccess(string uid)
	{
		if (!m_TRV_AccessIds)
			return false;
		for (int i = 0; i < m_TRV_AccessIds.Count(); i++)
		{
			if (m_TRV_AccessIds.Get(i) != uid)
				continue;
			m_TRV_AccessIds.Remove(i);
			if (m_TRV_AccessNames && i < m_TRV_AccessNames.Count())
				m_TRV_AccessNames.Remove(i);
			return true;
		}
		return false;
	}

	int TraderVehicleAccessCount()
	{
		if (!m_TRV_AccessIds)
			return 0;
		return m_TRV_AccessIds.Count();
	}

	string TraderVehicleAccessName(int index)
	{
		if (!m_TRV_AccessNames)
			return "";
		if (index < 0 || index >= m_TRV_AccessNames.Count())
			return "";
		return m_TRV_AccessNames.Get(index);
	}

	string TraderVehicleAccessId(int index)
	{
		if (!m_TRV_AccessIds)
			return "";
		if (index < 0 || index >= m_TRV_AccessIds.Count())
			return "";
		return m_TRV_AccessIds.Get(index);
	}

	override void EEInit()
	{
		super.EEInit();
		TraderVehicleEnsureAccess();
	}

	// ---- владелец сохраняется вместе с машиной (штатный хук движка) ----
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		TraderVehicleEnsureAccess();
		ctx.Write(TraderVehicleLock.STORE_VERSION);
		ctx.Write(m_TRV_OwnerId);
		ctx.Write(m_TRV_OwnerName);
		ctx.Write(m_TRV_Locked);

		int count = m_TRV_AccessIds.Count();
		ctx.Write(count);
		for (int i = 0; i < count; i++)
		{
			ctx.Write(m_TRV_AccessIds.Get(i));
			ctx.Write(m_TRV_AccessNames.Get(i));
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		TraderVehicleEnsureAccess();

		// у машин, купленных до появления замка, нашего блока нет - это не ошибка
		int storeVersion = 0;
		if (!ctx.Read(storeVersion))
			return true;
		if (storeVersion < 1)
			return true;

		ctx.Read(m_TRV_OwnerId);
		ctx.Read(m_TRV_OwnerName);
		ctx.Read(m_TRV_Locked);

		int count = 0;
		ctx.Read(count);
		if (count < 0 || count > 500)
			return true;

		for (int i = 0; i < count; i++)
		{
			string uid = "";
			string pname = "";
			ctx.Read(uid);
			ctx.Read(pname);
			m_TRV_AccessIds.Insert(uid);
			m_TRV_AccessNames.Insert(pname);
		}
		return true;
	}
};
