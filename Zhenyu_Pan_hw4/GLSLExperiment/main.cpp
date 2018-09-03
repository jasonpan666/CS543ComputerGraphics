// WPI CS543 HW4 Zhenyu Pan

#include "Angel.h"
#include "Mesh.h"
#include "Spotlight.h"
#include "CTMStack.h"

#define BASE_ROTATE_SPEED 0.25f
#define CUTOFF_CHANGE_AMOUNT 2
#define ROOT_TWO_OVER_TWO sqrt(2.0f) / 2.0f

//remember to prototype
void setUpOpenGLBuffers();
void display();
void drawShadows(Mesh* m);
void drawLine(Mesh* m1, Mesh* m2);
void resize(int newWidth, int newHeight);
void update();
void keyboard(unsigned char key, int x, int y);

using namespace std;

//Globals
GLuint program;
int width = 0;
int height = 0;
CTMStack ctmStack(0);
Mesh* floorMesh;
Mesh* wall1;
Mesh* wall2;
Mesh* root;
Mesh* child1;
Mesh* child2;
Mesh* child3;
Mesh* child4;
Mesh* child5;
Spotlight* light;

bool shouldRefract = false;
bool shouldReflect = false;
bool shouldDrawShadows = true;
bool areWallsTextured = true;
float currentRotationAmount;

//Set up openGL buffers
void setUpOpenGLBuffers() {
	//Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);

	//Setting up ctm
	ctmStack = CTMStack(program);

	//set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 3 * 4 * sizeof(float), BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 3 * 4 * sizeof(float), BUFFER_OFFSET(4 * sizeof(float)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 4, GL_FLOAT, GL_FALSE, 3 * 4 * sizeof(float), BUFFER_OFFSET(2 * 4 * sizeof(float)));

	//Set up light
	GLuint ambient = glGetUniformLocationARB(program, "ambient");
	glUniform4f(ambient, 0, 0, 0, 1);

	GLuint specular = glGetUniformLocationARB(program, "specular");
	glUniform4f(specular, 1, 1, 1, 1);

	GLuint shininess = glGetUniformLocationARB(program, "shininess");
	glUniform1f(shininess, 100);

	GLuint lightPos = glGetUniformLocationARB(program, "lightPos");
	glUniform4f(lightPos, light->getPosition().x, light->getPosition().y, light->getPosition().z, 0);

	GLuint lightDir = glGetUniformLocationARB(program, "lightDir");
	glUniform4f(lightDir, light->getDirection().x, light->getDirection().y, light->getDirection().z, 0);

	//sets the default color to clear screen
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}

//Draw the current model with the appropriate model and view matrices
void display() {
	//Clear the window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Build the view matrix
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];
	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];

	//Set up projection matricies
	GLuint viewMatrix = glGetUniformLocationARB(program, "projectionMatrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	//Draw the floors/walls
	glEnable(GL_STENCIL_TEST); //Stencil test prevents semi-tranparent shadows from drawing twice
	glStencilFunc(GL_ALWAYS, 111, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	ctmStack.pushMatrix(floorMesh->getModelMatrix());
	ctmStack.popMatrix();
	floorMesh->drawMesh(program, light);
	ctmStack.pushMatrix(Angel::identity());

	ctmStack.pushMatrix(wall1->getModelMatrix());
	ctmStack.popMatrix();
	wall1->drawMesh(program, light);
	ctmStack.pushMatrix(Angel::identity());

	ctmStack.pushMatrix(wall2->getModelMatrix());
	ctmStack.popMatrix();
	wall2->drawMesh(program, light);
	ctmStack.pushMatrix(Angel::identity());

	glDisable(GL_STENCIL_TEST);

	//Draw the sculpture heirarchy
	ctmStack.pushMatrix(root->getModelMatrix());
	ctmStack.peekMatrix();
	root->drawMesh(program, light);
	drawShadows(root);
	drawLine(root, child1);
	drawLine(root, child2);

	ctmStack.pushMatrix(child1->getModelMatrix());
	ctmStack.peekMatrix();
	child1->drawMesh(program, light);
	drawShadows(child1);
	drawLine(child1, child4);
	drawLine(child1, child5);

	ctmStack.pushMatrix(child4->getModelMatrix());
	ctmStack.peekMatrix();
	child4->drawMesh(program, light);
	drawShadows(child4);
	ctmStack.popMatrix();

	ctmStack.pushMatrix(child5->getModelMatrix());
	ctmStack.peekMatrix();
	child5->drawMesh(program, light);
	drawShadows(child5);
	ctmStack.popMatrix();
	ctmStack.popMatrix();

	ctmStack.pushMatrix(child2->getModelMatrix());
	ctmStack.peekMatrix();
	child2->drawMesh(program, light);
	drawShadows(child2);
	drawLine(child2, child3);

	ctmStack.pushMatrix(child3->getModelMatrix());
	ctmStack.peekMatrix();
	child3->drawMesh(program, light);
	drawShadows(child3);

	//Clean up the matrix stack
	ctmStack.clear();

	//Flush and show
	glFlush();
	glutSwapBuffers();
}

void drawShadows(Mesh* m) {
	if (shouldDrawShadows) {
		m->drawShadows(program, light, 0.5f, vec3(0, 0, 0), vec3(0, 0, 0), ctmStack.peekMatrix()); //Floor
		m->drawShadows(program, light, 1.0f, vec3(0, -0.5, 0), vec3(90, 45, 0), ctmStack.peekMatrix()); //Wall
	}
}

//Mesh 1 should be the child, mesh 2 should be the parent
void drawLine(Mesh* m1, Mesh* m2) {
	//Build buffer
	vec3 m1Center = m1->getCenterPosition();
	vec3 m2Center = m2->getCenterPosition(m2->getModelMatrix());
	vec4 buf[12];

	//Build a three part line strip.
	//	- First part, vertical down from parent
	//	- Second part, horizontal over to child, at the average height between the parent and child
	//	- Third part, vertical down to child
	int i = 0;
	buf[i] = m1Center;                                                          i++; buf[i] = vec4(1, 1, 1, 1); i++; buf[i] = vec4(0, 0, 0, 0); i++;
	buf[i] = vec4(m1Center.x, (m1Center.y + m2Center.y) / 2.0f, m1Center.z, 1); i++; buf[i] = vec4(1, 1, 1, 1); i++; buf[i] = vec4(0, 0, 0, 0); i++;
	buf[i] = vec4(m2Center.x, (m1Center.y + m2Center.y) / 2.0f, m2Center.z, 1); i++; buf[i] = vec4(1, 1, 1, 1); i++; buf[i] = vec4(0, 0, 0, 0); i++;
	buf[i] = m2Center;                                                          i++; buf[i] = vec4(1, 1, 1, 1); i++; buf[i] = vec4(0, 0, 0, 0); i++;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * 12, buf, GL_STATIC_DRAW);

	//Set color to white
	GLuint vColor = glGetUniformLocationARB(program, "overrideColor");
	glUniform4f(vColor, 1, 1, 1, 1);

	//Draw the PLY model
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINE_STRIP, 0, 4);
	glDisable(GL_DEPTH_TEST);
}

//Update the size of the viewport, report the new width and height, redraw the scene
void resize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
	display();
}

//Update rotation
void update() {
	root->rotateBy(0, -BASE_ROTATE_SPEED, 0);
	child1->rotateBy(0, BASE_ROTATE_SPEED * 2, 0);
	child2->rotateBy(0, BASE_ROTATE_SPEED * 2, 0);
	child3->rotateBy(0, -BASE_ROTATE_SPEED * 3, 0);
	child4->rotateBy(0, -BASE_ROTATE_SPEED * 3, 0);
	child5->rotateBy(0, -BASE_ROTATE_SPEED * 3, 0);
	display();

	currentRotationAmount += BASE_ROTATE_SPEED;
}

//keyboard handler
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		//Exit
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'p':
		light->setCutoff(light->getCutoff() + CUTOFF_CHANGE_AMOUNT);
		break;
	case 'P':
		light->setCutoff(light->getCutoff() - CUTOFF_CHANGE_AMOUNT);
		break;
	case 'a':
	case 'A':
		shouldDrawShadows = !shouldDrawShadows;
		break;
	case 'b':
	case 'B':
		areWallsTextured = !areWallsTextured;
		floorMesh->shouldDrawWithTexture(areWallsTextured);
		wall1->shouldDrawWithTexture(areWallsTextured);
		wall2->shouldDrawWithTexture(areWallsTextured);
		break;
	case 'c':
	case 'C':
		shouldReflect = !shouldReflect;
		shouldRefract = false;
		root->setShouldReflect(shouldReflect);
		child1->setShouldReflect(shouldReflect);
		child2->setShouldReflect(shouldReflect);
		child3->setShouldReflect(shouldReflect);
		child4->setShouldReflect(shouldReflect);
		child5->setShouldReflect(shouldReflect);
		break;
	case 'd':
	case 'D':
		shouldRefract = !shouldRefract;
		shouldReflect = false;
		root->setShouldRefract(shouldRefract);
		child1->setShouldRefract(shouldRefract);
		child2->setShouldRefract(shouldRefract);
		child3->setShouldRefract(shouldRefract);
		child4->setShouldRefract(shouldRefract);
		child5->setShouldRefract(shouldRefract);
		break;
	}
}

//Make a flat square mesh
Mesh* makeWall() {
	Mesh* m = new Mesh(2, 4);
	m->addVertex(-1, -1, 0, 0, 0, 0);
	m->addVertex(-1, 1, 0, 0, 3, 0);
	m->addVertex(1, 1, 0, 3, 3, 0);
	m->addVertex(1, -1, 0, 3, 0, 0);
	m->addPoly(0, 2, 1);
	m->addPoly(0, 3, 2);

	m->prepForDrawing();

	return m;
}

//entry point
int main(int argc, char **argv) {
	//Set up light
	light = new Spotlight(vec3(0.75f, 0.6f, 0.0f), vec3(1.0f, 0.75f, 1.0f), 360);

	//init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	width = 512;
	height = 512;

	//create window
	glutCreateWindow("WPI CS543 HW4 Zhenyu Pan");

	//init glew
	glewInit();

	//Set up buffers
	setUpOpenGLBuffers();

	//assign handlers
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);



	//Setting up the walls and floor
	floorMesh = makeWall();
	floorMesh->setTexture("Resources/grass.bmp");
	floorMesh->setColor(vec4(0.5, 0.5, 0.5, 1));
	floorMesh->rotateBy(270, 0, 45);
	floorMesh->moveBy(0, -0.5, 0);

	wall1 = makeWall();
	wall1->setTexture("Resources/stones.bmp");
	wall1->setColor(vec4(0, 0, 1, 1));
	wall1->rotateBy(0, 45, 0);
	wall1->moveBy(-ROOT_TWO_OVER_TWO, 0, -ROOT_TWO_OVER_TWO);

	wall2 = makeWall();
	wall2->setTexture("Resources/stones.bmp");
	wall2->setColor(vec4(0, 0, 1, 1));
	wall2->rotateBy(0, -45, 0);
	wall2->moveBy(ROOT_TWO_OVER_TWO, 0, -ROOT_TWO_OVER_TWO);



	root = loadMeshFromPLY("Resources/cow.ply");
	root->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	root->setColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	root->scaleBy(0.3f);
	root->moveBy(0, 0.25f, -0.5f);

	child1 = loadMeshFromPLY("Resources/shark.ply");
	child1->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	child1->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	child1->moveBy(-1.0f, -1.0f, 0.0f);

	child2 = loadMeshFromPLY("Resources/dolphins.ply");
	child2->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	child2->setColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	child2->moveBy(1.0f, -1.0f, 0.0f);

	child3 = loadMeshFromPLY("Resources/beethoven.ply");
	child3->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	child3->setColor(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	child3->moveBy(0.0f, -1.0f, 0.0f);

	child4 = loadMeshFromPLY("Resources/ant.ply");
	child4->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	child4->setColor(vec4(0.0f, 0.5f, 0.1f, 1.0f));
	child4->moveBy(-0.5f, -1.0f, 0.0f);

	child5 = loadMeshFromPLY("Resources/big_spider.ply");
	child5->setEnvironmentMap("Resources/nvposx.bmp", "Resources/nvposy.bmp", "Resources/nvposz.bmp", "Resources/nvnegx.bmp", "Resources/nvnegy.bmp", "Resources/nvnegz.bmp");
	child5->setColor(vec4(1.0f, 0.0f, 1.0f, 1.0f));
	child5->moveBy(0.5f, -1.0f, 0.0f);


	//enter the drawing loop
	glutMainLoop();
	return 0;
}
