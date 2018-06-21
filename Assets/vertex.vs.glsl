#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 um4mv;
uniform mat4 um4p;

uniform vec3 light_pos; // light position in world space

out VertexData
{
    vec3 N; // view space normal
    vec3 L; // view space light vector
    vec3 V;
    vec2 texcoord;
} vertexData;

out vec3 oripos;

//water
attribute vec3 position;
attribute vec3 normal;

uniform mat4 modelViewMat;
uniform mat4 perspProjMat;
uniform mat3 normalMat;

uniform float time;

varying vec2 texture_coord;

varying vec3 normalVect;
varying vec3 lightVect;
varying vec3 eyeVect;
varying vec3 halfWayVect;
varying vec3 reflectVect;
varying vec3 posVect;
//wwwww

uniform int state;
uniform vec3 eyepos;

void main()
{
	if (state == 3) {
		vec3 positionw = vec3(0.0, 1.0, 0.0);
		gl_Position = um4p * um4mv * vec4(position, 1.0);
		float tex_x = (position.x + time / 20.0) / 8.0 + 0.5;
		float tex_y = 0.5 - (position.y + time / 25.0) / 5.0;
		texture_coord = vec2(tex_x, tex_y);

		//vec3 eyePos = vec3(0.0, 0.0, 5.0);
		vec3 eyePos2 = eyepos;
		vec3 lightPos = vec3(1.0, 20.0, 1.0);
		vec3 ptVertex = vec3(um4mv * vec4(position, 1.0));

		posVect.x = position.x;
		posVect.y = position.z;
		posVect.z = position.y;

		eyeVect = normalize(eyePos2 - ptVertex);
		lightVect = normalize(lightPos - ptVertex);
		halfWayVect = eyeVect + lightVect;
		normalVect = normalMat * normal;
		reflectVect = 1.0 * eyeVect - 2.0 * dot(-1.0*eyeVect, normalVect) * normalVect;
	}
	else
	{


		// Calculate view-space coordinate
		vec4 P = um4mv * vec4(iv3vertex, 1.0);

		// Calculate normal in view-space
		// Using only the rotational components of mv_matrix
		vertexData.N = mat3(um4mv) * iv3normal;


		// Calculate light vector
		vertexData.L = mat3(um4mv) * light_pos - P.xyz;

		// Calculate view vector
		vertexData.V = -P.xyz;

		// Calculate the clip-space position of each vertex
		gl_Position = um4p * P;


		vertexData.texcoord = iv2tex_coord;

		oripos = iv3vertex;
	}
}
