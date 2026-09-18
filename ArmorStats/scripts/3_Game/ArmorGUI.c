class ArmorGUI
{
    static ref TStringArray m_configData = {"Health", "Shock"};
    static ref array<float> m_stats = {};
    static string m_previousItem = "";
    static const int m_amountOfWidgetsToColor = 1;

    static void GrabStats(string type)
    {
        if (type != m_previousItem)
        {
            m_stats.Clear();
            m_previousItem = type;
            float stat;

            foreach(string param : m_configData)
            {
                m_stats.Insert(GetGame().ConfigGetFloat("CfgVehicles " + type + " DamageSystem GlobalArmor Projectile " + param + " damage"));
            }
        }
    }

    static array<Widget> GrabWidgets(Widget root, TStringArray arr)
    {
        array<Widget> widgets = new array<Widget>;

        foreach (string widName : arr)
        {
            widgets.Insert(root.FindAnyWidget(widName));
        }
        return widgets;
    }
    
    static map<Widget, float> CreateMap(Widget root, TStringArray arr, string type)
    {
        array<Widget> widgets = GrabWidgets(root, arr);
        GrabStats(type);

        map<Widget, float> values = new map<Widget, float>();

        for (int i = 0; i < widgets.Count(); i++)
        {
            if (i <= 1)
            {
                values.Insert(widgets[i], m_stats[i]);
            }
            else
            {
                values.Insert(widgets[i], m_stats[i - 2]);
            }   
        }
        return values;
    }

    static void TypeCastAndSet(Widget inValue, out TextWidget argText, out ProgressBarWidget argBar, int stat = 0)
    {
        if (Class.CastTo(argBar, inValue))
        {
            argBar.SetCurrent(stat);
        }
        else
        {
            Class.CastTo(argText, inValue);
            argText.SetText("" + stat + "%");
        }
    }

    static void SetColors(Widget wid, int value = 0)
    {
        if (value >= 40 && value <= 60 )
        {
            wid.SetColor(ArmorGUIColors.ARMOR_ORANGE);
        }
        else if (value < 40)
        {
            wid.SetColor(ArmorGUIColors.ARMOR_RED);
        }
        else
        {
            wid.SetColor(ArmorGUIColors.ARMOR_GREEN);
        }
    }

    static void ShutUpAndColor(Widget root, TStringArray arg, string type)
    {
        map<Widget, float> values = CreateMap(root, arg, type);

        TextWidget text;
        ProgressBarWidget bar;
        
        for (int i = 0; i < values.Count(); i++)
		{
            Widget key = values.GetKey(i);
            float value = values.GetElement(i);
            
		    if (value >= 0)
		    {
			    int currentStat = 100 - value * 100;
                TypeCastAndSet(key, text, bar, currentStat);
			      
                if (i <= m_amountOfWidgetsToColor)
                {
                    SetColors(key, currentStat);  
                } 
		    }
		    else
		    {
			    TypeCastAndSet(key, text, bar);
                if (i <= m_amountOfWidgetsToColor)
                {
                    SetColors(key);
                }
		    }
		}
    }
}