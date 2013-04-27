#ifndef _GameHelper_
#define _GameHelper_

class GameHelper
{
public:
	GameHelper(): id(0){};
	~GameHelper() {id = 0;};
	int getID() {return ++id;};
private:
	int id;
};

#endif