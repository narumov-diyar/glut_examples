#include <math.h>
#include "glut.h"

struct Complex {
	double a = 0, b = 0;
};

Complex operator+(const Complex& c1, const Complex& c2) {
	return Complex{ c1.a + c2.a, c1.b + c2.b };
}

Complex operator-(const Complex& c1, const Complex& c2) {
	return Complex{ c1.a - c2.a, c1.b - c2.b };
}

Complex operator*(const Complex& c1, const Complex& c2) {
	return Complex{ c1.a * c2.a - c1.b * c2.b, c1.a * c2.b + c1.b * c2.a };
}

Complex operator/(const Complex& c1, const Complex& c2) {
	double length = c2.a * c2.a + c2.b * c2.b;
	return c1 * Complex{ c2.a / length, -c2.b / length };
}

int wWidth = 640, wHeight = 480;

void displayFunc() {
	// Очистка экрана
	glClear(GL_COLOR_BUFFER_BIT);
	// Установка рамок для вычисления
	double x0 = -0.9, xn = 0.9;
	double y0 = -0.8, yn = 0.8;
	double lX = xn - x0, lY = yn - y0;
	double lMax = (lX > lY ? lX : lY);
	lX /= lMax, lY /= lMax;
	double scale;
	// Определение масштаба вписываемого фрактала
	{
		double l = 0, r = 10000;
		while (r - l >= 1e-3) {
			double m = (l + r) / 2;
			if (int(lX * m) <= wWidth && int(lY * m) <= wHeight) {
				l = m;
			}
			else {
				r = m;
			}
		}
		scale = l;
	}
	// Вычисление размеров
	int width = lX * scale * 0.9, height = lY * scale * 0.9;
	// Отрисовка точек
	glBegin(GL_POINTS); {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Complex p;
				p.a = x0 + (xn - x0) * x / width;
				p.b = y0 + (yn - y0) * y / height;
				int iter = 0, maxIter = 100;
				for (iter; iter < maxIter; iter++) {
					Complex tp = p - (p * p * p - Complex{ 1 }) / (p * p * Complex{ 3 });
					// Проверка на достижение требуемой точности
					if (sqrt(pow(p.a - tp.a, 2) + pow(p.b - tp.b, 2)) < 1e-5) break;
					p = tp;
				}
				iter %= maxIter;
				// Установка цвета пикселя по градиенту
				glColor3ub(255.0 * sin(-0.05 * iter), 255.0 * sin(-0.1 * iter), 255.0 * sin(-0.2 * iter));
				glVertex2f(wWidth / 2 - width / 2 + x, wHeight / 2 - height / 2 + y);
			}
		}
	} glEnd();
	// Запрос на отрисовку
	glFlush();
}

void reshapeFunc(int width, int height) {
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wWidth = width;
	wHeight = height;
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(wWidth, wHeight);
	glutCreateWindow("Lab1");
	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop();
	return 0;
}