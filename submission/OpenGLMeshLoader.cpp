#include "Model_3DS.h"
#include <math.h>
#include <glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <random>


UINT SkyboxTexture[6];
float cameraX, cameraY, cameraZ = 130, eyeX, eyeY, eyeZ;
float cameraAcceleration = 0;
float timerTime = 0;
int count = 0;
int lastLevel = 0;
float characterSpeed;
float jumpTime;
float jumpBeginX;
float jumpBeginY;
float jumpEndY;
bool jump;
float moverRightLeftSpeed = 1.8;
bool win = false;
GLTexture level;
GLTexture skyFront;
GLTexture skyBack;
GLTexture skyLeft;
GLTexture skyRight;
GLTexture skyUp;
GLTexture skyDown;
Model_3DS banana;
float cangle = 0;
bool cinematic = false;
bool lost = false;
bool move = false;
float jumpVelocity = 25;
int coins = 0;
static void drawBox(GLfloat size, GLenum type)
{
	glColor3f(1, 1, 1);
	static GLfloat n[6][3] =
	{
		{ -1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.0, -1.0 }
	};
	static GLint faces[6][4] =
	{
		{ 0, 1, 2, 3 },
		{ 3, 2, 6, 7 },
		{ 7, 6, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 5, 6, 2, 1 },
		{ 7, 4, 0, 3 }
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0, 0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0, 1);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1, 1);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1, 0);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void drawBanana(float x, float y, float z) {
	glPushMatrix();
	glTranslated(x, y, -z);
	glScaled(1.5, 1.5, 1.5);
	banana.Draw();
	glPopMatrix();
}


class Level {
public:
	GLdouble x, y, xRight, xLeft, direction;
	Level *levelBefore, *levelAfter;
	bool drawB, counted;
	Level() {
		this->x = 0;
		this->y = 0;
		this->xRight = 0;
		this->xLeft = 0;
		this->direction = NULL;
		levelBefore = NULL;
		levelAfter = NULL;
		drawB = true;
		counted = false;
	}

	Level(GLdouble x, GLdouble y, GLdouble direction) {
		this->x = x;
		this->y = y;
		this->direction = direction;
		this->xLeft = direction - (0.5*x);
		this->xRight = direction + (0.5*x);
		levelBefore = NULL;
		levelAfter = NULL;
		drawB = true;
		counted = false;
	}
	void drawLevel() {
		level.Use();
		glPushMatrix();
		glTranslated(this->direction, this->y, 0);
		glScaled(this->x, 3, 3);
		drawBox(1, GL_QUADS);
		glPopMatrix();

	}
};
std::vector<Level> Levels;

class Character {
public:
	Model_3DS c;
	GLdouble x, y, rotate = 0;
	Level level;
	float angleOfLooking = 0;
	bool fall;
	Character() {
		this->x = 0;
		this->y = 0;
		this->level = *new Level(0, 0, -50);
		fall = false;
	}
	Character(GLdouble x, GLdouble y) {
		this->x = x;
		this->y = y;
		this->level = *new Level(0, 0, -50);
		fall = false;
	}
	void setLevel(Level level) {
		this->level = level;
		this->y = level.y + 5;
	}
	void drawCharacter() {

		glPushMatrix();
		glColor3f(1, 1, 1);

		glTranslated(this->x, this->y + 3.5, 0);
		glRotated(angleOfLooking, 0, 1, 0);
		//for (int i = 0; i<100000; i++);
		//glRotated(-1*angleOfLooking, 0, 1, 0);

		if (jump) {
			if (characterSpeed > 0)
				glRotated(rotate, 0, 0, 1);
			//glRotated(-45, 0, 0, 1);
			else
				glRotated(45, 0, 0, 1);

		}
		//if (fall)
		//	glRotated(180, 0, 0, 1);

		glScalef(5, 5, 5);
		c.Draw();
		glPopMatrix();
	}

};
Character character;

void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
	// Center the Skybox around the given x,y,z position
	x = x - width / 2;
	y = y - height / 2;
	z = z - length / 2;

	glEnable(GL_TEXTURE_2D);
	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, skyFront.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, skyBack.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, skyLeft.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, skyRight.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, skyUp.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x, y + height, z);
	glEnd();

	// Draw Down side
	glBindTexture(GL_TEXTURE_2D, skyDown.texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(8.0f, 8.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 8.0f); glVertex3f(x + width, y, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}



void setupLights() {
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat diffuse1[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	GLfloat specular1[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

	GLfloat lightPosition[] = { 100.0f, 30.0f, 100.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	GLfloat lightPosition1[] = { -3.0f, 100.0f, 20.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat shininess[] = { 96.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 700);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraX, cameraY + 100, cameraZ, eyeX, eyeY + 100, eyeZ, 0.0, 1.0, 0.0);


}
void rotateCamera() {
	cameraX = 120 * sin(cangle);
	cameraZ = 150 * cos(cangle);
	//cameraY = 100;
}

void displayText(float x, float y, float z, int r, int g, int b, std::string string) {
	long j = string.length();

	glColor3f(r, g, b);
	glRasterPos3f(x, y, z);
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}
void createScene() {
	for (int i = 0; i<Levels.size(); i++) {
		if (jump) {
			if (character.y - Levels[i].y >= 0
				&& character.y - Levels[i].y <= 25
				&& character.x <= Levels[i].xRight
				&& character.x >= Levels[i].xLeft
				) {
				character.level = (Levels[i]);
				character.y = Levels[i].y + 5;
				jumpEndY = character.y;
			}

		}
		if (character.y - Levels[i].y >= 0 && character.y - Levels[i].y <= 25)
			if (character.x>Levels[i].xRight || character.x<Levels[i].xLeft) {
				if (i != 0) {
					character.level = Levels[i - 1];
					//character.y = character.level.y + 5;
					character.fall = true;


					//sound for falling
				}

			}

		glColor3f(1, 1, 1);
		Levels[i].drawLevel();
		if (Levels[i].drawB) {
			drawBanana(Levels[i].direction - 2, Levels[i].y + 1, 35);
		}

		if ((int)Levels[i].direction >= (int)character.x - 18 && (int)Levels[i].direction < (int)character.x - 10 && (int)Levels[i].y >= (int)character.y - 14 && (int)Levels[i].y < (int)character.y - 2) {
			Levels[i].drawB = false;
			if (!Levels[i].counted) {
				++coins;
				Levels[i].counted = true;

				PlaySound(NULL, 0, 0);
				PlaySound(TEXT("sounds\\banana.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
		}
	}
}



void Display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	setupLights();
	setupCamera();
	drawBanana(30, cameraY + 90, 0);


	if (coins >= 50) {
		win = true;
		//sound for winning
	}

	if (cinematic)
		rotateCamera();
	glColor3f(1, 1, 1);
	Draw_Skybox(cameraX, cameraY, cameraZ, 400, 800, 400);
	if (Levels.size() > 0) {
		if (Levels[0].y <cameraY - 100) {
			printf("deleting level");
			Levels.erase(Levels.begin());
			//	for (int j = 0; j<Levels.size() - 1; j++) {
			//		Levels[j] = Levels[j + 1];
			//	}
			//	Levels.pop_back();
			if (Levels.size()>0) {
				Levels[0].levelBefore = NULL;
			}

		}
	}
	char result[10];
	sprintf(result, "%i", coins);
	//std::string name = "Score: ", result;
	displayText(50, cameraY + 110, 0, 1, 1, 1, result);

	if (!lost&!win) {
		createScene();
		character.drawCharacter();
		//std::string name = "Score: ";
		//	char numstr[2];
		///	result = name + numstr;
	}
	else {
		if (lost)
			displayText(-20, cameraY + 100, 0, 1, 1, 1, "GAME OVER!");

		if (win)
			displayText(-20, cameraY + 100, 0, 1, 1, 1, "YOU WON :)");

	}

	glFlush();
}


void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON&!lost&!win) {
		//if (state == GLUT_DOWN) {
		cameraZ += 2.5;
		//eyeZ += 2.5;
		printf("Z: %f", cameraZ);
		//}
	}
	else
		if (button == GLUT_RIGHT_BUTTON&!lost&!win)
		{
			cameraZ -= 2.5;
		}
	/*	else
	if (button == GLUT_MIDDLE_BUTTON){
	}*/
	// ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();
}
void key(unsigned char key, int x, int y) {
	if (key == ' ' &(!character.fall || character.x>60 || character.x<-70) && jumpVelocity<30) {
		// sound for jumping

		character.angleOfLooking = 0;
		jump = true;
		if (jumpVelocity < 30) {
			//sound for double jump
			jumpVelocity = 1.3*jumpVelocity;
			//cameraAcceleration = 4*cameraAcceleration;
			cameraY += jumpVelocity / 2;
			eyeY += jumpVelocity / 2;

		}
		else {
			jump = false;
		}
		jumpTime = clock();
		jumpBeginX = character.x;
		jumpBeginY = character.y;
		jumpEndY = character.y;
	}
	if (key == 'd') {
		moverRightLeftSpeed = fabs(moverRightLeftSpeed);
		characterSpeed = fabs(characterSpeed);
		character.x += characterSpeed;
		character.angleOfLooking = 90;
	}
	if (key == 'a') {
		moverRightLeftSpeed = fabs(moverRightLeftSpeed) *-1;
		characterSpeed = fabs(characterSpeed)*-1;
		character.x += characterSpeed;
		character.angleOfLooking = -90;

	}

	if (key == 'v'&!lost&!win) {
		cinematic = true;
		rotateCamera();
	}
	if (key == 'c') {
		eyeY--;
		cameraY--;
	}

}


void Timer(int value) {

	if (lost | win)
		return;

	for (int i = 0; i<10; i++) {
		lastLevel += ((rand() % 10) + 40);
		int x = ((rand() % 20) + 55);
		int a = rand();
		int direction;
		int b = rand() % 50;
		if (a % 2 == 0) {
			direction = ((rand() % 10) - b);
		}
		else {
			direction = ((rand() % 10) + b);
		}
		Level level = *new Level(x, lastLevel, direction);
		level.levelBefore = &Levels[Levels.size() - 1];
		//        printf("Levels.size()-1: %lu", Levels.size()-1);
		Levels[Levels.size() - 1].levelAfter = &level;
		Levels.push_back(level);
		x = ((rand() % 10) + 45);
		lastLevel += ((rand() % 10) + 40);
		a = rand();
		b = rand() % 50;
		if (a % 2 == 0)
			direction = ((rand() % 10) + b);
		else
			direction = ((rand() % 10) - b);
		Level level1 = *new Level(x, lastLevel, direction);
		level1.levelBefore = &Levels[Levels.size() - 1];
		//   printf("Levels.size()-1: %lu", Levels.size()-1);
		Levels[Levels.size() - 1].levelAfter = &level1;
		Levels.push_back(level1);
	}

	cameraAcceleration += 0.1;
	//cameraY += 0.001;
	glutPostRedisplay();
	glutTimerFunc(10000, Timer, 0);
}

void Timer2(int value) {
	if (lost) {
		PlaySound(NULL, 0, 0);
		PlaySound(TEXT("sounds\\lost.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return;
	}
	if (win) {
		PlaySound(NULL, 0, 0);
		PlaySound(TEXT("sounds\\win.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return;
	}
	glutTimerFunc(1, Timer2, 0);
}

void Anim() {
	if (lost | win)
		return;
	if (move) {
		cameraY += cameraAcceleration;
		eyeY += cameraAcceleration;
	}

	if (character.y < cameraY - 40) {
		lost = true;
		//sound for losing
	}
	if (cinematic) {
		if (cangle < 6.28)
			cangle += 0.01;
		else {
			cangle = 0;
			cinematic = false;
		}

	}


	if (character.level.y - cameraY >= 1.5 * 50) {
		cameraY += 0.3;
		eyeY += 0.3;

	}
	if (character.x>character.level.xRight || character.x<character.level.xLeft) {
		character.fall = true;
		character.angleOfLooking = 0;
		if (character.level.levelBefore != NULL && character.level.direction>30) {

			character.level = *character.level.levelBefore;
		}
		else {

		}
	}
	if (character.x <-70) {
		character.x = -70;
	}
	if (character.x >72) {
		character.x = 72;
	}
	if (character.fall) {
		character.y -= 1;
		if (character.y <= character.level.y + 5) {
			character.fall = false;
			character.angleOfLooking = 0;
			character.y = character.level.y + 5;
		}
	}
	if (character.x>65 || character.x<-65) {
		moverRightLeftSpeed *= -1;
		characterSpeed = characterSpeed *-1;
	}
	if (jump) {
		move = true;
		character.rotate += 10;
		float t = (clock() - jumpTime) / 100;
		character.y = (jumpVelocity *t - 0.5*9.81*t*t) + jumpBeginY;
		character.x += characterSpeed;
		if (character.y <jumpEndY) {
			character.y = jumpEndY;
			jumpVelocity = 28;
			jump = false;
			cameraAcceleration = 0.05;
		}
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	cameraX = 0;
	cameraY = 30;
	cameraZ = 150;
	eyeX = 0;
	eyeY = 0;
	eyeZ = 0;
	lastLevel = 0;
	cameraAcceleration = 0.1;
	timerTime = 2 * 1000;
	lastLevel += ((rand() % 10) + 40);
	jump = false;
	characterSpeed = 0.8;
	jumpTime = clock();
	jumpBeginY = clock();
	/*
	int x = ((rand() % 10) + 45);
	int direction = ((rand() % 10) - 15);*/
	int x = ((rand() % 10) + 705);
	int direction = 0;
	character.x = direction;
	character.y = lastLevel + 5;

	Level level1 = *new Level(x, lastLevel, direction);
	level1.levelBefore = new Level(-1000021, -111, 0);
	Levels.push_back(level1);
	character.setLevel(level1);
	glutInitWindowSize(800, 1000);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Icy Tower");
	glutDisplayFunc(Display);

	glutIdleFunc(Anim);
	glutKeyboardFunc(key);
	glutMouseFunc(Mouse);
	glutTimerFunc(0, Timer, 0);
	glutTimerFunc(0, Timer2, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	character.c.Load("Models/character.3ds");

	banana.Load("Models/banana.3ds");
	level.Load("Textures/brick3.bmp");
	skyFront.Load("Textures/brick.bmp");
	skyBack.Load("Textures/brick.bmp");
	skyLeft.Load("Textures/brick2.bmp");
	skyRight.Load("Textures/brick2.bmp");
	skyUp.Load("Textures/brick.bmp");
	skyDown.Load("Textures/grass2.bmp");
	glutMainLoop();
}













