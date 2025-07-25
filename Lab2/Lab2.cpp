#include <iostream>
#include <math.h>
#include "glut.h"

#define PI acos(-1)
#define DEG_TO_RAD(X) (PI * X / 180)

struct Matrix3x3 {
	double data[3][3];

	Matrix3x3() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				data[i][j] = 0;
			}
		}
	}

	static Matrix3x3 getIdentity();
	static Matrix3x3 getTranslation(double, double);
	static Matrix3x3 getScaling(double, double);
	static Matrix3x3 getRotation(double);

	static double* toGLUTFormat(const Matrix3x3&);

	friend Matrix3x3 operator*(const Matrix3x3&, const Matrix3x3&);
};

Matrix3x3 Matrix3x3::getIdentity() {
	Matrix3x3 m;
	for (int i = 0; i < 3; i++) {
		m.data[i][i] = 1;
	}
	return m;
}

Matrix3x3 Matrix3x3::getTranslation(double x, double y) {
	Matrix3x3 m = getIdentity();
	m.data[2][0] = x;
	m.data[2][1] = y;
	return m;
}

Matrix3x3 Matrix3x3::getScaling(double x, double y) {
	Matrix3x3 m = getIdentity();
	m.data[0][0] = x;
	m.data[1][1] = y;
	return m;
}

Matrix3x3 Matrix3x3::getRotation(double angle) {
	Matrix3x3 m = getIdentity();
	m.data[0][0] = m.data[1][1] = cos(angle);
	m.data[0][1] = m.data[1][0] = sin(angle);
	m.data[0][1] = -m.data[0][1];
	return m;
}

double* Matrix3x3::toGLUTFormat(const Matrix3x3& m) {
	double* pointer = new double[16];
	pointer[0] = m.data[0][0];
	pointer[1] = m.data[1][0];
	pointer[2] = 0;
	pointer[3] = 0;
	pointer[4] = m.data[0][1];
	pointer[5] = m.data[1][1];
	pointer[6] = 0;
	pointer[7] = 0;
	pointer[8] = 0;
	pointer[9] = 0;
	pointer[10] = 1;
	pointer[11] = 0;
	pointer[12] = m.data[2][0];
	pointer[13] = m.data[2][1];
	pointer[14] = 0;
	pointer[15] = m.data[2][2];

	return pointer;
}

Matrix3x3 operator*(const Matrix3x3& m1, const Matrix3x3& m2) {
	Matrix3x3 m;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				m.data[i][j] += m1.data[i][k] * m2.data[k][j];
			}
		}
	}
	return m;
}

struct Point {
	double x, y;
};

int wWidth = 640, wHeight = 480;
double sizeX, sizeY;

int const vertexCount = 5;
// clockwise...
Point polygon[vertexCount] = {
	Point { 2, 2 },
	Point { -1, -2 },
	Point { -1, 0 },
	Point { -3, -2 },
	Point { -2, 4 }
};

double rotationAngle = 0, rotationDelta = 90.0;
double translationX = 0, translationY = 0, translationDelta = 6.5;
double scalingX = 1, scalingY = 1, scalingDelta = 1.6;
bool invertX = false, invertY = false;

bool keys[12];

void displayFunc() {
	// Очистка экрана
	glClear(GL_COLOR_BUFFER_BIT);
	// Отрисовка осей системы координат
	glLineWidth(1.0);
	glColor3f(0, 1, 0);
	glBegin(GL_LINES); {
		glVertex2f(-sizeX, 0);
		glVertex2f(sizeX, 0);
		for (double x = 0; x < sizeX; x += 1) {
			glVertex2f(x, -0.2);
			glVertex2f(x, 0.2);
			glVertex2f(-x, -0.2);
			glVertex2f(-x, 0.2);
		}
		glVertex2f(0, -sizeY);
		glVertex2f(0, sizeY);
		for (double y = 0; y < sizeY; y += 1) {
			glVertex2f(-0.2, -y);
			glVertex2f(0.2, -y);
			glVertex2f(-0.2, y);
			glVertex2f(0.2, y);
		}
	} glEnd();
	// Отрисовка заданной фигуры
	glLineWidth(2.0);
	glColor3f(1, 1, 1);
	// Создание матрицы трансформации
	Matrix3x3 result = Matrix3x3::getIdentity();
	result = result * Matrix3x3::getRotation(DEG_TO_RAD(rotationAngle));
	if (invertX) result = result * Matrix3x3::getScaling(-1, 1);
	if (invertY) result = result * Matrix3x3::getScaling(1, -1);
	result = result * Matrix3x3::getScaling(scalingX, scalingY);
	result = result * Matrix3x3::getTranslation(translationX, translationY);
	glPushMatrix(); {
		// Применение трансформации
		glMultMatrixd(Matrix3x3::toGLUTFormat(result));
		glBegin(GL_LINE_LOOP); {
			for (int i = 0; i < vertexCount; i++) {
				glVertex2f(polygon[i].x, polygon[i].y);
			}
		} glEnd();
	} glPopMatrix();
	// Смена буферов
	glutSwapBuffers();
}

void reshapeFunc(int width, int height) {
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspect = 1.0 * width / height, size = 10;
	if (width < height) {
		sizeX = size;
		sizeY = size / aspect;
		gluOrtho2D(-sizeX, sizeX, -sizeY, sizeY);
	}
	else {
		sizeX = size * aspect;
		sizeY = size;
		gluOrtho2D(-sizeX, sizeX, -sizeY, sizeY);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wWidth = width;
	wHeight = height;
}

void keyboardFunc(unsigned char key, int x, int y) {
	if (key == 'W' || key == 'w') keys[0] = true;
	if (key == 'S' || key == 's') keys[1] = true;
	if (key == 'A' || key == 'a') keys[2] = true;
	if (key == 'D' || key == 'd') keys[3] = true;
	if (key == 'Q' || key == 'q') keys[4] = true;
	if (key == 'E' || key == 'e') keys[5] = true;
	if (key == 'Z' || key == 'z') keys[6] = true;
	if (key == 'X' || key == 'x') keys[7] = true;
	if (key == 'C' || key == 'c') keys[8] = true;
	if (key == 'V' || key == 'v') keys[9] = true;
	if (key == 'R' || key == 'r') keys[10] = true;
	if (key == 'F' || key == 'f') keys[11] = true;
}

void keyboardUpFunc(unsigned char key, int x, int y) {
	if (key == 'W' || key == 'w') keys[0] = false;
	if (key == 'S' || key == 's') keys[1] = false;
	if (key == 'A' || key == 'a') keys[2] = false;
	if (key == 'D' || key == 'd') keys[3] = false;
	if (key == 'Q' || key == 'q') keys[4] = false;
	if (key == 'E' || key == 'e') keys[5] = false;
	if (key == 'Z' || key == 'z') keys[6] = false;
	if (key == 'X' || key == 'x') keys[7] = false;
	if (key == 'C' || key == 'c') keys[8] = false;
	if (key == 'V' || key == 'v') keys[9] = false;
	if (key == 'R' || key == 'r') keys[10] = false;
	if (key == 'F' || key == 'f') keys[11] = false;
}

void timerFunc(int time) {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	double timeDelta = 1.0 * (currentTime - time) / 1000;
	if (keys[0]) translationY += (translationDelta * timeDelta);
	if (keys[1]) translationY -= (translationDelta * timeDelta);
	if (keys[2]) translationX -= (translationDelta * timeDelta);
	if (keys[3]) translationX += (translationDelta * timeDelta);
	if (keys[4]) rotationAngle += (rotationDelta * timeDelta);
	if (keys[5]) rotationAngle -= (rotationDelta * timeDelta);
	if (keys[6] && scalingX - scalingDelta * timeDelta > 0) scalingX -= (scalingDelta * timeDelta);
	if (keys[7]) scalingX += (scalingDelta * timeDelta);
	if (keys[8] && scalingY - scalingDelta * timeDelta > 0) scalingY -= (scalingDelta * timeDelta);
	if (keys[9]) scalingY += (scalingDelta * timeDelta);
	if (keys[10]) {
		invertX = !invertX;
		keys[10] = false;
	}
	if (keys[11]) {
		invertY = !invertY;
		keys[11] = false;
	}
	glutPostRedisplay();
	glutTimerFunc(1, timerFunc, currentTime);
}

void printHints() {
	setlocale(LC_ALL, "Russian");
	std::cout << "Управление:\n";
	std::cout << "Перемещение (\'W\', \'S\', \'A\', \'D\')\n";
	std::cout << "Поворот (\'Q\', \'E\')\n";
	std::cout << "Масштабирование (\'Z\', \'X\', \'C\', \'V\')\n";
	std::cout << "Отражение (\'R\', \'F\')\n";
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(wWidth, wHeight);
	glutCreateWindow("Lab2");
	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutTimerFunc(1, timerFunc, 0);
	printHints();
	glutMainLoop();
	return 0;
}