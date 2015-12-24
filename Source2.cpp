//
//  //
//  Created by Maggie Moheb on 11/3/15.
//  Copyright (c) 2015 Maggie Moheb. All rights reserved.
//
/*
#include <time.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <random>
#include <glut.h>


float cameraX, cameraY, cameraZ, eyeX, eyeY, eyeZ;
float cameraAcceleration = 0;
float timerTime = 0;
int count = 0;
int lastLevel = 0;
float ballSpeed;
clock_t jumpTime;
float jumpBeginX;
float jumpBeginY;
float jumpEndY;
bool jump;
class Level {
public:
	GLdouble x, y, xRight, xLeft, direction;
	Level *levelBefore, *levelAfter;

	Level() {
		this->x = 0;
		this->y = 0;
		this->xRight = 0;
		this->xLeft = 0;
		this->direction = NULL;
		levelBefore = NULL;
		levelAfter = NULL;
	}

	Level(GLdouble x, GLdouble y, GLdouble direction) {
		this->x = x;
		this->y = y;
		this->xLeft = direction - (x);
		this->xRight = direction + (x);
		this->direction = direction;
		levelBefore = NULL;
		levelAfter = NULL;
	}
	void drawLevel() {
		glPushMatrix();
		glColor3f(0.2, 0.8, 0.2);
		glTranslated(this->direction, this->y, 0);
		glScaled(this->x * 2, 2, 2);
		glutSolidCube(1);
		glPopMatrix();
	}
};
std::vector<Level> Levels;

class Ball {
public:
	GLdouble x, y;
	Level level;
	bool fall;
	Ball() {
		this->x = 0;
		this->y = 0;
		this->level = *new Level(0, 0, -50);
		fall = false;
	}
	Ball(GLdouble x, GLdouble y) {
		this->x = x;
		this->y = y;
		this->level = *new Level(0, 0, -50);
		fall = false;
	}
	void setLevel(Level level) {
		this->level = level;
		this->y = level.y + 5;
	}
	void drawBall() {
		glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslated(this->x, this->y, 0);
		glutSolidSphere(4, 25, 25);
		glPopMatrix();
	}

};
Ball ball;




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
	gluPerspective(60, 640 / 480, 0.001, 300);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraX, cameraY + 100, cameraZ, eyeX, eyeY + 100, eyeZ, 0.0, 1.0, 0.0);
}

void createScene() {

	for (int i = 0; i<Levels.size(); i++) {
		if (jump) {
			if (ball.y - Levels[i].y >= 0
				&& ball.y - Levels[i].y <= 25
				&& ball.x <= Levels[i].xRight
				&& ball.x >= Levels[i].xLeft
				) {
				ball.level = (Levels[i]);
				ball.y = Levels[i].y + 5;
				jumpEndY = ball.y;
			}
		}
		else {
			if (ball.y - Levels[i].y >= 0
				&& ball.y - Levels[i].y <= 25)
				//            printf("%f %f %f %f \n",ball.x,Levels[i].x,Levels[i].xRight,Levels[i].xLeft);
				if (ball.y - Levels[i].y >= 0 && ball.y - Levels[i].y <= 25) {
					if ((ball.x>Levels[i].xRight&& ball.x>Levels[i].xLeft)
						|| (ball.x<Levels[i].xRight&& ball.x<Levels[i].xLeft)) {
						ball.level = (Levels[i - 1]);
						ball.y = Levels[i - 1].y + 5;
					}
				}
		}
		Levels[i].drawLevel();
	}
}


void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	setupLights();
	setupCamera();

	if (Levels[0].y <cameraY - 100) {
		for (int j = 0; j<Levels.size() - 1; j++) {
			Levels[j] = Levels[j + 1];
		}
		Levels.pop_back();
		Levels[0].levelBefore = NULL;
	}
	createScene();
	ball.drawBall();
	glFlush();
}

void key(unsigned char key, int x, int y) {
	if (key == ' '&& !jump) {
		jump = true;
		jumpTime = clock();
		jumpBeginX = ball.x;
		jumpBeginY = ball.y;
		jumpEndY = ball.y;

	}
	if (key == 'd') {
		ballSpeed = fabs(ballSpeed);
		ball.x += ballSpeed;
		//        printf("*****************************");
		//        printf("ballx: %f", ball.x);
		//        printf("direction: %f", ball.level.direction);
		//        printf("x: %f", ball.level.x);
		//        printf("xleft: %f", ball.level.xLeft);
		//        printf("xRight: %f", ball.level.xRight);
		//        printf("*****************************");

	}
	if (key == 'a') {
		ballSpeed = fabs(ballSpeed)*-1;
		ball.x += ballSpeed;
	}
}

void Timer(int value) {
	for (int i = 0; i<10; i++) {
		lastLevel += ((rand() % 10) + 40);
		int x = ((rand() % 10) + 45);
		int direction = ((rand() % 10) - 40);
		Level level = *new Level(x, lastLevel, direction);

		level.levelBefore = &Levels[Levels.size() - 1];
		//  printf("level.levelBeforeX: %f", level.levelBefore->y);
		//        printf("Levels.size()-1: %lu", Levels.size()-1);
		Levels[Levels.size() - 1].levelAfter = &level;
		Levels.push_back(level);
		x = ((rand() % 10) + 45);
		lastLevel += ((rand() % 10) + 40);
		direction = ((rand() % 10) + 40);
		Level level1 = *new Level(x, lastLevel, direction);
		level1.levelBefore = &Levels[Levels.size() - 1];
		// printf("level1.levelBeforeX: %f", level1.levelBefore->y);

		//   printf("Levels.size()-1: %lu", Levels.size()-1);
		Levels[Levels.size() - 1].levelAfter = &level1;
		Levels.push_back(level1);
	}

	cameraAcceleration += 0.01;
	cameraY += 0.1;

	glutPostRedisplay();
	glutTimerFunc(5000, Timer, 0);
}


void Anim() {
	//    cameraY += cameraAcceleration;
	//    eyeY += cameraAcceleration;
	if (ball.y - cameraY >= 1.05 * 20) {
		//        cameraY += 5;
		//        eyeY += 5;

	}
	if (jump) {
		float t = (clock() - jumpTime) / 1200;
		ball.y = (35 * t - 0.5*9.81*t*t) + jumpBeginY;
		ball.x += ballSpeed;
		if (ball.y <jumpEndY) {
			ball.y = jumpEndY;
			jump = false;
		}
	}


	//    if((ball.x>ball.level.xRight || ball.x<ball.level.xLeft) && (ball.level.xLeft != ball.level.xRight)) {
	//        ball.fall = true;
	//        
	//        if(ball.level.levelBefore != NULL && (ball.level.direction>30|| ball.level.direction<-20||ball.level.direction == 0)) {
	//
	//            ball.level = *ball.level.levelBefore;
	//        } else {
	//           
	//        }
	//    }
	if (ball.x <-70) {
		ball.x = -70;
	}
	if (ball.x >72) {
		ball.x = 72;
	}
	//    if(ball.fall) {
	//        ball.y -=1;
	////        printf("ball y: %f", ball.y);
	////        printf("ball.levely: %f", ball.level.y+5);
	//        if(ball.y<=ball.level.y+5) {
	//            ball.fall = false;
	//            ball.y = ball.level.y+5;
	//        }
	//    }
	//    if(ball.x>65 || ball.x<-65) {
	//        ballSpeed = ballSpeed *-1;
	//    }

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	cameraX = 0;
	cameraY = 30;
	cameraZ = 130;
	eyeX = 0;
	eyeY = 0;
	eyeZ = 0;
	lastLevel = 0;
	cameraAcceleration = 0.1;
	timerTime = 2 * 1000;
	lastLevel += ((rand() % 10) + 40);

	ballSpeed = 3;
	jumpTime = clock();
	jumpBeginY = clock();
	int x = ((rand() % 10) + 705);
	int direction = 0;
	ball.x = direction;
	ball.y = lastLevel + 5;

	Level level1 = *new Level(x, lastLevel, direction);
	level1.levelBefore = new Level(-1000021, -111, 0);
	Levels.push_back(level1);
	ball = *new Ball(ball.x, ball.y);

	ball.setLevel(level1);
	//    printf("Initial ball level x: %f ", ball.level.x);
	glutInitWindowSize(800, 1000);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Icy Tower");
	glutDisplayFunc(Display);
	glutIdleFunc(Anim);
	glutKeyboardFunc(key);
	glutTimerFunc(0, Timer, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
*/
