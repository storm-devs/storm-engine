#include "seadatabase.h"

BISeaDatabase::BISeaDatabase() :
	m_aImage(_FL),
	m_aImgGroup(_FL)
{
}

BISeaDatabase::~BISeaDatabase()
{
	Release();
}

void BISeaDatabase::Initialization()
{
}

void BISeaDatabase::Release()
{
}
