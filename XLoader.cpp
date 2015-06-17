#include "XLoader.h"

//
// global Variables
//
static char* Pointer;
static char Token[1024];


//-------------------------------------------------------------------------------------------------
// Name : CheckToken()
// Desc : 指定された文字列とトークンをチェックする
//-------------------------------------------------------------------------------------------------
static bool CheckToken(const char* str)
{
	if ( strcmp(Token, str) == 0 )
		return true;
	else
		return false;
}

//-------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得する
//-------------------------------------------------------------------------------------------------
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

static void GetToken( const char ch )
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

//--------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得し指定された文字列を比較を行う
//--------------------------------------------------------------------------------------------------
static bool GetToken(const char* token)
{
	GetToken();
	if ( strcmp(Token, token) != 0 )
	{
		cout << "Error : 想定トークンと読込トークンが一致しません\n";
		cout << "想定トークン：" << token << endl;
		cout << "読込トークン：" << Token << endl;
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------
// Name : GetFloatToken()
// Desc : トークンをfloat型に変換し取得する
//--------------------------------------------------------------------------------------------------
static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

//--------------------------------------------------------------------------------------------------
// Name : GetIntToken()
// Desc : トークンをint型に変換し取得する
//--------------------------------------------------------------------------------------------------
static int GetIntToken()
{
	GetToken();
	return (int)atoi(Token);
}

//-------------------------------------------------------------------------------------------------
// Name : SkipNode()
// Desc : ノードを飛ばす
//-------------------------------------------------------------------------------------------------
static void SkipNode()
{
	while ( *Pointer != '\0' )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) break;
	}
	int count = 1;
	while ( *Pointer != '\0' && count > 0 )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) count++;
		else if ( strchr(Token, '}') ) count--;
	}
	if ( count > 0 )
	{
		cout << "Error : カッコが一致していません\n";
		return;
	}
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

////////////////////////////////////////////////////////////////////////
// XFace class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Name : XFace()
// Desc : コンストラクタ
//-------------------------------------------------------------------------------------------------
XFace::XFace()
{
	element = 0;
	indexMaterial = -1;
	for(int i=0; i<4; i++ )
	{
		indexVertices[i] = -1;
		indexNormals[i] = -1;
		indexTexCoords[i] = -1;
	}
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XFace& XFace::operator =(XFace &ob)
{
	element = ob.element;
	indexMaterial = ob.indexMaterial;
	for(int i=0; i<4; i++ )
	{
		indexVertices[i] = ob.indexVertices[i];
		indexNormals[i] = ob.indexNormals[i];
		indexTexCoords[i] = ob.indexTexCoords[i];
	}
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetVertexIndex()
// Desc : 頂点インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetVertexIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexVertices[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetNormalIndex()
// Desc : 法線インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetNormalIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexNormals[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetTexCoordIndex()
// Desc : テクスチャ座標インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetTexCoordIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexTexCoords[i] = index[i];
}

////////////////////////////////////////////////////////////////////////
// XMaterial class
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Name : XMaterial()
// Desc : コンストラクタ
//--------------------------------------------------------------------------------------------------
XMaterial::XMaterial()
{
	strcpy_s(name, "null");
	index = -1;
	ambient.r = 0.0f;	ambient.g = 0.0f;	ambient.b = 0.0f;	ambient.a = 0.0f;
	diffuse.r = 0.0f;	diffuse.g = 0.0f;	diffuse.b = 0.0f;	diffuse.a = 0.0f;
	specular.r = 0.0f;	specular.g = 0.0f;	specular.b = 0.0f;	specular.a = 0.0f;
	emissive.r = 0.0f;	emissive.g = 0.0f;	emissive.g = 0.0f;	emissive.a = 0.0f;
	power = 0.0f;
	strcpy_s(textureFileName, "null");
	imageTex = 0;
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMaterial& XMaterial::operator =(XMaterial &ob)
{
	strcpy_s(name, ob.name);
	ambient = ob.ambient;
	diffuse = ob.diffuse;
	specular = ob.specular;
	emissive = ob.emissive;
	power = ob.power;
	strcpy_s(textureFileName, ob.textureFileName);
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : 名前をセットする
//--------------------------------------------------------------------------------------------------
void XMaterial::SetName(char *str)
{
	strcpy_s(name, str);
}

//--------------------------------------------------------------------------------------------------
// Name : SetTextureFileName()
// Desc : テクスチャファイル名をセットする
//--------------------------------------------------------------------------------------------------
void XMaterial::SetTextureFileName(char *fileName, char* dirName)
{
	strcpy_s(textureFileName, dirName);
	strcat_s(textureFileName, fileName);
}

void XMaterial::LoadImageTex()
{
	Texture* tex = new Texture();
	if( !tex->Load(textureFileName) )
	{
		cout << "XLoader Error : failed to load texture file: " << textureFileName << endl;
		imageTex = -1;
	}
	else
	{
		tex->CreateTexture( (GLuint*)&imageTex );
	}
	delete tex;
}

////////////////////////////////////////////////////////////////////////
// XMesh class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XMesh()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XMesh::XMesh()
{
	strcpy_s(name, "null");
	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
	vertex = (XVector3*)malloc(1*sizeof(XVector3));
	normal = (XVector3*)malloc(1*sizeof(XVector3));
	texcoord = (XVector2*)malloc(1*sizeof(XVector2));
	face = (XFace*)malloc(1*sizeof(XFace));
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMesh& XMesh::operator= (XMesh &ob)
{
	strcpy_s(name, ob.name);
	numVertices = ob.numVertices;
	numNormals = ob.numNormals;
	numTexCoords = ob.numTexCoords;
	numFaces = ob.numFaces;

	vertex = (XVector3*)malloc(numVertices*sizeof(XVector3));
	normal = (XVector3*)malloc(numNormals*sizeof(XVector3));
	texcoord = (XVector2*)malloc(numTexCoords*sizeof(XVector2));
	face = (XFace*)malloc(numFaces*sizeof(XFace));

	for(int i=0; i<numVertices; i++ )		vertex[i] = ob.vertex[i];
	for(int i=0; i<numNormals; i++ )		normal[i] = ob.normal[i];
	for(int i=0; i<numTexCoords; i++)		texcoord[i] = ob.texcoord[i];
	for(int i=0; i<numFaces; i++ )		face[i] = ob.face[i];
	
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : AddVertex()
// Desc : 頂点を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddVertex(XVector3 &ob)
{
	numVertices++;
	vertex = (XVector3*)realloc(vertex, numVertices*sizeof(XVector3));
	vertex[numVertices-1] = ob;
	return numVertices;
}

//--------------------------------------------------------------------------------------------------
// Name : AddNormal()
// Desc : 法線を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddNormal(XVector3 &ob)
{
	numNormals++;
	normal = (XVector3*)realloc(normal, numNormals*sizeof(XVector3));
	normal[numNormals-1] = ob;
	return numNormals;
}

//--------------------------------------------------------------------------------------------------
// Name : AddTexCoord()
// Desc : テクスチャ座標を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddTexCoord(XVector2 &ob)
{
	numTexCoords++;
	texcoord = (XVector2*)realloc(texcoord, numTexCoords*sizeof(XVector2));
	texcoord[numTexCoords-1] = ob;
	return numTexCoords;
}

//--------------------------------------------------------------------------------------------------
// Name : AddFace()
// Desc : 面を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddFace(XFace &ob)
{
	numFaces++;
	face = (XFace*)realloc(face, numFaces*sizeof(XFace));
	face[numFaces-1] = ob;
	return numFaces;
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XMesh::Release()
{
	if ( vertex )
	{
		free((XVector3*)vertex);
		vertex = NULL;
	}

	if ( normal )
	{
		free((XVector3*)normal);
		normal = NULL;
	}

	if ( texcoord )
	{
		free((XVector2*)texcoord);
		texcoord = NULL;
	}

	if ( face )
	{
		free((XFace*)face);
		face = NULL;
	}

	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
}

//---------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : 名前をセットする
//---------------------------------------------------------------------------------------------------
void XMesh::SetName(char *str)
{
	strcpy_s(name, str);
}

////////////////////////////////////////////////////////////////////////
// XModel class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XModel()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XModel::XModel()
{
	numMeshes = 0;
	numMaterials = 0;
	mesh = (XMesh*)malloc(1*sizeof(XMesh));
}

//--------------------------------------------------------------------------------------------------
// Name : AddMesh()
// Desc : メッシュを追加する
//--------------------------------------------------------------------------------------------------
int XModel::AddMesh(XMesh ob)
{
	numMeshes++;
	mesh = (XMesh*)realloc(mesh, numMeshes*sizeof(XMesh));
	mesh[numMeshes-1] = ob;
	return numMeshes;
}

//--------------------------------------------------------------------------------------------------
// Name : AddMaterial()
// Desc : マテリアルを追加する
//--------------------------------------------------------------------------------------------------
int XModel::AddMaterial(XMaterial ob)
{
	numMaterials++;
	ob.index = numMaterials-1;
	material.push_back(ob);
	return numMaterials;
}

void XModel::ChangeMaterialIndex(XMaterial ob, int index)
{
	list<XMaterial>::iterator it = material.begin();
	while( it != material.end() )  // listの末尾まで
	{
		if( strcmp(it->textureFileName,ob.textureFileName)==0 ){
			it->index = index;
			break;
		}
		++it;  // イテレータを１つ進める
	}
}

void XModel::DeleteMaterial(XMaterial ob)
{
	numMaterials--;
	list<XMaterial>::iterator it = material.begin();
	while( it != material.end() )  // listの末尾まで
	{
		if( strcmp(it->textureFileName,ob.textureFileName)==0 ){
			material.erase(it);
			break;
		}
		++it;  // イテレータを１つ進める
	}
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XModel::Release()
{
	material.clear();
	if ( mesh )
	{
		free((XMesh*)mesh);
		mesh = NULL;
	}
	if( matArr )
	{
		delete[] matArr;
		matArr = NULL;
	}
	if( matList )
	{
		delete[] matList;
		matList = NULL;
	}
	if( vbo )
	{
		glDeleteBuffers(numMeshes*4, vbo);
		delete[] vbo;
		vbo = NULL;
	}
	numMaterials = 0;
	numMeshes = 0;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingSphere()
// Desc : バウンディングスフィアを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingSphere()
{
	int count = 0;
	float tempRadius = 0.0f;
	XVector3 tempCenter;
	tempCenter.x = 0.0f;
	tempCenter.y = 0.0f;
	tempCenter.z = 0.0f;

	//　中心座標の算出
	for ( int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			tempCenter.x += mesh[i].vertex[j].x;
			tempCenter.y += mesh[i].vertex[j].y;
			tempCenter.z += mesh[i].vertex[j].z;
			count++;
		}
	}
	tempCenter.x /= (float)count;
	tempCenter.y /= (float)count;
	tempCenter.z /= (float)count;

	//　半径の算出
	for (int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			float d = 0.0f;
			XVector3 temp;
			temp.x = mesh[i].vertex[j].x - tempCenter.x;
			temp.y = mesh[i].vertex[j].y - tempCenter.y;
			temp.z = mesh[i].vertex[j].z - tempCenter.z;
			d = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
			tempRadius = (tempRadius < d ) ? d : tempRadius;
		}
	}

	//　結果を格納
	sphere.center = tempCenter;
	sphere.radius = tempRadius;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingBox()
// Desc : バウンディングボックスを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingBox()
{
	XVector3 tempMin, tempMax;

	//　最初の頂点で初期化
	tempMin = tempMax = mesh[0].vertex[0];

	for ( int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			//　x成分
			if ( tempMin.x > mesh[i].vertex[j].x ) tempMin.x = mesh[i].vertex[j].x;
			if ( tempMax.x < mesh[i].vertex[j].x ) tempMax.x = mesh[i].vertex[j].x;

			//　y成分
			if ( tempMin.y > mesh[i].vertex[j].y ) tempMin.y = mesh[i].vertex[j].y;
			if ( tempMax.y < mesh[i].vertex[j].y ) tempMax.y = mesh[i].vertex[j].y;

			//　z成分
			if ( tempMin.z > mesh[i].vertex[j].z ) tempMin.z = mesh[i].vertex[j].z;
			if ( tempMax.z < mesh[i].vertex[j].z ) tempMax.z = mesh[i].vertex[j].z;
		}
	}

	//　結果を格納
	box.min = tempMin;
	box.max = tempMax;
}

//--------------------------------------------------------------------------------------------------
// Name : Load()
// Desc : テキストフォーマットのXファイルをロードする
//--------------------------------------------------------------------------------------------------
bool XModel::Load(char *filename, float scale)
{
	//　カウント用変数など	
	bool b_Mesh = false;
	int matCount = 0;
	int vertCount = 0;
	int faceCount = 0;
	int normCount = 0;
	int uvCount = 0;
	int meshCount = 0;
	
	XFace* tempFace = NULL;
	XMesh tempMesh;

	// get relative directory path
	char* dirName;
	char buff[1024];
	strcpy_s( buff, filename );
	char* p = strrchr( buff, '\\' );
	if( p )
	{
		while( strrchr( p+1, '\\' ) )
			p = strrchr( buff, '\\' );
		int dirNameSize = p - buff + 1;
		dirName = new char[ dirNameSize+1 ];
		strncpy_s( dirName, dirNameSize+1, buff, dirNameSize );
		dirName[ dirNameSize ] = '\0';
	}
	else{
		dirName = new char[1];
		*dirName = '\0';
	}

	//　ファイルを読み込む
	FILE *fp;
	//　チェック
	if ( fopen_s(&fp,filename,"rb") )
	{
		cout << "XLoader Error : failed to open file: " << filename << endl;
		return false;
	}

	//　ファイルサイズを取得
	struct stat stat_buf;
	fstat( _fileno(fp), &stat_buf );
	int fileSize = stat_buf.st_size;

	char* buffer = new char[fileSize+1];
	buffer[fileSize] = '\0';

	//　バッファに格納
	int read_size = fread_s( buffer, fileSize, fileSize, 1, fp );
	fclose(fp);

	//　サイズチェック
	if ( read_size != 1 )
	{
		cout << "Error : 読み込みサイズとサイズが一致していません\n";
		return false;
	}

	CommentDelete(buffer);

	//　Pointerに読み込んだバッファをセット
	Pointer = buffer;

	matCount = 0;

	//　ループ
	while ( *Pointer != '\0' )
	{
		//　トークンを取得
		GetToken();

		//　templateの場合
		if ( CheckToken("template") )
		{
			//　スキップする
			SkipNode();
		}

		//　Meshの場合
		else if ( CheckToken("Mesh") )
		{
			//　トークンを取得
			GetToken();

			//　トークンが" { " でない場合
			if ( !CheckToken("{") )
			{
				//　名前をセット
				tempMesh.SetName(Token);

				//　トークンを取得
				GetToken("{");
			}

			//　トークンが" { "の場合
			else
			{
				//　名前を決める
				char tempName[MAX_PATH] = "obj";
				sprintf_s(tempName, "%s%d", tempName, meshCount+1);

				//　名前をセット
				tempMesh.SetName(tempName);
			}

			//　メッシュ数をカウントする
			meshCount++;

			//　カウント数が1より大きい場合
			if ( meshCount > 1 )
			{
				//　面データを追加
				for(int i=0; i<faceCount; i++ )
					tempMesh.AddFace(tempFace[i]);
				
				//　メッシュを追加
				AddMesh(tempMesh);

				//　確保したメモリを解放
				if( tempFace )
				{
					delete[] tempFace;
					tempFace = NULL;
				}

				//　ゼロに戻す
				ZeroMemory(&tempMesh, sizeof(tempMesh));
			}

			//　頂点数のカウンターを0に戻す
			vertCount = 0;

			//　トークンから頂点数を取得
			vertCount = GetIntToken();

			for( int i=0; i<vertCount; i++ )
			{
				//　トークンから頂点データを取得
				XVector3 tempVertex;
				tempVertex.x = GetFloatToken();
				tempVertex.y = GetFloatToken();
				tempVertex.z = GetFloatToken();

				//　頂点データを追加
				tempMesh.AddVertex(tempVertex);
			}

			//　頂点数をチェック
			if ( tempMesh.numVertices != vertCount )
			{
				cout << "Error : 頂点数が一致していません\n";
				return false;
			}

			//　面数のカウンターを0に戻す
			faceCount = 0;

			//　トークンから面数を取得
			faceCount = GetIntToken();

			//　メモリを確保
			tempFace = new XFace[faceCount];

			//　トークンから面データを取得
			for( int i=0; i<faceCount; i++ )
			{
				int tempIndex[4] = { -1, -1, -1, -1 };
				tempFace[i].element = 0;
				tempFace[i].indexMaterial = -1;

				//　トークンを取得
				GetToken();
			
				//　三角形の場合
				if ( CheckToken("3") )
				{
					//　要素数は3にする
					tempFace[i].element = 3;

					//　トークンから頂点インデックスを取得
					for( int j=0; j<3; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　4番目のインデックスには-1を格納
					tempIndex[3] = -1;

					//　頂点インデックスをセット
					tempFace[i].SetVertexIndex(tempIndex);

					//　テクスチャ座標インデックスをセット
					tempFace[i].SetTexCoordIndex(tempIndex);
				}

				//　四角形の場合
				else if ( CheckToken("4") )
				{
					//　要素数は4にする
					tempFace[i].element = 4;

					//　トークンから頂点インデックスを取得
					for ( int j=0; j<4; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　頂点インデックスをセット
					tempFace[i].SetVertexIndex(tempIndex);

					//　テクスチャ座標インデックスをセット
					tempFace[i].SetTexCoordIndex(tempIndex);
				}
			}
		}

		//　MeshNormalsの場合
		else if ( CheckToken("MeshNormals") )
		{
			//　トークンを取得
			GetToken("{");

			//　法線数のカウンターを0に戻す
			normCount = 0;

			//　トークンから法線数を取得
			normCount = GetIntToken();

			//　トークンから法線データを取得
			for ( int i=0; i<normCount; i++ )
			{
				XVector3 tempNormal;
				tempNormal.x = GetFloatToken();
				tempNormal.y = GetFloatToken();
				tempNormal.z = GetFloatToken();

				//　法線データを追加
				tempMesh.AddNormal(tempNormal);
			}

			//　法線数をチェック
			if ( tempMesh.numNormals != normCount )
			{
				cout << "Error : 法線数が一致していません\n";
				return false;
			}

			//　法線インデックス数をチェック
			if ( GetIntToken() != faceCount )
			{
				cout << "Error : 面数と法線インデックス数が一致していません\n";
				return false;
			}

			for ( int i=0; i<faceCount; i++ )
			{
				int tempIndex[4] = { -1, -1, -1, -1 };

				//　トークンを取得
				GetToken();

				//　三角形の場合
				if ( CheckToken("3") )
				{
					//　トークンから法線インデックスを取得
					for ( int j=0; j<3; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　4番目のインデックスには-1をセット
					tempIndex[3] = -1;

					//　法線インデックスをセット
					tempFace[i].SetNormalIndex(tempIndex);
				}

				//　四角形の場合
				else if( CheckToken("4") )
				{
					//　法線インデックスを取得
					for ( int j=0; j<4; j++ )
					{
						tempIndex[j] = GetIntToken();
					}
					
					//　法線インデックスをセット
					tempFace[i].SetNormalIndex(tempIndex);
				}
			}
		}

		//　MeshTextureCoordsの場合
		else if ( CheckToken("MeshTextureCoords") )
		{
			//　トークンを取得
			GetToken("{");

			//　テクスチャ座標数のカウンターを0に戻す
			uvCount = 0;

			//　トークンからテクスチャ座標数を取得
			uvCount = GetIntToken();

			for ( int i=0; i<uvCount; i++ )
			{
				//　トークンからテクスチャ座標データを取得
				XVector2 tempUV;
				tempUV.x = GetFloatToken();
				tempUV.y = GetFloatToken();

				//　テクスチャ座標データを追加
				tempMesh.AddTexCoord(tempUV);
			}
		}

		//　MeshMaterialListの場合
		else if ( CheckToken("MeshMaterialList") )
		{
			//　トークンを取得
			GetToken("{");

			//　マテリアル数のカウンターを0に戻す
			matCount = 0;

			//　トークンからマテリアル数を取得
			matCount = GetIntToken();

			//　マテリアル数をチェック
			if ( GetIntToken() != faceCount )
			{
				cout << "Error : 面数とマテリアルリスト数が一致しません\n";
				return false;
			}

			//　トークンからマテリアルインデックスを取得
			for ( int i=0; i<faceCount; i++ )
			{
				tempFace[i].indexMaterial = GetIntToken();
			}

			for ( int i=0; i<matCount; i++ )
			{
				XMaterial tempMaterial;
			
				//　トークンを取得
				GetToken();
				if( CheckToken("{") )
				{
					GetToken();

					list<XMaterial>::iterator it = material.begin();
					while( it != material.end() )  // listの末尾まで
					{
						if( strcmp(it->textureFileName,Token)==0 ){
							break;
						}
						++it;  // イテレータを１つ進める
					}
					ChangeMaterialIndex(*it, i);

					GetToken("}");
				}
				else{
					GetToken();

					//　トークンが" { "でない場合
					if ( !CheckToken("{") )
					{
						//　名前をセット
						tempMaterial.SetName(Token);
					
						//　トークンを取得
						GetToken("{");
					}

					//　トークンが" { "の場合
					else
					{
						//　名前を決める
						char tempName[MAX_PATH] = "mat";
						sprintf_s(tempName, "%s%d", tempName, i+1);
		
						//　名前をセット
						tempMaterial.SetName(tempName);
					}

					//　Ambient Color
					tempMaterial.ambient.r = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.a = 1.0f;
	
					//　Diffuse Color
					tempMaterial.diffuse.r = GetFloatToken();
					tempMaterial.diffuse.g = GetFloatToken();
					tempMaterial.diffuse.b = GetFloatToken();
					tempMaterial.diffuse.a = GetFloatToken();

					//　Shiness
					tempMaterial.power = GetFloatToken();

					//　Specular Color
					tempMaterial.specular.r = GetFloatToken();
					tempMaterial.specular.g = GetFloatToken();
					tempMaterial.specular.b = GetFloatToken();
					tempMaterial.specular.a = 1.0f;

					//　Emissive Color
					tempMaterial.emissive.r = GetFloatToken();
					tempMaterial.emissive.g = GetFloatToken();
					tempMaterial.emissive.b = GetFloatToken();
					tempMaterial.emissive.a = 1.0f;

					//　トークンを取得
					GetToken();

					//　TextureFileNameの場合
					if ( CheckToken("TextureFilename") )
					{
						//　トークンを取得
						GetToken("{");

						//　トークンを取得
						GetToken('\"');

						//　ファイル名をセット
						tempMaterial.SetTextureFileName(Token,dirName);
	
						// テクスチャを読込む
						tempMaterial.LoadImageTex();

						//　トークンを取得
						GetToken("}");

						//　トークンを取得
						GetToken();
					}

					//　トークンが" } "の場合
					if ( CheckToken("}") )
					{
						//　マテリアルデータを追加
						AddMaterial(tempMaterial);
					}
				}
			}
		}
		else if ( CheckToken("Material") )
		{
			XMaterial tempMaterial;
			
			//　トークンを取得
			GetToken();

			//　トークンが" { "でない場合
			if ( !CheckToken("{") )
			{
				//　名前をセット
				tempMaterial.SetName(Token);
				
				//　トークンを取得
				GetToken("{");
			}

			//　トークンが" { "の場合
			else
			{
				//　名前を決める
				char tempName[MAX_PATH] = "mat";
				sprintf_s(tempName, "%s%d", tempName, matCount+1);
		
				//　名前をセット
				tempMaterial.SetName(tempName);
			}

			//　Ambient Color
			tempMaterial.ambient.r = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.a = 1.0f;

			//　Diffuse Color
			tempMaterial.diffuse.r = GetFloatToken();
			tempMaterial.diffuse.g = GetFloatToken();
			tempMaterial.diffuse.b = GetFloatToken();
			tempMaterial.diffuse.a = GetFloatToken();

			//　Shiness
			tempMaterial.power = GetFloatToken();

			//　Specular Color
			tempMaterial.specular.r = GetFloatToken();
			tempMaterial.specular.g = GetFloatToken();
			tempMaterial.specular.b = GetFloatToken();
			tempMaterial.specular.a = 1.0f;

			//　Emissive Color
			tempMaterial.emissive.r = GetFloatToken();
			tempMaterial.emissive.g = GetFloatToken();
			tempMaterial.emissive.b = GetFloatToken();
			tempMaterial.emissive.a = 1.0f;

			//　トークンを取得
			GetToken();

			//　TextureFileNameの場合
			if ( CheckToken("TextureFilename") )
			{
				//　トークンを取得
				GetToken("{");

				//　トークンを取得
				GetToken('\"');

				//　ファイル名をセット
				tempMaterial.SetTextureFileName(Token,dirName);

				// テクスチャを読込む
				tempMaterial.LoadImageTex();

				//　トークンを取得
				GetToken("}");

				//　トークンを取得
				GetToken();
			}

			//　トークンが" } "の場合
			if ( CheckToken("}") )
			{
				//　マテリアルデータを追加
				AddMaterial(tempMaterial);

				matCount++;
			}
		}
	}

	//　メッシュ数が1以上の場合
	if ( meshCount >= 1 )
	{
		//　面データを追加
		for ( int i=0; i<faceCount; i++ )
			tempMesh.AddFace(tempFace[i]);
		
		//　メッシュデータを追加
		AddMesh(tempMesh);

		//　確保したメモリを解放
		if( tempFace )
		{
			delete[] tempFace;
			tempFace = NULL;
		}
	}
	if( buffer )
	{
		delete[] buffer;
		buffer = NULL;
	}

	//　バウンディングスフィアを計算
	ComputeBoundingSphere();

	//　バウンディングボックスを計算
	ComputeBoundingBox();

	//  ここから魔改造
	int i,j,k;
	if( numMaterials )
	{
		matArr = new MaterialStruct[numMaterials];
		for( i=0; i<numMaterials; i++ )
		{
			list<XMaterial>::iterator it = material.begin();
			while( it != material.end() )
			{
				if( it->index==i ){
					matArr[i].ambient = it->ambient;
					matArr[i].diffuse = it->diffuse;
					matArr[i].specular = it->specular;
					matArr[i].emissive = it->emissive;
					matArr[i].imageTex = it->imageTex;
					break;
				}
				++it;
			}
		}
	}
	material.clear();

	if( numMeshes && numMaterials )
	{
		matList = new MaterialList*[numMeshes];
		vbo = new GLuint[numMeshes*4];
	}

	for( i=0; i<numMeshes; i++ )
	{
		if( mesh[i].numVertices < mesh[i].numTexCoords )
		{
			cout << "未対応" << endl;
			getchar();
			return false;
		}

		matList[i] = new MaterialList[numMaterials*2];

		int indexCount = 0;
		for( j=0; j<mesh[i].numFaces; j++ )
			indexCount += mesh[i].face[j].element;

		float* vertexArr;
		float* normalArr;
		float* texcoordArr;
		uint* indexArr;
		vertexArr = new float[mesh[i].numVertices*3];
		normalArr = new float[mesh[i].numVertices*3];
		texcoordArr = new float[mesh[i].numVertices*2];
		indexArr = new uint[indexCount];

		for( j=0; j<mesh[i].numVertices; j++ )
		{
			vertexArr[j*3+0] = mesh[i].vertex[j].x*scale;
			vertexArr[j*3+1] = mesh[i].vertex[j].y*scale;
			vertexArr[j*3+2] = mesh[i].vertex[j].z*scale;
		}
		if( mesh[i].numTexCoords == mesh[i].numVertices )
		{
			for( j=0; j<mesh[i].numVertices; j++ )
			{
				texcoordArr[j*2+0] = mesh[i].texcoord[j].x;
				texcoordArr[j*2+1] = mesh[i].texcoord[j].y;
			}
		}
		else
		{
			for( j=0; j<mesh[i].numFaces; j++ )
				for( k=0; k<mesh[i].face[j].element; k++ )
				{
					texcoordArr[mesh[i].face[j].indexVertices[k]*2+0] = mesh[i].texcoord[mesh[i].face[j].indexTexCoords[k]].x;
					texcoordArr[mesh[i].face[j].indexVertices[k]*2+1] = mesh[i].texcoord[mesh[i].face[j].indexTexCoords[k]].y;
				}
		}
		if( mesh[i].numVertices == mesh[i].numNormals )
		{
			for( j=0; j<mesh[i].numVertices; j++ )
			{
				normalArr[j*3+0] = mesh[i].normal[j].x;
				normalArr[j*3+1] = mesh[i].normal[j].y;
				normalArr[j*3+2] = mesh[i].normal[j].z;
			}
		}
		else
		{
			// 頂点に合うように法線データを作成
			for( j=0; j<mesh[i].numFaces; j++ )
				for( k=0; k<mesh[i].face[j].element; k++ )
				{
					normalArr[mesh[i].face[j].indexVertices[k]*3+0] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].x;
					normalArr[mesh[i].face[j].indexVertices[k]*3+1] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].y;
					normalArr[mesh[i].face[j].indexVertices[k]*3+2] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].z;
				}
		}

		int index = 0;
		for( j=0; j<numMaterials; j++ )
		{
			int faceNum = 0;
			for( k=0; k<mesh[i].numFaces; k++ )
			{
				if( mesh[i].face[k].element==3 &&
					mesh[i].face[k].indexMaterial==j )
				{
					indexArr[index+0] = mesh[i].face[k].indexVertices[0];
					indexArr[index+1] = mesh[i].face[k].indexVertices[1];
					indexArr[index+2] = mesh[i].face[k].indexVertices[2];
					index += 3;
					faceNum ++;
				}
			}
			matList[i][j*2].elementNum = 3;
			matList[i][j*2].faceNum = faceNum;
			matList[i][j*2].materialIndex = j;

			faceNum = 0;
			for( k=0; k<mesh[i].numFaces; k++ )
			{
				if( mesh[i].face[k].element==4 &&
					mesh[i].face[k].indexMaterial==j )
				{
					indexArr[index+0] = mesh[i].face[k].indexVertices[0];
					indexArr[index+1] = mesh[i].face[k].indexVertices[1];
					indexArr[index+2] = mesh[i].face[k].indexVertices[2];
					indexArr[index+3] = mesh[i].face[k].indexVertices[3];
					index += 4;
					faceNum ++;
				}
			}
			matList[i][j*2+1].elementNum = 4;
			matList[i][j*2+1].faceNum = faceNum;
			matList[i][j*2+1].materialIndex = j;
		}

		int bufferSize = 0;

		glGenBuffers(4, &vbo[i*4+0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i*4+0]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices*3*sizeof(float), vertexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if( mesh[i].numVertices*3*sizeof(float) != bufferSize )
		{
			 glDeleteBuffers(1, &vbo[i*4+0]);
			 cout << "Model Error : VBOの作成に失敗しました。" << endl;
			 Release();
			 return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i*4+1]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices*3*sizeof(float), normalArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if( mesh[i].numVertices*3*sizeof(float) != bufferSize )
		{
			 glDeleteBuffers(2, &vbo[i*4+0]);
			 cout << "Model Error : VBOの作成に失敗しました。" << endl;
			 Release();
			 return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i*4+2]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices*2*sizeof(float), texcoordArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if( mesh[i].numVertices*2*sizeof(float) != bufferSize )
		{
			 glDeleteBuffers(3, &vbo[i*4+0]);
			 cout << "Model Error : VBOの作成に失敗しました。" << endl;
			 Release();
			 return false;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[i*4+3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint), indexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if( indexCount*sizeof(uint) != bufferSize )
		{
			 glDeleteBuffers(4, &vbo[i*4+0]);
			 cout << "Model Error : VBOの作成に失敗しました。" << endl;
			 Release();
			 return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] vertexArr;
		delete[] normalArr;
		delete[] texcoordArr;
		delete[] indexArr;
	}
	if( mesh )
	{
		delete[] mesh;
		mesh = NULL;
	}
	delete[] dirName;

	return true;
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : メッシュのインデックスを指定して描画
//---------------------------------------------------------------------------------------------------
void XModel::RenderMesh(int index)
{
	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[index*4+0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index*4+1]);
    glNormalPointer(GL_FLOAT, 0, 0); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index*4+2]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[index*4+3]);

	int i;
	int indexCount = 0;
	for( i=0; i<numMaterials*2; i++ )
	{
		int matIndex = matList[index][i].materialIndex;
		if( matArr[matIndex].imageTex != -1 )
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D , matArr[matIndex].imageTex);
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matArr[matIndex].ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matArr[matIndex].diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matArr[matIndex].specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matArr[matIndex].emissive);

		if( matList[index][i].elementNum==3 )
			glDrawElements(GL_TRIANGLES, matList[index][i].faceNum * matList[index][i].elementNum, 
					GL_UNSIGNED_INT, BUFFER_OFFSET(indexCount*sizeof(uint)) );
		else
			glDrawElements(GL_QUADS, matList[index][i].faceNum * matList[index][i].elementNum, 
					GL_UNSIGNED_INT, BUFFER_OFFSET(indexCount*sizeof(uint)) );
		indexCount += matList[index][i].faceNum * matList[index][i].elementNum;

		if( matArr[matIndex].imageTex != -1 )
		{
			glBindTexture(GL_TEXTURE_2D , 0);
			glDisable(GL_TEXTURE_2D);
		}
	}
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : 描画処理
//---------------------------------------------------------------------------------------------------
void XModel::Render()
{
	for ( int i=0; i<numMeshes; i++ )
	{
		RenderMesh(i);
	}
}