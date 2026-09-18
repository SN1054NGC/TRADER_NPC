// ============================================================
// ФАЙЛ: TraderKillReward.c
//
// Убийство заражённого даёт игроку:
//   * +1 к счётчику убийств -> вклад в рейтинг выживания (и в скидку);
//   * небольшую выплату = цена 1 шт. самого слабого патрона в конфиге
//     (MissionServer.TraderComputeKillReward, ключ <KillReward> может
//     переопределить значение; деньги "стакаются" в купюры).
//
// Работает только на сервере: смерть заражённых считается серверной логикой.
// Учитывается и стрелок (killer), и владелец источника (машина/турель).
// ============================================================
modded class ZombieBase
{
	override void EEKilled( Object killer )
	{
		super.EEKilled( killer );

		if ( !GetGame().IsServer() )
			return;

		PlayerBase player = PlayerBase.Cast( killer );
		if ( !player )
		{
			EntityAI killerEnt = EntityAI.Cast( killer );
			if ( killerEnt )
				player = PlayerBase.Cast( killerEnt.GetHierarchyRootPlayer() );
		}

		if ( !player || !player.IsAlive() )
			return;

		player.TraderKillReward();
	}
}
