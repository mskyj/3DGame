#ifndef _ITEM_H_
#define _ITEM_H_

#define ITEM_NUM 15

enum ITEM_TYPE{
	POTATO = 0, MEAT, BOOK
};

#include "XLoader.h"
#include "Vector.h"
#include "Map.h"
#include "Sound.h"
#include <time.h>

class Item
{
private:
	Vector3f pos[ITEM_NUM];
	ITEM_TYPE type[ITEM_NUM];
	int num[3];
	Sound sound[3];
public:
	Item();
	~Item();
	void Reset();
	void Init( int index );
	bool Load();
	void Release();
	void Render();
	void Repop( int index );
	void Gain( int index );
	Vector3f GetPos( int index );
	int GetNum( ITEM_TYPE type );
	ITEM_TYPE GetType( int index );
};



#endif