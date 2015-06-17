#include "Item.h"

XModel itemModel[3];

Item::Item()
{
	num[POTATO] = 0;
	num[MEAT] = 0;
	num[BOOK] = 0;
}

Item::~Item()
{
}

static int GetRandom(int min,int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

void Item::Init( int index )
{
	int random = GetRandom(1, 20);
	if( random == 20 )
		type[index] = BOOK;
	else if( random > 17 )
		type[index] = MEAT;
	else
		type[index] = POTATO;

	pos[index].x = (float)GetRandom(-100, 200);
	pos[index].z = (float)GetRandom(-190, -300);
	float d = GetDepthData(pos[index].x, pos[index].z);
	while( d > 0.0f && GetRandom(0, 10) >= 5 )
	{
		pos[index].x = (float)GetRandom(-100, 200);
		pos[index].z = (float)GetRandom(-190, -300);
		d = GetDepthData(pos[index].x, pos[index].z);
	}
	if( type[index] == BOOK )
		pos[index].y = d + 1.0f;
	if( type[index] == POTATO )
		pos[index].y = d + 2.0f;
	if( type[index] == MEAT )
		pos[index].y = d + 2.5f;
}

void Item::Reset()
{
	num[POTATO] = 0;
	num[MEAT] = 0;
	num[BOOK] = 0;
	for( int i=0; i<ITEM_NUM; i++ )
		Init(i);
}

bool Item::Load()
{
	if( !itemModel[POTATO].Load("model\\imo\\ˆð.x") )
	{
		cout << "Item Error : Load() failed to load model\\imo\\ˆð.x" << endl;
		return false;
	}
	if( !itemModel[MEAT].Load("model\\niku\\ƒ}ƒ“ƒK“÷.x") )
	{
		cout << "Item Error : Load() failed to load model\\niku\\ƒ}ƒ“ƒK“÷.x" << endl;
		return false;
	}
	if( !itemModel[BOOK].Load("model\\book\\book.x") )
	{
		cout << "Item Error : Load() failed to load model\\book\\book.x" << endl;
		return false;
	}
	if( sound[0].LoadWave("sound\\imo.wav") == AL_FALSE )
		return false;
	if( sound[1].LoadWave("sound\\oniku.wav") == AL_FALSE )
		return false;
	if( sound[2].LoadWave("sound\\get.wav") == AL_FALSE )
		return false;

	srand((unsigned int)time(NULL));
	for( int i=0; i<ITEM_NUM; i++ )
	{
		Init(i);
	}

	return true;
}

void Item::Release()
{
	itemModel[POTATO].Release();
	itemModel[MEAT].Release();
	itemModel[BOOK].Release();
	for( int i=0; i<3; i++ )
		sound[i].Release();
}

void Item::Render()
{
	static int count = 0;

	for( int i=0; i<ITEM_NUM; i++ )
	{
		glPushMatrix();
		glTranslatef( pos[i].x, pos[i].y, pos[i].z );
		if( type[i] != BOOK )
		{
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef((float)count, 1.0f, 0.0f, 0.0f);
		}
		else
			glRotatef((float)count, 0.0f, 1.0f, 0.0f);
		itemModel[type[i]].Render();
		glPopMatrix();
	}
	count++;
	if( count == 360 )
		count = 0;
}

void Item::Gain( int index )
{
	num[type[index]]++;
	sound[type[index]].SetPosition(pos[index]+Vector3f(0.0f,2.0f,0.0f));
	sound[type[index]].Play();
}

void Item::Repop( int index )
{
	Init(index);
}

Vector3f Item::GetPos( int index )
{
	return pos[index];
}

int Item::GetNum( ITEM_TYPE type )
{
	return num[type];
}

ITEM_TYPE Item::GetType( int index )
{
	return type[index];
}