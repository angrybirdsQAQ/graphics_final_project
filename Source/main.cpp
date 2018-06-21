#include "../Externals/Include/Include.h"

#include "load_utils.hpp"
#include "light.h"
#include "UI.hpp"
#include "skybox.h"
#include "water.h"
#include "rain.h"
#include <ctime>
#include <stdlib.h>

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
char modelDir[] = "../Assets/city_block/";
char modelFile[] = "city_block.obj";

// lighting
Light light;

//UI
UI *ui;
static const GLfloat window_positions[16] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

//
GLuint eyepos_link;
//
bool rain;

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
void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

void My_Init()
{
    glClearColor(0.843f, 0.913f, 0.988f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// turn on MSAA for anti-aliasing
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	// detect current settings
	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);
	printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);
    

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

	//UI
	ui = new UI();

    // configure lighting
	light.useDefaultSettings();
	light.getUniformLocations(program);
   
	//skybox
	state_link = glGetUniformLocation(program, "state");
	eyepos_link = glGetUniformLocation(program, "eyepos");
	//eyepos_link = glGetAttribLocation(program, "eyepos");

	vector<std::string> faces;
	faces.push_back("../Assets/skybox/right.jpg");
	faces.push_back("../Assets/skybox/left.jpg");
	faces.push_back("../Assets/skybox/top.jpg");
	faces.push_back("../Assets/skybox/bottom.jpg");
	faces.push_back("../Assets/skybox/back.jpg");
	faces.push_back("../Assets/skybox/front.jpg");

	cubemapTexture = loadCubemap(faces);

	glGenVertexArrays(1, &skyvao);
	glGenBuffers(1, &skyvbo);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(skyvao);

	glBindBuffer(GL_ARRAY_BUFFER, skyvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	//rain
	/////rain

	// Ground Verticies
	// Ground Colors
	//int x, z;
	//for (z = 0; z < 21; z++) {
	//	for (x = 0; x < 21; x++) {
	//		ground_points[x][z][0] = x - 50.0;
	//		ground_points[x][z][1] = accum;
	//		ground_points[x][z][2] = z - 50.0;

	//		ground_colors[z][x][0] = 0.0; // red value
	//		ground_colors[z][x][1] = 0.2; // green value
	//		ground_colors[z][x][2] = 0.8; // blue value
	//		ground_colors[z][x][3] = 0.5; // acummulation factor
	//	}
	//}

	// Initialize particles
	rain = false;
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}

	//init water
	initWave();

	GLfloat materAmbient[] = { 0.76, 0.94, 0.94, 1.0 };
	GLfloat materSpecular[] = { 0.8, 0.8, 0.9, 1.0 };
	GLfloat lightDiffuse[] = { 0.7, 0.7, 0.8, 1.0 };
	GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat envirAmbient[] = { 0.76, 0.94, 0.94, 1.0 };
	glUniform4fv(glGetUniformLocation(program, "materAmbient"), 1, materAmbient);
	glUniform4fv(glGetUniformLocation(program, "materSpecular"), 1, materSpecular);
	glUniform4fv(glGetUniformLocation(program, "lightDiffuse"), 1, lightDiffuse);
	glUniform4fv(glGetUniformLocation(program, "lightAmbient"), 1, lightAmbient);
	glUniform4fv(glGetUniformLocation(program, "lightSpecular"), 1, lightSpecular);
	glUniform4fv(glGetUniformLocation(program, "envirAmbient"), 1, envirAmbient);

	names.attributes.position = glGetAttribLocation(program, "position");
	glGenBuffers(1, &names.vertex_buffer);

	names.attributes.normal = glGetAttribLocation(program, "normal");
	glGenBuffers(1, &names.normal_buffer);

	glm::mat4 Projection = glm::perspective(45.0f, (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 1.0f, 100.f);
	glm::mat4 viewTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -1.0f, -4.5f));
	glm::mat4 viewRotateMat = glm::rotate(viewTransMat, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ModelViewMat = glm::scale(viewRotateMat, glm::vec3(0.3f, 0.3f, 0.3f));
	glm::mat3 NormalMat = glm::transpose(glm::inverse(glm::mat3(ModelViewMat)));
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMat"), 1, GL_FALSE, glm::value_ptr(ModelViewMat));
	glUniformMatrix4fv(glGetUniformLocation(program, "perspProjMat"), 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix3fv(glGetUniformLocation(program, "normalMat"), 1, GL_FALSE, glm::value_ptr(NormalMat));

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
    
}

void My_Display()
{

	glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // set uniforms
    glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(projMat));
    glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(viewMat * modelMat));
    
	light.setUniforms();
	

	//draw skybox

	sky_state = 1;
	glUniform1i(state_link, sky_state);

	glBindVertexArray(skyvao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	sky_state = 0;
	glUniform1i(state_link, sky_state);
	
    // render model
	model->render();

	//draw rain
	if (rain)
	{
		sky_state = 2;
		glUniform1i(state_link, sky_state);
		drawRain();
		sky_state = 0;
		glUniform1i(state_link, sky_state);
	}

	//draw water
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	sky_state = 3;
	glUniform1i(state_link, sky_state);
	calcuWave();

	glUniform1f(glGetUniformLocation(program, "time"), values.time);

	glBindBuffer(GL_ARRAY_BUFFER, names.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(names.attributes.position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(names.attributes.position);

	glBindBuffer(GL_ARRAY_BUFFER, names.normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW);
	glVertexAttribPointer(names.attributes.normal, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(names.attributes.normal);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, names.normal_texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, names.diffuse_texture);

	for (int c = 0; c<(STRIP_COUNT - 1); c++)
		glDrawArrays(GL_TRIANGLE_STRIP, STRIP_LENGTH * 2 * c, STRIP_LENGTH * 2);


	sky_state = 0;
	glUniform1i(state_link, sky_state);
	glDepthMask(GL_TRUE);

	//UI
	glUseProgram(0);
	ui->draw();

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
		if (ui->click_rain(x, y)) {
			//
			rain = true;
		}
		else if (ui->click_sun(x, y)) {
			//ê∞ìV
			rain = false;
		}
		
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
	glutSetOption(GLUT_MULTISAMPLE, 8);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
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
	glutIdleFunc(&idleFunc);
    
	// Enter main event loop.
	glutMainLoop();

	return 0;
}
