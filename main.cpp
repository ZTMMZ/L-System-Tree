/****Jianchang Bi (bij@oregonstate.edu)*******************/
/****CS557 Final******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#define _USE_MATH_DEFINES

#include <math.h>
#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "bmptotexture.cpp"
#include "textotexture.cpp"
#include "glslprogram.h"
#include "Grammar.h"

#define MS_IN_THE_ANIMATION_CYCLE	10000
#define PI 3.14

const char *WINDOWTITLE = { "CS557-Final" };
const char *GLUITITLE = { "User Interface Window" };
// what the glui package defines as true and false:
const int GLUITRUE = { true };
const int GLUIFALSE = { false };
// the escape key:
#define ESCAPE		0x1b
// initial window size:
const int INIT_WINDOW_SIZE = { 1000 };
// size of the box:
const float BOXSIZE = { 2.f };
// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };
// minimum allowable scale factor:
const float MINSCALE = { 0.05f };
// active mouse buttons (or them together):
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };
// which projection:
enum Projections {
	ORTHO,
	PERSP
};
// which button:
enum ButtonVals {
	RESET,
	TOPLOOK,
	NOLITCOLOR,
	QUIT
};
// window background color (rgba):
const GLfloat BACKCOLOR[] = { 0.1, 0.1, 0.1, 1. };
// line width for the axes:
const GLfloat AXES_WIDTH = { 3. };

// fog parameters:
const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };

//VBO
const GLfloat CubeVertices[] = {
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,

	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

//Light constants 
float LightX = 6., LightY= 14., LightZ = 20.;
float AmbientColorR = .09, AmbientColorG = .19, AmbientColorB = .30;
float DiffuseColorR = .97, DiffuseColorG = .67, DiffuseColorB = .62;
float SpecularColorR = 1., SpecularColorG = 1., SpecularColorB = 1.;
int IsFlat = 0;
float Attenuation = 60.;

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
float   Time;
int		Speed;																			// control speed
int		Frozen;
GLSLProgram	*CubeMappingShader, *TreeTexShader, *TerrainShader, *WaterShader;
GLuint	cubeMappingVAO, cubeMappingVBO, CubeTex, TerrainTex, TerrainContrastTex, Noise2DTex, ReflectTex, LeafTex, BarkTex;
std::string  grammarResult[11];
// function prototypes:
void	Animate();
void	Display();
void	TopLook();
void	NoLightColor();
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char *, bool);
void	DoStrokeString(float, float, float, float, char *);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void	Axes(float);

void	DoChangeSpeed(int);																// control the speed of distortion
void	load2DTexture(GLuint*, char*, int, int, bool);
void	loadCubeTexture(GLuint*, char**, int, int, bool);
void	drawSquare(float, float, int, int, float);
void	drawTrunk(float, float, float);
void	drawLeaf(float,float);

struct	TreeParamter {
	float unitDegreeX;
	float unitDegreeY;
	float unitDegreeZ;
	float baseHeight;
	float heightReduceRate;
	float baseRad;
	float radReduceRate;
	float leafWidth;
	float leafHeight;
};
std::string getGrammar(int);
void	drawTree(std::string, struct TreeParamter);




void load3DNoise(GLuint *tex, char *fileName, int width, int height, int depth) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_3D, *tex);
	int level = 0, border = 0;
	unsigned char *Texture = TexTo3DTexture(fileName, &width, &height, &depth);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage3D(GL_TEXTURE_3D, level, GL_RGBA, width, height, depth, border, GL_RGBA, GL_UNSIGNED_BYTE, Texture);
}

void load2DNoise(GLuint *tex, char *fileName, int width, int height) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	int level = 0, border = 0;
	unsigned char *Texture = TexTo2DTexture(fileName, &width, &height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, Texture);
}

void load2DTexture(GLuint *tex, char *fileName, int width, int height, bool alpha) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	int level = 0, border = 0;
	unsigned char *Texture = BmpToTexture(fileName, &width, &height, alpha);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, Texture);
}

void loadCubeTexture(GLuint *texs, char **fileName, int width, int height, bool alpha) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, texs);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *texs);
	int level = 0, border = 0;
	unsigned char *Texture;

	for (int i = 0; i<6; i++) {
		Texture = BmpToTexture(fileName[i], &width, &height , alpha);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, Texture);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}


std::string getGrammar(int iterate) {
	Grammar* grammar = new Grammar();
	grammar->setGrammarName("Tree0");
	grammar->addGeneration('S', "F[$^X][%*X][%&X]");
	grammar->addGeneration('X', "F[%^D][$&D][$/D][%*D]");
	grammar->addGeneration('X', "F[%&D][$*D][$/D][%^D]");
	grammar->addGeneration('D', "F[$^X][%*FX][%&X]");
	grammar->setStart("S");
	grammar->iterateFor(iterate);
	std::string str = std::string(grammar->getResult());
	grammar->clear();
	free(grammar);
	return str;
}
void drawSquare(float width, float height, int wCut, int hCut, float texCut) {
	glNormal3f(0, 1, 0);
	for (int i = 0; i < hCut; i++) {
		for (int j = 0; j < wCut; j++) {
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(texCut* j * (1. / wCut), texCut*i * (1. / hCut));
				glVertex3f(-width / 2. + j * (width / wCut), 0., height / 2. - i * (height / hCut));
				glTexCoord2f(texCut*j * (1. / wCut), texCut*(i + 1) * (1. / hCut));
				glVertex3f(-width / 2. + j * (width / wCut), 0., height / 2. - (i + 1) * (height / hCut));
				glTexCoord2f(texCut*(j + 1) * (1. / wCut), texCut*i * (1. / hCut));
				glVertex3f(-width / 2. + (j + 1) * (width / wCut), 0., height / 2. - i * (height / hCut));
				glTexCoord2f(texCut*(j + 1) * (1. / wCut), texCut*(i + 1) * (1. / hCut));
				glVertex3f(-width / 2. + (j + 1) * (width / wCut), 0., height / 2. - (i + 1) * (height / hCut));
			glEnd();
		}
	}
}

void drawTrunk(float height, float radiusB, float radiusA) {
	int i;
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < 360; i += 45) {
		float rad = (float)i / 180 * M_PI;
		glTexCoord2f((float)i / 360, height*2.);
		glNormal3f(cos(rad)*radiusB, height, -sin(rad)*radiusB);
		glVertex3f(cos(rad)*radiusB, height, -sin(rad)*radiusB);
		glTexCoord2f((float)i / 360, 0.);
		glNormal3f(cos(rad)*radiusA, 0, -sin(rad)*radiusA);
		glVertex3f(cos(rad)*radiusA, 0, -sin(rad)*radiusA);
	}
	glTexCoord2f((float)i / 360, height*2.);
	glNormal3f(cos(0)*radiusB, height, -sin(0)*radiusB);
	glVertex3f(cos(0)*radiusB, height, -sin(0)*radiusB);
	glTexCoord2f((float)i / 360, 0.);
	glNormal3f(cos(0)*radiusA, 0, -sin(0)*radiusA);
	glVertex3f(cos(0)*radiusA, 0, -sin(0)*radiusA);
	glEnd();
}
void drawLeaf (float leafWidth, float leafHeight) {
	glBegin(GL_QUADS);
		glTexCoord2f(0., 0.);
		glVertex3f(-leafWidth / 2., 0., 0.);
		glTexCoord2f(0., 1.);
		glVertex3f(-leafWidth / 2., leafHeight, 0.);
		glTexCoord2f(1., 1.);
		glVertex3f(leafWidth / 2., leafHeight, 0.);
		glTexCoord2f(1., 0.);
		glVertex3f(leafWidth / 2., 0., 0.);
	glEnd();
}
struct TreeParamter t0 = {
	35.,35.,30.,2.,0.75, 0.1, 0.5, 0.3, 0.46
};

struct TreeParamter t1 = {
	30.,35.,35.,1.,0.75, 0.1, 0.5, 0.3, 0.46
};

struct TreeParamter t2 = {
	35.,35.,30.,2.,0.75, 0.3, 0.75, 0.3, 0.46
};

void drawTree(std::string grammer, struct TreeParamter tp) {
	float unitDegreeX = tp.unitDegreeX;
	float unitDegreeY = tp.unitDegreeY;
	float unitDegreeZ = tp.unitDegreeZ;
	float baseHeight = tp.baseHeight;
	float heightReduceRate = tp.heightReduceRate;
	float baseRad = tp.baseRad;
	float radReduceRate = tp.radReduceRate;
	float leafWidth = tp.leafWidth;
	float leafHeight = tp.leafHeight;
	int scaleTime = 0;
	for (int i = 0; i<grammer.size(); i++) {
		char ch = grammer[i];
		switch (ch) {
		case 'F':
			TreeTexShader->SetUniformVariable("uTex", 4);
			drawTrunk(baseHeight, baseRad*radReduceRate,baseRad);
			glTranslatef(0., baseHeight, 0.);
			glScalef(heightReduceRate, heightReduceRate, heightReduceRate);
			break;
		case '^':
			glRotatef(unitDegreeX,1.,0.,0.);
			break;
		case '&':
			glRotatef(-unitDegreeX, 1., 0., 0.);
			break;
		case '$':
			glRotatef(unitDegreeY, 0., 1., 0.);
			break;
		case '%':
			glRotatef(-unitDegreeY, 0., 1., 0.);
			break;
		case '*':
			glRotatef(unitDegreeZ, 0., 0., 1.);
			break;
		case '/':
			glRotatef(-unitDegreeZ, 0., 0., 1.);
			break;
		case '[':
			glPushMatrix();
			scaleTime++;
			break;
		case ']':
			glPopMatrix();
			scaleTime--;
			break;
		case 'D':
		case 'X':
			TreeTexShader->SetUniformVariable("uTex", 5);
			glPushMatrix();
				glScalef(1. / pow(heightReduceRate, scaleTime), 1. / pow(heightReduceRate, scaleTime), 1. / pow(heightReduceRate, scaleTime));
				drawLeaf(leafWidth, leafHeight);
			glPopMatrix();
			break;
		default:
			break;
		}
	}
}

// main program:
int main(int argc, char *argv[]) {
	srand((int)time(0));
	glutInit(&argc, argv);
	InitGraphics();
	InitLists();
	Reset();
	InitMenus();
	glutSetWindow(MainWindow);
	glutMainLoop();
	return 0;
}

void Animate() {
	int ms = glutGet(GLUT_ELAPSED_TIME);
	int cycle = MS_IN_THE_ANIMATION_CYCLE / (Speed * 2);
	ms %= cycle;
	Time = (float)ms / (float)cycle;				// [ 0., 1.)

	LightX = 0. + 10.*sin(Time * 2 * PI);


	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:
void Display() {
	if (DebugOn != 0) {
		fprintf(stderr, "Display\n");
	}

	glutSetWindow(MainWindow);

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (DepthBufferOn != 0)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glShadeModel(GL_FLAT);

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(60., 1., 0.01, 1000.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0., 0.25, 1., 0., 0., 0., 0., 1., 0.);

	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	if (DepthCueOn != 0) {
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else {
		glDisable(GL_FOG);
	}
	if (AxesOn != 0) {
		float white[3] = { 1., 1., 1. };
		glColor3fv(white);
		glCallList(AxesList);
	}
	glEnable(GL_NORMALIZE);
	/***Draw skybox*****************************************************************************/
	CubeMappingShader->Use();
	glPushMatrix();
	glScalef(120, 120, 120);
	glRotatef(180, 1.0f, 0.0f, 0.0f);
	glBindVertexArray(cubeMappingVAO);
	glActiveTexture(GL_TEXTURE0);
	CubeMappingShader->SetUniformVariable("uCubeMap", 0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTex);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glPopMatrix();
	CubeMappingShader->Use(0);
	
	/***Draw Terrain***************************************************************************/
	TerrainShader->Use();
	TerrainShader->SetUniformVariable("uKa", float(0.5));
	TerrainShader->SetUniformVariable("uKd", float(0.35));
	TerrainShader->SetUniformVariable("uKs", float(0.03));
	TerrainShader->SetUniformVariable("uSpecularColor", SpecularColorR, SpecularColorG, SpecularColorB, float(1.0));
	TerrainShader->SetUniformVariable("uAmbientColor", AmbientColorR, AmbientColorG, AmbientColorG, float(1.0));
	TerrainShader->SetUniformVariable("uDiffuseColor", DiffuseColorR, DiffuseColorG, DiffuseColorG, float(1.0));
	TerrainShader->SetUniformVariable("uAttenuation", Attenuation);
	TerrainShader->SetUniformVariable("uShininess", float(1.));
	TerrainShader->SetUniformVariable("uLightX", LightX);
	TerrainShader->SetUniformVariable("uLightY", LightY);
	TerrainShader->SetUniformVariable("uLightZ", LightZ);
	TerrainShader->SetUniformVariable("uFlat", IsFlat);
	TerrainShader->SetUniformVariable("uDelta", float(1024.));
	TerrainShader->SetUniformVariable("uRatio", float(4.));
	TerrainShader->SetUniformVariable("uColor", float(22./255.), float(50. / 255.), float(90. / 255.), float(1.));
	//TerrainShader->SetUniformVariable("uColor", float(0.56), float(0.53), float(0.46), float(1.));
	//TerrainShader->SetUniformVariable("uColor", float(0.76), float(0.76), float(0.76), float(1.));
	glActiveTexture(GL_TEXTURE1);
	TerrainShader->SetUniformVariable("uTerrainTex", 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TerrainTex);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, -0.25, 0);
	drawSquare(2., 2., 512., 512., 1);
	glPopMatrix();
	TerrainShader->Use(0);

	/***Draw Water*****************************************************************************/
	WaterShader->Use();
	WaterShader->SetUniformVariable("uKa", float(0.5));
	WaterShader->SetUniformVariable("uKd", float(0.35));
	WaterShader->SetUniformVariable("uKs", float(0.3));
	WaterShader->SetUniformVariable("uSpecularColor", SpecularColorR, SpecularColorG, SpecularColorB, float(1.0));
	WaterShader->SetUniformVariable("uAmbientColor", AmbientColorR, AmbientColorG, AmbientColorG, float(1.0));
	WaterShader->SetUniformVariable("uDiffuseColor", DiffuseColorR, DiffuseColorG, DiffuseColorG, float(1.0));
	WaterShader->SetUniformVariable("uAttenuation", Attenuation);
	WaterShader->SetUniformVariable("uShininess", float(650.));
	WaterShader->SetUniformVariable("uLightX", LightX);
	WaterShader->SetUniformVariable("uLightY", LightY);
	WaterShader->SetUniformVariable("uLightZ", LightZ);
	WaterShader->SetUniformVariable("uFlat", IsFlat);
	WaterShader->SetUniformVariable("uDelta", float(250.));
	WaterShader->SetUniformVariable("uRatio", float(2.5));
	WaterShader->SetUniformVariable("uColor", float(0.23), float(0.49), float(0.76), float(1.));
	//WaterShader->SetUniformVariable("uColor", float(1.), float(1.), float(1.), float(1.));
	WaterShader->SetUniformVariable("uNoiseAmp", float(0.04));
	WaterShader->SetUniformVariable("uNoiseFreq", float(6.));
	WaterShader->SetUniformVariable("uTimer", Time);
	glActiveTexture(GL_TEXTURE6);
	WaterShader->SetUniformVariable("uTerrainContrastTex", 6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TerrainContrastTex);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	WaterShader->SetUniformVariable("uNoise2", 2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Noise2DTex);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3);
	WaterShader->SetUniformVariable("uReflectTex", 3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ReflectTex);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, -0.25, 0);
	drawSquare(2., 2., 500, 500, 1);
	glPopMatrix();
	WaterShader->Use(0);
	/***Draw Tree***************************************************************************/
	TreeTexShader->Use();
	TreeTexShader->SetUniformVariable("uKa", float(0.5));
	TreeTexShader->SetUniformVariable("uKd", float(0.35));
	TreeTexShader->SetUniformVariable("uKs", float(0.3));
	TreeTexShader->SetUniformVariable("uSpecularColor", SpecularColorR, SpecularColorG, SpecularColorB, float(1.0));
	TreeTexShader->SetUniformVariable("uAmbientColor", AmbientColorR, AmbientColorG, AmbientColorG, float(1.0));
	TreeTexShader->SetUniformVariable("uDiffuseColor", DiffuseColorR, DiffuseColorG, DiffuseColorG, float(1.0));
	TreeTexShader->SetUniformVariable("uAttenuation", Attenuation);
	TreeTexShader->SetUniformVariable("uShininess", float(10.));
	TreeTexShader->SetUniformVariable("uLightX", LightX);
	TreeTexShader->SetUniformVariable("uLightY", LightY);
	TreeTexShader->SetUniformVariable("uLightZ", LightZ);
	TreeTexShader->SetUniformVariable("uFlat", IsFlat);

	glPushMatrix();
	glTranslatef(0.5, -0.17, -0.75);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[0], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.8, -0.18, -0.95);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[1], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., -0.19, 0.);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[2], t0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.3, -0.19, -0.1);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[3], t0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.5, -0.18, 0.25);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[4], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.95, -0.2, 0.25);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[5], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.6, -0.2, 0.35);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[6], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.87, -0.19, 0.45);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[7], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, -0.18, 0.9);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[8], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.82, -0.2, 0.87);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[9], t0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.9, -0.18, 0.67);
	glScalef(0.05, 0.05, 0.05);
	drawTree(grammarResult[10], t0);
	glPopMatrix();


	TreeTexShader->Use(0);

	/***Draw End*******************************************************************************/

	if (DepthFightingOn != 0) {
		glPushMatrix();
		glRotatef(90., 0., 1., 0.);
		glPopMatrix();
	}

	// draw some gratuitous text that just rotates on top of the scene:

	/*glDisable(GL_DEPTH_TEST);
	glColor3f(0., 1., 1.);
	DoRasterString(0., 1., 0., "Text That Moves");*/


	// draw some gratuitous text that is fixed on the screen:
	
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., 100., 0., 100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1., 1., 1.);
	DoRasterString(5., 8.5, 0., "557 - Final - Jianchang Bi (bij)", true);

	char num[3];
	itoa(Speed, num, 10);
	char* str = "Press + or - to control the speed.        Current speed:";
	char s[100] = { '\0' };
	memcpy(s, str, strlen(str));
	memcpy(s + strlen(str), num, strlen(num));
	DoRasterString(5., 5., 0., s, false);

	glutSwapBuffers();
	glFlush();
}

void DoChangeSpeed(int increment) {
	if ((Speed + increment) >= 1 && (Speed + increment) <= 10)
		Speed += increment;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void TopLook() {
	WhichProjection = ORTHO;
	Scale = 3.0;
	Xrot = 90.-180.*atan(0.25)/PI;
	Yrot = 0.;
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void NoLightColor() {
	AmbientColorR = 1., AmbientColorG = 1., AmbientColorB = 1.;
	DiffuseColorR = 1., DiffuseColorG = 1., DiffuseColorB = 1.;
	SpecularColorR = 1., SpecularColorG = 1., SpecularColorB = 1.;
}


void DoDebugMenu(int id) {
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthBufferMenu(int id) {
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthFightingMenu(int id) {
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthMenu(int id) {
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoMainMenu(int id) {
	switch (id) {
	case RESET:
		Reset();
		break;
	case TOPLOOK:
		TopLook();
		break;
	case NOLITCOLOR:
		NoLightColor();
		break;
	case QUIT:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;
	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoProjectMenu(int id) {
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoRasterString(float x, float y, float z, char *s, bool isLarge) {
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);
	char c;
	for (; (c = *s) != '\0'; s++) {
		if (isLarge)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		else
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
	}
}

void DoStrokeString(float x, float y, float z, float ht, char *s) {
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;												// one character to print
	for (; (c = *s) != '\0'; s++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}

float ElapsedSeconds() {
	int ms = glutGet(GLUT_ELAPSED_TIME);
	return (float)ms / 1000.f;
}

void InitMenus() {
	glutSetWindow(MainWindow);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);

	glutAddMenuEntry("TopLook", TOPLOOK);
	glutAddMenuEntry("NoLightColor", NOLITCOLOR);
	//glutAddSubMenu("Depth Buffer", depthbuffermenu);
	//glutAddSubMenu("Depth Fighting", depthfightingmenu);
	//glutAddSubMenu("Depth Cue", depthcuemenu);
	//glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	/*glutAddSubMenu("Debug", debugmenu);*/
	glutAddMenuEntry("Quit", QUIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void InitGraphics() {

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);
	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);
	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);
	glutIdleFunc(Animate);

	// Translucent effect, enable the color blend and anti-aliasing
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points  
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines  
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Unitized the normal vectors
	glEnable(GL_NORMALIZE);

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	CubeMappingShader = new GLSLProgram();
	CubeMappingShader->Create("Shader/CubeMappingShader.vert", "Shader/CubeMappingShader.frag");

	TreeTexShader = new GLSLProgram();
	TreeTexShader->Create("Shader/treeTexShader.vert", "Shader/treeTexShader.frag");

	TerrainShader = new GLSLProgram();
	TerrainShader->Create("Shader/terrainShader.vert", "Shader/terrainShader.frag");

	WaterShader = new GLSLProgram();
	WaterShader->Create("Shader/waterShader.vert", "Shader/waterShader.frag");
	
}

void InitLists() {
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);

	// create the axes:
	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();

	//SkyBox
	glGenVertexArrays(1, &cubeMappingVAO);
	glGenBuffers(1, &cubeMappingVBO);
	glBindVertexArray(cubeMappingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeMappingVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	//CubeMappingTexture
	char* fileNames[6] = { "Textures/greenhaze_rt.bmp","Textures/greenhaze_lf.bmp" ,"Textures/greenhaze_dn.bmp" ,
		"Textures/greenhaze_up.bmp" ,"Textures/greenhaze_bk.bmp" ,"Textures/greenhaze_ft.bmp" };
	//char* fileNames[6] = { "Textures/sahara_rt.bmp","Textures/sahara_lf.bmp" ,"Textures/sahara_dn.bmp" ,
	//"Textures/sahara_up.bmp" ,"Textures/sahara_bk.bmp" ,"Textures/sahara_ft.bmp" };
	loadCubeTexture(&CubeTex, fileNames, 512, 512, false);

	//TerrainTexSampler
	load2DTexture(&TerrainTex, "Textures/T.bmp", 1024, 1024, false);

	//WaterTexSampler
	load2DNoise(&Noise2DTex, "Textures/noise2d.064.tex", 64, 64);

	//ReflectTexSampler
	load2DTexture(&ReflectTex, "Textures/greenhaze_up.bmp", 512, 512, false);

	//LeafTexSampler
	load2DTexture(&LeafTex, "Textures/leaf.bmp", 330, 512, true);

	//BarkTexSampler
	load2DTexture(&BarkTex, "Textures/bark.bmp", 64, 128, false);

	//TerrainContrastedTexSampler
	load2DTexture(&TerrainContrastTex, "Textures/T_Contrast.bmp", 1024, 1024, false);

	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, BarkTex);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, LeafTex);
	glDisable(GL_TEXTURE_2D);

	for (int i = 0; i < sizeof(grammarResult) / sizeof(grammarResult[0]); i++) {
		grammarResult[i] = getGrammar(5);
	}


	//fprintf(stderr,"%s",(char*)grammarResult.data());
}

// the keyboard callback:
void Keyboard(unsigned char c, int x, int y) {
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);
	switch (c) {
	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;
	case '+':
		DoChangeSpeed(1);
		break;

	case '-':
		DoChangeSpeed(-1);
		break;

	case 'X':
	case 'x':
		IsFlat = !IsFlat;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void MouseButton(int button, int state, int x, int y) {
	int b = 0;
	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);
	switch (button) {
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;
	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	if (state == GLUT_DOWN) {
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else {
		ActiveButton &= ~b;		// clear the proper bit
	}
}

void MouseMotion(int x, int y) {
	if (DebugOn != 0)
		fprintf(stderr, "MouseMotion: %d, %d\n", x, y);

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0) {
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}
	if ((ActiveButton & MIDDLE) != 0) {
		Scale += SCLFACT * (float)(dx - dy);
		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;	
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void Reset() {
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 1;
	Scale = 1.0;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	Speed = 1;
	Frozen = 0;

	AmbientColorR = .09, AmbientColorG = .19, AmbientColorB = .30;
	DiffuseColorR = .97, DiffuseColorG = .67, DiffuseColorB = .62;
	SpecularColorR = 1., SpecularColorG = 1., SpecularColorB = 1.;

}

void Resize(int width, int height) {
	if (DebugOn != 0)
		fprintf(stderr, "ReSize: %d, %d\n", width, height);
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void Visibility(int state) {
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE) {
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else {
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////
// the stroke characters 'X' 'Y' 'Z' :
static float xx[] = {
	0.f, 1.f, 0.f, 1.f
};
static float xy[] = {
	-.5f, .5f, .5f, -.5f
};
static int xorder[] = {
	1, 2, -3, 4
};
static float yx[] = {
	0.f, 0.f, -.5f, .5f
};
static float yy[] = {
	0.f, .6f, 1.f, 1.f
};
static int yorder[] = {
	1, 2, 3, -2, 4
};
static float zx[] = {
	1.f, 0.f, 1.f, 0.f, .25f, .75f
};
static float zy[] = {
	.5f, .5f, -.5f, -.5f, 0.f, 0.f
};
static int zorder[] = {
	1, 2, 3, 4, -5, 6
};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;
// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;
//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)
void Axes(float length) {
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++) {
		int j = xorder[i];
		if (j < 0) {

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact*xx[j], fact*xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++) {
		int j = yorder[i];
		if (j < 0) {

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact*yx[j], base + fact*yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++) {
		int j = zorder[i];
		if (j < 0) {

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact*zy[j], base + fact*zx[j]);
	}
	glEnd();
}