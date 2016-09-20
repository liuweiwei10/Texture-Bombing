#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h> 

using namespace std;

// includes for glm 
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

// include glm extensions
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "vgl.h"
#include "LoadShaders.h"
#include "loadObj.h"
#include "SOIL.h"

#define PI 3.14159265

using namespace glm;


const GLuint vPosition = 0;
const GLuint vColor = 1;
const GLuint Num = 10;
const GLfloat Degree = 1.0f * PI / 180.0f;
const GLuint IS_DEFAULT_VIEW = 0; // key p
const GLuint IS_DOLLY_IN = 1; // key w
const GLuint IS_DOLLY_OUT = 2; // key s
const GLuint IS_TRUCK_LEFT = 3; // key a 
const GLuint IS_TRUCK_RIGHT = 4; // key d
const GLuint IS_TILT_DOWN = 5; // key q
const GLuint IS_TILT_UP = 6; // key e
const GLuint IS_ROTATE_CAMERA_CLOCKWISE = 7; // key z
const GLuint IS_ROTATE_CAMERA_COUNTER_CLOCKWISE = 8; // key x
const GLuint IS_CAMERA_UP = 9; // key r
const GLuint IS_CAMERA_DOWN = 10; // key t
const GLuint IS_ROTATE_VIEWUP_CLOCKWISE = 11; //key c
const GLuint IS_ROTATE_VIEWUP_COUNTER_CLOCKWISE = 12; //key v
const GLuint IS_SET_CAMERA = 13; //key j
const GLuint IS_SET_CENTER = 14; //key k
const GLuint IS_SET_VIEWUP = 15; //key l
const GLuint IS_UNCHANGED = 16; 

GLfloat Step = 0.1f;
GLuint objectCount = 0;
GLuint VAOs[Num];
GLuint VAOsizes[Num];
GLfloat bounds[Num][6];
char fileNames[Num][128];
GLfloat maxDimension;
GLboolean isSolid = GL_TRUE;
GLuint action = IS_DEFAULT_VIEW;
GLuint focalLen;
GLfloat tiltedDegree = .0f;

GLfloat xCenter = .0f;
GLfloat yCenter = .0f;
GLfloat zCenter = .0f;

GLuint Random;
GLuint Image;

glm::vec3 defaultEye;
glm::vec3 defaultCenter;
glm::vec3 defaultViewUp;

glm::vec3 curEye;
glm::vec3 curCenter;
glm::vec3 curViewUp;

glm::vec3 gaze;

struct VertexData {
    GLubyte color[4];
    GLfloat position[4];
};

GLfloat r = 0, g = 0, b = 1;
glm::mat4 modelMatrix[Num];
glm::mat4 viewMatrix, projectionMatrix;
glm::vec2 SCREEN_SIZE(800, 600);

void myKeyboardFunc(unsigned char, int, int);
GLboolean isValidPoint(GLfloat);
void changeColor(void);
int readControlFile(char*);
void printMatrix(glm::mat4);
glm::vec3 getDefaultEye();
glm::vec3 getDefaultCenter();
glm::vec3 getDefaultViewUp();
void setDefaultProjectionMatrix();
glm::vec3 moveVec(glm::vec3, GLfloat, GLfloat, GLfloat);
glm::vec3 inputVector(string); 
GLuint loadTexture(char*);
void usage();





////////////////////////////////////////////////////////////////////
//  init
////////////////////////////////////////////////////////////////////

void init (void )
{

    GLfloat xRange = .0f;
    GLfloat yRange = .0f;
    GLfloat zRange = .0f;

    //cout << "objectCount:" <<  objectCount << endl;

    for(int i = 0; i < objectCount; i++) {
    	char* fileName = fileNames[i];
    	//cout << "fileName:" << fileName<< endl;
        loadObjFile(fileName, bounds[i], &VAOs[i], &VAOsizes[i] );
        //cout << "model matrix "<< i << endl;
        //printMatrix(modelMatrix[i]);
        xRange =  glm::max(xRange, (bounds[i][1] - bounds[i][0]));
        yRange =  glm::max(yRange, (bounds[i][3] - bounds[i][2]));
        zRange =  glm::max(zRange, (bounds[i][5] - bounds[i][4]));
        xCenter += 0.5 * (bounds[i][1] + bounds[i][0]);
        yCenter += 0.5 * (bounds[i][3] + bounds[i][2]);
        zCenter += 0.5 * (bounds[i][5] + bounds[i][4]);
    }
    xCenter = xCenter / objectCount;
    yCenter = yCenter / objectCount;
    zCenter = zCenter / objectCount;

    maxDimension = glm::max(xRange, yRange);
    maxDimension = glm::max(maxDimension, zRange);
    Step = 0.03f * maxDimension;
    defaultEye = getDefaultEye();
    defaultCenter = getDefaultCenter();
    defaultViewUp = getDefaultViewUp();
    focalLen = glm::sqrt( glm::pow((defaultEye.x - defaultCenter.x), 2)  + glm::pow((defaultEye.y - defaultCenter.z), 2) + glm::pow((defaultEye.z - defaultCenter.z), 2));

    char* filePath = "./random.bmp";
    Random = loadTexture(filePath);
}


////////////////////////////////////////////////////////////////////
//  display
////////////////////////////////////////////////////////////////////

void display (void )
{
	glClear( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    GLfloat deltaX;
    GLfloat deltaY;
    GLfloat deltaZ;

    switch(action)
	{ 
	    case 0:  
	    	curEye = defaultEye;
    		curCenter = defaultCenter;
    		curViewUp = defaultViewUp;
        	break;
        case 1: // dolloy in 
            gaze = curCenter - curEye;
            curEye = glm::vec3(glm::translate(glm::mat4(1.0f), Step * gaze) * vec4(curEye, 1.0f));    
            curCenter = glm::vec3(glm::translate(glm::mat4(1.0f), Step * gaze) * vec4(curCenter, 1.0f)); 	
      
            //if(curEye.x >= 0) 
            //	deltaX = Step;
            //else deltaX = -Step;
            //if(curEye.y >= 0) 
            //	deltaY = Step;
            //else deltaY = -Step;
            //if(curEye.z >= 0) 
            //	deltaZ = Step;
            //else deltaZ = -Step;
            //abs(curEye.x) + Step;
            //curEye = moveVec(curEye, -deltaX, -deltaY, -deltaZ);
            //curCenter = moveVec(curCenter, -deltaX, -deltaY, -deltaZ);		
        	break;
        case 2: // dolly out
            gaze = curCenter - curEye;
            curEye = glm::vec3(glm::translate(glm::mat4(1.0f), -Step * gaze) * vec4(curEye, 1.0f));    
            curCenter = glm::vec3(glm::translate(glm::mat4(1.0f), -Step * gaze) * vec4(curCenter, 1.0f)); 	
            //if(curEye.x >= 0) 
            //	deltaX = Step;
            //else deltaX = -Step;
            //if(curEye.y >= 0) 
            //	deltaY = Step;
            //else deltaY = -Step;
            //if(curEye.z >= 0) 
            //	deltaZ = Step;
            //else deltaZ = -Step;
            //curEye = moveVec(curEye, deltaX, deltaY, deltaZ);
            //curCenter = moveVec(curCenter, deltaX, deltaY, deltaZ);
        	break;
        case 3: //truck left
            curEye = moveVec(curEye, Step, -Step, 0);
    	 	curCenter = moveVec(curCenter, Step, -Step, 0);
        	break;
        case 4: //truck right
            curEye = moveVec(curEye, -Step, Step, 0);
    	 	curCenter = moveVec(curCenter, -Step, Step, 0);
        	break;
        case 5: //tilt down
    	 	gaze = curCenter - curEye;
    	 	gaze = glm::mat3(glm::rotate(glm::mat4(1.0f), Degree, vec3(curEye.x, -curEye.y, 0))) * gaze;
    	 	curCenter = gaze + curEye;
        	break;
        case 6: // tilt up
            //tiltedDegree = tiltedDegree + Degree;
            //deltaX = - glm::sqrt(2) * 0.5 * focalLen * (cos(tiltedDegree) -1);
            //deltaY = deltaX;
            //deltaZ = focalLen * sin(tiltedDegree);
    	 	//curCenter = moveVec(defaultCenter, deltaX, deltaY, deltaZ);
    	    gaze = curCenter - curEye;
    	 	gaze = glm::mat3(glm::rotate(glm::mat4(1.0f), -Degree, vec3(curEye.x, -curEye.y, 0))) * gaze;
    	 	curCenter = gaze + curEye;
        	break;
        case 7: // rotate eye position/focal point around view up vector clockwise                    	 
            curEye = glm::mat3(glm::rotate(glm::mat4(1.0f), -Degree, curViewUp)) * curEye;
            curCenter = glm::mat3(glm::rotate(glm::mat4(1.0f), -Degree, curViewUp)) * curCenter;
            break;
        case 8: // rotate eye position/focal point around view up vector counter-clockwise
            curEye = glm::mat3(glm::rotate(glm::mat4(1.0f), Degree, curViewUp)) * curEye;
            curCenter =glm::mat3(glm::rotate(glm::mat4(1.0f), Degree, curViewUp)) * curCenter;
            break;
        case 9: // move camera up
            curEye = moveVec(curEye, 0, 0, Step);
            curCenter = moveVec(curCenter, 0, 0, Step);
            break;
        case 10: // move camera down
            curEye = moveVec(curEye, 0, 0, -Step);
            curCenter = moveVec(curCenter, 0, 0, -Step);
            break;
        case 11: // rotate viewup around gase vector clockwise
            gaze = curCenter - curEye;
            curViewUp = glm::mat3(glm::rotate(glm::mat4(1.0f), -Degree, gaze)) * curViewUp;
            break;
        case 12: // rotate viewup around gase vector clockwise
            gaze = curCenter - curEye;
            curViewUp = glm::mat3(glm::rotate(glm::mat4(1.0f), Degree, gaze)) * curViewUp;
            break;
        case 13: // set camera
            curEye = inputVector("Please input the camera position, e.g. 3 3 3");
            break;
        case 14: // set center
            curCenter = inputVector("Please input the focal point, e.g. 0 0 0");
            break;
        case 15: // set viewup 
            curViewUp = inputVector("Please input the viewup  vector, e.g. 0 1 0");
            break;
        case 16: // unchanged
            break;
        default:
	    	curEye = defaultEye;
    		curCenter = defaultCenter;
    		curViewUp = defaultViewUp;
            break;	    	
	}
	viewMatrix = glm::lookAt(curEye, curCenter, curViewUp);
    setDefaultProjectionMatrix(); 
    action = IS_UNCHANGED;

    glUniform1i(glGetUniformLocation(program, "randomTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Random);
  
	for(int i = 0; i < objectCount ; i++) {		
		//cout << "drawing object " <<  i << "....."<<endl;
   		GLuint modelID = glGetUniformLocation(program, "model");
    	GLuint viewID = glGetUniformLocation(program, "view");
    	GLuint projectionID = glGetUniformLocation(program, "projection");
    	glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(modelMatrix[i]));
    	printMatrix(modelMatrix[i]);
    	glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projectionMatrix));   

		glBindVertexArray( VAOs[i] );
		glDrawArrays(GL_TRIANGLES, 0, VAOsizes[i]);	

	}
	
	glFlush();
}

void reshape(int w, int h)
{
	if(h == 0)                 
    	h = 1; 
    glViewport(0, 0, w, h); 
    SCREEN_SIZE.x = w;
    SCREEN_SIZE.y = h;   
}


////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{

    if(argc != 3 || strcmp(argv[1], "-c") != 0) {
        usage();
        return 1;
    }

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DEPTH );
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
    readControlFile(argv[2]);
	init();
	glutDisplayFunc( display );
	glutReshapeFunc( reshape);
	glutKeyboardFunc(myKeyboardFunc);

	glutMainLoop();

	return 0;
}

int readControlFile(char *fileName) 
{
 	FILE * myFile = fopen(fileName, "r");
 	char head[128];
    while (true) {
		int result = fscanf(myFile, "%s", head);
		if (result == EOF) {
			break;
		}
		if (strcmp(head, "obj" ) == 0) {			    	            
			objectCount++;
			if(objectCount > Num) {
				cout << "error: too many objects!" << endl;
				return 0;
 			}
			char fileName[128];
			fscanf(myFile, " %s ", fileName);
			strcpy(fileNames[objectCount - 1] , fileName);
			modelMatrix[objectCount - 1] = glm::mat4(1.0f); //initiate modelMatrix
   		} else if( strcmp(head, "s" ) == 0 ) {
   			glm::vec3 scale;
            fscanf(myFile, "%f %f %f ",&scale.x, &scale.y, &scale.z);
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
   			modelMatrix[objectCount - 1] = scaleMatrix * modelMatrix[objectCount - 1] ;  			
    	} else if( strcmp(head, "t" ) == 0 ) {
   			glm::vec3 translate;
            fscanf(myFile, "%f %f %f ",&translate.x, &translate.y, &translate.z);
   			glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), translate);
   			modelMatrix[objectCount - 1] = transMatrix * modelMatrix[objectCount - 1];  			  			
    	} else if(strcmp(head, "rx" ) == 0) {
   			GLfloat rx;
            fscanf(myFile, "%f", &rx);
            rx = rx * PI / 180.0f;
            glm::mat4 rotateXMatrix = glm::rotate(glm::mat4(1.0f), rx, glm::vec3(1, 0, 0));
            modelMatrix[objectCount - 1] = rotateXMatrix * modelMatrix[objectCount - 1];  			  			

   			//modelMatrix[objectCount-1] = glm::rotate(modelMatrix[objectCount-1], rx, glm::vec3(1, 0, 0));   			
    	} else if(strcmp(head, "ry" ) == 0) {
   			GLfloat ry;
            fscanf(myFile, "%f", &ry);
            ry = ry * PI / 180.0f;
   			glm::mat4 rotateYMatrix = glm::rotate(glm::mat4(1.0f), ry, glm::vec3(0, 1, 0)); 
   			modelMatrix[objectCount - 1] = rotateYMatrix * modelMatrix[objectCount - 1];  			  			
  			
    	} else if(strcmp(head, "rz" ) == 0) {
   			GLfloat rz;
            fscanf(myFile, "%f", &rz);
            rz = rz * PI / 180.0f;
   			glm::mat4 rotateZMatrix = glm::rotate(glm::mat4(1.0f), rz, glm::vec3(0, 0, 1));  
   			modelMatrix[objectCount - 1] = rotateZMatrix * modelMatrix[objectCount - 1];  			  			
 			
    	} 
    }
 	return 1;    
}
////////////////////////////////////////////////////////////////////
//  MyKeyboardFun
////////////////////////////////////////////////////////////////////
void myKeyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{ 
	    case 27:  
	    	exit(0); 
	    	break;//quit with "ESC"
	    case 'o': // toggle between wireframe and solid
	        action = IS_UNCHANGED;
	        isSolid = !isSolid;
	        if(isSolid) {
	            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	    		glutPostRedisplay();
	        } else {
	        	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	        	glutPostRedisplay();
	        }
	        break;
	    case 'p': //reset view to default values
	    	action = IS_DEFAULT_VIEW;
	    	glutPostRedisplay();
	    	break;
	    case 'w': //dolly in
            action = IS_DOLLY_IN;
           	glutPostRedisplay();
	        break;
 		case 's': //dolly out
 			action = IS_DOLLY_OUT;
           	glutPostRedisplay();
	        break;
	    case 'a': //truck left
	        action = IS_TRUCK_LEFT;
           	glutPostRedisplay();
	        break;
	    case 'd': //truck right
	        action = IS_TRUCK_RIGHT;
	        glutPostRedisplay();
	        break;
	    case 'q': //tilt down
	        action = IS_TILT_DOWN;
	        glutPostRedisplay();
	        break;
	    case 'e': //tilt up
	        action = IS_TILT_UP;
	        glutPostRedisplay();
	        break;
	    case 'z':
	        action = IS_ROTATE_CAMERA_CLOCKWISE;
	        glutPostRedisplay();
	        break;
	    case 'x':
	        action = IS_ROTATE_CAMERA_COUNTER_CLOCKWISE;
	        glutPostRedisplay();
	        break;
	    case 'r':
	        action = IS_CAMERA_UP;
	        glutPostRedisplay();
	        break;
	    case 't':
	        action = IS_CAMERA_DOWN;
	        glutPostRedisplay();
	        break;
	    case 'c':
	        action = IS_ROTATE_VIEWUP_CLOCKWISE;
	        glutPostRedisplay();
	        break;
	    case 'v':
	        action = IS_ROTATE_VIEWUP_COUNTER_CLOCKWISE;
	        glutPostRedisplay();
	        break;
		case 'j':
	        action = IS_SET_CAMERA;
	        glutPostRedisplay();
	        break;
		case 'k':
	        action = IS_SET_CENTER;
	        glutPostRedisplay();
	        break;
		case 'l':
		    action = IS_SET_VIEWUP;
	        glutPostRedisplay();
	        break;
	    default:
	    	break;
	}
}


GLboolean isValidPoint(GLfloat value) 
{
    GLboolean result = GL_TRUE;
	if(value > 1 || value < 0) {
		cout << "error: parameter out of range, should lie in between [0, 1]!!" << endl;
		result = GL_FALSE;
	}
	return result;
}

void printMatrix(glm::mat4 matrix) {
	for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			cout << matrix[k][j] << " ";
			cout << endl;;
		}
}

glm::vec3 getDefaultEye() 
{
	glm::vec3 eye(3, 3, 3);
//	glm::vec3 eye(3.0f * maxDimension, 3.0f * maxDimension, 1.0f * maxDimension);
	return eye;
}

glm::vec3 getDefaultCenter() 
{   
    glm::vec3 center(0, 0, 0);
    //glm::vec3 center(xCenter, yCenter, zCenter);
    return center;
}

glm::vec3 getDefaultViewUp() 
{
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    return up;
}

void setDefaultProjectionMatrix() 
{
	float nearDist = 0.5f;
    float farDist = 100.f;
    projectionMatrix = glm::perspective(45.0f, (float)SCREEN_SIZE.x / (float)SCREEN_SIZE.y, nearDist, farDist);
}

glm::vec3 moveVec(glm::vec3 vector, GLfloat deltaX, GLfloat deltaY, GLfloat deltaZ)
{
    glm::vec3 newVector;
    newVector.x = vector.x + deltaX;
    newVector.y = vector.y + deltaY;
    newVector.z = vector.z + deltaZ;
    return newVector;
}

glm::vec3 inputVector(string message) 
{
	glm::vec3 newVector;
	cout << message << endl;
	cin >>  newVector.x >> newVector.y >> newVector.z;
    while(cin.fail()) {
    	cout << "error: invalid input."<< endl <<message << endl;
        cin.clear();
        cin.ignore(256,'\n');
	    cin >>  newVector.x >> newVector.y >> newVector.z;
    }
    return newVector;
}

GLuint loadTexture(char* textureFileName)
{   
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

    std::cout << "Usage:\n" << "    ./viewer -c controlFile\n";

}


