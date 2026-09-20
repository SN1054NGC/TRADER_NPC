// ============================================================
// Клиентская часть ИИ-торговца: сообщение в чате, начинающееся с "!",
// уходит на сервер (RPC_AI_ASK) вместо общего чата, а ответ модели
// приходит строкой в чат (RPC_AI_ANSWER).
// ============================================================
modded class ChatInputMenu
{
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!finished)
			return super.OnChange(w, x, y, finished);

		EditBoxWidget box = EditBoxWidget.Cast(w);
		if (!box)
			return super.OnChange(w, x, y, finished);

		string text = box.GetText();
		if (text.Length() < 2)
			return super.OnChange(w, x, y, finished);

		// обычный чат не трогаем - только сообщения с "!"
		if (text.Get(0) != "!")
			return super.OnChange(w, x, y, finished);

		string question = text.Substring(1, text.Length() - 1);
		question = question.Trim();
		if (question == "")
			return super.OnChange(w, x, y, finished);

		// очищаем поле: ванильная отправка не уйдёт в общий чат,
		// но меню закроется штатно
		box.SetText("");

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			GetGame().RPCSingleParam(player, TRPCs.RPC_AI_ASK, new Param1<string>(question), true);
			g_Game.Chat("Вы -> торговец: " + question, "colorAction");
		}

		return super.OnChange(w, x, y, finished);
	}
}