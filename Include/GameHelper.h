#ifndef _GameHelper_
#define _GameHelper_

class GameHelper
{
public:
	GameHelper();
	~GameHelper();
	int getID() {return ++id;};
private:
	int id;
};

GameHelper::GameHelper() : id(0)
{
	//
}

GameHelper::~GameHelper()
{
	id = 0;
}

#endif