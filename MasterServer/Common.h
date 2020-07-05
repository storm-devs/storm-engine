#ifndef COMMON_HPP
#define COMMON_HPP

#define _FL_			__FILE__, __LINE__

#include <sys/types.h> 
#include <sys/timeb.h>
#include <winsock2.h>

#include "..\common_h\templates\array.h"
#include "..\NET\NetMessage.h"
#include "..\NET\crc.h"

#include "..\ENGINE\PROGRAM\NET\net_defines.h"

#include "Client.h"

template<class T> T Min(T t1, T t2) { return ((t1 < t2) ? t1 : t2); };

#endif