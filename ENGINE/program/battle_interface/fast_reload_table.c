
#define FRP_STORE			0
#define FRT_STORE			0

#define FRP_SHIPYARD		1
#define FRT_SHIPYARD		0

#define FRP_TAVERN			2
#define FRT_TAVERN			0

#define FRP_RESIDENCE		4
#define FRT_RESIDENCE		0

#define FRP_USURER			3
#define FRT_USURER			0

#define FRP_CHURCH			6
#define FRT_CHURCH			0

#define FRP_BROTHEL			9
#define FRT_BROTHEL			0

#define FRP_PORT_OFFICE		8
#define FRT_PORT_OFFICE		0


void initFastReloadTable()
{
	InitTownTable();

	objFastReloadTable.reloadEnable = true;
	aref curTable;

	makearef(curTable,objFastReloadTable.table.SentJons);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "SentJons_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "SentJons_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "SentJons_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "SentJons_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "SentJons_bank";
	//church
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "SentJons_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "SentJons_PortOffice";

	makearef(curTable,objFastReloadTable.table.Bridgetown);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Bridgetown_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Bridgetown_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Bridgetown_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Bridgetown_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Bridgetown_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Bridgetown_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "Bridgetown_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "Bridgetown_PortOffice";

	makearef(curTable,objFastReloadTable.table.PortRoyal);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "PortRoyal_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "PortRoyal_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "PortRoyal_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "PortRoyal_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "PortRoyal_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "PortRoyal_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "PortRoyal_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "PortRoyal_PortOffice";

	makearef(curTable,objFastReloadTable.table.FortOrange);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "FortOrange_store";
	// tavern
	curTable.l2.pic = FRP_TAVERN;
	curTable.l2.tex = FRT_TAVERN;
	curTable.l2.location = "FortOrange_tavern";
	// residence
	curTable.l3.pic = FRP_RESIDENCE;
	curTable.l3.tex = FRT_RESIDENCE;
	curTable.l3.location = "FortOrange_townhall";

	makearef(curTable,objFastReloadTable.table.Pirates);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Pirates_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Pirates_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Pirates_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Pirates_townhall";

	makearef(curTable,objFastReloadTable.table.Villemstad);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Villemstad_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Villemstad_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Villemstad_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Villemstad_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Villemstad_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Villemstad_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Villemstad_PortOffice";

	makearef(curTable,objFastReloadTable.table.BasTer);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "BasTer_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "BasTer_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "BasTer_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "BasTer_townhall";
	// church
	curTable.l5.pic = FRP_CHURCH;
	curTable.l5.tex = FRT_CHURCH;
	curTable.l5.location = "BasTer_church";
	// usurer
	curTable.l6.pic = FRP_USURER;
	curTable.l6.tex = FRT_USURER;
	curTable.l6.location = "BasTer_bank";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "BasTer_PortOffice";

	makearef(curTable,objFastReloadTable.table.Santiago);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Santiago_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Santiago_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Santiago_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Santiago_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Santiago_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Santiago_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Santiago_PortOffice";

 	makearef(curTable,objFastReloadTable.table.PuertoPrincipe);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "PuertoPrincipe_store";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "PuertoPrincipe_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "PuertoPrincipe_townhall";

	makearef(curTable,objFastReloadTable.table.LaVega);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "LaVega_store";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "LaVega_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "LaVega_townhall";

	makearef(curTable,objFastReloadTable.table.FortFrance);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "FortFrance_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "FortFrance_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "FortFrance_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "FortFrance_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "FortFrance_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "FortFrance_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "FortFrance_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "FortFrance_PortOffice";

	makearef(curTable,objFastReloadTable.table.LeFransua);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "LeFransua_store";
	// tavern
	curTable.l2.pic = FRP_TAVERN;
	curTable.l2.tex = FRT_TAVERN;
	curTable.l2.location = "LeFransua_tavern";
	// residence
	curTable.l3.pic = FRP_RESIDENCE;
	curTable.l3.tex = FRT_RESIDENCE;
	curTable.l3.location = "LeFransua_townhall";

	makearef(curTable,objFastReloadTable.table.Havana);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Havana_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Havana_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Havana_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Havana_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Havana_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Havana_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Havana_PortOffice";
	
	makearef(curTable,objFastReloadTable.table.Charles);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Charles_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Charles_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Charles_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Charles_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Charles_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Charles_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "Charles_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "Charles_PortOffice";

	makearef(curTable,objFastReloadTable.table.PortPax);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "PortPax_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "PortPax_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "PortPax_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "PortPax_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "PortPax_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "PortPax_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "PortPax_PortOffice";

	makearef(curTable,objFastReloadTable.table.SanJuan);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "SanJuan_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "SanJuan_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "SanJuan_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "SanJuan_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "SanJuan_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "SanJuan_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "SanJuan_PortOffice";

	makearef(curTable,objFastReloadTable.table.SantoDomingo);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "SantoDomingo_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "SantoDomingo_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "SantoDomingo_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "SantoDomingo_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "SantoDomingo_Bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "SantoDomingo_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "SantoDomingo_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "SantoDomingo_PortOffice";

	makearef(curTable,objFastReloadTable.table.PortSpein);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "PortSpein_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "PortSpein_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "PortSpein_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "PortSpein_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "PortSpein_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "PortSpein_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "PortSpein_PortOffice";

	makearef(curTable,objFastReloadTable.table.Marigo);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Marigo_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Marigo_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Marigo_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Marigo_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Marigo_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Marigo_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "Marigo_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "Marigo_PortOffice";

	makearef(curTable,objFastReloadTable.table.Tortuga);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Tortuga_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Tortuga_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Tortuga_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Tortuga_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Tortuga_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Tortuga_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "Tortuga_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "Tortuga_PortOffice";

	makearef(curTable,objFastReloadTable.table.Panama);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Panama_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Panama_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Panama_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Panama_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Panama_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Panama_church";
	// brothel
	curTable.l7.pic = FRP_BROTHEL;
	curTable.l7.tex = FRT_BROTHEL;
	curTable.l7.location = "Panama_brothel";

	curTable.l8.pic = FRP_PORT_OFFICE;
	curTable.l8.tex = FRT_PORT_OFFICE;
	curTable.l8.location = "Panama_PortOffice";

	makearef(curTable,objFastReloadTable.table.PortoBello);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "PortoBello_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "PortoBello_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "PortoBello_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "PortoBello_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "PortoBello_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "PortoBello_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "PortoBello_PortOffice";
	
	makearef(curTable,objFastReloadTable.table.Cartahena);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Cartahena_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Cartahena_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Cartahena_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Cartahena_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Cartahena_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Cartahena_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Cartahena_PortOffice";

	makearef(curTable,objFastReloadTable.table.Maracaibo);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Maracaibo_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Maracaibo_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Maracaibo_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Maracaibo_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Maracaibo_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Maracaibo_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Maracaibo_PortOffice";
	
	makearef(curTable,objFastReloadTable.table.Caracas);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Caracas_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Caracas_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Caracas_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Caracas_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Caracas_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Caracas_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Caracas_PortOffice";

	makearef(curTable,objFastReloadTable.table.Cumana);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Cumana_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Cumana_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Cumana_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Cumana_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Cumana_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Cumana_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Cumana_PortOffice";

	makearef(curTable,objFastReloadTable.table.SantaCatalina);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "SantaCatalina_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "SantaCatalina_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "SantaCatalina_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "SantaCatalina_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "SantaCatalina_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "SantaCatalina_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "SantaCatalina_PortOffice";
	
	makearef(curTable,objFastReloadTable.table.Beliz);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "Beliz_store";
	// shipyard
	curTable.l2.pic = FRP_SHIPYARD;
	curTable.l2.tex = FRT_SHIPYARD;
	curTable.l2.location = "Beliz_shipyard";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "Beliz_tavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "Beliz_townhall";
	// usurer
	curTable.l5.pic = FRP_USURER;
	curTable.l5.tex = FRT_USURER;
	curTable.l5.location = "Beliz_bank";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "Beliz_church";

	curTable.l7.pic = FRP_PORT_OFFICE;
	curTable.l7.tex = FRT_PORT_OFFICE;
	curTable.l7.location = "Beliz_PortOffice";

	makearef(curTable,objFastReloadTable.table.LostShipsCity);
	// store
	curTable.l1.pic = FRP_STORE;
	curTable.l1.tex = FRT_STORE;
	curTable.l1.location = "EsmeraldaStoreBig";
	// tavern
	curTable.l3.pic = FRP_TAVERN;
	curTable.l3.tex = FRT_TAVERN;
	curTable.l3.location = "FleuronTavern";
	// residence
	curTable.l4.pic = FRP_RESIDENCE;
	curTable.l4.tex = FRT_RESIDENCE;
	curTable.l4.location = "SanAugustineResidence";
	// chucrh
	curTable.l6.pic = FRP_CHURCH;
	curTable.l6.tex = FRT_CHURCH;
	curTable.l6.location = "GloriaChurch";
}

void InitTownTable()
{
	//loc.townsack
	objTownStateTable.towns.t1.name = "SentJons";
	objTownStateTable.towns.t1.captured = false;
	objTownStateTable.towns.t1.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t1.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t1.crew.data.year = 1;
	objTownStateTable.towns.t1.crew.data.month = 1;
	objTownStateTable.towns.t1.crew.data.day = 1;

	objTownStateTable.towns.t2a.name = "Bridgetown";
	objTownStateTable.towns.t2a.captured = false;
	objTownStateTable.towns.t2a.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t2a.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t2a.crew.data.year = 1;
	objTownStateTable.towns.t2a.crew.data.month = 1;
	objTownStateTable.towns.t2a.crew.data.day = 1;

	objTownStateTable.towns.t2.name = "PortRoyal";
	objTownStateTable.towns.t2.captured = false;
	objTownStateTable.towns.t2.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t2.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t2.crew.data.year = 1;
	objTownStateTable.towns.t2.crew.data.month = 1;
	objTownStateTable.towns.t2.crew.data.day = 1;

	objTownStateTable.towns.t3.name = "Villemstad";
	objTownStateTable.towns.t3.captured = false;
	objTownStateTable.towns.t3.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t3.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t3.crew.data.year = 1;
	objTownStateTable.towns.t3.crew.data.month = 1;
	objTownStateTable.towns.t3.crew.data.day = 1;

	objTownStateTable.towns.t4.name = "BasTer";
	objTownStateTable.towns.t4.captured = false;
	objTownStateTable.towns.t4.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t4.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t4.crew.data.year = 1;
	objTownStateTable.towns.t4.crew.data.month = 1;
	objTownStateTable.towns.t4.crew.data.day = 1;

	objTownStateTable.towns.t5.name = "Pirates";
	objTownStateTable.towns.t5.captured = false;
	objTownStateTable.towns.t5.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t5.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t5.crew.data.year = 1;
	objTownStateTable.towns.t5.crew.data.month = 1;
	objTownStateTable.towns.t5.crew.data.day = 1;
	
	objTownStateTable.towns.t6.name = "Santiago";
	objTownStateTable.towns.t6.captured = false;
	objTownStateTable.towns.t6.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t6.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t6.crew.data.year = 1;
	objTownStateTable.towns.t6.crew.data.month = 1;
	objTownStateTable.towns.t6.crew.data.day = 1;
	
	objTownStateTable.towns.t7.name = "LaVega";
	objTownStateTable.towns.t7.captured = false;
	objTownStateTable.towns.t7.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t7.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t7.crew.data.year = 1;
	objTownStateTable.towns.t7.crew.data.month = 1;
	objTownStateTable.towns.t7.crew.data.day = 1;

	objTownStateTable.towns.t8.name = "PuertoPrincipe";
	objTownStateTable.towns.t8.captured = false;
	objTownStateTable.towns.t8.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t8.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t8.crew.data.year = 1;
	objTownStateTable.towns.t8.crew.data.month = 1;
	objTownStateTable.towns.t8.crew.data.day = 1;
	
	objTownStateTable.towns.t9.name = "FortFrance";
	objTownStateTable.towns.t9.captured = false;
	objTownStateTable.towns.t9.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t9.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t9.crew.data.year = 1;
	objTownStateTable.towns.t9.crew.data.month = 1;
	objTownStateTable.towns.t9.crew.data.day = 1;
	
	objTownStateTable.towns.t10.name = "LeFransua";
	objTownStateTable.towns.t10.captured = false;
	objTownStateTable.towns.t10.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t10.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t10.crew.data.year = 1;
	objTownStateTable.towns.t10.crew.data.month = 1;
	objTownStateTable.towns.t10.crew.data.day = 1;

	objTownStateTable.towns.t11.name = "Havana";
	objTownStateTable.towns.t11.captured = false;
	objTownStateTable.towns.t11.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t11.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t11.crew.data.year = 1;
	objTownStateTable.towns.t11.crew.data.month = 1;
	objTownStateTable.towns.t11.crew.data.day = 1;
	
	objTownStateTable.towns.t12.name = "Charles";
	objTownStateTable.towns.t12.captured = false;
	objTownStateTable.towns.t12.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t12.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t12.crew.data.year = 1;
	objTownStateTable.towns.t12.crew.data.month = 1;
	objTownStateTable.towns.t12.crew.data.day = 1;
	
	objTownStateTable.towns.t13.name = "PortPax";
	objTownStateTable.towns.t13.captured = false;
	objTownStateTable.towns.t13.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t13.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t13.crew.data.year = 1;
	objTownStateTable.towns.t13.crew.data.month = 1;
	objTownStateTable.towns.t13.crew.data.day = 1;
	
	objTownStateTable.towns.t14.name = "SanJuan";
	objTownStateTable.towns.t14.captured = false;
	objTownStateTable.towns.t14.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t14.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t14.crew.data.year = 1;
	objTownStateTable.towns.t14.crew.data.month = 1;
	objTownStateTable.towns.t14.crew.data.day = 1;
	
	objTownStateTable.towns.t15.name = "SantoDomingo";
	objTownStateTable.towns.t15.captured = false;
	objTownStateTable.towns.t15.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t15.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t15.crew.data.year = 1;
	objTownStateTable.towns.t15.crew.data.month = 1;
	objTownStateTable.towns.t15.crew.data.day = 1;

	objTownStateTable.towns.t17.name = "PortSpein";
	objTownStateTable.towns.t17.captured = false;
	objTownStateTable.towns.t17.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t17.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t17.crew.data.year = 1;
	objTownStateTable.towns.t17.crew.data.month = 1;
	objTownStateTable.towns.t17.crew.data.day = 1;

	objTownStateTable.towns.t18.name = "Marigo";
	objTownStateTable.towns.t18.captured = false;
	objTownStateTable.towns.t18.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t18.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t18.crew.data.year = 1;
	objTownStateTable.towns.t18.crew.data.month = 1;
	objTownStateTable.towns.t18.crew.data.day = 1;

	objTownStateTable.towns.t19.name = "Tortuga";
	objTownStateTable.towns.t19.captured = false;
	objTownStateTable.towns.t19.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t19.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t19.crew.data.year = 1;
	objTownStateTable.towns.t19.crew.data.month = 1;
	objTownStateTable.towns.t19.crew.data.day = 1;

	objTownStateTable.towns.t20.name = "FortOrange";
	objTownStateTable.towns.t20.captured = false;
	objTownStateTable.towns.t20.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t20.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t20.crew.data.year = 1;
	objTownStateTable.towns.t20.crew.data.month = 1;
	objTownStateTable.towns.t20.crew.data.day = 1;

	objTownStateTable.towns.t21.name = "Panama";
	objTownStateTable.towns.t21.captured = false;
	objTownStateTable.towns.t21.crew.quantity = 200 + Rand(300);
	objTownStateTable.towns.t21.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t21.crew.data.year = 1;
	objTownStateTable.towns.t21.crew.data.month = 1;
	objTownStateTable.towns.t21.crew.data.day = 1;

	objTownStateTable.towns.t22.name = "PortoBello";
	objTownStateTable.towns.t22.captured = false;
	objTownStateTable.towns.t22.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t22.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t22.crew.data.year = 1;
	objTownStateTable.towns.t22.crew.data.month = 1;
	objTownStateTable.towns.t22.crew.data.day = 1;

	objTownStateTable.towns.t23.name = "Cartahena";
	objTownStateTable.towns.t23.captured = false;
	objTownStateTable.towns.t23.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t23.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t23.crew.data.year = 1;
	objTownStateTable.towns.t23.crew.data.month = 1;
	objTownStateTable.towns.t23.crew.data.day = 1;
	
	objTownStateTable.towns.t24.name = "Maracaibo";
	objTownStateTable.towns.t24.captured = false;
	objTownStateTable.towns.t24.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t24.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t24.crew.data.year = 1;
	objTownStateTable.towns.t24.crew.data.month = 1;
	objTownStateTable.towns.t24.crew.data.day = 1;

	objTownStateTable.towns.t25.name = "Caracas";
	objTownStateTable.towns.t25.captured = false;
	objTownStateTable.towns.t25.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t25.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t25.crew.data.year = 1;
	objTownStateTable.towns.t25.crew.data.month = 1;
	objTownStateTable.towns.t25.crew.data.day = 1;

	objTownStateTable.towns.t26.name = "Cumana";
	objTownStateTable.towns.t26.captured = false;
	objTownStateTable.towns.t26.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t26.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t26.crew.data.year = 1;
	objTownStateTable.towns.t26.crew.data.month = 1;
	objTownStateTable.towns.t26.crew.data.day = 1;

	objTownStateTable.towns.t27.name = "SantaCatalina";
	objTownStateTable.towns.t27.captured = false;
	objTownStateTable.towns.t27.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t27.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t27.crew.data.year = 1;
	objTownStateTable.towns.t27.crew.data.month = 1;
	objTownStateTable.towns.t27.crew.data.day = 1;

	objTownStateTable.towns.t28.name = "Beliz";
	objTownStateTable.towns.t28.captured = false;
	objTownStateTable.towns.t28.crew.quantity = 100 + Rand(100);
	objTownStateTable.towns.t28.crew.morale = MORALE_NORMAL - 20 + Rand(40);
	objTownStateTable.towns.t28.crew.data.year = 1;
	objTownStateTable.towns.t28.crew.data.month = 1;
	objTownStateTable.towns.t28.crew.data.day = 1;
}