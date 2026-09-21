// ============================================================
// FILE: TraderVehicleShop.c   (module 4_World)
//
// Покупка машины у торговца: вместо предмета в инвентарь создаётся
// сама машина на точке выезда торговца, на неё вешаются запчасти из
// TraderNpcVehicleParts.txt, после чего она запирается на ID покупателя.
//
// Коды строк в TraderNpcConfig.txt:
//   VNK -> -6  машина без ключа (наш основной вариант: ключа-предмета нет)
//   V   -> -7  машина с ключом (ванильного ключа нет, выдаём так же - с замком)
//
// Только ванильные средства: CreateObjectEx, CarScript, GetInventory().
// ============================================================

class TraderVehicleShop
{
	static const int ROW_VEHICLE_NOKEY = -6;
	static const int ROW_VEHICLE_KEY   = -7;

	static const string PARTS_FILE = "$profile:Trader_NPC_Prof/TraderNpcVehicleParts.txt";

	static bool IsVehicleRow(int quantity)
	{
		if (quantity == ROW_VEHICLE_NOKEY)
			return true;
		if (quantity == ROW_VEHICLE_KEY)
			return true;
		return false;
	}

	// точка выезда этого торговца; "0 0 0" -> перед игроком
	static vector ResolveSpawn(PlayerBase player, int traderIndex, out vector orientation)
	{
		orientation = "0 0 0";

		if (player.m_Trader_TraderVehicleSpawns && traderIndex >= 0 && traderIndex < player.m_Trader_TraderVehicleSpawns.Count())
			orientation = player.m_Trader_TraderVehicleSpawnsOrientation.Get(traderIndex);

		vector pos = "0 0 0";
		if (player.m_Trader_TraderVehicleSpawns && traderIndex >= 0 && traderIndex < player.m_Trader_TraderVehicleSpawns.Count())
			pos = player.m_Trader_TraderVehicleSpawns.Get(traderIndex);

		if (pos[0] != 0 || pos[1] != 0 || pos[2] != 0)
			return pos;

		// точка не задана в TraderNpcObjects.txt - ставим машину перед игроком
		vector dir = player.GetDirection();
		pos = player.GetPosition() + (dir * 10.0);
		TraderMessage.ServerLog("[TRADER] vehicle spawn not configured, falling back to player front " + pos);
		return pos;
	}

	// вешаем запчасти, перечисленные для этого класса в TraderNpcVehicleParts.txt
	static int AttachParts(CarScript car, string className)
	{
		if (!car)
			return 0;

		FileHandle fh = OpenFile(PARTS_FILE, FileMode.READ);
		if (fh == 0)
		{
			TraderMessage.ServerLog("[TRADER] vehicle parts file not found: " + PARTS_FILE);
			return 0;
		}

		int attached = 0;
		bool inSection = false;
		string line = "";

		while (FGets(fh, line) != -1)
		{
			line = TraderNpcText.Clean(line);
			if (line == "")
				continue;

			if (line.Contains("<VehicleParts>"))
			{
				line.Replace("<VehicleParts>", "");
				string section = TraderNpcText.Tidy(line);
				inSection = (section == className);
				continue;
			}

			if (!inSection)
				continue;

			if (line.Contains("<FileEnd>") || line.Contains("<OpenFile>"))
				break;

			string part = TraderNpcText.Tidy(line);
			if (part == "")
				continue;

			EntityAI attachedItem = car.GetInventory().CreateAttachment(part);
			if (!attachedItem)
			{
				TraderMessage.ServerLog("[TRADER] vehicle part failed: " + className + " <- " + part);
				continue;
			}
			attached++;
		}

		CloseFile(fh);
		return attached;
	}

	// строка конфига, по которой какой-нибудь торговец скупает этот класс.
	// -1 = никто не скупает.
	static int FindSellRow(PlayerBase player, string className)
	{
		if (!player.m_Trader_ItemsClassnames || !player.m_Trader_ItemsSellValue)
			return -1;
		for (int i = 0; i < player.m_Trader_ItemsClassnames.Count(); i++)
		{
			if (player.m_Trader_ItemsClassnames.Get(i) != className)
				continue;
			if (player.m_Trader_ItemsSellValue.Get(i) >= 0)
				return i;
		}
		return -1;
	}

	// цена продажи с учётом состояния: здоровье машины плюс небольшая надбавка
	// за топливо в баке. Ржавую машину скупают за минимум 10% цены строки.
	static int ComputeSellPrice(PlayerBase player, int row, CarScript car)
	{
		int base = player.m_Trader_ItemsSellValue.Get(row);
		if (base < 1)
			return 0;

		float condition = car.GetHealth01();
		if (condition < 0.1)
			condition = 0.1;

		float coef = condition + (car.GetFluidFraction(CarFluid.FUEL) * 0.05);
		if (coef > 1.0)
			coef = 1.0;

		int price = Math.Round(base * coef);
		if (price < 1)
			price = 1;
		return price;
	}

	// полный бак + охлаждение + заряженный АКБ (ванильные Fill / SetEnergy0To1).
	// Нужно и при выдаче машины, и для команды !кар service.
	static void Service(CarScript car)
	{
		if (!car)
			return;

		car.Fill(CarFluid.FUEL, car.GetFluidCapacity(CarFluid.FUEL));
		car.Fill(CarFluid.COOLANT, car.GetFluidCapacity(CarFluid.COOLANT));

		ItemBase battery = car.GetBattery();
		if (!battery)
			return;

		ComponentEnergyManager em = battery.GetCompEM();
		if (!em)
			return;

		em.SetEnergy0To1(1.0);
	}

	// выдаёт машину покупателю. false = не выдали, деньги списывать нельзя
	static bool Deliver(PlayerBase player, string className, int traderIndex)
	{
#ifdef SERVER
		if (!player)
			return false;

		vector orientation;
		vector spawnPos = ResolveSpawn(player, traderIndex, orientation);

		// ECE_PLACE_ON_SURFACE = CREATEPHYSICS|UPDATEPATHGRAPH|TRACE: машина садится на грунт.
		// Без ECE_DYNAMIC_PERSISTENCY объект сохраняется центральной экономикой,
		// ECE_NOLIFETIME - машина не исчезает по таймеру.
		Object obj = GetGame().CreateObjectEx(className, spawnPos, ECE_SETUP | ECE_PLACE_ON_SURFACE | ECE_NOLIFETIME, RF_IGNORE);
		if (!obj)
		{
			TraderMessage.ServerLog("[TRADER][ERROR] vehicle spawn failed: " + className + " at " + spawnPos);
			return false;
		}

		CarScript car;
		if (!Class.CastTo(car, obj))
		{
			GetGame().ObjectDelete(obj);
			TraderMessage.ServerLog("[TRADER][ERROR] not a vehicle class: " + className);
			return false;
		}

		car.SetPosition(spawnPos);
		car.SetOrientation(orientation);

		int parts = AttachParts(car, className);

		// Только что созданная машина приходит с пустым баком и разряженным АКБ:
		// без этого двигатель не заводится.
		Service(car);

		TraderMessage.ServerLog("[TRADER] vehicle ready: parts=" + parts + " fuel=" + car.GetFluidFraction(CarFluid.FUEL));

		string uid;
		string pname;
		if (TraderVehicleLock.GetPlayerId(player, uid, pname))
			car.TraderVehicleSetOwner(uid, pname);
		car.TraderVehicleSetLocked(true);

		TraderMessage.ServerLog("[TRADER] VEHICLE delivered: " + className + " to " + pname + " (" + uid + ") pos=" + car.GetPosition() + " parts=" + parts);
		TraderMessage.TradesLog("bought vehicle " + className + " pos=" + car.GetPosition());
		return true;
#else
		return false;
#endif
	}
};
