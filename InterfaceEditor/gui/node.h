#ifndef _IEDITOR_NODE_H_
#define _IEDITOR_NODE_H_

#include "defines.h"
#include "utils.h"

class EditorManager;

class EdNode
{
public:
	EdNode( EditorManager* pOwner );
	virtual ~EdNode();

	virtual void Release();
	virtual void Execute( float fDeltaTime );

protected:
};

#endif
