#include "light.h"


Light::Light()
{
	// default setting : set settings as sun
	useDefaultSettings();
}

void Light::useDefaultSettings()
{
	// default setting : set settings as sun
	settings.pos = vec3(300.0f, 300.0f, 300.0f);
	settings.color = vec3(100.0f, 80.0f, 20.0f) / 255.0f;
	settings.amb_color = vec3(1.0f, 1.0f, 1.0f);
	settings.k_amb = 0.4f;
	settings.k_diff = vec3(0.7f, 0.7f, 0.7f);
	settings.k_spec = vec3(1.0f, 1.0f, 1.0f);
	settings.pow_spec = 60.0f;
}

// get light uniform location
void Light::getUniformLocations(GLuint program)
{
	luni.light_pos = glGetUniformLocation(program, "light_pos");
	luni.light_color = glGetUniformLocation(program, "light_color");
	luni.amb_color = glGetUniformLocation(program, "amb_color");
	luni.k_amb = glGetUniformLocation(program, "k_amb");
	luni.k_diff = glGetUniformLocation(program, "k_diff");
	luni.k_spec = glGetUniformLocation(program, "k_spec");
	luni.pow_spec = glGetUniformLocation(program, "pow_spec");
}

// set light uniforms
void Light::setUniforms()
{
	glUniform3fv(luni.light_pos, 1, &settings.pos[0]);
	glUniform3fv(luni.light_color, 1, &settings.color[0]);
	glUniform3fv(luni.amb_color, 1, &settings.amb_color[0]);
	glUniform1f(luni.k_amb, settings.k_amb);
	glUniform3fv(luni.k_spec, 1, &settings.k_spec[0]);
	glUniform3fv(luni.k_diff, 1, &settings.k_diff[0]);
	glUniform1f(luni.pow_spec, settings.pow_spec);
}
