#ifndef _IEDITOR_MANAGER_H_
#define _IEDITOR_MANAGER_H_

class EditorManager
{
public:
	EditorManager();
	~EditorManager();

	void Release();
	void Init();

	void Execute( float fDeltaTime );

protected:
};

#endif
