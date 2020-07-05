#ifndef BACKGROUNDDATE_H
#define BACKGROUNDDATE_H

class ATTRIBUTES;

class BackgroundDataDescriber
{
public:
	long year;
	long month;
	long day;

	long hour;
	long min;

	BackgroundDataDescriber();
	BackgroundDataDescriber( const BackgroundDataDescriber& dd );
	~BackgroundDataDescriber();

	const BackgroundDataDescriber& operator = ( const BackgroundDataDescriber& dd );
	bool operator == ( const BackgroundDataDescriber& dd );
	bool operator < ( const BackgroundDataDescriber& dd );
	bool operator <= ( const BackgroundDataDescriber& dd );
	bool operator > ( const BackgroundDataDescriber& dd );
	bool operator >= ( const BackgroundDataDescriber& dd );

	void AddMinutes( long nQuantity );
	void AddHour( long nQuantity );
	void AddDay( long nQuantity );
	void AddMonth( long nQuantity );
	void AddYears( long nQuantity );

	void WriteDataToAttributeClass( ATTRIBUTES* pA );
	void ReadDataFromAttributeClass( ATTRIBUTES* pA );
};

#endif
