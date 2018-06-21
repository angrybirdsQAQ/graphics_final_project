#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 V; // eye space halfway vector
    vec2 texcoord;
} vertexData;

// light settings
uniform vec3 light_color;   // sun/moon color
uniform vec3 amb_color;     // ambient color (when there's no sun/moon)

// material properties
uniform float k_amb;    // ambient strength
uniform vec3 k_diff;    // diffuse coefficient
uniform vec3 k_spec;    // specular coefficient
uniform float pow_spec;


uniform sampler2D tex;


//skybox
uniform samplerCube skybox;
uniform int state;
in vec3 oripos;

uniform vec3 eyepos;

///////water
uniform vec4 materAmbient, materSpecular;
uniform vec4 lightDiffuse, lightAmbient, lightSpecular;
uniform vec4 envirAmbient;

varying vec2 texture_coord;

varying vec3 normalVect;
varying vec3 lightVect;
varying vec3 eyeVect;
varying vec3 halfWayVect;
varying vec3 reflectVect;
varying vec3 posVect;


///////


void main()
{
	if (state == 0)
	{
		// Normalize the incoming N, L and V vectors
		vec3 N = normalize(vertexData.N);
		vec3 L = normalize(vertexData.L);
		vec3 V = normalize(vertexData.V);
		vec3 H = normalize(L + V);

		// Compute the diffuse and specular components for each fragment
		vec3 light_color = vec3(100.0f, 80.0f, 20.0f) / 255;
		vec3 ambient = k_amb * amb_color;
		vec3 diffuse = max(dot(N, L), 0.0) * k_diff * light_color;
		vec3 specular = pow(max(dot(N, H), 0.0), pow_spec) * k_spec * light_color;

		// Write final color to the framebuffer
		vec3 texColor = texture(tex, vertexData.texcoord).rgb;
		vec3 result = (ambient + diffuse + specular) * texColor;
		fragColor = vec4(result, 1.0);
	}
	else if (state == 1)
	{
		fragColor = texture(skybox, oripos);
		//fragColor = vec4(0.0, 1.0, 1.0, 1.0);
	}
	else if (state == 3)
	{
		vec3 I = normalize(posVect - eyepos);
		vec3 R = reflect(I, normalize(normalVect));
		fragColor = vec4(texture(skybox, R).rgb, 0.45) + vec4(0.76 / 2, 0.94 / 2, 0.94 / 2, 0.0);
	}
	else if (state == 2) {
		fragColor = vec4(0.7, 0.8, 1.0, 1.0);
	}

}
