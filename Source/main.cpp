#include "../Externals/Include/Include.h"
#include "load_utils.hpp"
#include <ctime>

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

using namespace glm;
using namespace std;

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

// program
GLuint program;

// model view matrices
GLuint um4p;
GLuint um4mv;
mat4 viewMat;
mat4 projMat;
mat4 modelMat = mat4();

// viewing positions for different models
const vec3 pos = vec3(10.0f, 90.0f, 10.0f);
const vec3 front = vec3(-1.0f, -1.0f, -1.0f);

// camera
vec3 cameraPos = pos;
vec3 cameraFront = front;
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float cameraSpd = 0.05f * timer_speed;
float pitchDeg = -30.0f;
float yawDeg = 0.0f;

// mouse control
bool firstMouse = true;
int lastX, lastY;

// load model
Model *model;
char modelDir[] = "../Assets/street/";
char modelFile[] = "unityexport2.obj";

// lighting
typedef struct _light{
    vec3 pos;
    vec3 diffuse_albedo;
    vec3 specular_albedo;
    float specular_power;
} Light;

typedef struct _light_uniform{
    GLuint light_pos;
    GLuint diffuse_albedo;
    GLuint specular_albedo;
    GLuint specular_power;
} Luniform;

Light light;
Luniform luni;

char** loadShaderSource(const char* file)
{
    FILE* fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[sz + 1];
    fread(src, sizeof(char), sz, fp);
    src[sz] = '\0';
    char **srcp = new char*[1];
    srcp[0] = src;
    return srcp;
}

void freeShaderSource(char** srcp)
{
    delete[] srcp[0];
    delete[] srcp;
}


void My_Init()
{
    glClearColor(0.843f, 0.913f, 0.988f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// enable face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
    
    // load shaders and program
    program = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
    char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    freeShaderSource(vertexShaderSource);
    freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);
    
    
    // get uniform location
    um4p = glGetUniformLocation(program, "um4p");
    um4mv = glGetUniformLocation(program, "um4mv");
    
    // load model
    model = new Model(modelDir, modelFile);
    
    // set light properties
    light.pos = vec3(20.0f, 20.0f, 20.0f);
    light.diffuse_albedo = vec3(0.5f, 0.5f, 0.5f);
    light.specular_albedo = vec3(1.0f, 1.0f, 1.0f);
    light.specular_power = 60.0f;
    
    // get light uniform location
    luni.light_pos = glGetUniformLocation(program, "light_pos");
    luni.diffuse_albedo = glGetUniformLocation(program, "diffuse_albedo");
    luni.specular_albedo = glGetUniformLocation(program, "specular_albedo");
    luni.specular_power = glGetUniformLocation(program, "specular_power");
}

// recalculate the view matrix
void refreshView()
{
    // camera
    pitchDeg = clamp(pitchDeg, -89.0f, 89.0f);  // restrict pitch degrees
    cameraFront.x = cos(radians(pitchDeg)) * cos(radians(yawDeg));
    cameraFront.y = sin(radians(pitchDeg));
    cameraFront.z = cos(radians(pitchDeg)) * sin(radians(yawDeg));
    cameraFront = normalize(cameraFront);
    
    
    viewMat = lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
    printf("cameraFront now at (%f, %f, %f)\n",
           cameraFront.x, cameraFront.y, cameraFront.z);
    printf("cameraPos now at (%f, %f, %f)\n",
           cameraPos.x, cameraPos.y, cameraPos.z);
}

void My_Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // set uniforms
    glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(projMat));
    glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(viewMat * modelMat));
    
    // set light uniforms
    glUniform3fv(luni.light_pos, 1, &light.pos[0]);
    glUniform3fv(luni.specular_albedo, 1, &light.specular_albedo[0]);
    glUniform3fv(luni.diffuse_albedo, 1,  &light.diffuse_albedo[0]);
    glUniform1f(luni.specular_power, light.specular_power);
	

    // render model
	model->render();
        
    glutSwapBuffers();
}


void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
    
    float viewportAspect = (float)width / (float)height;
    projMat = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
    refreshView();
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}

void My_Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
	}
	else if(state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void myMouseMove(int x, int y)
{
    if(firstMouse){
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    
    
    float deltaX = (float)(x - lastX);
    float deltaY = (float)(lastY - y);
    
    yawDeg   += deltaX * cameraSpd;
    pitchDeg += deltaY * cameraSpd;
    
    lastX = x;
    lastY = y;
    
    refreshView();
}

void My_Keyboard(unsigned char key, int x, int y)
{
    switch(key){
        case 'w':
            cameraPos += cameraSpd * cameraFront;
            break;
            
        case 's':
            cameraPos -= cameraSpd * cameraFront;
            break;
            
        case 'a':
            cameraPos -= cameraSpd * normalize(cross(cameraFront, cameraUp));
            break;
            
        case 'd':
            cameraPos += cameraSpd * normalize(cross(cameraFront, cameraUp));
            break;
            
        case 'q':
            yawDeg -= cameraSpd;
            break;
            
        case 'e':
            yawDeg += cameraSpd;
            break;
            
        case 'z':
            cameraPos += cameraSpd * cameraUp;
            break;
            
        case 'x':
            cameraPos -= cameraSpd * cameraUp;
            break;
            
        default:
            printf("Key %c is pressed at (%d, %d)\n", key, x, y);
            break;
    }
    refreshView();
}

void My_SpecialKeys(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_F1:
            printf("F1 is pressed at (%d, %d)\n", x, y);
            break;
            
        case GLUT_KEY_PAGE_UP:
            printf("Page up is pressed at (%d, %d)\n", x, y);
            break;
            
        case GLUT_KEY_LEFT:
            printf("Left arrow is pressed at (%d, %d)\n", x, y);
            break;
            
        case GLUT_KEY_RIGHT:
            printf("Right arrow is pressed at (%d, %d)\n", x, y);
            break;
            
        default:
            printf("Other special key is pressed at (%d, %d)\n", x, y);
            break;
    }
}

void My_Menu(int id)
{
	switch(id)
	{
	case MENU_TIMER_START:
		if(!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    // Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("AS2_Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
    glewInit(); // added
    glPrintContextInfo();
	My_Init();

	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
    glutPassiveMotionFunc(myMouseMove);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 

    
	// Enter main event loop.
	glutMainLoop();

	return 0;
}
