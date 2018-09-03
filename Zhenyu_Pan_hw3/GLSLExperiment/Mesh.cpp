// CS543 HW3 Zhenyu Pan

#include <limits.h>
#include "Mesh.h"
#include "textfile.h"

//Constructor, destructor
Mesh::Mesh(int polyCount, int vertCount) {
	position = vec3(0, 0, 0);
	rotationAboutPosition = vec3(0, 0, 0);
	scale = vec3(1, 1, 1);
	color = vec4(1, 1, 1, 1);

	polyListSize = polyCount;
	numPolys = 0;
	polys = new Face*[polyCount];

	vertsListSize = vertCount;
	numVerts = 0;
	verts = new point4*[vertCount];
}
Mesh::~Mesh() {
	for (int i = 0; i < numPolys; i++) { delete polys[i]; }
	for (int i = 0; i < numVerts; i++) {
		delete verts[i]->point;
		delete verts[i]->normal;
		delete verts[i];
	}
	delete polys;
	delete verts;
}

//Methods for building meshes
void Mesh::addVertex(float x, float y, float z) {
	if (numVerts >= vertsListSize) {
		printf("Failed to add point (%f, %f, %f) - verts list is full!\n", x, y, z);
		return;
	}
	point4* toAdd = new point4();
	toAdd->point = new vec4(x, y, z, 1);
	toAdd->normal = new vec4(0, 0, 0, 1);

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
	numPolys++;
}
void Mesh::normalize() {
	//Center and scale the mesh
	float meshWidth = getWidth();
	float meshHeight = getHeight();
	float meshDepth = getDepth();
	float biggestDimension = fmaxf(meshWidth, fmaxf(meshHeight, meshDepth));
	float scaleFactor = 1.0f / biggestDimension;

	mat4 correctionTransform = Angel::identity();
	mat4 moveToOrigin = Angel::Translate(getCenterPosition() * -1);
	mat4 normalizeScale = Angel::Scale(scaleFactor, scaleFactor, scaleFactor);
	correctionTransform = correctionTransform * normalizeScale;
	for (int i = 0; i < numVerts; i++) {
		vec4 temp = correctionTransform * *(verts[i]->point);
		verts[i]->point->x = temp.x;
		verts[i]->point->y = temp.y;
		verts[i]->point->z = temp.z;
	}
}
void Mesh::buildNormals() {
	for (int i = 0; i < numVerts; i++) {
		vec3 pointNormal = vec3(0, 0, 0);
		for (int j = 0; j < numPolys; j++) {
			if (polys[j]->p1 == verts[i] || polys[j]->p2 == verts[i] || polys[j]->p3 == verts[i]) {
				pointNormal += calcNormal(polys[j]);
			}
		}

		vec4 normalized = Angel::normalize(pointNormal);
		normalized.w = 1;
		verts[i]->normal = new vec4(normalized);
	}
}
void Mesh::setColor(vec4 newColor) {
	color = newColor;
}

//Methods for getting information about meshes
float Mesh::getWidth(mat4 CTM) {
	auto getXLambda = [mat = CTM](point4* point) -> float { return (mat * *(point->point)).x; };
	return getMax(getXLambda) - getMin(getXLambda);
}
float Mesh::getHeight(mat4 CTM) {
	auto getYLambda = [mat = CTM](point4* point) -> float { return (mat * *(point->point)).y; };
	return getMax(getYLambda) - getMin(getYLambda);
}
float Mesh::getDepth(mat4 CTM) {
	auto getZLambda = [mat = CTM](point4* point) -> float { return (mat * *(point->point)).z; };
	return getMax(getZLambda) - getMin(getZLambda);
}
vec3 Mesh::getCenterPosition(mat4 CTM) {
	auto getXLambda = [](point4* point) -> float { return point->point->x; };
	auto getYLambda = [](point4* point) -> float { return point->point->y; };
	auto getZLambda = [](point4* point) -> float { return point->point->z; };

	float xCenter = (getMin(getXLambda) + getMax(getXLambda)) / 2.0f;
	float yCenter = (getMin(getYLambda) + getMax(getYLambda)) / 2.0f;
	float zCenter = (getMin(getZLambda) + getMax(getZLambda)) / 2.0f;

	vec4 transformedPos = CTM * vec3(xCenter, yCenter, zCenter);
	return vec3(transformedPos.x, transformedPos.y, transformedPos.z);
}

//Methods for drawing
void Mesh::drawMesh(int program, Spotlight light) {
	//Build a buffer of all points in the model
	int bufferSize = 2 * 3 * numPolys;
	vec4* points = new vec4[bufferSize];
	int pointsIndex = 0;
	for (int i = 0; i < numPolys; i++) {
		points[pointsIndex] = *(polys[i]->p1->point);  pointsIndex++;
		points[pointsIndex] = *(polys[i]->p1->normal); pointsIndex++;
		points[pointsIndex] = *(polys[i]->p2->point);  pointsIndex++;
		points[pointsIndex] = *(polys[i]->p2->normal); pointsIndex++;
		points[pointsIndex] = *(polys[i]->p3->point);  pointsIndex++;
		points[pointsIndex] = *(polys[i]->p3->normal); pointsIndex++;
	}

	//Send the data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * bufferSize, points, GL_STATIC_DRAW);
	delete points;

	//Clear global color
	GLuint vColor = glGetUniformLocationARB(program, "overrideColor");
	glUniform4f(vColor, 0, 0, 0, 0);

	//Set up color
	GLuint ambient = glGetUniformLocationARB(program, "ambient");
	glUniform4f(ambient, color.x * 0.2f, color.y * 0.2f, color.z * 0.2f, 1);

	GLuint diffuse = glGetUniformLocationARB(program, "diffuse");
	glUniform4f(diffuse, color.x, color.y, color.z, color.w);

	GLuint specular = glGetUniformLocationARB(program, "specular");
	glUniform4f(specular, 1, 1, 1, 1);

	GLuint shininess = glGetUniformLocationARB(program, "shininess");
	glUniform1f(shininess, 10);

	//Set up light
	GLuint lightPos = glGetUniformLocationARB(program, "lightPos");
	glUniform4f(lightPos, light.getPosition().x, light.getPosition().y, light.getPosition().z, 0);

	GLuint lightDir = glGetUniformLocationARB(program, "lightDir");
	glUniform4f(lightDir, light.getDirection().x, light.getDirection().y, light.getDirection().z, 0);

	GLuint lightFalloff = glGetUniformLocationARB(program, "lightFalloff");
	glUniform1f(lightFalloff, 20.0f / (light.getCutoff() / 45.0f));

	GLuint lightCutoff = glGetUniformLocationARB(program, "lightCutoff");
	glUniform1f(lightCutoff, light.getCutoff() * Angel::DegreesToRadians);

	//Draw the PLY model
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 3 * numPolys);
	glDisable(GL_DEPTH_TEST);
}

//Methods for transforming meshes
void Mesh::moveTo(float x, float y, float z) {
	position = vec3(x, y, z);
}
void Mesh::moveBy(float x, float y, float z) {
	position += vec3(x, y, z);
}
void Mesh::rotateTo(float xRollAmount, float yRollAmount, float zRollAmount) {
	rotationAboutPosition = vec3(xRollAmount, yRollAmount, zRollAmount);
}
void Mesh::rotateBy(float xRollAmount, float yRollAmount, float zRollAmount) {
	rotationAboutPosition += vec3(xRollAmount, yRollAmount, zRollAmount);
}
void Mesh::scaleTo(float x, float y, float z) {
	scale = vec3(x, y, z);
}
void Mesh::scaleBy(float x) {
	scale *= x;
}
mat4 Mesh::getModelMatrix() {
	mat4 ModelMatrix = Angel::identity();
	mat4 scaleMatrix = Angel::Scale(scale);
	mat4 rotateMatrix = Angel::RotateX(rotationAboutPosition.x) * Angel::RotateY(rotationAboutPosition.y) * Angel::RotateZ(rotationAboutPosition.z);
	mat4 translateMatrix = Angel::Translate(position);

	ModelMatrix = ModelMatrix * translateMatrix * rotateMatrix * scaleMatrix;
	return ModelMatrix;
}

//Convenience stuff
vec3 Mesh::calcNormal(Face* face) {
	vec4 vectors[4];
	vectors[0] = *face->p1->point;
	vectors[1] = *face->p2->point;
	vectors[2] = *face->p3->point;
	vectors[3] = *face->p1->point;

	float mx = 0;
	float my = 0;
	float mz = 0;

	for (int i = 0; i < 3; i++) {
		mx += (vectors[i].y - vectors[i + 1].y) * (vectors[i].z + vectors[i + 1].z);
		my += (vectors[i].z - vectors[i + 1].z) * (vectors[i].x + vectors[i + 1].x);
		mz += (vectors[i].x - vectors[i + 1].x) * (vectors[i].y + vectors[i + 1].y);
	}

	vec3 normalized = Angel::normalize(vec3(mx, my, mz));
	return normalized;
}
float Mesh::getMin(std::function<float(point4*)> func) {
	float min = INFINITY;
	for (int i = 0; i < numVerts; i++) {
		if (func(verts[i]) < min) { min = func(verts[i]); }
	}
	return min;
}
float Mesh::getMax(std::function<float(point4*)> func) {
	float max = -INFINITY;
	for (int i = 0; i < numVerts; i++) {
		if (func(verts[i]) > max) { max = func(verts[i]); }
	}
	return max;
}

//Loading meshes
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
	Mesh* mesh = new Mesh(numPolys, numVerts);

	//Read in verts
	for (int i = 0; i < numVerts; i++) {
		float x, y, z;
		success = sscanf(token, "%f %f %f", &x, &y, &z);
		if (success != 3) {
			printf("could not read vert #%d from file \"%s\"\n", i, filename);
			return new Mesh(0, 0);
		}
		mesh->addVertex(x, y, z);
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
		mesh->addPoly(p1, p2, p3);
		token = strtok(nullptr, "\n");
	}

	mesh->normalize();
	mesh->buildNormals();

	return mesh;
}
