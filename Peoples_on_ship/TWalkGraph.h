#ifndef _TWALKGRAPH_H_
#define _TWALKGRAPH_H_

#define MAX_PAIRS 300
///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TWalkGraph  
{
public:
	TWalkGraph();
	virtual ~TWalkGraph();

	void AddPair(long v1, long v2, long t1, long t2);
	void Clear();
	bool TestPair(long v1, long v2);
	int  FindRandomLinkedWithType(int sourceI, int t);
	int  FindRandomLinkedWithoutType(int sourceI, int t);
	int  FindRandomLinkedAnyType(int sourceI);
	int  FindRandomWithType(int t);

private:
	long pairs[MAX_PAIRS][4];
	int pairsCount;
};

#endif