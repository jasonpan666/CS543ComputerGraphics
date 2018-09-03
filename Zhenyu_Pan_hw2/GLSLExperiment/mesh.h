// CS543 Computer Graphics HW2
// Zhenyu Pan
#pragma once
#include "Angel.h"

typedef vec4 point4;
typedef vec3 point3;
typedef Angel::vec4  color4;

struct face {
	point4* p1;
	point4* p2;
	point4* p3;
};
typedef struct face Face;

class Mesh {
public:
	Mesh(int polyCount, int vertCount);
	~Mesh();
	void addVertex(float x, float y, float z);
	void addPoly(int vertIndex1, int vertIndex2, int vertIndex3);
	float getWidth();
	float getHeight();
	float getDepth();
	point3 getCenter();
	void setUpBuffers(float pulseAmount);
	void drawMesh();
	void setUpNormalsBuffers(float length, float pulseAmount);
	void drawNormalsLines();

private:
	//Polygon vars
	int numPolys;
	int polyListSize;
	Face** polys;
	vec3** normals;

	//Vertex vars
	int numVerts;
	int vertsListSize;
	point4** verts;

	//Convenience methods for mins and maxes
	vec3* calcNormal(Face* face);
	float Mesh::getMin(float(*getVal)(point4*));
	float Mesh::getMax(float(*getVal)(point4*));
};

Mesh* loadMeshFromPLY(char* filename);

