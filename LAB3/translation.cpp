#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Cấu trúc điểm
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

// Vector tịnh tiến
Point translateVector = {0, 0};
int translateClickCount = 0;
Point translateStart;

// Hàm nhân ma trận 3x3 với điểm
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

// Phép tịnh tiến
void translate(float dx, float dy) {
    float matrix[3][3] = {
        {1, 0, dx},
        {0, 1, dy},
        {0, 0, 1}
    };
    
    A_prime = multiplyMatrix(matrix, A);
    B_prime = multiplyMatrix(matrix, B);
    C_prime = multiplyMatrix(matrix, C);
    
    hasTransformed = true;
    
    std::cout << "Tinh tien: dx=" << dx << ", dy=" << dy << std::endl;
    std::cout << "A' = (" << A_prime.x << ", " << A_prime.y << ")" << std::endl;
    std::cout << "B' = (" << B_prime.x << ", " << B_prime.y << ")" << std::endl;
    std::cout << "C' = (" << C_prime.x << ", " << C_prime.y << ")" << std::endl;
}

// Hàm vẽ tam giác
void drawTriangle(Point p1, Point p2, Point p3, float r, float g, float b, bool filled = false) {
    glColor3f(r, g, b);
    
    if (filled) {
        glBegin(GL_TRIANGLES);
    } else {
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
    }
    
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
    
    // Vẽ các đỉnh
    glPointSize(8.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();
    glPointSize(1.0f);
}

// Hàm vẽ vector
void drawVector(Point start, Point end, float r, float g, float b) {
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    
    // Vẽ đường
    glBegin(GL_LINES);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
    
    // Vẽ mũi tên
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float len = sqrt(dx*dx + dy*dy);
    
    if (len > 0) {
        dx /= len;
        dy /= len;
        
        float arrowSize = 10.0f;
        float angle = 30.0f * M_PI / 180.0f;
        
        float x1 = end.x - arrowSize * (dx * cos(angle) + dy * sin(angle));
        float y1 = end.y - arrowSize * (dy * cos(angle) - dx * sin(angle));
        
        float x2 = end.x - arrowSize * (dx * cos(angle) - dy * sin(angle));
        float y2 = end.y - arrowSize * (dy * cos(angle) + dx * sin(angle));
        
        glBegin(GL_TRIANGLES);
        glVertex2f(end.x, end.y);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }
    
    glLineWidth(1.0f);
}

// Hàm vẽ
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vẽ hệ trục tọa độ
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, 300); glVertex2f(600, 300); // Trục X
    glVertex2f(300, 0); glVertex2f(300, 600); // Trục Y
    glEnd();
    
    // Vẽ tam giác gốc (màu trắng - nét đứt)
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    drawTriangle(A, B, C, 0.5f, 0.5f, 0.5f, false);
    glDisable(GL_LINE_STIPPLE);
    
    // Vẽ vector tịnh tiến nếu đang chọn
    if (translateClickCount == 1) {
        glPointSize(6.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(translateStart.x, translateStart.y);
        glEnd();
        glPointSize(1.0f);
    }
    
    if (translateClickCount == 2) {
        Point end;
        end.x = translateStart.x + translateVector.x;
        end.y = translateStart.y + translateVector.y;
        drawVector(translateStart, end, 0.0f, 1.0f, 1.0f);
    }
    
    // Vẽ tam giác sau biến đổi (màu xanh lá - nét liền)
    if (hasTransformed) {
        drawTriangle(A_prime, B_prime, C_prime, 0.0f, 1.0f, 0.0f, false);
    }
    
    // Hiển thị hướng dẫn
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 580);
    const char* msg = "Click 2 diem de tao vector tinh tien. R: Reset. ESC: Thoat";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glRasterPos2f(10, 560);
    const char* info = "Tam giac goc (trang net dut) -> Tam giac bien doi (xanh la net lien)";
    for (const char* c = info; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

// Hàm xử lý chuột
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mouseX = (float)x;
        float mouseY = (float)(600 - y);
        
        if (translateClickCount == 0) {
            translateStart.x = mouseX;
            translateStart.y = mouseY;
            translateClickCount = 1;
            std::cout << "Diem bat dau vector: (" << mouseX << ", " << mouseY << ")" << std::endl;
        } else if (translateClickCount == 1) {
            translateVector.x = mouseX - translateStart.x;
            translateVector.y = mouseY - translateStart.y;
            translateClickCount = 2;
            std::cout << "Vector tinh tien: (" << translateVector.x << ", " << translateVector.y << ")" << std::endl;
            
            // Thực hiện tịnh tiến
            translate(translateVector.x, translateVector.y);
        }
        
        glutPostRedisplay();
    }
}

// Hàm xử lý bàn phím
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        hasTransformed = false;
        translateClickCount = 0;
        translateVector.x = translateVector.y = 0;
        std::cout << "Reset!" << std::endl;
        glutPostRedisplay();
    } else if (key == 27) {
        exit(0);
    }
}

// Hàm thiết lập viewport
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 600.0, 0.0, 600.0);
    glMatrixMode(GL_MODELVIEW);
}

// Hàm khởi tạo
void init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Phep Tinh Tien - Translation 2D");
    
    init();
    
    std::cout << "\n=== PHEP TINH TIEN 2D ===" << std::endl;
    std::cout << "Tam giac ABC goc:" << std::endl;
    std::cout << "A = (" << A.x << ", " << A.y << ")" << std::endl;
    std::cout << "B = (" << B.x << ", " << B.y << ")" << std::endl;
    std::cout << "C = (" << C.x << ", " << C.y << ")\n" << std::endl;
    std::cout << "Click chuot trai 2 lan de tao vector tinh tien" << std::endl;
    std::cout << "Phim R: Reset" << std::endl;
    std::cout << "Phim ESC: Thoat\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
