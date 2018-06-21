#include <stdlib.h>
#define MAX_PARTICLES 10000
#define WCX		640
#define WCY		480
#define RAIN	0
#define SNOW	1
#define	HAIL	2


float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall;

//floor colors
float cr = 0.0;
float cg = 1.0;
float cb = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -20.0;

typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
				// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

particles par_sys[MAX_PARTICLES];

void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 100) - 10;
	par_sys[i].ypos = 150.0;
	par_sys[i].zpos = (float)(rand() % 100) - 10;

	par_sys[i].red = 1.0;
	par_sys[i].green = 0.1;
	par_sys[i].blue = 0.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}



void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			//glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			//glColor3f(0.5, 0.0, 0.0);
			glColor3ub(200, 1, 1);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 100);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}
