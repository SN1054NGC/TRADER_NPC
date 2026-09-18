modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
    {
        super.RegisterOneHanded( pType, pBehavior );

        // ============================================================
        // УБИРАЕМ VehicleKeyBase И TM_Wallet (УДАЛЕНЫ)
        // ============================================================
        // pType.AddItemInHandsProfileIK("VehicleKeyBase", ...);
        // pType.AddItemInHandsProfileIK("TM_Wallet", ...);
    }
}