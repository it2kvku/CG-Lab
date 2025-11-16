#include <GL/glut.h>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Point {
    float x, y;
};

// Tam giác ABC gốc
Point A = {200, 200};
Point B = {350, 200};
Point C = {275, 350};

// Tam giác sau biến đổi
Point A_prime, B_prime, C_prime;
bool hasTransformed = false;

// Tâm quay và góc quay
Point rotationCenter = {300, 300};
float angle = 45.0f; // độ
int clickCount = 0;

Point multiplyMatrix(float matrix[3][3], Point p) {
    Point result;
    float w;
    
    result.x = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2];
    result.y = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2];
    w = matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2];
    
    if (w != 0) {
        result.x /= w;
        result.y /= w;
    }
    
    return result;
}

// Phép quay với tâm (x0, y0)
void rotateWithCenter(float angleDeg, float x0, float y0) {
    float angleRad = angleDeg * M_PI / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    
    // Ma trận tịnh tiến về gốc
    float T1[3][3] = {
        {1, 0, -x0},
        {0, 1, -y0},
        {0, 0, 1}
    };
    
    // Ma trận quay
    float R[3][3] = {
        {cosA, -sinA, 0},
        {sinA, cosA, 0},
        {0, 0, 1}
    };
    
    // Ma trận tịnh tiến về vị trí ban đầu
    float T2[3][3] = {
        {1, 0, x0},
        {0, 1, y0},
        {0, 0, 1}
    };
    
    // Kết hợp: T2 * R * T1
    float temp[3][3];
    float result[3][3];
    
    // temp = R * T1
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                temp[i][j] += R[i][k] * T1[k][j];
            }
        }
    }
    
    // result = T2 * temp
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                result[i][j] += T2[i][k] * temp[k][j];
            }
        }
    }
    
    A_prime = multiplyMatrix(result, A);
    B_prime = multiplyMatrix(result, B);
    C_prime = multiplyMatrix(result, C);
    
    hasTransformed = true;
    
    std::cout << "Quay: goc=" << angleDeg << " do, tam=(" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "A' = (" << A_prime.x << ", " << A_prime.y << ")" << std::endl;
    std::cout << "B' = (" << B_prime.x << ", " << B_prime.y << ")" << std::endl;
    std::cout << "C' = (" << C_prime.x << ", " << C_prime.y << ")" << std::endl;
}

void drawTriangle(Point p1, Point p2, Point p3, float r, float g, float b) {
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
    
    glPointSize(8.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
    glPointSize(1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Hệ trục tọa độ
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, 300); glVertex2f(600, 300);
    glVertex2f(300, 0); glVertex2f(300, 600);
    glEnd();
    
    // Tam giác gốc (trắng - nét đứt)
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    drawTriangle(A, B, C, 0.5f, 0.5f, 0.5f);
    glDisable(GL_LINE_STIPPLE);
    
    // Vẽ tâm quay
    if (clickCount >= 1) {
        glPointSize(10.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(rotationCenter.x, rotationCenter.y);
        glEnd();
        
        // Vẽ đường tròn nhỏ quanh tâm quay
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 10) {
            float rad = i * M_PI / 180.0f;
            glVertex2f(rotationCenter.x + 15 * cos(rad), 
                      rotationCenter.y + 15 * sin(rad));
        }
        glEnd();
        
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(rotationCenter.x + 20, rotationCenter.y + 20);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'O');
        glPointSize(1.0f);
    }
    
    // Tam giác sau biến đổi (xanh lá)
    if (hasTransformed) {
        drawTriangle(A_prime, B_prime, C_prime, 0.0f, 1.0f, 0.0f);
    }
    
    // Hướng dẫn
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 580);
    const char* msg = "Click: Chon tam quay. Mui ten: Tang/giam goc. R: Reset";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    char info[100];
    sprintf(info, "Goc quay hien tai: %.1f do", angle);
    glRasterPos2f(10, 560);
    for (const char* c = info; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        rotationCenter.x = (float)x;
        rotationCenter.y = (float)(600 - y);
        clickCount = 1;
        
        std::cout << "Tam quay: (" << rotationCenter.x << ", " << rotationCenter.y << ")" << std::endl;
        
        // Thực hiện quay
        rotateWithCenter(angle, rotationCenter.x, rotationCenter.y);
        glutPostRedisplay();
    }
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        angle += 5.0f;
    } else if (key == GLUT_KEY_DOWN) {
        angle -= 5.0f;
    } else if (key == GLUT_KEY_RIGHT) {
        angle += 15.0f;
    } else if (key == GLUT_KEY_LEFT) {
        angle -= 15.0f;
    }
    
    if (clickCount >= 1) {
        rotateWithCenter(angle, rotationCenter.x, rotationCenter.y);
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        hasTransformed = false;
        clickCount = 0;
        angle = 45.0f;
        std::cout << "Reset!" << std::endl;
        glutPostRedisplay();
    } else if (key == 27) {
        exit(0);
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 600.0, 0.0, 600.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Phep Quay - Rotation 2D");
    
    init();
    
    std::cout << "\n=== PHEP QUAY 2D ===" << std::endl;
    std::cout << "Click chuot: Chon tam quay" << std::endl;
    std::cout << "Phim mui ten: Tang/giam goc quay" << std::endl;
    std::cout << "Phim R: Reset\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
