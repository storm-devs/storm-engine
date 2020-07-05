void CreateOtherCharacters()
{
	ref ch;
	
	////////////////////////////////////////////////////////////////////////////////////////
	/// персонаж для ВАНО
	////////////////////////////////////////////////////////////////////////////////////////
	

	makeref(ch,Characters[BOAT_CHARACTER]);		// boat character  // boal

	ch.name 	= "Моряк";
	ch.lastname = "Моряк";
	ch.id		= "Boat1234";
	ch.nation	= ENGLAND;
	ch.activity_begin = "0";
	ch.activity_end = "24";
	ch.experience = "0";
	ch.Ship.Name = "Лодка";
	ch.Ship.Type = GenerateShip(SHIP_BOAT, 1);
	ch.Ship.Stopped = true;

}
