// Zhenyu Pan  CS543  HW1

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Angel.h" // Angel.h is homegrown include file that also includes glew and freeglut

// remember to prototype
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void tEventHandler( void );
void drawPolylineFile(char *);
void setWindow(GLfloat ,GLfloat ,GLfloat ,GLfloat );
void drawMode(int , int , int , int);
void myMouse(int , int , int , int );
void gingerbreadMan( void );
void fernFractal(void);
void myReshape(int , int  );
void myInit( void );
void glViewportMain(int , int );
void glViewportThumbs(int , int );
void drawFiveTimesFive( void );
void colorChange( int );

typedef vec2 point2;

mat4 ortho;

using namespace std;

//Windows Size
static int width = 640;
static int height = 480;

//Number of points in polyline
int NumPoints = 3;
static int isBKeyPressed = 0;
static int hasPrepoint = 0;
static int polylineIndex = 0;
static int pointIndex = 0;

//Array for polyline
point2 points[30000];
point2 points2[3]; // for test use
static point2 pointsForDrawMode[2];
static point2 pointsForPolyline[100][100];
static int isIndrawMode = 0;
static char currentKeyboardEvent = 'p';

static int randomArray[5][5] = {0}; // for T event (reshape use)

static int countOfPointsForPolyline[100] = { 0 };
static int nearestPointIndex = 0; // for m event

//store the origin size when e event happend
static int originWidthForE = 0;
static int originHeightForE = 0;

//the following two are not used for this time
static int lastWidthForE = 0;
static int lastHeightForE = 0;

static int nearestPolylineIndex = 0; // for d event, store polyline index
static int nearestPolylinePointIndex = 0; // for d event, store point index in above polyline

static point2 startPoint = point2(115, 121);

// store the random picked file index, used for reshape
static int randNum;

//ratio is not used this time
static float ratio = 0; 

static int cc = 0;

typedef vec4 color4;

static int colors = 0;

GLuint program;
GLuint ProjLoc;

static char fileName[10][20] = {
	                     "vinci.dat",
	                     "dino.dat", 
						 "birdhead.dat", 						  
						 "house.dat", 
						 "knight.dat", 
						 "rex.dat", 
						 "scene.dat", 
						 "usa.dat", 
						 "dragon.dat",
						 "dragon.dat"
						};

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

void setWindow(GLfloat left,GLfloat right,GLfloat bottom,GLfloat top)
{
	ProjLoc = glGetUniformLocation( program, "Proj" );
	mat4 ortho = Ortho2D( left, right, bottom, top );
    glUniformMatrix4fv( ProjLoc, 1, GL_FALSE, ortho );
}

void drawPolylineFile(char *FileName)
{
	char line[256];
	GLfloat left, right, bottom, top;
	FILE *inStream;
	GLint countOfPolylines, countOfPointsForPolyline; 
	GLfloat	x, y;

	if((inStream = fopen(FileName, "rt")) == NULL) // Open The File
	{
		printf("File does not exist!");
		exit(0);
	}

	if(strcmp(FileName,"dino.dat")==0)
	{
		left = 0;
		right = 640;
		bottom = 0;
		top = 480;
	}
	else 
	{
		while(!feof(inStream))
		{
			//Just go through comments
			memset(line, 0, 256);
			fscanf(inStream, "%s", line);
			if(line[0] == '*')
				break;
			else
				continue;
		}
		// read in the four directions of extents
		fscanf(inStream, "%f %f %f %f\n", &left, &top, &right, &bottom);
	 }
	
	// read point count for each polyline
	fscanf(inStream,"%d", &countOfPolylines);			
	for(int j = 0; j < countOfPolylines; j++)
	{	//read each polyline
		fscanf(inStream,"%d", &countOfPointsForPolyline);
		for (int i = 0; i < countOfPointsForPolyline; i++)
		{
			fscanf(inStream,"%f %f", &x, &y);
			points[i] = point2( x , y);
		}
		setWindow(left, right, bottom,top);
		glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
		glDrawArrays( GL_LINE_STRIP, 0, countOfPointsForPolyline ); 
		glFlush();
	}
	fclose(inStream);
}

void shaderSetup(void)
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");

	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	colorChange(0);

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

//----------------------------------------------------------------------------
// glViewport for thumbs handler
void glViewportThumbs(int w, int h)
{

}

//----------------------------------------------------------------------------
// glViewport for main place handler
void glViewportMain(int w, int h)
{
	//keep the main panel is the center, and no distortion
	//when widht > height, keep height
	if(width/(height-0.075*width) > 1.33)
	{
		glViewport((width-1.067*(height-0.075*width))/2, 0 , 1.067*(height-0.075*width) , height-0.075*width);
	}
	else
	{
		glViewport(0, (height-0.825*width)/2 , width , 0.75*width);
	}
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	//Just display 10 thumbnails at the top, no other operations

    //glClear( GL_COLOR_BUFFER_BIT );     // clear the window, Comment it, otherwise, when left click, there will be obvious page refresh
	////////////////////////////////////////////////////////////////
	// Begin from here
	////////////////////////////////////////////////////////////////
	int thumbIndex = 0;
	printf("width=%d\theight=%d\n", width, height);

	for(thumbIndex = 0; thumbIndex < 10; thumbIndex++)
	{
		glViewport(width/10*thumbIndex,height-0.075*width,width/10 ,0.075*width);
		drawPolylineFile(fileName[thumbIndex]);
	}
	////////////////////////////////////////////////////////////////
	// End of buffer dealing
	////////////////////////////////////////////////////////////////
	
}

//----------------------------------------------------------------------------
// keyboard t event handler
void drawFiveTimesFive( void )
{
	//Draw 5*5 thumbnails in the main panel, with the file indexes stored in the randomArray
	for(int y = 0; y < 5; y++)
	{
		for(int x = 0; x < 5; x++)
		{
			//randNum = rand()%10;
			glViewport(106*(5-x)+20,364-(62*y),64 ,48);
			//when widht > height, keep height
			if(width/(height-0.075*width) > 1.33)
			{
				int w = (1.067*(height-0.075*width))/6;
				int h = (height-0.075*width)/6;
				int leftOffset = (width - 6*w)/2; 
				glViewport(leftOffset+w*x, 5*h-h*y , 0.8*w , 0.8*h);
			}
			else
			{
				int w = width/6;
				int h = width/8;
				int buttomOffset = (height-0.075*width-6*h)/2;
				glViewport(w*x, 5*h+buttomOffset-h*y , 0.8*w , 0.8*h);
			}
			drawPolylineFile(fileName[randomArray[y][x]]);
		}
	}
}

//----------------------------------------------------------------------------
// keyboard t event handler
void tEventHandler( void )
{
	// Generate the 5*5 random integer from (1,10), then call drawSixTimesSix( ) to draw thumbnails
	int x = 0;
	int y = 0;
	int randNum;
	srand(time(NULL));
	
	for(y = 0; y < 5; y++)
	{
		for(x = 0; x < 5; x++)
		{
			randNum = rand()%10;
			randomArray[y][x] = randNum;
		}
	}

	drawFiveTimesFive();
}

//----------------------------------------------------------------------------
// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	isBKeyPressed = 0; // Just in case
    switch ( key ) 
	{
		case 'p':
			currentKeyboardEvent = 'p';
			srand(time(NULL));
			//generate a random number to select a random file
			randNum = rand()%10;
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			drawPolylineFile(fileName[randNum]);
			isIndrawMode = 0;
			glutMouseFunc(myMouse);
			break;

		case 't':
			currentKeyboardEvent = 't';
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			tEventHandler();
			isIndrawMode = 0;
			break;

		case 'e':
			currentKeyboardEvent = 'e';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			isIndrawMode = 1;
			polylineIndex = 0;
			isBKeyPressed = 0;
			hasPrepoint = 0;
			// Begin of drawing mode
			glutMouseFunc(drawMode); //After this, you must reset: glutMouseFunc(myMouse);
			// End of drawing mode
			break;

		case 'b':
			currentKeyboardEvent = 'e';
			if (isIndrawMode == 1)
			{
				isBKeyPressed = 1;
				glutMouseFunc(drawMode);
			}
			break;

		case 'g':
			currentKeyboardEvent = 'g';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			gingerbreadMan();
			isIndrawMode = 0;
			break;

		case 'f':
			currentKeyboardEvent = 'f';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			fernFractal();
			//isIndrawMode = 0;
			break;

		case 'c':
			currentKeyboardEvent = 'c';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			cc++;
			colorChange(cc);
			isIndrawMode = 0;
			glutPostRedisplay();
			break;

		case 033:
			exit( EXIT_SUCCESS );
			break;

		default:
			isBKeyPressed = 0;
			isIndrawMode = 0;
    }
}

//----------------------------------------------------------------------------
// gingerbreadMan handler
void gingerbreadMan( void )
{
    int M = 40;
	int L = 3;
	int iterationTimes = 1000;
	point2 p = startPoint;
	for(int j = 0; j < iterationTimes; j++)
	{
		for(int i = 0; i < 3000; i++)
		{
			points[i].x = M*(1+2*L)-p.y+abs(p.x-L*M);
			points[i].y = p.x;
		
			p = points[i];
		}
		glViewport(0, 0 , width , height);
		setWindow(0, 640, 0, 480);
		glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
		glDrawArrays( GL_POINTS, 0, 3000 ); 
		glFlush();
	}
}

//----------------------------------------------------------------------------
// fernFractal handler
void fernFractal( void )
{
	float a[4] = { 0.0,0.2,-0.15,0.85 };
	float b[4] = { 0.0,0.23,0.26,-0.04 };
	float c[4] = { 0.0,-0.26,0.28,0.04 };
	float d[4] = { 0.16,0.22,0.24,0.85 };
	float tx[4] = { 0,0,0,0 };
	float ty[4] = { 0,1.6,0.44,1.6 };

	points[0] = point2(0, 0);
	int s;
	srand((unsigned)time(NULL));
	for (int j = 0; j < 200;j++) {
		for (int i = 1; i < 30000; i++)
		{
			s = rand() % 100 + 1;
			if (s == 1)
			{
				points[i].x = a[0] * points[i - 1].x + c[0] * points[i - 1].y + tx[0];
				points[i].y = b[0] * points[i - 1].x + d[0] * points[i - 1].y + ty[0];
			}
			else if (s > 1 && s <= 8)
			{
				points[i].x = a[1] * points[i - 1].x + c[1] * points[i - 1].y + tx[1];
				points[i].y = b[1] * points[i - 1].x + d[1] * points[i - 1].y + ty[1];
			}
			else if (s > 8 && s <= 15)
			{
				points[i].x = a[2] * points[i - 1].x + c[2] * points[i - 1].y + tx[2];
				points[i].y = b[2] * points[i - 1].x + d[2] * points[i - 1].y + ty[2];
			}
			else
			{
				points[i].x = a[3] * points[i - 1].x + c[3] * points[i - 1].y + tx[3];
				points[i].y = b[3] * points[i - 1].x + d[3] * points[i - 1].y + ty[3];
			}
		}

		glViewport(0, 0, width, height*0.8);
		//ortho = Ortho2D(-5, 5, 0, 10);
		ProjLoc = glGetUniformLocation(program, "Proj");
		glUniformMatrix4fv(ProjLoc, 1, GL_TRUE, ortho);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, 30000);
		glFlush();
	}
}

//----------------------------------------------------------------------------
// color change
void colorChange( int i )
{
	i = cc % 3;
	color4 colors[3] = {color4(1, 0, 0, 1), color4(0, 1, 0, 1), color4(0,0,1,1)};
	GLint vertexColorLocation = glGetUniformLocation(program, "rgbColor");
	glUseProgram(program);
	glUniform4f(vertexColorLocation, colors[i].x, colors[i].y, colors[i].z, 1.0f);
}

//----------------------------------------------------------------------------
// drawing handler
void drawMode(int button, int state, int x, int y)
{
	switch ( button ) 
	{
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) 
			{
				if(hasPrepoint == 0 || isBKeyPressed == 1)
				{
					if(hasPrepoint == 0)
					{
						originWidthForE = width;
						originHeightForE = height;
					}
					if(width/height > originWidthForE/originHeightForE)
					{
						ratio = 1.0 * originHeightForE / height;
					}
					else
					{
						ratio = 1.0 * originWidthForE / width;
					}
					pointsForDrawMode[1] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x , height - y); //point2( x*ratio , (height - y)*ratio); 
					//countOfPointsForPolyline[polylineIndex] = pointIndex;
					if(hasPrepoint != 0) 
					{
						polylineIndex++;
					}
					hasPrepoint = 1; 
					pointIndex = 0;
					
					pointsForPolyline[polylineIndex][pointIndex] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x*ratio , originHeightForE - y*ratio); //point2( x , height - y);
					pointIndex++;
					lastWidthForE = width;
					lastHeightForE = height;
				}
				else
				{
					if(width/height > originWidthForE/originHeightForE)
					{
						ratio = 1.0 * originHeightForE / height;
					}
					else
					{
						ratio = 1.0 * originWidthForE / width;
					}
					pointsForPolyline[polylineIndex][pointIndex] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x*ratio , originHeightForE - y*ratio); //point2( x , height - y);
					pointIndex++;

					countOfPointsForPolyline[polylineIndex] = pointIndex;

					pointsForDrawMode[0] = pointsForDrawMode[1];

					pointsForDrawMode[1] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x , height - y); //point2( x*ratio , (height - y)*ratio);  
					
					glViewport(0, 0 , width , height);
				
					setWindow(0, originWidthForE, 0, originHeightForE);
					glBufferData( GL_ARRAY_BUFFER, sizeof(pointsForDrawMode), pointsForDrawMode, GL_STATIC_DRAW );
					glDrawArrays( GL_LINE_STRIP, 0, 2 ); 
					glFlush();
					lastWidthForE = width;
					lastHeightForE = height;
				}
				isBKeyPressed = 0;
			}
			break;
	}
}

//----------------------------------------------------------------------------
// mouse handler
void myMouse(int button, int state, int x, int y)
{
	int index = 0;
	currentKeyboardEvent = 'p';
	switch ( button ) 
	{
		case GLUT_LEFT_BUTTON:
			// Judge whether the mouse point is clicked on thumb images
			if(state == GLUT_DOWN && y <= 0.075*width) 
			{
				index = 10*x/width;
				randNum = index; // for reshape use
				glClear( GL_COLOR_BUFFER_BIT );
				display();
				glViewportMain(width, height);
				drawPolylineFile(fileName[index]);
			};
			break;
		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN) glClear(GL_COLOR_BUFFER_BIT); // clear the window
			glFlush( );
			break;
		case GLUT_MIDDLE_BUTTON:
			exit( EXIT_SUCCESS );
			break;
    }
	
}

//----------------------------------------------------------------------------
// reshape handler
void myReshape(int reshapeWidth, int reshapeHeight )
{
	width = reshapeWidth;
	height = reshapeHeight;
	switch(currentKeyboardEvent)
	{
		case 'p':
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			drawPolylineFile(fileName[randNum]);
			break;

		case 't':
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			drawFiveTimesFive();
			break;

		case 'g':
			//glClear( GL_COLOR_BUFFER_BIT );
			gingerbreadMan();
			break;
		
		case 'f':
			//glClear(GL_COLOR_BUFFER_BIT);
			fernFractal();
			break;

		case 'e':
			isIndrawMode = 1;
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			//glViewport(0, 0 , width , height);
			int tempWidth, tempHeight;
			if(width/height > originWidthForE/originHeightForE)
			{
				tempWidth = height * originWidthForE/originHeightForE;
				tempHeight = height;
			}
			else
			{
				tempWidth = width;
				tempHeight = width * originHeightForE/originWidthForE ;
			}
			//maintain correct aspect ratio
			glViewport(0, 0 , tempWidth , tempHeight);
			setWindow(0, originWidthForE, 0, originHeightForE);
			for(int i =0; i <= polylineIndex; i++)
			{
				glBufferData( GL_ARRAY_BUFFER, sizeof(pointsForPolyline[0]), pointsForPolyline[i], GL_STATIC_DRAW );
				glDrawArrays( GL_LINE_STRIP, 0, countOfPointsForPolyline[i] ); 
				glFlush();
			}
			//glutMouseFunc(drawMode);
			break;

		default:
			glutDisplayFunc( display );
			glutKeyboardFunc( keyboard );
			glutMouseFunc(myMouse);
			myInit(  );
	}
}

//----------------------------------------------------------------------------
// vinci in the main screen, Draw "vinci" as default
void myInit( void )
{
	glViewport(0, 0 , width , height);
	drawPolylineFile("vinci.dat");
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );
    glutInitWindowSize( width, height );

	// create GLUT window for drawing
    glutCreateWindow( "Zhenyu Pan  CS543  HW1" );

	// init glew
    glewInit();
    initGPUBuffers( );
    shaderSetup( );
	myInit( );

	// assign default handlers
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	glutMouseFunc(myMouse);
	glutReshapeFunc(myReshape);

    glutMainLoop();
    return 0;
}
