#ifndef _X_LOADER_H_
#define _X_LOADER_H_

#include <iostream>
#include <cmath>
#include <MyGlewLib.h>
#include <ctype.h>
#include <list>
#include <sys/stat.h>
#include "TexLoader.h"
using namespace std;

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

typedef unsigned int uint;

//
// forward declarations
//
class XFace;
class XMesh;
class XMaterial;
class XModel;

//
// XVector2 struct
//
struct XVector2
{
	float x, y; 
	operator float* () { return (float*)&x; }
	operator const float*() const { return (const float*)&x; }
};

//
// XVector3 struct
//
struct XVector3
{ 
	float x, y, z; 
	operator float*() { return (float*)&x; }
	operator const float*() const { return (const float*)&x; }
};

//
// XColor struct
//
struct XColor 
{
	float r, g, b, a; 
	operator float*() { return (float*)&r; }
	operator const float*() const { return (const float*)&r; }
	XColor& operator=(const XColor& c){
		r = c.r; g = c.g; b = c.b; a = c.a;
		return *this;
	}
};

//
// XBoundingSphere struct
//
struct XBoundingSphere
{
	XVector3 center;
	float radius;
};

//
// XBoundingBox struct
//
struct XBoundingBox
{
	XVector3 min;
	XVector3 max;
};

//
// XFace class
//
class XFace
{
public :
	int element;
	int indexMaterial;
	int indexVertices[4];
	int indexNormals[4];
	int indexTexCoords[4];
	XFace &operator = (XFace &ob);
	XFace();
	void SetVertexIndex(int index[]);
	void SetNormalIndex(int index[]);
	void SetTexCoordIndex(int index[]);
};

//
// XMaterial class
//
class XMaterial
{
public :
	char name[MAX_PATH];
	int index;
	XColor ambient;
	XColor diffuse;
	XColor specular;
	XColor emissive;
	float power;
	char textureFileName[MAX_PATH];
	int imageTex;
	XMaterial &operator = (XMaterial &ob);
	XMaterial();
	void SetName(char *str);
	void SetTextureFileName(char *fileName, char* dirName);
	void LoadImageTex();
};

//
// XMesh class
//
class XMesh
{
public :
	char name[MAX_PATH];
	int numVertices;
	int numNormals;
	int numTexCoords;
	int numFaces;
	XVector3* vertex;
	XVector3* normal;
	XVector2* texcoord;
	XFace* face;
	XMesh &operator = (XMesh &ob);
	XMesh();
	int AddVertex(XVector3 &ob);
	int AddNormal(XVector3 &ob);
	int AddTexCoord(XVector2 &ob);
	int AddFace(XFace &ob);
	void Release();
	void SetName(char *str);
};

struct MaterialStruct
{
	XColor ambient;
	XColor diffuse;
	XColor specular;
	XColor emissive;
	int imageTex;
};

struct MaterialList
{
	int materialIndex;
	int elementNum;
	int faceNum;
};

//
// XModel class
//
class XModel
{
private:
	void ComputeBoundingSphere();
	void ComputeBoundingBox();

public:
	int numMeshes;
	int numMaterials;
	XMesh *mesh;
	list<XMaterial> material;
	XBoundingSphere sphere;
	XBoundingBox box;

	MaterialStruct* matArr;
	MaterialList** matList;
	GLuint* vbo;

	bool Load(char *filename, float scale=1.0f);
	void Release();
	void RenderMesh(int index);
	void Render();
	int AddMesh(XMesh ob);
	int AddMaterial(XMaterial ob);
	void ChangeMaterialIndex(XMaterial ob, int index);
	void DeleteMaterial(XMaterial ob);
	XModel();
};

#endif	//Å@_X_LOADER_H_INCLUDED_