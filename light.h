#ifndef light_h
#define light_h


#include "../Externals/Include/Include.h"
using namespace glm;
using namespace std;

// used to save the settings for light
typedef struct _light {
	vec3 pos;		// position in world space
	vec3 color;		
	vec3 amb_color;
	float k_amb;
	vec3 k_diff;
	vec3 k_spec;
	float pow_spec;
} Light_settings;

// used to save the uniform locations from program
typedef struct _light_uniform {
	GLuint light_pos;
	GLuint light_color;
	GLuint amb_color;
	GLuint k_amb;
	GLuint k_diff;
	GLuint k_spec;
	GLuint pow_spec;
} Luniform;

class Light {
public:
	// `settings` is a public variable
	// you can do Light.settings.pos = ....
	Light_settings settings;

	Light();
	void useDefaultSettings();
	void getUniformLocations(GLuint program);
	void setUniforms();

private:
	Luniform luni;
};


#endif	/* light_h */