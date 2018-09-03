There are 18 functions in the main source code file as following:

//Buffer and shader initialization
void initGPUBuffers( void );
void shaderSetup( void );

//Display just show 10 thmbnails at the header, and deal with the reshape
void display( void );

//Deal with Keyboard signal, then call different functions
void keyboard( unsigned char key, int x, int y );

//Deal with "T" event, generate 36 random numbers from 0 to 10, then call drawSixTimesSix() to draw images in main panel
void tEventHandler( void );

//Load dat files, then draw pplylines which will generate a image
void drawPolylineFile(char *);

//Set Ortho2D( ) for different iamges
void setWindow(GLfloat ,GLfloat ,GLfloat ,GLfloat );

//After "E" or "B" is pressed, it will go into this function to deal with mouse click
void drawMode(int , int , int , int);

//Intialization of mouse event function, deal with when click thumb, the big image will be displayed at main panel
void myMouse(int , int , int , int );

//Deal with "M" event, move a point
void mouseMotion(int, int, GLint  , GLint  );

//Deal with "D" event, delete a point
void deleteHandler( int , int , GLint  , GLint);

//Draw gingerbread man
void gingerbreadMan( void );

//Deal with reshap, I re-write each event in myReshape()
void myReshape(int , int  );

//Read hello.dat, and draw "Hello" in main panel
void myInit( void );

//Set glViewport for main panel, reshape use
void glViewportMain(int , int );

//Not used
void glViewportThunbs(int , int );

//It is so clear about this function, just draw 6*6 images at main panel
void drawSixTimesSix( void );

