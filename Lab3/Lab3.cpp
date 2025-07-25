#include <math.h>
#include "glut.h"

int wWidth = 640, wHeight = 480;

double rotationAngle = 0, rotationDelta = 75;

void displayFunc() {
	// Очистка буферов цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix(); {
		// Перемещение каркасного куба вперед камеры
		glTranslatef(0, 0, -5);
		// Поворот куба
		glRotatef(rotationAngle, 1, 0, 0);
		glPushMatrix(); {
			glRotatef(rotationAngle, 0, 1, 0);
			// Закраска ребер белым цветом
			glColor3f(1, 1, 1);
			glutWireCube(3);
		} glPopMatrix();
	} glPopMatrix();
	glPushMatrix(); {
		// Перемещение четырехугольной пирамиды вперед камеры
		glTranslatef(0, 0, -5);
		// Вращение пирамиды
		glRotatef(rotationAngle, 1, 0, 0);
		glPushMatrix(); {
			glRotatef(-2 * rotationAngle, 0, 1, 0);
			// Установка размера
			double size = 1;
			// Отрисовка боковых граней веером треугольников
			glBegin(GL_TRIANGLE_FAN); {
				glVertex3f(0, size, 0);
				glColor3f(1, 1, 0);
				glVertex3f(-size, -size, -size);
				glColor3f(1, 0, 0);
				glVertex3f(-size, -size, size);
				glColor3f(0, 1, 0);
				glVertex3f(size, -size, size);
				glColor3f(0, 0, 1);
				glVertex3f(size, -size, -size);
				glColor3f(1, 0, 1);
				glVertex3f(-size, -size, -size);
				glColor3f(1, 1, 0);
			} glEnd();
			// Отрисовка основания
			glBegin(GL_POLYGON); {
				glVertex3f(size, -size, -size);
				glVertex3f(size, -size, size);
				glVertex3f(-size, -size, size);
				glVertex3f(-size, -size, -size);
			} glEnd();
		} glPopMatrix();
	} glPopMatrix();
	// Вывод на экран
	glutSwapBuffers();
}

void reshapeFunc(int width, int height) {
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double fovy = 60.0, zNear = 1e-3, zFar = 1e4;
	double aspect = 1.0 * width / height;
	gluPerspective(fovy, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wWidth = width;
	wHeight = height;
}

void timerFunc(int time) {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	double timeDelta = 1.0 * (currentTime - time) / 1000;
	rotationAngle += (rotationDelta * timeDelta);
	glutPostRedisplay();
	glutTimerFunc(1, timerFunc, currentTime);
}

void init() {
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);
	glutCreateWindow("Lab3");
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutTimerFunc(1, timerFunc, 0);
	init();
	glutMainLoop();
	return 0;
}