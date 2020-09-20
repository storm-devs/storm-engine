
//===========================================================================================
//			RULE:
//	pStore.StoreSize	- размер магазина ("large" или "small") - влияет на кол-во товара,
//						по умолчанию берется равным "large".
//	pStore.Colony		- id острова к которому принадлежит магазин.
//	pStore.Goods.имя_товара.NotUsed		- устанавливается в TRUE для тех товаров, которыми
//										нельзя торговать в данном магазине.
//										(по умолчанию false).
//===========================================================================================
void StoreInit()
{
	ref pStore;

	// Barbados (Bridgetown)
	Stores[Bridgetown_STORE].Colony	= "Bridgetown";
	Stores[Bridgetown_STORE].group	= "Bridgetown";
	Stores[Bridgetown_STORE].StoreSize = "large";

	// PortRoyal (Jamaica)
	Stores[PortRoyal_STORE].Colony	= "PortRoyal";
	Stores[PortRoyal_STORE].group	= "PortRoyal";
	Stores[PortRoyal_STORE].StoreSize = "large";

	// FortOrange (Jamaica)
	Stores[FortOrange_STORE].Colony	= "FortOrange";
	Stores[FortOrange_STORE].group	= "FortOrange";
	Stores[FortOrange_STORE].StoreSize = "small";
	
	// PuertoRico
	Stores[SanJuan_STORE].Colony = "SanJuan";
	Stores[SanJuan_STORE].group	= "SanJuan";
	Stores[SanJuan_STORE].StoreSize = "large";

	// Santiago
	Stores[Santiago_STORE].Colony	= "Santiago";
	Stores[Santiago_STORE].group	= "Santiago";
	Stores[Santiago_STORE].StoreSize = "large";

	// PuertoPrincipe
	Stores[PuertoPrincipe_STORE].Colony	= "PuertoPrincipe";
	Stores[PuertoPrincipe_STORE].group	= "PuertoPrincipe";
	Stores[PuertoPrincipe_STORE].StoreSize = "small";

	// Curacao (Villemstad)
	Stores[Villemstad_STORE].Colony	= "Villemstad";
	Stores[Villemstad_STORE].Group	= "Villemstad";
	Stores[Villemstad_STORE].StoreSize = "large";

	// Tortuga
	Stores[Tortuga_STORE].Colony	= "Tortuga";
	Stores[Tortuga_STORE].Group	= "Tortuga";	
	Stores[Tortuga_STORE].StoreSize = "large";

	// SentMartin (Marigo)
	Stores[Marigo_STORE].Colony	= "Marigo";
	Stores[Marigo_STORE].Group	= "Marigo";
	Stores[Marigo_STORE].StoreSize = "large";
	
	// PortSpein  (Trinidad)
	Stores[PortSpein_STORE].Colony	= "PortSpein";
	Stores[PortSpein_STORE].group	= "PortSpein";
	Stores[PortSpein_STORE].StoreSize = "large";
	
	// Charles  (Nevis)
	Stores[Charles_STORE].Colony	= "Charles";
	Stores[Charles_STORE].group	= "Charles";
	Stores[Charles_STORE].StoreSize = "large";

	// StEustatius
	Stores[PortoBello_STORE].Colony	= "PortoBello";
	Stores[PortoBello_STORE].group	= "PortoBello";
	Stores[PortoBello_STORE].StoreSize = "large";

	// SentJons (Antigua)
	Stores[SentJons_STORE].Colony	= "SentJons";
	Stores[SentJons_STORE].group	= "SentJons";
	Stores[SentJons_STORE].StoreSize = "large";

	// Havana
	Stores[Havana_STORE].Colony	= "Havana";
	Stores[Havana_STORE].group	= "Havana";
	Stores[Havana_STORE].StoreSize = "large";
	
	// BasTer
	Stores[BasTer_STORE].Colony	= "BasTer";
	Stores[BasTer_STORE].group	= "BasTer";
	Stores[BasTer_STORE].StoreSize = "large";
	
	// FortFrance (Martinique)
	Stores[FortFrance_STORE].Colony	= "FortFrance";
	Stores[FortFrance_STORE].group	= "FortFrance";
	Stores[FortFrance_STORE].StoreSize = "large";
	// LeFransua (Martinique)
	Stores[LeFransua_STORE].Colony	= "LeFransua";
	Stores[LeFransua_STORE].group	= "LeFransua";
	Stores[LeFransua_STORE].StoreSize = "small";
	
	// LaVega
	Stores[LaVega_STORE].Colony	= "LaVega";
	Stores[LaVega_STORE].group	= "LaVega";
	Stores[LaVega_STORE].StoreSize = "small";
	
	// SantoDomingo
	Stores[SantoDomingo_STORE].Colony	= "SantoDomingo";
	Stores[SantoDomingo_STORE].group	= "SantoDomingo";
	Stores[SantoDomingo_STORE].StoreSize = "large";
	
	// Pirates Town
	Stores[Pirates_STORE].Colony = "Pirates";
	Stores[Pirates_STORE].group	= "Pirates";
	Stores[Pirates_STORE].StoreSize = "small";

	// PortPax
	Stores[PortPax_STORE].Colony	= "PortPax";
	Stores[PortPax_STORE].group	= "PortPax";
	Stores[PortPax_STORE].StoreSize = "large";

	// Panama
	Stores[Panama_STORE].Colony	= "Panama";
	Stores[Panama_STORE].group	= "Panama";
	Stores[Panama_STORE].StoreSize = "large";

	// Cartahena
	Stores[Cartahena_STORE].Colony	= "Cartahena";
	Stores[Cartahena_STORE].group	= "Cartahena";
	Stores[Cartahena_STORE].StoreSize = "large";

	// Maracaibo
	Stores[Maracaibo_STORE].Colony	= "Maracaibo";
	Stores[Maracaibo_STORE].group	= "Maracaibo";
	Stores[Maracaibo_STORE].StoreSize = "large";

	// Caracas
	Stores[Caracas_STORE].Colony	= "Caracas";
	Stores[Caracas_STORE].group	= "Caracas";
	Stores[Caracas_STORE].StoreSize = "large";

	// Cumana
	Stores[Cumana_STORE].Colony	= "Cumana";
	Stores[Cumana_STORE].group	= "Cumana";
	Stores[Cumana_STORE].StoreSize = "large";

	// SantaCatalina
	Stores[SantaCatalina_STORE].Colony	= "SantaCatalina";
	Stores[SantaCatalina_STORE].group	= "SantaCatalina";
	Stores[SantaCatalina_STORE].StoreSize = "large";
	
	// Beliz
	Stores[Beliz_STORE].Colony	= "Beliz";
	Stores[Beliz_STORE].group	= "Beliz";
	Stores[Beliz_STORE].StoreSize = "large";

	Stores[SHIP_STORE].Colony	= "none";
	Stores[SHIP_STORE].group	= "none";
	Stores[SHIP_STORE].StoreSize = "small";

}
