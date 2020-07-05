#ifndef NETCHAT_HPP
#define NETCHAT_HPP

#include "..\..\common.h"

class NetSay : public ENTITY
{
public:
	NetSay();
	~NetSay();

	bool Init();
	void Execute(DWORD __DeltaTime);
	void Realize(DWORD __DeltaTime);
	dword AttributeChanged(ATTRIBUTES * pA);

	enum MESSAGE_TYPE
	{
		GAME_MESSAGE = 0,
		CHAT_MESSAGE,

		MESSAGE_TYPE_FORCE_DWORD = 0x7FFFFFFF
	};

	struct say_t
	{
		say_t()
		{
			fAlpha = 1.0f;
			fTime = 0.0f;
		}

		FLOAT			fAlpha;
		string			sName;
		string			sMessage;
		float			fTime;
		dword			dwColor;
		MESSAGE_TYPE	Type;
	};

	array<say_t*> aSay;
private:

	say_t * GetSay();

};

#endif