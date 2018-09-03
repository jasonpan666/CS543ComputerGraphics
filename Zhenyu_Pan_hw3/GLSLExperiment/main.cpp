// CS543 HW3 Zhenyu Pan

#include "Angel.h"
#include "Mesh.h"
#include "Spotlight.h"
#include "CTMStack.h"

#define BASE_ROTATE_SPEED 0.25f
#define CUTOFF_CHANGE_AMOUNT 2
#define SINUSOID_AMPLITUDE 0.001f

//remember to prototype
void setUpOpenGLBuffers();
void display();
void drawLine(Mesh* m1, Mesh* m2);
void drawBox(Mesh* m, mat4 parentMat = Angel::identity());
void resize(int newWidth, int newHeight);
void update();
void keyboard(unsigned char key, int x, int y);

using namespace std;

//Globals
GLuint program;
int width = 0;
int height = 0;
CTMStack ctmStack(0);
Mesh* root;
Mesh* child1;
Mesh* child2;
Mesh* child3;
Mesh* child4;
Mesh* child5;
Spotlight light(vec3(0.0f, 1, -0.5f), vec3(0.0f, 1.0f, 0.0f), 45.0f);

bool shouldDrawBox;
bool shouldUseSinusoid;
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
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 2 * 4 * sizeof(float), BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 2 * 4 * sizeof(float), BUFFER_OFFSET(4 * sizeof(float)));

	//sets the default color to clear screen
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}

//Draw the current model with the appropriate model and view matrices
void display() {
	//Clear the window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	//Draw the sculpture heirarchy
	drawBox(root);
	ctmStack.pushMatrix(root->getModelMatrix());
	ctmStack.peekMatrix();
	root->drawMesh(program, light);
	drawLine(root, child1);
	drawLine(root, child2);

	drawBox(child1);
	ctmStack.pushMatrix(child1->getModelMatrix());
	ctmStack.peekMatrix();
	child1->drawMesh(program, light);
	drawLine(child1, child4);
	drawLine(child1, child5);

	drawBox(child4);
	ctmStack.pushMatrix(child4->getModelMatrix());
	ctmStack.popMatrix();
	child4->drawMesh(program, light);

	drawBox(child5);
	ctmStack.pushMatrix(child5->getModelMatrix());
	ctmStack.popMatrix();
	child5->drawMesh(program, light);
	ctmStack.popMatrix();

	drawBox(child2);
	ctmStack.pushMatrix(child2->getModelMatrix());
	ctmStack.peekMatrix();
	child2->drawMesh(program, light);
	drawLine(child2, child3);

	drawBox(child3);
	ctmStack.pushMatrix(child3->getModelMatrix());
	ctmStack.peekMatrix();
	child3->drawMesh(program, light);

	//Clean up the matrix stack
	ctmStack.clear();

	//Flush and show
	glFlush();
	glutSwapBuffers();
}

//Mesh 1 should be the child, mesh 2 should be the parent
void drawLine(Mesh* m1, Mesh* m2) {
	//Build buffer
	vec3 m1Center = m1->getCenterPosition();
	vec3 m2Center = m2->getCenterPosition(m2->getModelMatrix());
	vec4 buffer[8];

	//Build a three part line strip.
	//	- First part, vertical down from parent
	//	- Second part, horizontal over to child, at the average height between the parent and child
	//	- Third part, vertical down to child
	buffer[0] = m1Center;
	buffer[1] = vec4(1, 1, 1, 1); //Fake normal
	buffer[2] = vec4(m1Center.x, (m1Center.y + m2Center.y) / 2.0f, m1Center.z, 1);
	buffer[3] = vec4(1, 1, 1, 1); //Fake normal
	buffer[4] = vec4(m2Center.x, (m1Center.y + m2Center.y) / 2.0f, m2Center.z, 1);
	buffer[5] = vec4(1, 1, 1, 1); //Fake normal
	buffer[6] = m2Center;
	buffer[7] = vec4(1, 1, 1, 1); //Fake normal
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * 8, buffer, GL_STATIC_DRAW);

	//Set color to white
	GLuint vColor = glGetUniformLocationARB(program, "overrideColor");
	glUniform4f(vColor, 1, 1, 1, 1);

	//Draw the PLY model
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINE_STRIP, 0, 4);
	glDisable(GL_DEPTH_TEST);
}

void drawBox(Mesh* m, mat4 parentMat) {
	if (!shouldDrawBox) {
		return;
	}

	mat4 ctm = ctmStack.peekMatrix();
	float halfWidth = m->getWidth(m->getModelMatrix()) / 2.0f;
	float halfHeight = m->getHeight(m->getModelMatrix()) / 2.0f;
	float halfDepth = m->getDepth(m->getModelMatrix()) / 2.0f;
	vec4 center = m->getCenterPosition(m->getModelMatrix());

	vec4 topUpperLeftCorner = center + vec4(-halfWidth, halfHeight, halfDepth, 0);
	vec4 topUpperRightCorner = center + vec4(halfWidth, halfHeight, halfDepth, 0);
	vec4 topLowerLeftCorner = center + vec4(-halfWidth, halfHeight, -halfDepth, 0);
	vec4 topLowerRightCorner = center + vec4(halfWidth, halfHeight, -halfDepth, 0);
	vec4 bottomUpperLeftCorner = center + vec4(-halfWidth, -halfHeight, halfDepth, 0);
	vec4 bottomUpperRightCorner = center + vec4(halfWidth, -halfHeight, halfDepth, 0);
	vec4 bottomLowerLeftCorner = center + vec4(-halfWidth, -halfHeight, -halfDepth, 0);
	vec4 bottomLowerRightCorner = center + vec4(halfWidth, -halfHeight, -halfDepth, 0);

	//Build a cube
	vec4 buffer[24 * 2];
	int i = 0;
	//Side line
	buffer[i] = topUpperLeftCorner;     i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomUpperLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topUpperRightCorner;    i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomUpperRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerLeftCorner;     i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerRightCorner;    i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;

	//Top square lines
	buffer[i] = topUpperLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topUpperRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topUpperRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topLowerLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = topUpperLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;

	//Bottom square lines
	buffer[i] = bottomUpperLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomUpperRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomUpperRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerRightCorner; i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomLowerLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;
	buffer[i] = bottomUpperLeftCorner;  i++; buffer[i] = vec4(1, 1, 1, 1); i++;

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * 24 * 2, buffer, GL_STATIC_DRAW);

	//Set color to white
	GLuint vColor = glGetUniformLocationARB(program, "overrideColor");
	glUniform4f(vColor, 1, 1, 1, 1);

	//Draw the PLY model
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, 24);
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
	if (shouldUseSinusoid) {
		child1->moveBy(0, sin(currentRotationAmount * Angel::DegreesToRadians) *  SINUSOID_AMPLITUDE, 0);
		child2->moveBy(0, sin(currentRotationAmount * Angel::DegreesToRadians) *  SINUSOID_AMPLITUDE, 0);
		child3->moveBy(0, sin(currentRotationAmount * Angel::DegreesToRadians) * -SINUSOID_AMPLITUDE * 1.5f, 0);
		child5->moveBy(0, sin(currentRotationAmount * Angel::DegreesToRadians) * -SINUSOID_AMPLITUDE * 1.5f, 0);
	}
}

//keyboard handler
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		//Exit
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'e':
	case 'E':
		shouldDrawBox = !shouldDrawBox;
		break;
	case 'm':
		glUniform1i(glGetUniformLocationARB(program, "useSmooth"), GL_FALSE);
		break;
	case 'M':
		glUniform1i(glGetUniformLocationARB(program, "useSmooth"), GL_TRUE);
		break;
	case 'p':
		light.setCutoff(light.getCutoff() + CUTOFF_CHANGE_AMOUNT);
		break;
	case 'P':
		light.setCutoff(light.getCutoff() - CUTOFF_CHANGE_AMOUNT);
		break;
	case 's':
	case 'S':
		shouldUseSinusoid = !shouldUseSinusoid;
		break;
	}
}

//entry point
int main(int argc, char **argv) {

	root = loadMeshFromPLY("ply_files/cow.ply");
	root->setColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	root->scaleBy(0.3f);
	root->moveBy(0, 0.25f, -0.5f);

	child1 = loadMeshFromPLY("ply_files/shark.ply");
	child1->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	child1->moveBy(-1.0f, -1.0f, 0.0f);

	child2 = loadMeshFromPLY("ply_files/dolphins.ply");
	child2->setColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	child2->moveBy(1.0f, -1.0f, 0.0f);

	child3 = loadMeshFromPLY("ply_files/beethoven.ply");
	child3->setColor(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	child3->moveBy(0.0f, -1.0f, 0.0f);

	child4 = loadMeshFromPLY("ply_files/ant.ply");
	child4->setColor(vec4(0.0f, 0.5f, 0.1f, 1.0f));
	child4->moveBy(-0.5f, -1.0f, 0.0f);

	child5 = loadMeshFromPLY("ply_files/big_spider.ply");
	child5->setColor(vec4(1.0f, 0.0f, 1.0f, 1.0f));
	child5->moveBy(0.5f, -1.0f, 0.0f);

	//init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	width = 512;
	height = 512;

	//create window
	glutCreateWindow("CS543 HW3 Zhenyu Pan");

	//init glew
	glewInit();

	//Set up buffers
	setUpOpenGLBuffers();

	//assign handlers
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);

	//enter the drawing loop
	glutMainLoop();
	return 0;
}
