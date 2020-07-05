
void SetNames()
{
	for(int i = 1; i <MAX_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "name") || Characters[i].name == "none")
		{
			SetRandomNameToCharacter(&Characters[i]);
		}
	}
}

void InitTowns()
{	
	Towns[0].nation = FRANCE;
	Towns[0].name = "Париж";
	
	Towns[1].nation = FRANCE;
	Towns[1].name = "Орлеан";
	
	Towns[2].nation = FRANCE;
	Towns[2].name = "Байё";
	
	Towns[3].nation = FRANCE;
	Towns[3].name = "Амьен";
	
	Towns[4].nation = FRANCE;
	Towns[4].name = "Шартр";
	
	Towns[5].nation = FRANCE;
	Towns[5].name = "Шартр";
	
	Towns[6].nation = FRANCE;
	Towns[6].name = "Эгморт";
	
	Towns[7].nation = FRANCE;
	Towns[7].name = "Ля Рошель";
	
	Towns[8].nation = FRANCE;
	Towns[8].name = "Лион";
	
	Towns[9].nation = FRANCE;
	Towns[9].name = "Эг-Морт";
	
	Towns[10].nation = ENGLAND;
	Towns[10].name = "Лондон";
	
	Towns[11].nation = ENGLAND;
	Towns[11].name = "Йорк";
	
	Towns[12].nation = ENGLAND;
	Towns[12].name = "Честер";
	
	Towns[13].nation = ENGLAND;
	Towns[13].name = "Глостер";
	
	Towns[14].nation = ENGLAND;
	Towns[14].name = "Солсбери";
	
	Towns[15].nation = ENGLAND;
	Towns[15].name = "Вифлеем";
	
	Towns[16].nation = ENGLAND;
	Towns[16].name = "Истбери";
	
	Towns[17].nation = ENGLAND;
	Towns[17].name = "Уэльс";
	
	Towns[18].nation = ENGLAND;
	Towns[18].name = "Оксфорд";
	
	Towns[19].nation = ENGLAND;
	Towns[19].name = "Оксфорд";
	
	Towns[20].nation = SPAIN;
	Towns[20].name = "Толедо";
	
	Towns[21].nation = SPAIN;
	Towns[21].name = "Мадрид";
	
	Towns[22].nation = SPAIN;
	Towns[22].name = "Барселона";
	
	Towns[23].nation = SPAIN;
	Towns[23].name = "Жирон";
	
	Towns[24].nation = SPAIN;
	Towns[24].name = "Альтея";
	
	Towns[25].nation = SPAIN;
	Towns[25].name = "Севилья";
	
	Towns[26].nation = SPAIN;
	Towns[26].name = "Сарагоса";
	
	Towns[27].nation = SPAIN;
	Towns[27].name = "Кордова";
	
	Towns[28].nation = SPAIN;
	Towns[28].name = "Валенсия";
	
	Towns[29].nation = SPAIN;
	Towns[29].name = "Гибралтар";
		
	Towns[30].nation = HOLLAND;
	Towns[30].name = "Лиссабон";
	
	Towns[31].nation = HOLLAND;
	Towns[31].name = "Порту";
	
	Towns[32].nation = HOLLAND;
	Towns[32].name = "Синтра";
	
	Towns[33].nation = HOLLAND;
	Towns[33].name = "Портимау";
	
	Towns[34].nation = HOLLAND;
	Towns[34].name = "Брага";
	
	Towns[35].nation = HOLLAND;
	Towns[35].name = "Коимбра";
	
	Towns[36].nation = HOLLAND;
	Towns[36].name = "Эвора";
	
	Towns[37].nation = HOLLAND;
	Towns[37].name = "Мадейра";
	
	Towns[38].nation = HOLLAND;
	Towns[38].name = "Шавеш";
	
	Towns[39].nation = HOLLAND;
	Towns[39].name = "Обидуш";
}
