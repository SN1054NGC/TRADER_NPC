modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        
        // ТОЛЬКО ТОРГОВЛЯ
        actions.Insert(ActionTrade);
    }
}