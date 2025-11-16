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
Point A = {200, 150};
Point B = {350, 150};
Point C = {275, 300};

// Tam giác sau biến đổi
Point A_prime, B_prime, C_prime;
bool hasTransformed = false;

// Tâm tỉ lệ và hệ số
Point scaleCenter = {300, 300};
float scaleX = 1.5f;
float scaleY = 1.5f;
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

// Phép tỉ lệ với tâm (x0, y0)
void scaleWithCenter(float sx, float sy, float x0, float y0) {
    // Ma trận tịnh tiến về gốc tọa độ
    float T1[3][3] = {
        {1, 0, -x0},
        {0, 1, -y0},
        {0, 0, 1}
    };
    
    // Ma trận tỉ lệ
    float S[3][3] = {
        {sx, 0, 0},
        {0, sy, 0},
        {0, 0, 1}
    };
    
    // Ma trận tịnh tiến về vị trí ban đầu
    float T2[3][3] = {
        {1, 0, x0},
        {0, 1, y0},
        {0, 0, 1}
    };
    
    // Kết hợp: T2 * S * T1
    float temp[3][3];
    float result[3][3];
    
    // temp = S * T1
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                temp[i][j] += S[i][k] * T1[k][j];
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
    
    std::cout << "Ti le: sx=" << sx << ", sy=" << sy << ", tam=(" << x0 << ", " << y0 << ")" << std::endl;
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
    
    // Vẽ tâm tỉ lệ
    if (clickCount >= 1) {
        glPointSize(10.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(scaleCenter.x, scaleCenter.y);
        glEnd();
        
        // Vẽ tên tâm
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(scaleCenter.x + 10, scaleCenter.y + 10);
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
    const char* msg = "Click: Chon tam ti le. +/-: Tang/giam ti le. R: Reset";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    char info[100];
    sprintf(info, "Ti le hien tai: sx=%.2f, sy=%.2f", scaleX, scaleY);
    glRasterPos2f(10, 560);
    for (const char* c = info; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        scaleCenter.x = (float)x;
        scaleCenter.y = (float)(600 - y);
        clickCount = 1;
        
        std::cout << "Tam ti le: (" << scaleCenter.x << ", " << scaleCenter.y << ")" << std::endl;
        
        // Thực hiện tỉ lệ
        scaleWithCenter(scaleX, scaleY, scaleCenter.x, scaleCenter.y);
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '+' || key == '=') {
        scaleX += 0.1f;
        scaleY += 0.1f;
        if (clickCount >= 1) {
            scaleWithCenter(scaleX, scaleY, scaleCenter.x, scaleCenter.y);
        }
        glutPostRedisplay();
    } else if (key == '-' || key == '_') {
        scaleX -= 0.1f;
        scaleY -= 0.1f;
        if (scaleX < 0.1f) scaleX = 0.1f;
        if (scaleY < 0.1f) scaleY = 0.1f;
        if (clickCount >= 1) {
            scaleWithCenter(scaleX, scaleY, scaleCenter.x, scaleCenter.y);
        }
        glutPostRedisplay();
    } else if (key == 'r' || key == 'R') {
        hasTransformed = false;
        clickCount = 0;
        scaleX = scaleY = 1.5f;
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
    glutCreateWindow("Phep Ti Le - Scaling 2D");
    
    init();
    
    std::cout << "\n=== PHEP TI LE 2D ===" << std::endl;
    std::cout << "Click chuot: Chon tam ti le" << std::endl;
    std::cout << "Phim +: Tang ti le" << std::endl;
    std::cout << "Phim -: Giam ti le" << std::endl;
    std::cout << "Phim R: Reset\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
