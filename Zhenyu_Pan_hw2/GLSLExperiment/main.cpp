/*
// Draws colored cube  

#include "Angel.h"


//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void generateGeometry( void )
{	
    colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

void drawCube(void)
{
	// change to GL_FILL
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDisable( GL_DEPTH_TEST ); 
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	// SOME RANDOM TIPS
	//========================================================================
	// remember to enable depth buffering when drawing in 3d

	// avoid using glTranslatex, glRotatex, push and pop
	// pass your own view matrix to the shader directly
	// refer to the latest OpenGL documentation for implementation details

    // Do not set the near and far plane too far appart!
	// depth buffers do not have unlimited resolution
	// surfaces will start to fight as they come nearer to each other
	// if the planes are too far appart (quantization errors :(   )

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawCube();
    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Color Cube" );

	// init glew
    glewInit();

    generateGeometry();

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
*/


// CS543 Computer Graphics HW2
// Zhenyu Pan

#include "Angel.h"
#include "mesh.h"

#define TRANSLATE_LENGTH 0.03f
#define SHEAR_LENGTH 0.03f
#define ROTATE_SPEED 0.2f
#define PULSE_LENGTH 0.15f    
#define PULSE_SPEED 0.008f    
#define NORMAL_LINE_LENGHTH 0.06f

//remember to prototype
void setUpBuffers();
void resize(int newWidth, int newHeight);
void reset3DModel();
void updateRotatePulse();
void display();
void keyboard(unsigned char key, int x, int y);

typedef Angel::vec4  point4;

using namespace std;

//Globals
GLuint program;
int width = 0;
int height = 0;

Mesh* models[43];
int currentModel;
point3 currentLoc = point3(0, 0, 0);
float currentXShear = 0;
float currentXRotation = 0;
float currentPulseAmount = 0;
float currentPulseTime = 0;
bool rotatingFlag = false;
bool pulsingFlag = false;
bool drawNormals = false;

//Set up buffers
void setUpBuffers() {
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

	//set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//set the default color to clear screen
	glClearColor(0.0, 0.0, 0.0, 1.0); // set black background
}

//updateRotatePulse the size of the viewport, report the new width and height, redraw the scene
void resize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
	display();
}

//Reset the position and orientation of the current mesh. Also stop any rotation, pulsing, or normals.
void reset3DModel() {
	currentLoc = point3(0, 0, 0);
	currentXShear = 0;
	currentXRotation = 0;
	currentPulseAmount = 0;
	currentPulseTime = 0;
	rotatingFlag = false;
	pulsingFlag = false;
	drawNormals = false;
}

//updateRotatePulse rotation and pulsing (or neither if none are active)
void updateRotatePulse() {
	if (rotatingFlag) {
		currentXRotation += ROTATE_SPEED;
		if (currentXRotation > 360) {
			currentXRotation = 0;
		}
	}

	if (pulsingFlag) {
		currentPulseTime += PULSE_SPEED;

		if (currentPulseTime >= PULSE_LENGTH) {
			currentPulseTime = 0;
			currentPulseAmount = 0;
		}
		else if (currentPulseTime < PULSE_LENGTH / 2.0f) {
			currentPulseAmount += PULSE_SPEED;
		}
		else {
			currentPulseAmount -= PULSE_SPEED;
		}
	}

	display();
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

	//Build the model matrix
	point3 meshCenter = models[currentModel]->getCenter();
	float meshWidth = models[currentModel]->getWidth();
	float meshHeight = models[currentModel]->getHeight();
	float meshDepth = models[currentModel]->getDepth();

	//Figure out how to scale the mesh
	float biggestDimension = fmaxf(meshWidth, meshHeight);
	float scaleFactor = 1.0f / biggestDimension;

	//Build the model matrix
	mat4 CTM = Angel::identity();
	mat4 moveToOrigin = Angel::Translate(-meshCenter.x, -meshCenter.y, -meshCenter.z);
	mat4 scale = Angel::Scale(scaleFactor, scaleFactor, scaleFactor);
	mat4 shear = Angel::identity(); shear[0][1] = currentXShear;
	mat4 rotate = Angel::RotateX(currentXRotation);
	mat4 moveTocurrentLoc = Angel::Translate(currentLoc.x, currentLoc.y, currentLoc.z - 1.0f);
	CTM = CTM * moveTocurrentLoc * rotate * shear * scale * moveToOrigin;

	float CTMf[16];
	CTMf[0] = CTM[0][0];CTMf[4] = CTM[0][1];
	CTMf[1] = CTM[1][0];CTMf[5] = CTM[1][1];
	CTMf[2] = CTM[2][0];CTMf[6] = CTM[2][1];
	CTMf[3] = CTM[3][0];CTMf[7] = CTM[3][1];

	CTMf[8] = CTM[0][2];CTMf[12] = CTM[0][3];
	CTMf[9] = CTM[1][2];CTMf[13] = CTM[1][3];
	CTMf[10] = CTM[2][2];CTMf[14] = CTM[2][3];
	CTMf[11] = CTM[3][2];CTMf[15] = CTM[3][3];

	//Set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, CTMf);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	//Set up color
	GLuint vColor = glGetUniformLocationARB(program, "vColor");

	//Draw the PLY model
	glUniform4f(vColor, 0, 1, 1, 1); //Set to cyan
	models[currentModel]->setUpBuffers(currentPulseAmount / scaleFactor);
	models[currentModel]->drawMesh();

	//Draw the normals (if enabled)
	if (drawNormals) {
		glUniform4f(vColor, 1, 0, 0, 1); //Set to red
		models[currentModel]->setUpNormalsBuffers(NORMAL_LINE_LENGHTH / scaleFactor, currentPulseAmount / scaleFactor);
		models[currentModel]->drawNormalsLines();
	}

	glFlush(); //force output to graphics hardware

			   //use this call to double buffer
	glutSwapBuffers();
}

//keyboard handler
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		//Exit
	case 033:
		exit(EXIT_SUCCESS);
		break;

		//Reset
	case 'w':
	case 'W':
		reset3DModel();
		break;

		//Next and Previous
	case 'n':
	case 'N':
		if (currentModel < 42) { currentModel++; } 
		reset3DModel();
		break;

	case 'p':
	case 'P':
		if (currentModel > 0) { currentModel--; }
		reset3DModel();
		break;

		//Translating
	case 'X':
		currentLoc.x += TRANSLATE_LENGTH;
		break;
	case 'x':
		currentLoc.x -= TRANSLATE_LENGTH;
		break;
	case 'Y':
		currentLoc.y += TRANSLATE_LENGTH;
		break;
	case 'y':
		currentLoc.y -= TRANSLATE_LENGTH;
		break;
	case 'Z':
		currentLoc.z += TRANSLATE_LENGTH;
		break;
	case 'z':
		currentLoc.z -= TRANSLATE_LENGTH;
		break;

		//Rotating
	case 'r':
	case 'R':
		rotatingFlag = !rotatingFlag;
		break;

		//Shearing
	case 'H':
		currentXShear += SHEAR_LENGTH;
		break;
	case 'h':
		currentXShear -= SHEAR_LENGTH;
		break;

		//Pulsing
	case 'b':
	case 'B':
		pulsingFlag = !pulsingFlag;
		break;

		//Normals
	case 'm':
	case 'M':
		drawNormals = !drawNormals;
		break;
	}
}

//entry point
int main(int argc, char **argv) {
	//Load all models
	models[0] = loadMeshFromPLY("ply_files/airplane.ply");
	models[1] = loadMeshFromPLY("ply_files/ant.ply");
	models[2] = loadMeshFromPLY("ply_files/apple.ply");
	models[3] = loadMeshFromPLY("ply_files/balance.ply");
	models[4] = loadMeshFromPLY("ply_files/beethoven.ply");
	models[5] = loadMeshFromPLY("ply_files/big_atc.ply");
	models[6] = loadMeshFromPLY("ply_files/big_dodge.ply");
	models[7] = loadMeshFromPLY("ply_files/big_porsche.ply");
	models[8] = loadMeshFromPLY("ply_files/big_spider.ply");
	models[9] = loadMeshFromPLY("ply_files/canstick.ply");
	models[10] = loadMeshFromPLY("ply_files/chopper.ply");
	models[11] = loadMeshFromPLY("ply_files/cow.ply");
	models[12] = loadMeshFromPLY("ply_files/dolphins.ply");
	models[13] = loadMeshFromPLY("ply_files/egret.ply");
	models[14] = loadMeshFromPLY("ply_files/f16.ply");
	models[15] = loadMeshFromPLY("ply_files/footbones.ply");
	models[16] = loadMeshFromPLY("ply_files/fracttree.ply");
	models[17] = loadMeshFromPLY("ply_files/galleon.ply");
	models[18] = loadMeshFromPLY("ply_files/hammerhead.ply");
	models[19] = loadMeshFromPLY("ply_files/helix.ply");
	models[20] = loadMeshFromPLY("ply_files/hind.ply");
	models[21] = loadMeshFromPLY("ply_files/kerolamp.ply");
	models[22] = loadMeshFromPLY("ply_files/ketchup.ply");
	models[23] = loadMeshFromPLY("ply_files/mug.ply");
	models[24] = loadMeshFromPLY("ply_files/part.ply");
	models[25] = loadMeshFromPLY("ply_files/pickup_big.ply");
	models[26] = loadMeshFromPLY("ply_files/pump.ply");
	models[27] = loadMeshFromPLY("ply_files/pumpa_tb.ply");
	models[28] = loadMeshFromPLY("ply_files/sandal.ply");
	models[29] = loadMeshFromPLY("ply_files/saratoga.ply");
	models[30] = loadMeshFromPLY("ply_files/scissors.ply");
	models[31] = loadMeshFromPLY("ply_files/shark.ply");
	models[32] = loadMeshFromPLY("ply_files/steeringweel.ply");
	models[33] = loadMeshFromPLY("ply_files/stratocaster.ply");
	models[34] = loadMeshFromPLY("ply_files/street_lamp.ply");
	models[35] = loadMeshFromPLY("ply_files/teapot.ply");
	models[36] = loadMeshFromPLY("ply_files/tennis_shoe.ply");
	models[37] = loadMeshFromPLY("ply_files/tommygun.ply");
	models[38] = loadMeshFromPLY("ply_files/trashcan.ply");
	models[39] = loadMeshFromPLY("ply_files/turbine.ply");
	models[40] = loadMeshFromPLY("ply_files/urn2.ply");
	models[41] = loadMeshFromPLY("ply_files/walkman.ply");
	models[42] = loadMeshFromPLY("ply_files/weathervane.ply");
	currentModel = 0;

	//init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	width = 900;
	height = 900;

	//create window
	glutCreateWindow("CS543 Computer Graphics HW2 Zhenyu Pan");

	//init glew
	glewInit();

	//Set up buffers
	setUpBuffers();

	//assign handlers
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(updateRotatePulse);
	glutKeyboardFunc(keyboard);

	//enter the drawing loop
	glutMainLoop();
	return 0;
}