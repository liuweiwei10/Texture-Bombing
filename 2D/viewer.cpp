////////////////////////////////////////////////////////////////////
//  example1.cpp : Modified from triangles.cpp from the
//		   OpenGL Red Book Chapter 1.
////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h> 
#include <string.h>
#include "SOIL.h"
#include <glm/glm.hpp>


using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#define PI 3.14159265

const GLuint Square = 0, NumVAOs = 1;
const GLuint NumBuffers = 4;
const GLuint vPosition = 0;
const GLuint vColor = 1;

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];


GLfloat r = 0, g = 0, b = 1;
GLuint Image;
GLuint Random;
GLuint Choices[5];
char* fragShaderName;
glm::vec2 SCREEN_SIZE(512, 512);

GLuint loadTexture(char*);
void usage();




////////////////////////////////////////////////////////////////////
//  init
////////////////////////////////////////////////////////////////////

void init (void )
{

    GLfloat vertices[6][2] = {
                { -0.90f, -0.90f },      // square
                { -0.90f, 0.90f },
                {  0.90f, 0.90f },
                {  0.90f, 0.90f },
                {  0.90f, -0.9f },
                { -0.90f, -0.90f }      
    };
	
	glGenVertexArrays( NumVAOs, VAOs );
	glGenBuffers( NumBuffers, Buffers );

	glBindVertexArray( VAOs[Square] );
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[Square] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray( vPosition );
    glBindVertexArray(0);
    char* filePath;
    filePath = "./star.png";
    Image = loadTexture(filePath);
    filePath = "./random.bmp";
    Random = loadTexture(filePath);   

    if(strcmp(fragShaderName, "7.frag") == 0) {
        Choices[0] = loadTexture("./minion1.png");
        Choices[1] = loadTexture("./minion2.png");
        Choices[2] = loadTexture("./minion3.png");
        Choices[3] = loadTexture("./minion4.png");
        Choices[4] = loadTexture("./minion5.png");
    } else {
        Choices[0] = loadTexture("./fruit1.png");
        Choices[1] = loadTexture("./fruit2.png");
        Choices[2] = loadTexture("./fruit3.png");
        Choices[3] = loadTexture("./fruit4.png");
        Choices[4] = loadTexture("./fruit5.png");
    }

}

void reshape(int w, int h)
{
    if(h == 0)                 
        h = 1; 
    glViewport(0, 0, w, h); 
    SCREEN_SIZE.x = w;
    SCREEN_SIZE.y = h;   
}

////////////////////////////////////////////////////////////////////
//  display
////////////////////////////////////////////////////////////////////

void display (void )
{
	glClearColor(0, 43/255.0, 54/255.0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, fragShaderName },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders( shaders );
    glUseProgram( program );
    glUniform3f(glGetUniformLocation(program, "uColor"), r, g, b);

    glUniform1i(glGetUniformLocation(program, "imageTex"), 0);
    glUniform1i(glGetUniformLocation(program, "randomTex"), 1);
    glUniform1i(glGetUniformLocation(program, "choiceTex1"), 2);
    glUniform1i(glGetUniformLocation(program, "choiceTex2"), 3);
    glUniform1i(glGetUniformLocation(program, "choiceTex3"), 4);
    glUniform1i(glGetUniformLocation(program, "choiceTex4"), 5);
    glUniform1i(glGetUniformLocation(program, "choiceTex5"), 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Image);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Random);


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Choices[0]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, Choices[1]);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Choices[2]);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, Choices[3]);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, Choices[4]);


	glBindVertexArray( VAOs[Square] );
    glDrawArrays(GL_TRIANGLES, 0, 6);

	glFlush();
}

void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
        case 'q':
            exit(0);
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    if(argc != 3 || strcmp(argv[1], "-f") != 0) {
        usage();
        return 1;
    }

    fragShaderName = argv[2];

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA );
    glutInitWindowSize( SCREEN_SIZE.x, SCREEN_SIZE.y );

	//glutInitContextVersion( 4, 3 );
	//glutInitContextProfile( GLUT_CORE_PROFILE );

	glutCreateWindow( argv[0] );

	glewExperimental = GL_TRUE;	// added for glew to work!

	if ( glewInit() )
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit (EXIT_FAILURE );
	}

	// Code from OpenGL 4 Shading Language Cookbook, 2nd Ed.

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString (GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv( GL_MAJOR_VERSION, &major );
	glGetIntegerv( GL_MINOR_VERSION, &minor );

	cout << "GL Vendor            :" << vendor << endl;
	cout << "GL Renderer          :" << renderer  << endl;
	cout << "GL Version (string)  :" << version << endl;
	cout << "GL Version (integer) :" << major << " " << minor << endl;
	cout << "GLSL Version         :" << glslVersion << endl;

	/*
	GLint nExtensions;
	glGetIntegerv( GL_NUM_EXTENSIONS, &nExtensions );
	for ( int i = 0; i < nExtensions; i++ )
		cout << glGetStringi( GL_EXTENSIONS, i )  << endl;
	*/
	

	init();
	glutDisplayFunc( display );
	glutKeyboardFunc(myKeyboardFunc);
    glutReshapeFunc( reshape);

	glutMainLoop();

	return 0;
}

 
GLuint loadTexture(char* textureFileName)
{   
    cout << "loading texture:" << textureFileName << "\n";
	unsigned char* image;
	int width; 
	int height; 
	int channels;

    image = SOIL_load_image(textureFileName, &width, &height, &channels, SOIL_LOAD_AUTO);//use SOIL to read image file
    
    if (image == NULL)
    {
        std::cerr << "An error occurred while loading image." << std::endl;
        return -1;
    }


    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    // flip image
    for (int j = 0; j * 2 < height; ++j)
    {
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;
		for (int i = width * channels; i > 0; --i)
		{
			unsigned char temp = image[index1];
			image[index1] = image[index2];
			image[index2] = temp;
			++index1;
			++index2;
		}
	}

    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    if(!strstr(textureFileName, "random")){
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    } else {
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    SOIL_free_image_data(image);// free the array SOIL used to store the image data

    return tex;
}


void usage()
{
    std::cout << "Usage:\n" << "    ./viewer -f triangle.frag\n";

}




