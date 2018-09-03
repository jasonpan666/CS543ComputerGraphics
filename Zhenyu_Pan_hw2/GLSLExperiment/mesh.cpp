// CS543 Computer Graphics HW2
// Zhenyu Pan

#include <limits.h>
#include "mesh.h"
#include "textfile.h"

Mesh::Mesh(int polyCount, int vertCount) {
	polyListSize = polyCount;
	numPolys = 0;
	polys = new Face*[polyCount];
	normals = new vec3*[polyCount];

	vertsListSize = vertCount;
	numVerts = 0;
	verts = new point4*[vertCount];
}

Mesh::~Mesh() {
	for (int i = 0; i < numPolys; i++) { delete polys[i]; }
	for (int i = 0; i < numPolys; i++) { delete normals[i]; }
	for (int i = 0; i < numVerts; i++) { delete verts[i]; }
	delete polys;
	delete verts;
}

void Mesh::addVertex(float x, float y, float z) {
	if (numVerts >= vertsListSize) {
		printf("Failed to add point (%f, %f, %f) - verts list is full!\n", x, y, z);
		return;
	}
	point4* toAdd = new point4(x, y, z, 1);
	verts[numVerts] = toAdd;
	numVerts++;
}

void Mesh::addPoly(int vertIndex1, int vertIndex2, int vertIndex3) {
	if (numPolys >= polyListSize) {
		printf("Failed to add poly with verts %d, %d, %d - poly list is full!\n", vertIndex1, vertIndex2, vertIndex3);
		return;
	}

	Face* toAdd = new Face();
	toAdd->p1 = verts[vertIndex1];
	toAdd->p2 = verts[vertIndex2];
	toAdd->p3 = verts[vertIndex3];
	polys[numPolys] = toAdd;
	normals[numPolys] = calcNormal(toAdd);
	numPolys++;
}

vec3* Mesh::calcNormal(Face* face) {
	point4* vectors[4];
	vectors[0] = face->p1;
	vectors[1] = face->p2;
	vectors[2] = face->p3;
	vectors[3] = face->p1;

	float mx = 0;
	float my = 0;
	float mz = 0;

	for (int i = 0; i < 3; i++) {
		mx += (vectors[i]->y - vectors[i + 1]->y) * (vectors[i]->z + vectors[i + 1]->z);
		my += (vectors[i]->z - vectors[i + 1]->z) * (vectors[i]->x + vectors[i + 1]->x);
		mz += (vectors[i]->x - vectors[i + 1]->x) * (vectors[i]->y + vectors[i + 1]->y);
	}

	vec3 normalized = Angel::normalize(vec3(mx, my, mz));
	return 	new vec3(normalized);
}

float Mesh::getMin(float(*getVal)(point4*)) {
	float min = INFINITY;
	for (int i = 0; i < numVerts; i++) {
		if (getVal(verts[i]) < min) { min = getVal(verts[i]); }
	}
	return min;
}

float Mesh::getMax(float(*getVal)(point4*)) {
	float max = -INFINITY;
	for (int i = 0; i < numVerts; i++) {
		if (getVal(verts[i]) > max) { max = getVal(verts[i]); }
	}
	return max;
}

float Mesh::getWidth() {
	auto getXLambda = [](point4* point) -> float { return point->x; };
	return getMax(getXLambda) - getMin(getXLambda);
}

float Mesh::getHeight() {
	auto getYLambda = [](point4* point) -> float { return point->y; };
	return getMax(getYLambda) - getMin(getYLambda);
}

float Mesh::getDepth() {
	auto getZLambda = [](point4* point) -> float { return point->z; };
	return getMax(getZLambda) - getMin(getZLambda);
}


point3 Mesh::getCenter() {
	auto getXLambda = [](point4* point) -> float { return point->x; };
	auto getYLambda = [](point4* point) -> float { return point->y; };
	auto getZLambda = [](point4* point) -> float { return point->z; };

	float xCenter = (getMin(getXLambda) + getMax(getXLambda)) / 2.0f;
	float yCenter = (getMin(getYLambda) + getMax(getYLambda)) / 2.0f;
	float zCenter = (getMin(getZLambda) + getMax(getZLambda)) / 2.0f;

	return point3(xCenter, yCenter, zCenter);
}

void Mesh::setUpBuffers(float pulseAmount = 0) {
	//Build a buffer of all points in the model
	point4* points = new point4[3 * numPolys];
	vec3* normalsPerPoint = new vec3[3 * numPolys];
	int pointsIndex = 0;
	for (int i = 0; i < numPolys; i++) {
		points[pointsIndex] = *(polys[i]->p1); normalsPerPoint[pointsIndex] = *(normals[i]); pointsIndex++;
		points[pointsIndex] = *(polys[i]->p2); normalsPerPoint[pointsIndex] = *(normals[i]); pointsIndex++;
		points[pointsIndex] = *(polys[i]->p3); normalsPerPoint[pointsIndex] = *(normals[i]); pointsIndex++;
	}

	//Inflate points. Need different transformation per vertex, so its done on the CPU, before other transforms are applied
	for (int i = 0; i < 3 * numPolys; i++) {
		vec3 pulseNormal = pulseAmount * normalsPerPoint[i];
		points[i].x += pulseNormal.x;
		points[i].y += pulseNormal.y;
		points[i].z += pulseNormal.z;
	}

	//Send the data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * numPolys * 3, points, GL_STATIC_DRAW);
	delete points;
	delete normalsPerPoint;
}

void Mesh::drawMesh() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 3 * numPolys);
	glDisable(GL_DEPTH_TEST);
}

void Mesh::setUpNormalsBuffers(float length = 1, float pulseAmount = 0) {
	//Build a buffer lines between the centroid of each face and the centroid plus the normal vector
	point4* points = new point4[2 * numPolys];
	int pointsIndex = 0;
	for (int i = 0; i < numPolys; i++) {
		float xAvg = ((polys[i]->p1->x + polys[i]->p2->x + polys[i]->p3->x) / 3.0f) + (pulseAmount * normals[i]->x);
		float yAvg = ((polys[i]->p1->y + polys[i]->p2->y + polys[i]->p3->y) / 3.0f) + (pulseAmount * normals[i]->y);
		float zAvg = ((polys[i]->p1->z + polys[i]->p2->z + polys[i]->p3->z) / 3.0f) + (pulseAmount * normals[i]->z);

		points[pointsIndex] = point4(xAvg, yAvg, zAvg, 1); pointsIndex++;
		points[pointsIndex] = point4((xAvg + length * normals[i]->x) + (pulseAmount * normals[i]->x),
			(yAvg + length * normals[i]->y) + (pulseAmount * normals[i]->y),
			(zAvg + length * normals[i]->z) + (pulseAmount * normals[i]->z), 1); pointsIndex++;
	}

	//Send the data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * numPolys * 2, points, GL_STATIC_DRAW);
	delete points;
}

void Mesh::drawNormalsLines() {
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, 2 * numPolys);
	glDisable(GL_DEPTH_TEST);
}

Mesh* loadMeshFromPLY(char* filename) {
	int success;
	char* fileContents = textFileRead(filename);

	//Tokenize on newlines
	char* token = strtok(fileContents, "\n");

	//Bail if not PLY
	if (!strcmp(token, "ply\n")) {
		printf("File \"%s\" is not a PLY file!\n", filename);
		return new Mesh(0, 0);
	}
	token = strtok(nullptr, "\n");
	token = strtok(nullptr, "\n"); //Skip ASCII line

								   //Read in verts
	int numVerts;
	success = sscanf(token, "element vertex %d", &numVerts);
	if (success != 1) {
		printf("could not read number of verts from file \"%s\"\n", filename);
		return new Mesh(0, 0);
	}
	token = strtok(nullptr, "\n");
	token = strtok(nullptr, "\n"); //Skip 1st property line
	token = strtok(nullptr, "\n"); //Skip 2nd property line
	token = strtok(nullptr, "\n"); //Skip 3rd property line

								   //Read in polys
	int numPolys;
	success = sscanf(token, "element face %d", &numPolys);
	if (success != 1) {
		printf("could not read number of polys from file \"%s\"\n", filename);
		return new Mesh(0, 0);
	}
	token = strtok(nullptr, "\n");
	token = strtok(nullptr, "\n"); //Skip 4th property line
	token = strtok(nullptr, "\n"); //Skip "end_header" line

								   //Build the vertex list
	Mesh* vertexList = new Mesh(numPolys, numVerts);

	//Read in verts
	for (int i = 0; i < numVerts; i++) {
		float x, y, z;
		success = sscanf(token, "%f %f %f", &x, &y, &z);
		if (success != 3) {
			printf("could not read vert #%d from file \"%s\"\n", i, filename);
			return new Mesh(0, 0);
		}
		vertexList->addVertex(x, y, z);
		token = strtok(nullptr, "\n");
	}

	//Read in polys
	for (int i = 0; i < numPolys; i++) {
		int p1, p2, p3;
		success = sscanf(token, "3 %d %d %d", &p1, &p2, &p3);
		if (success != 3) {
			printf("could not read poly #%d from file \"%s\"\n", i, filename);
			return new Mesh(0, 0);
		}
		vertexList->addPoly(p1, p2, p3);
		token = strtok(nullptr, "\n");
	}

	return vertexList;
}
