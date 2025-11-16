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
Point A = {150, 200};
Point B = {250, 150};
Point C = {200, 300};

// Tam giác sau biến đổi
Point A_prime, B_prime, C_prime;
bool hasTransformed = false;

// Đường thẳng y = ax + b
float a = 1.0f;  // Hệ số góc
float b = 0.0f;  // Hệ số tự do
int clickCount = 0;
Point linePoint1, linePoint2;

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

// Phép đối xứng qua đường thẳng y = ax + b
void reflectOverLine(float a, float b) {
    // Chuyển về dạng ax - y + b = 0
    // A = a, B = -1, C = b
    
    float A = a;
    float B = -1.0f;
    float C = b;
    
    float denominator = A*A + B*B;
    
    // Ma trận đối xứng qua đường thẳng Ax + By + C = 0
    float matrix[3][3] = {
        {(B*B - A*A) / denominator, -2*A*B / denominator, -2*A*C / denominator},
        {-2*A*B / denominator, (A*A - B*B) / denominator, -2*B*C / denominator},
        {0, 0, 1}
    };
    
    A_prime = multiplyMatrix(matrix, A);
    B_prime = multiplyMatrix(matrix, B);
    C_prime = multiplyMatrix(matrix, C);
    
    hasTransformed = true;
    
    std::cout << "Doi xung qua duong thang y = " << a << "x + " << b << std::endl;
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

void drawLine(float a, float b) {
    // Vẽ đường thẳng y = ax + b
    float x1 = 0;
    float y1 = a * x1 + b;
    float x2 = 600;
    float y2 = a * x2 + b;
    
    glColor3f(1.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Hệ trục tọa độ
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, 300); glVertex2f(600, 300);
    glVertex2f(300, 0); glVertex2f(300, 600);
    glEnd();
    
    // Vẽ đường thẳng đối xứng (màu hồng)
    if (clickCount >= 2) {
        drawLine(a, b);
        
        // Hiển thị phương trình
        char eq[100];
        sprintf(eq, "y = %.2fx + %.2f", a, b);
        glColor3f(1.0f, 0.0f, 1.0f);
        glRasterPos2f(10, 540);
        for (const char* c = eq; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }
    
    // Tam giác gốc (trắng - nét đứt)
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    drawTriangle(A, B, C, 0.5f, 0.5f, 0.5f);
    glDisable(GL_LINE_STIPPLE);
    
    // Vẽ các điểm đã click
    if (clickCount >= 1) {
        glPointSize(8.0f);
        glColor3f(1.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex2f(linePoint1.x, linePoint1.y);
        if (clickCount >= 2) {
            glVertex2f(linePoint2.x, linePoint2.y);
        }
        glEnd();
        glPointSize(1.0f);
    }
    
    // Tam giác sau biến đổi (xanh lá)
    if (hasTransformed) {
        drawTriangle(A_prime, B_prime, C_prime, 0.0f, 1.0f, 0.0f);
    }
    
    // Hướng dẫn
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 580);
    const char* msg = "Click 2 diem de tao duong thang doi xung. R: Reset";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2f(10, 560);
    const char* info = "Duong thang doi xung: mau hong";
    for (const char* c = info; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mouseX = (float)x;
        float mouseY = (float)(600 - y);
        
        if (clickCount == 0) {
            linePoint1.x = mouseX;
            linePoint1.y = mouseY;
            clickCount = 1;
            std::cout << "Diem 1: (" << mouseX << ", " << mouseY << ")" << std::endl;
        } else if (clickCount == 1) {
            linePoint2.x = mouseX;
            linePoint2.y = mouseY;
            clickCount = 2;
            std::cout << "Diem 2: (" << mouseX << ", " << mouseY << ")" << std::endl;
            
            // Tính a và b từ 2 điểm
            float dx = linePoint2.x - linePoint1.x;
            float dy = linePoint2.y - linePoint1.y;
            
            if (fabs(dx) > 0.001f) {
                a = dy / dx;
                b = linePoint1.y - a * linePoint1.x;
            } else {
                // Đường thẳng dọc, không thể biểu diễn dạng y = ax + b
                std::cout << "Duong thang dung! Vui long chon 2 diem khac." << std::endl;
                clickCount = 0;
                return;
            }
            
            std::cout << "Duong thang: y = " << a << "x + " << b << std::endl;
            
            // Thực hiện đối xứng
            reflectOverLine(a, b);
        }
        
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        hasTransformed = false;
        clickCount = 0;
        a = 1.0f;
        b = 0.0f;
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
    glutCreateWindow("Phep Doi Xung - Reflection 2D");
    
    init();
    
    std::cout << "\n=== PHEP DOI XUNG QUA DUONG THANG ===" << std::endl;
    std::cout << "Click chuot trai 2 lan de tao duong thang doi xung" << std::endl;
    std::cout << "Duong thang co dang: y = ax + b" << std::endl;
    std::cout << "Phim R: Reset\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
