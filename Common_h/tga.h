#ifndef TGA_HPP
#define TGA_HPP

typedef struct tagTGA_H{
	BYTE byte1; // = 0
	BYTE byte2; // = 0
	BYTE type;
	BYTE byte4_9[9]; // = 0
	WORD width;
	WORD height;
	BYTE bpp; // bit per pixel
	union
	{
		BYTE attr8;
		struct
		{
			BYTE attr:4;
			BYTE rez:1;
			BYTE origin:1;
			BYTE storage:2;
		};
	};
}TGA_H;

#endif