#include <stdlib.h>
#include "TWalkGraph.h"

//--------------------------------------------------------------------
TWalkGraph::TWalkGraph()
	:pairsCount(0)
{

}

//--------------------------------------------------------------------
TWalkGraph::~TWalkGraph()
{

}

//--------------------------------------------------------------------
void TWalkGraph::AddPair(long v1, long v2, long t1, long t2)
{
	if (pairsCount >= MAX_PAIRS)
		return;

	pairs[pairsCount][0] = v1;
	pairs[pairsCount][1] = v2;
	pairs[pairsCount][2] = t1;
	pairs[pairsCount][3] = t2;
	++pairsCount;
}

//--------------------------------------------------------------------
void TWalkGraph::Clear()
{
	pairsCount = 0;
}

//--------------------------------------------------------------------
bool TWalkGraph::TestPair(long v1, long v2)
{
	for (int i=0; i < pairsCount; i++)
	{
		if ((pairs[i][0] == v1) && (pairs[i][1] == v2))
			return true;
		if ((pairs[i][0] == v2) && (pairs[i][1] == v1))
			return true;
	}

	return false;
}

//--------------------------------------------------------------------
int TWalkGraph::FindRandomWithType(int t)
{
	int chosenCount = 0;
	static int chosen[MAX_PAIRS];

	for (int i=0; i < pairsCount; i++)
	{
		if (pairs[i][2] == t)
			chosen[chosenCount++] = pairs[i][0];
		if (pairs[i][3] == t)
			chosen[chosenCount++] = pairs[i][1];
	}

	if (chosenCount)
		return chosen[rand() % chosenCount];
	else
		return -1;
}

//--------------------------------------------------------------------
int TWalkGraph::FindRandomLinkedWithType(int sourceI, int t)
{
	int chosenCount = 0;
	static int chosen[MAX_PAIRS];

	for (int i=0; i < pairsCount; i++)
	{
		if (pairs[i][0] == pairs[i][1])
			continue;

		if ((pairs[i][0] == sourceI) && (pairs[i][3] == t))
			chosen[chosenCount++] = pairs[i][1];
		if ((pairs[i][1] == sourceI) && (pairs[i][2] == t))
			chosen[chosenCount++] = pairs[i][0];
	}

	if (chosenCount)
		return chosen[rand() % chosenCount];
	else
		return sourceI;
}

//--------------------------------------------------------------------
int TWalkGraph::FindRandomLinkedWithoutType(int sourceI, int t)
{
	int chosenCount = 0;
	static int chosen[MAX_PAIRS];

	for (int i=0; i < pairsCount; i++)
	{
		if (pairs[i][0] == pairs[i][1])
			continue;

		if ((pairs[i][0] == sourceI) && (pairs[i][3] != t))
			chosen[chosenCount++] = pairs[i][1];
		if ((pairs[i][1] == sourceI) && (pairs[i][2] != t))
			chosen[chosenCount++] = pairs[i][0];
	}

	if (chosenCount)
		return chosen[rand() % chosenCount];
	else
		return sourceI;
}

//--------------------------------------------------------------------
int TWalkGraph::FindRandomLinkedAnyType(int sourceI)
{
	int chosenCount = 0;
	static int chosen[MAX_PAIRS];

	for (int i=0; i < pairsCount; i++)
	{
		if (pairs[i][0] == pairs[i][1])
			continue;

		if (pairs[i][0] == sourceI)
			chosen[chosenCount++] = pairs[i][1];
		if (pairs[i][1] == sourceI)
			chosen[chosenCount++] = pairs[i][0];
	}

	if (chosenCount)
		return chosen[rand() % chosenCount];
	else
		return sourceI;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
