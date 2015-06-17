#include "VpdLoader.h"

static char* Pointer;
static char Token[128];

static void GetToken()
{
	char *p = Pointer;
	char *q = Token;

	while ( *p != '\0' && strchr(" \t\r\n,;\"", *p) ) 
		p++;

	if ( *p == '{' || *p == '}' )
		(*q++) = (*p++);
	else
		while ( *p != '\0' && !strchr(" \t\r\n,;\"{}", *p ) )
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

static void GetToken2( char ch )
{
	char *p = Pointer;
	char *q = Token;

	while ( *p != '\0' && strchr(" \t\r\n,;\"", *p) ) 
		p++;

	if ( *p == '{' || *p == '}' )
		(*q++) = (*p++);
	else
		while ( *p != '\0' && *p != ch )
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

static bool GetToken(const char* token)
{
	GetToken();
	if ( strcmp(Token, token) != 0 )
	{
		cout << "VPD Loader Error : 想定トークンと読込トークンが一致しません" << endl;
		cout << "想定トークン：" << token << endl;
		cout << "読込トークン：" << Token << endl;
		return false;
	}
	return true;
}

static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

static int substring(char *src, char* dest, const char *str1, const char *str2)
{
	char *from, *to;
	int len;

	if ((from = strstr(src, str1)) == NULL)
		return 0;
	if ((to = strstr(from, str2)) == NULL)
		return 0;
	len = strlen(from) - strlen(to) + strlen(str2);
	strncpy_s(dest, 1024, from, len);
	dest[len] = '\0';

	return 1;
}

static void strdelete(char *buf, const char *str1)
{
	char *p;
	int strLen,bufLen;

	strLen = strlen(str1);
	while ((p = strstr(buf, str1)) != NULL) {
		bufLen = strlen(p);
		for(int i=0; i<bufLen-strLen; i++){
			*(p+i) = *(p+i+strLen);
		}
		*(p+bufLen-strLen) = '\0';
	}
}

static void CommentDelete(char* buf)
{
	char str[1024];

	while( (substring(buf, str, "//", "\n"))!=NULL ){
		strdelete(buf, str);
	}
}

Vpd::Vpd()
{
	boneNum = 0;
	bone = NULL;
	useFlag = false;
}

Vpd::~Vpd()
{
	Release();
}

bool Vpd::Load( const char* path )
{
	int fileSize;
	char* src;
	char* srcHead;
	struct stat stat_buf;

	FILE* fp;
	if( fopen_s(&fp, path, "rb" ) )
	{
		cout << "VPD Loader Error : can't open file: " << path << endl;
		return false;
	}
	fstat( _fileno(fp), &stat_buf );
	fileSize = stat_buf.st_size;
	src = new char[ fileSize+1 ];
	fread_s( src, fileSize+1, fileSize, 1, fp );
	src[fileSize] = '\0';
	srcHead = src;
	fclose(fp);

	CommentDelete(src);
	
	Pointer = src;

	GetToken();
	if( !strcmp(Token, "Vocaloid Pose Data file") )
	{
		cout << "VPD Loader Error : file" << path << "is not vpd" << endl;
		return false;
	}
	GetToken2(';');
	GetToken2(';');
	boneNum = atoi(Token);

	if( boneNum<=0 )
	{
		cout << "VPD Loader Error : boneNum is not correct" << endl;
		return false;
	}

	bone = new VpdBone[boneNum];
	for( int i=0; i<boneNum; i++ )
	{
		GetToken();
		GetToken("{");
		GetToken();

		//cout << "Name:" << Token << endl;
		strcpy_s(bone[i].name, Token);

		bone[i].trans.x = GetFloatToken();
		bone[i].trans.y = GetFloatToken();
		bone[i].trans.z = GetFloatToken();
		bone[i].rot.x = -GetFloatToken();
		bone[i].rot.y = -GetFloatToken();
		bone[i].rot.z = GetFloatToken();
		bone[i].rot.w = -GetFloatToken();

		//cout << "Translate:" << bone[i].trans << endl;
		//cout << "Rotate:" << bone[i].rot << endl;

		GetToken("}");
	}
	if( srcHead )
		delete[] srcHead;

	return true;
}

void Vpd::Release()
{
	boneNum = 0;
	if( bone )
	{
		delete[] bone;
		bone = NULL;
	}
}