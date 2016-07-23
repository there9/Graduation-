#include <iostream>
#include <string>
#include <list>
using namespace std;

typedef struct ReturnCar
{
public:
	string carNumber;
	int detection_X;
	CvRect RoiImage;
};
extern ReturnCar returnCar;

struct CarNumberInfo
{
public:
	string carString;
	int count = 0;
};
class  StringOverCount
{
private:
	list<CarNumberInfo> carNumber;
	int numberReturnCount = 0;
public:
	
	void init()
	{
		carNumber.clear();
	}
	string MaxCallCarString()
	{
		if (carNumber.empty() == true)
		{
			return "¾øÀ½";
		}
		int max = 0;
		string carString;
		for each (CarNumberInfo obj in carNumber)
		{
			if (max < obj.count)
			{
				carString = obj.carString;
				max = obj.count;
			}
		}
		cout << carString << endl;
		return carString;
	}
	
	bool NumberCountReturn()
	{
		if (numberReturnCount < 5)
		{
			return false;
		}
		numberReturnCount = 0; 
		return true;
	}

	void CheckingCarNumber(string s)
	{
		numberReturnCount++;
		list<CarNumberInfo>::iterator itor;
		for (itor = carNumber.begin(); itor != carNumber.end(); itor++)
		{
			if (s == itor->carString)
			{
				itor->count++;
				
				return;
			}
		}
		CarNumberInfo carNubmerInfo;
		carNubmerInfo.carString = s;
		carNubmerInfo.count = 1;
		carNumber.push_front(carNubmerInfo);
		return;
	}
};
