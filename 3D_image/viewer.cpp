#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h> 
#include "SOIL.h"

using namespace std;

// includes for glm 
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

// include glm extensions
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "vgl.h"
#include "LoadShaders.h"
#include "loadObj3.h"

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
const GLuint MAXLIGHTS = 4;

GLfloat Step = 0.1f;
GLuint objectCount = 0;
GLuint VAOs[Num];
GLuint VAOsizes[Num];
GLfloat bounds[Num][6];
char fileNames[Num][128];
GLfloat maxDimension = .0f;
GLboolean isSolid = GL_TRUE;
GLuint action = IS_DEFAULT_VIEW;
GLuint focalLen;
GLfloat tiltedDegree = .0f;

GLfloat xCenter = .0f;
GLfloat yCenter = .0f;
GLfloat zCenter = .0f;

GLfloat xRange = .0f;
GLfloat yRange = .0f;
GLfloat zRange = .0f;

glm::vec3 defaultEye;
glm::vec3 defaultCenter;
glm::vec3 defaultViewUp;

glm::vec3 curEye;
glm::vec3 curCenter;
glm::vec3 curViewUp;

glm::vec3 gaze;

GLuint program;

struct VertexData {
    GLubyte color[4];
    GLfloat position[4];
};

GLfloat r = 0, g = 0, b = 1;
glm::mat4 modelMatrix[Num];
glm::mat4 viewMatrix, projectionMatrix;
glm::vec2 SCREEN_SIZE(800, 600);

//objInfo *theseObjects[Num];
//int numObj[Num];
vector<objInfo> objInfoList;
lightProperties lights[MAXLIGHTS];

GLuint Image;
GLuint Random;

char* controlFileName;

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
void viewUsage();
void lightUsage(); 
void printLights();
void initializeLights();
void printView();
void usage();
GLuint loadTexture(char*);




////////////////////////////////////////////////////////////////////
//  init
////////////////////////////////////////////////////////////////////

void init (void )
{

    //cout << "objectCount:" <<  objectCount << endl;

    for(int i = 0; i < objectCount; i++) {
    	char* fileName = fileNames[i];
    	//cout << "fileName:" << fileName<< endl;
        objInfo *theseObjects = NULL;
        int num;
        loadObjFile(fileName, &theseObjects, &num);
        for(int j = 0; j < num; j++) {
            theseObjects[j].modelMatrix = modelMatrix[i];
            objInfoList.push_back(theseObjects[j]);
        }
    }

    focalLen = glm::sqrt( glm::pow((defaultEye.x - defaultCenter.x), 2)  + glm::pow((defaultEye.y - defaultCenter.z), 2) + glm::pow((defaultEye.z - defaultCenter.z), 2));
              
    viewMatrix = glm::lookAt(defaultEye, defaultCenter, defaultViewUp);

    ShaderInfo  shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" },
        { GL_NONE, NULL }
    };

    program = LoadShaders( shaders );
    glUseProgram( program );

    char* filePath = "./random.bmp";
    Random = loadTexture(filePath);
    if(strcmp(controlFileName, "control1.txt") == 0) {
        Image = loadTexture("./fish.png");
    } else {
        Image = loadTexture("./rose.png");
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
    glClear( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);

    GLfloat deltaX;
    GLfloat deltaY;
    GLfloat deltaZ;

    switch(action)
	{ 
	    case 0:  
	    	curEye = defaultEye;
            cout << "eye:(" << curEye.x << ", " <<  curEye.y << ", " << curEye.z << ")\n";
    		curCenter = defaultCenter;
            cout << "center:(" << curCenter.x << ", " <<  curCenter.y << ", " << curCenter.z << ")\n";
    		curViewUp = defaultViewUp;
            cout << "viewup:(" << curViewUp.x << ", " <<  curViewUp.y << ", " << curViewUp.z << ")\n";

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


    
    for(int j = 0; j < MAXLIGHTS; j++) {
        string name = "Lights[";
        name.append(std::to_string(j)).append("]");
        string prop = name;
        GLuint enableID = glGetUniformLocation(program, prop.append(".isEnabled").c_str());
        glUniform1i(enableID, lights[j].isEnabled); 
        prop = name;
        GLuint localID = glGetUniformLocation(program, prop.append(".isLocal").c_str());
        glUniform1i(localID, lights[j].isLocal); 
        prop = name;
        GLuint spotID = glGetUniformLocation(program, prop.append(".isSpot").c_str());
        glUniform1i(spotID, lights[j].isSpot); 
        prop = name;
        GLuint ambientID = glGetUniformLocation(program, prop.append(".ambient").c_str());
        glUniform3fv(ambientID, 1, glm::value_ptr(lights[j].ambient)); 
        prop = name;
        GLuint colorID = glGetUniformLocation(program, prop.append(".color").c_str());
        glUniform3fv(colorID, 1, glm::value_ptr(lights[j].color)); 
        prop = name;
        GLuint positionID = glGetUniformLocation(program, prop.append(".position").c_str());
       // glUniform3fv(positionID, 1, glm::value_ptr( lights[j].position)); 

        //change position to eye coordinates
        vec3 newPosition;
        if (lights[j].isLocal ) {
            newPosition = glm::vec3(viewMatrix * glm::vec4(lights[j].position, 1));
        } else {
            newPosition = glm::mat3(viewMatrix) * lights[j].position;
        }
        glUniform3fv(positionID, 1, glm::value_ptr( newPosition)); 


        prop = name;
        GLuint halfVectorID = glGetUniformLocation(program, prop.append(".halfVector").c_str());
        glUniform3fv(halfVectorID, 1, glm::value_ptr(lights[j].halfVector)); 
        prop = name;
        GLuint coneDirectionID = glGetUniformLocation(program, prop.append(".coneDirection").c_str());    
        if(lights[j].isSpot) {
            vec3 newConeDirection = glm::mat3(viewMatrix) * lights[j].coneDirection;
            glUniform3fv(coneDirectionID, 1, glm::value_ptr(newConeDirection)); 
        } else {
            glUniform3fv(coneDirectionID, 1, glm::value_ptr(lights[j].coneDirection)); 
        }
        prop = name;
        GLuint spotCosCutoffID = glGetUniformLocation(program, prop.append(".spotCosCutoff").c_str());
        glUniform1f(spotCosCutoffID, lights[j].spotCosCutoff); 
        prop = name;
        GLuint spotExponentID = glGetUniformLocation(program, prop.append(".spotExponent").c_str());
        glUniform1f(spotExponentID, lights[j].spotExponent) ; 
        prop = name;
        GLuint constantAttenuationID = glGetUniformLocation(program, prop.append(".constantAttenuation").c_str());
        glUniform1f(constantAttenuationID, lights[j].constantAttenuation) ; 
        prop = name;
        GLuint linearAttenuationID = glGetUniformLocation(program, prop.append(".linearAttenuation").c_str());
        glUniform1f(linearAttenuationID, lights[j].linearAttenuation) ; 
        prop = name;
        GLuint quadraticAttenuationID = glGetUniformLocation(program, prop.append(".quadraticAttenuation").c_str());
        glUniform1f(quadraticAttenuationID, lights[j].quadraticAttenuation) ; 
    } 

    glUniform1i(glGetUniformLocation(program, "imageTex"), 0);
    glUniform1i(glGetUniformLocation(program, "randomTex"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Image);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Random);


    for(int i = 0; i < objInfoList.size() ; i++) { 
       
        GLuint modelID = glGetUniformLocation(program, "model");
        GLuint viewID = glGetUniformLocation(program, "view");
        GLuint projectionID = glGetUniformLocation(program, "projection");
        GLuint ambientID = glGetUniformLocation(program, "ambient");
        GLuint diffuseID = glGetUniformLocation(program, "diffuse");
        GLuint specularID = glGetUniformLocation(program, "specular");
        GLuint shininessID = glGetUniformLocation(program, "shininess");
        //GLuint texID = glGetUniformLocation(program, "tex");
        GLuint istextureID = glGetUniformLocation(program, "istexture");

        glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(objInfoList[i].modelMatrix));
        // printMatrix(modelMatrix[i]);
        glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
          
        glUniform3fv(ambientID, 1, glm::value_ptr(objInfoList[i].Ka));
        glUniform3fv(diffuseID, 1, glm::value_ptr(objInfoList[i].Kd));
        glUniform3fv(specularID, 1, glm::value_ptr(objInfoList[i].Ks));
        glUniform1f(shininessID, objInfoList[i].n);
          
        //glUniform1i(texID, objInfoList[i].diffuseTexMapID); 
        //glUniform1i(texID, 0); 

        //if(objInfoList[i].diffuseTexMapID != -1) {
            //std::cout << " is texture map " << objInfoList[i].diffuseTexMapID << "\n" ;
        //    glUniform1i(istextureID, 1);
        //} else {
        //    glUniform1i(istextureID, 0);
            //std::cout << " is not texture map\n" ;
        //}
        //glBindTexture(GL_TEXTURE_2D, objInfoList[i].diffuseTexMapID);
        glBindVertexArray(objInfoList[i].VAO); 
        glDrawArrays(GL_TRIANGLES, 0, objInfoList[i].VAOsize); 
	}
	
	glFlush();
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
   
    glEnable (GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

    controlFileName =  argv[2];
    initializeLights();
    if( readControlFile(argv[2]) <= 0 ) {
        usage();
        return 1;
    }
    //printLights();
    //printView();
	init();
	glutDisplayFunc( display );
	glutReshapeFunc( reshape);
    glutReshapeFunc( reshape);
	glutKeyboardFunc(myKeyboardFunc);

	glutMainLoop();

	return 0;
}

int readControlFile(char *fileName) 
{
 	FILE * myFile = fopen(fileName, "r");
    if (myFile == NULL) {
        cerr << "ERROR:cannot open file " << fileName  << "!\n";
        return 0;
    }
 	char head[128];
    bool isView = false;
    bool isLight = false;
    int lightCount = 0;
    while (true) {
		int result = fscanf(myFile, "%s", head);
		if (result == EOF) {
			break;
		}
		if (strcmp(head, "obj" ) == 0) {			    	            
			objectCount++;
			if(objectCount > Num) {
				cerr << "error: too many objects!" << endl;
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
    	} else if(strcmp(head, "view") == 0) {
            char camera[128];
            char focal[128];
            char viewup[128];
            vec3 eye;
            vec3 center;
            vec3 up;
            fscanf(myFile, "%s %f %f %f %s %f %f %f %s %f %f %f", 
                   camera, &eye.x, &eye.y, &eye.z,
                   focal, &center.x, &center.y, &center.z,
                   viewup, &up.x, &up.y, &up.z);
            if(strcmp(camera, "camera" ) != 0 || strcmp(focal, "focal") != 0 || strcmp(viewup, "viewup") != 0  ) {
                cerr << "Wrong view values in control file! \n";
                viewUsage();
                return 0;
            }
            defaultEye = eye;
            defaultCenter = center;
            defaultViewUp = up;
            isView = true;
        } else if(strcmp(head, "light") == 0) {
            lightCount++;
            char lightType[128];
            fscanf(myFile, "%s", lightType);
            if(lightCount >= 4) {
                continue;
            }

            //get ambient, color, position
            char ambient[128];
            char color[128];
            char position[128];
            vec3 ambientVec;
            vec3 colorVec;
            vec3 positionVec;
            fscanf(myFile, "%s %f %f %f %s %f %f %f %s %f %f %f", 
                   ambient, &ambientVec.x, &ambientVec.y, &ambientVec.z,
                   color, &colorVec.x, &colorVec.y, &colorVec.z,
                   position, &positionVec.x, &positionVec.y, &positionVec.z);
            if(strcmp(ambient, "ambient" ) != 0 || strcmp(color, "color") != 0 || strcmp(position, "position" ) != 0 ) {
                cerr << "Wrong light values in control file! \n";
                lightUsage();
                return 0;
            }
            lights[lightCount-1].ambient = ambientVec;
            lights[lightCount-1].color = colorVec;
            lights[lightCount-1].position = positionVec; //change to eye coordinate

            if(strcmp(lightType, "local") == 0 || strcmp(lightType, "spot") ==0) {
                //constAtt 0.2 linearAtt 0.2 quadAtt .02
                char con[128];
                char lin[128];
                char quad[128];
                float constAtt;
                float linearAtt;
                float quadAtt;
                fscanf(myFile, "%s %f %s %f %s %f", 
                       con, &constAtt, lin, &linearAtt, quad, &quadAtt);
                if(strcmp(con, "constAtt" ) != 0 || strcmp(lin, "linearAtt") != 0  || strcmp(quad, "quadAtt" ) != 0) {
                    cerr << "Wrong light values in control file! \n";
                    lightUsage();
                    return 0;
                }
                lights[lightCount-1].constantAttenuation = constAtt;
                lights[lightCount-1].linearAttenuation = linearAtt;
                lights[lightCount-1].quadraticAttenuation = quadAtt;
                lights[lightCount-1].isEnabled = true;
                lights[lightCount-1].isLocal = true; 
                lights[lightCount-1].isSpot = false;

                if(strcmp(lightType, "spot") ==0) {
                    //coneDirection 0. 0. -1. spotCosCutoff .93 spotExponent 25.
                    char cone[128];
                    char cutoff[128];
                    char exponent[128];
                    vec3 coneDirection;
                    float spotCosCutoff;
                    float spotExponent;
                    fscanf(myFile, "%s %f %f %f %s %f %s %f", 
                            cone, &coneDirection.x, &coneDirection.y, &coneDirection.z,
                            cutoff, &spotCosCutoff, exponent, &spotExponent);
                    if(strcmp(cone, "coneDirection" ) != 0 || strcmp(cutoff, "spotCosCutoff") != 0  || strcmp(exponent, "spotExponent" ) != 0) {
                        cerr << "Wrong light values in control file! \n";
                        lightUsage();
                        return 0;
                    }
        
                    lights[lightCount-1].coneDirection = coneDirection;
                    lights[lightCount-1].spotCosCutoff = spotCosCutoff;
                    lights[lightCount-1].spotExponent = spotExponent;                   
                    lights[lightCount-1].isSpot = true;
                }
            } else if(strcmp(lightType, "directional") == 0) {
                lights[lightCount-1].isEnabled = true;
                lights[lightCount-1].isLocal = false; 
                lights[lightCount-1].isSpot = false;

            } else {
                cerr << "Wrong light type \"" << lightType  << "\"in control file! \n";
                lightUsage();
                return 0;
            }
            isLight = true;
        }
    }

    if(!isView) {
        cerr << "Please specify view values in control file!\n" ;
        viewUsage();
        return 0;
    }
    if(!isLight) {
        cerr << "Please specify light values in control file!\n";
        lightUsage();
    }
 	return 1;    
}

void initializeLights() {
    for(int i =0; i < MAXLIGHTS; i++) {
        lights[i].isEnabled = false; // is this light active
        lights[i].isLocal = false;   // true for point light source, false for directional light
        lights[i].isSpot = false;    // true for spotlight
        lights[i].ambient = vec3(.0f, .0f, .0f);   // light's contribution to ambient light (r,g,b)
        lights[i].color = vec3(.0f, .0f, .0f);     // color of light (r,g,b)
        lights[i].position = vec3(.0f, .0f, .0f);  // location of local light source, direction to light source otherwise
        lights[i].halfVector = vec3(.0f, .0f, .0f);
        lights[i].coneDirection = vec3(.0f, .0f, .0f);     // spotlight attributes
        lights[i].spotCosCutoff = .0f;
        lights[i].spotExponent = .0f;
        lights[i].constantAttenuation = .0f;  // local light attenuation coefficients
        lights[i].linearAttenuation = .0f;
        lights[i].quadraticAttenuation = .0f;
    }
}

void printLights() {
    for(int i =0; i < MAXLIGHTS; i++) {
        if(!lights[i].isEnabled) {
            continue;
        }
        cout << "light "  << i << endl;
        cout << "      isEnabled:" << lights[i].isEnabled << endl;
        cout << "      isLocal:" << lights[i].isLocal << endl;
        cout << "      isSpot:" << lights[i].isSpot << endl;
        cout << "      ambient:" << lights[i].ambient.x << " " << lights[i].ambient.y << " " << lights[i].ambient.z << endl;
        cout << "      color:" << lights[i].color.x << " " << lights[i].color.y << " " << lights[i].color.z << endl;
        cout << "      position:" << lights[i].position.x << " " << lights[i].position.y << " " << lights[i].position.z << endl;
        if(lights[i].isLocal) {
            cout << "      constantAttenuation:" << lights[i].constantAttenuation << endl;
            cout << "      linearAttenuation:" << lights[i].linearAttenuation << endl;
            cout << "      quadraticAttenuation:" << lights[i].quadraticAttenuation << endl;
            if(lights[i].isLocal) {
                cout << "      coneDirection:" << lights[i].coneDirection.x << " " << lights[i].coneDirection.y << " " << lights[i].coneDirection.z << endl;
                cout << "      spotCosCutoff:" << lights[i].spotCosCutoff << endl;
                cout << "      spotExponent:" << lights[i].spotExponent << endl;
            }
        }
    }
}

void printView() {
      cout << "view" << endl;
      cout << "      eye:" << defaultEye.x << " " << defaultEye.y << " " << defaultEye.z << endl;
      cout << "      focal:" << defaultCenter.x << " " << defaultCenter.y << " " << defaultCenter.z << endl;
      cout << "      viewup:" << defaultViewUp.x << " " << defaultViewUp.y << " " << defaultViewUp.z << endl;
}

void viewUsage()
{
    cout << "Usage:\n" << "     view camera x y z focal x y z viewup x y z\n"; 
}

void lightUsage()
{
    cout << "Usage:\n" << "     light [local|spot|directional] ambient r g b color r g b position x y z\n"
                       << "     [constAtt ca linearAtt la quadAtt qa]\n"
                       << "     [coneDirection x y z spotCosCutoff cutoff spotExponent exp]\n";
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

    //glm::vec3 eye(1.5f * xRange, 1.5f * yRange, 1.5f * zRange);
    glm::vec3 eye(3, 3, 3);
    //cout << "maxDimension:" << maxDimension << endl; 
    //glm::vec3 eye(xRange/1.5, yRange/1.5, zRange/1.5);
 
	//glm::vec3 eye(3.0f * maxDimension, 3.0f * maxDimension, 1.0f * maxDimension);
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
    float farDist = 1000.f;
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

void usage()
{

    std::cout << "Usage:\n" << "    ./viewer -c controlFile\n";

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
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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



