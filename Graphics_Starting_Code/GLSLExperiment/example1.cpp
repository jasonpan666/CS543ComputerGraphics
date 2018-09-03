// Starter program for HW 1. 
// Program draws a triangle. Study the program first
// The function generateGeometry can be modified to draw many 2D drawings (e.g. 2D Sierpinski Gasket)
// Generated using randomly selected vertices and bisection

#include "Angel.h"  // Angel.h is homegrown include file, which also includes glew and freeglut

// Number of points in polyline
const int NumPoints = 3;


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );

typedef vec2 point2;

using namespace std;

// Array for polyline
point2 points[NumPoints];
GLuint program;


void generateGeometry( void )
{
	// Specifiy the vertices for a triangle
	points[0] = point2( -0.5, -0.5 );
	points[1] = point2( 0.0, 0.5 );
	points[2] = point2( 0.5, -0.5 );
}


void initGPUBuffers( void )
{
	// Create a vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
}


void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        // sets white as color used to clear screen
}


void display( void )
{
	// All drawing happens in display function
    glClear( GL_COLOR_BUFFER_BIT );                // clear window
    glDrawArrays( GL_LINE_LOOP, 0, NumPoints );    // draw the points
    glFlush();										// force output to graphics hardware
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
    }
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       // intialize GLUT  
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB ); // single framebuffer, colors in RGB
    glutInitWindowSize( 640, 480 );                // Window size: 640 wide X 480 high
	glutInitWindowPosition(100,150);               // Top left corner at (100, 150)
    glutCreateWindow( "Starting App" );            // Create Window

    glewInit();										// init glew

    generateGeometry( );                           // Call function that generates points to draw
    initGPUBuffers( );							   // Create GPU buffers
    shaderSetup( );                                // Connect this .cpp file to shader file

    glutDisplayFunc( display );                    // Register display callback function
    glutKeyboardFunc( keyboard );                  // Register keyboard callback function

	// Can add minimalist menus here
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
    glutMainLoop();
    return 0;
}
