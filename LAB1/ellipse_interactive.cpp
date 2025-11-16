#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

// Define GLUT_BUILDING_LIB to fix the entry point issue
#ifndef GLUT_BUILDING_LIB
#define GLUT_BUILDING_LIB
#endif

int W = 800, H = 600;

// Cấu trúc lưu điểm
struct Point {
    int x, y;
    Point(int px = 0, int py = 0) : x(px), y(py) {}
};

// Cấu trúc lưu ellipse
struct EllipseShape {
    Point center;
    int radiusX, radiusY;
    float r, g, b;  // Màu sắc
    
    EllipseShape(Point c, int rx, int ry, float red, float green, float blue)
        : center(c), radiusX(rx), radiusY(ry), r(red), g(green), b(blue) {}
};

// Biến toàn cục
std::vector<EllipseShape> ellipses;
Point centerClick(-1, -1);
bool hasCenterClick = false;
bool showGrid = true;

// Hàm vẽ một điểm
void drawPoint(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Thuật toán Bresenham vẽ ellipse
void bresenhamEllipse(int xc, int yc, int a, int b) {
    // a là bán kính theo trục x
    // b là bán kính theo trục y
    // (xc, yc) là tọa độ tâm

    int x = 0;
    int y = b;
    
    // Tham số quyết định ban đầu cho khu vực 1
    long long d1 = (b * b) - (a * a * b) + (a * a) / 4;
    int dx = 2 * b * b * x;
    int dy = 2 * a * a * y;

    // Khu vực 1: dy/dx > 1
    while (dx < dy) {
        // Vẽ 4 điểm đối xứng qua 4 góc phần tư
        drawPoint(xc + x, yc + y);
        drawPoint(xc - x, yc + y);
        drawPoint(xc + x, yc - y);
        drawPoint(xc - x, yc - y);

        // Kiểm tra và cập nhật tham số quyết định
        if (d1 < 0) {
            x++;
            dx += 2 * b * b;
            d1 += dx + b * b;
        } else {
            x++;
            y--;
            dx += 2 * b * b;
            dy -= 2 * a * a;
            d1 += dx - dy + b * b;
        }
    }

    // Tham số quyết định ban đầu cho khu vực 2
    long long d2 = (b * b * (x + 0.5) * (x + 0.5)) + 
                   (a * a * (y - 1) * (y - 1)) - 
                   (a * a * b * b);

    // Khu vực 2: dy/dx < 1
    while (y >= 0) {
        // Vẽ 4 điểm đối xứng qua 4 góc phần tư
        drawPoint(xc + x, yc + y);
        drawPoint(xc - x, yc + y);
        drawPoint(xc + x, yc - y);
        drawPoint(xc - x, yc - y);

        // Kiểm tra và cập nhật tham số quyết định
        if (d2 > 0) {
            y--;
            dy -= 2 * a * a;
            d2 += a * a - dy;
        } else {
            y--;
            x++;
            dx += 2 * b * b;
            dy -= 2 * a * a;
            d2 += dx - dy + a * a;
        }
    }
}

// Vẽ tất cả ellipse
void drawAllEllipses() {
    glPointSize(2.0f);
    for (const EllipseShape& e : ellipses) {
        glColor3f(e.r, e.g, e.b);
        bresenhamEllipse(e.center.x, e.center.y, e.radiusX, e.radiusY);
    }
}

// Vẽ điểm tâm tạm thời
void drawTemporaryCenter() {
    if (hasCenterClick) {
        // Vẽ điểm tâm
        glColor3f(1.0f, 1.0f, 0.0f);  // Màu vàng
        glPointSize(8.0f);
        drawPoint(centerClick.x, centerClick.y);
        
        // Vẽ chữ thập tại tâm
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2i(centerClick.x - 10, centerClick.y);
        glVertex2i(centerClick.x + 10, centerClick.y);
        glVertex2i(centerClick.x, centerClick.y - 10);
        glVertex2i(centerClick.x, centerClick.y + 10);
        glEnd();
    }
}

// Vẽ ellipse preview khi di chuyển chuột
Point currentMouse(-1, -1);
void drawPreviewEllipse() {
    if (hasCenterClick && currentMouse.x >= 0) {
        int rx = std::abs(currentMouse.x - centerClick.x);
        int ry = std::abs(currentMouse.y - centerClick.y);
        
        if (rx > 5 && ry > 5) {  // Chỉ vẽ khi đủ lớn
            glColor3f(0.5f, 0.5f, 0.5f);  // Màu xám
            glPointSize(1.0f);
            bresenhamEllipse(centerClick.x, centerClick.y, rx, ry);
        }
    }
}

// Vẽ grid hướng dẫn
void drawGrid() {
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    
    // Vẽ lưới dọc
    for (int x = 0; x < W; x += 50) {
        glVertex2i(x, 0);
        glVertex2i(x, H);
    }
    
    // Vẽ lưới ngang
    for (int y = 0; y < H; y += 50) {
        glVertex2i(0, y);
        glVertex2i(W, y);
    }
    glEnd();
    
    // Vẽ trục tọa độ
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // Trục X
    glVertex2i(0, H/2);
    glVertex2i(W, H/2);
    // Trục Y
    glVertex2i(W/2, 0);
    glVertex2i(W/2, H);
    glEnd();
}

// Hiển thị thông tin
void drawInfo() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, H - 20);
    
    std::string info = "Ellipses: " + std::to_string(ellipses.size());
    info += " | Press G: toggle grid, C: clear, ESC: quit";
    
    for (char c : info) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    if (hasCenterClick) {
        glRasterPos2f(10, H - 40);
        std::string msg = "Center selected at (" + std::to_string(centerClick.x) + 
                         ", " + std::to_string(centerClick.y) + "). ";
        msg += "Click again to set radii and draw ellipse.";
        for (char c : msg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    } else {
        glRasterPos2f(10, H - 40);
        std::string msg = "Click to set center of ellipse.";
        for (char c : msg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
}

// Hiển thị
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (showGrid) {
        drawGrid();
    }
    
    drawAllEllipses();
    drawPreviewEllipse();
    drawTemporaryCenter();
    drawInfo();
    
    glutSwapBuffers();
}

// Xử lý click chuột
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Chuyển đổi tọa độ (OpenGL có gốc ở dưới trái)
        int glX = x;
        int glY = H - y;
        
        if (!hasCenterClick) {
            // Lưu điểm tâm
            centerClick = Point(glX, glY);
            hasCenterClick = true;
            std::cout << "Center point: (" << glX << ", " << glY << ")\n";
        } else {
            // Tính bán kính và vẽ ellipse
            int radiusX = std::abs(glX - centerClick.x);
            int radiusY = std::abs(glY - centerClick.y);
            
            if (radiusX > 5 && radiusY > 5) {  // Kiểm tra kích thước tối thiểu
                // Chọn màu ngẫu nhiên
                float r = (rand() % 100) / 100.0f + 0.3f;
                float g = (rand() % 100) / 100.0f + 0.3f;
                float b = (rand() % 100) / 100.0f + 0.3f;
                
                ellipses.push_back(EllipseShape(centerClick, radiusX, radiusY, r, g, b));
                
                std::cout << "Ellipse drawn - Center: (" << centerClick.x << ", " 
                         << centerClick.y << "), RadiusX: " << radiusX 
                         << ", RadiusY: " << radiusY << "\n";
            } else {
                std::cout << "Ellipse too small. Please click farther from center.\n";
            }
            
            // Reset
            hasCenterClick = false;
            centerClick = Point(-1, -1);
        }
        
        glutPostRedisplay();
    }
}

// Xử lý di chuyển chuột (passive motion)
void passiveMotion(int x, int y) {
    currentMouse.x = x;
    currentMouse.y = H - y;
    
    if (hasCenterClick) {
        glutPostRedisplay();
    }
}

// Xử lý phím
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'g':
        case 'G':
            showGrid = !showGrid;
            std::cout << "Grid " << (showGrid ? "ON" : "OFF") << "\n";
            glutPostRedisplay();
            break;
        case 'c':
        case 'C':
            ellipses.clear();
            hasCenterClick = false;
            centerClick = Point(-1, -1);
            std::cout << "All ellipses cleared.\n";
            glutPostRedisplay();
            break;
        case 27:  // ESC
            exit(0);
            break;
    }
}

void reshape(int w, int h) {
    W = w; 
    H = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Nền đen
    
    std::cout << "=== BRESENHAM ELLIPSE ALGORITHM ===\n";
    std::cout << "====================================\n";
    std::cout << "Thuật toán vẽ ellipse Bresenham\n";
    std::cout << "Chia làm 2 khu vực:\n";
    std::cout << "- Khu vực 1: dy/dx > 1\n";
    std::cout << "- Khu vực 2: dy/dx < 1\n\n";
    std::cout << "Hướng dẫn:\n";
    std::cout << "- Click lần 1: Chọn tâm ellipse\n";
    std::cout << "- Click lần 2: Xác định bán kính (khoảng cách từ tâm)\n";
    std::cout << "- Di chuyển chuột: Xem preview ellipse\n";
    std::cout << "- Phím G: Bật/tắt lưới\n";
    std::cout << "- Phím C: Xóa tất cả ellipse\n";
    std::cout << "- Phím ESC: Thoát\n";
    std::cout << "====================================\n\n";
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("LAB 1.3 - Bresenham Ellipse - Click to Draw");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
