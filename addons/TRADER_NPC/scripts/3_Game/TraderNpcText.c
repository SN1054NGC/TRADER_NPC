// ============================================================
// ФАЙЛ: TraderNpcText.c   (модуль 3_Game — общий для сервера и клиента)
//
// Собственный текстовый помощник TRADER_NPC для разбора конфигов
// (TraderNpcVariables.txt / TraderNpcObjects.txt / TraderNpcConfig*.txt).
// Используются только ванильные средства: FGets() для чтения строк и
// методы класса string. Кода сторонних модов здесь нет.
//
// Отличие от прежнего самодельного хелпера авторa: "//" считается
// комментарием, только если это не часть схемы URL ("://"), поэтому
// адреса вида http://127.0.0.1:8080 больше не обрезаются.
// ============================================================
class TraderNpcText
{
	// Читает файл до строки, содержащей searchTerm (или abortTerm). Пустая строка = конец файла.
	static string NextTerm(FileHandle f, string searchTerm, string abortTerm)
	{
		while (true)
		{
			string line = "";
			if (FGets(f, line) == -1)
				break;

			line = Clean(line);

			if (line.Contains(searchTerm))
				return line;

			if (abortTerm != "" && line.Contains(abortTerm))
				return line;
		}

		return string.Empty;
	}

	// То же, но ищет любой терм из набора.
	static string NextTerms(FileHandle f, array<string> searchTerms, string abortTerm)
	{
		while (true)
		{
			string line = "";
			if (FGets(f, line) == -1)
				break;

			line = Clean(line);

			if (abortTerm != "" && line.Contains(abortTerm))
				return line;

			for (int i = 0; i < searchTerms.Count(); i++)
			{
				if (line.Contains(searchTerms.Get(i)))
					return line;
			}
		}

		return string.Empty;
	}

	// Отрезает комментарий "//" (кроме "://") и убирает табы/пробелы по краям.
	static string Clean(string line)
	{
		int cut = -1;
		int len = line.Length();

		for (int i = 0; i + 1 < len; i++)
		{
			if (line.Get(i) != "/")
				continue;
			if (line.Get(i + 1) != "/")
				continue;
			if (i > 0 && line.Get(i - 1) == ":")
				continue;

			cut = i;
			break;
		}

		if (cut >= 0)
			line = line.Substring(0, cut);

		return Tidy(line);
	}

	// Убирает табы (в любом месте) и пробелы по краям.
	static string Tidy(string line)
	{
		line.Replace("	", "");
		return line.Trim();
	}
};
