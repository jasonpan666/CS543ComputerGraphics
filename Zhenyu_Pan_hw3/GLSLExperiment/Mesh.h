// CS543 HW3 Zhenyu Pan

#pragma once
#include <functional>
#include "Angel.h"
#include "Spotlight.h"

typedef Angel::vec4  color4;

struct point4 {
	vec4* point;
	vec4* normal;
};
typedef struct point4 point4;

struct face {
	point4* p1;
	point4* p2;
	point4* p3;
};
typedef struct face Face;

class Mesh {
public:
	//Constructor, destructor
	Mesh(int polyCount, int vertCount);
	~Mesh();

	//Methods for building meshes
	void addVertex(float x, float y, float z);
	void addPoly(int vertIndex1, int vertIndex2, int vertIndex3);
	void Mesh::buildNormals();
	void normalize(); //Move to be centered on the origin, make the mesh's largest dimension 1
	void setColor(vec4 newColor);

	//Methods for getting information about meshes
	float getWidth(mat4 CTM = Angel::identity());
	float getHeight(mat4 CTM = Angel::identity());
	float getDepth(mat4 CTM = Angel::identity());
	vec3 getCenterPosition(mat4 CTM = Angel::identity());

	//Methods for drawing
	void drawMesh(int program, Spotlight light);

	//Methods for transforming meshes
	void moveTo(float x, float y, float z);
	void moveBy(float x, float y, float z);
	void rotateTo(float xRollAmount, float yRollAmount, float zRollAmount);
	void rotateBy(float xRollAmount, float yRollAmount, float zRollAmount);
	void scaleTo(float x, float y, float z);
	void scaleBy(float x);
	mat4 getModelMatrix();

private:
	//Model matrix stuff
	vec3 position;
	vec3 rotationAboutPosition;
	vec3 scale;
	vec4 color;

	//Polygon vars
	int numPolys;
	int polyListSize;
	Face** polys;

	//Vertex vars
	int numVerts;
	int vertsListSize;
	point4** verts;

	//Convenience methods for mins and maxes
	vec3 calcNormal(Face* face);
	float Mesh::getMin(std::function<float(point4*)> func);
	float Mesh::getMax(std::function<float(point4*)> func);
};

Mesh* loadMeshFromPLY(char* filename);

#pragma once
