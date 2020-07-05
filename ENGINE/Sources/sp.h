#ifndef _SP_H_

template<class CType>
class SP
{
	CType * pPointer;
public:
	SP(){pPointer = 0;};
	SP(CType * p){pPointer = p;}
	~SP(){if(pPointer) delete pPointer;};
	/*inline*/ CType& operator = (const CType& p) 
	{
		if(pPointer) delete pPointer;
		pPointer = p; 
		return &pPointer;
	};
};

#endif _SP_H_