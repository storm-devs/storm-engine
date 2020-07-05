// запускается сразу после начала абордажа
void QuestAbordageStartEvent()
{
	int iCharacter1 = GetEventData();
	int iCharacter2 = GetEventData();

	int iTestCharacter = -2;//GetCharacterIndex("Pirate Captain 04");

	if (iCharacter1 == iTestCharacter || iCharacter2 == iTestCharacter)
	{
		//Characters[GetCharacterIndex("Thomas O'Reily")].quest.contraband = "boarding";
		AddCharacterGoods(GetMainCharacter(), GOOD_SANDAL, 50);
	}
}
