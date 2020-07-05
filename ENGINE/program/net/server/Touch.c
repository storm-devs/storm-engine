object NSTouch;

void NetServer_DeleteTouchEnvironment()
{
	DeleteClass(&NSTouch);
}

void NetServer_CreateTouchEnvironment(int iSMsg)
{
	NSTouch.Server = true;
	CreateEntity(&NSTouch, "NetTouch");
	LayerAddObject("net_execute", &NSTouch, 1);
	NSTouch.CollisionDepth = -10.0;
}