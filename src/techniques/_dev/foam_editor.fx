technique FoamPoints
{
	pass p0
	{
		ZEnable = false;
		Lighting = false;
		CullMode = none;
		AlphaTestEnable = false;
		AlphaBlendEnable = false;

		ColorOp[0] = selectarg1;
		ColorArg1[0] = diffuse;

		AlphaOp[0] = disable;

		ColorOp[1] = disable;
	}
}