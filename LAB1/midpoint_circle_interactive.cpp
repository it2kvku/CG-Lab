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

// Cấu trúc lưu đường tròn
struct Circle {
    Point center;
    int radius;
    float r, g, b;  // Màu sắc
    
    Circle(Point c, int rad, float red, float green, float blue)
        : center(c), radius(rad), r(red), g(green), b(blue) {}
};

// Biến toàn cục
std::vector<Circle> circles;
Point centerClick(-1, -1);
bool hasCenterClick = false;
bool showGrid = true;

// Hàm vẽ một điểm
void drawPoint(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Thuật toán Midpoint Circle (Bresenham Circle)
void midpointCircle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 1 - r;  // Tham số quyết định ban đầu
    
    // Vẽ 8 điểm đối xứng ban đầu
    auto plot8Points = [xc, yc](int x, int y) {
        drawPoint(xc + x, yc + y);
        drawPoint(xc - x, yc + y);
        drawPoint(xc + x, yc - y);
        drawPoint(xc - x, yc - y);
        drawPoint(xc + y, yc + x);
        drawPoint(xc - y, yc + x);
        drawPoint(xc + y, yc - x);
        drawPoint(xc - y, yc - x);
    };
    
    plot8Points(x, y);
    
    // Vẽ đường tròn theo thuật toán Midpoint
    while (x < y) {
        x++;
        if (d < 0) {
            // Chọn điểm E (East)
            d += 2 * x + 1;
        } else {
            // Chọn điểm SE (South-East)
            y--;
            d += 2 * (x - y) + 1;
        }
        plot8Points(x, y);
    }
}

// Vẽ tất cả đường tròn
void drawAllCircles() {
    glPointSize(2.0f);
    for (const Circle& c : circles) {
        glColor3f(c.r, c.g, c.b);
        midpointCircle(c.center.x, c.center.y, c.radius);
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

// Vẽ circle preview khi di chuyển chuột
Point currentMouse(-1, -1);
void drawPreviewCircle() {
    if (hasCenterClick && currentMouse.x >= 0) {
        int dx = currentMouse.x - centerClick.x;
        int dy = currentMouse.y - centerClick.y;
        int radius = (int)sqrt(dx * dx + dy * dy);
        
        if (radius > 5) {  // Chỉ vẽ khi đủ lớn
            glColor3f(0.5f, 0.5f, 0.5f);  // Màu xám
            glPointSize(1.0f);
            midpointCircle(centerClick.x, centerClick.y, radius);
            
            // Vẽ bán kính
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            glVertex2i(centerClick.x, centerClick.y);
            glVertex2i(currentMouse.x, currentMouse.y);
            glEnd();
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
    
    std::string info = "Circles: " + std::to_string(circles.size());
    info += " | Press G: toggle grid, C: clear, ESC: quit";
    
    for (char c : info) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    if (hasCenterClick) {
        glRasterPos2f(10, H - 40);
        std::string msg = "Center selected at (" + std::to_string(centerClick.x) + 
                         ", " + std::to_string(centerClick.y) + "). ";
        msg += "Click again to set radius and draw circle.";
        for (char c : msg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    } else {
        glRasterPos2f(10, H - 40);
        std::string msg = "Click to set center of circle.";
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
    
    drawAllCircles();
    drawPreviewCircle();
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
            // Tính bán kính và vẽ đường tròn
            int dx = glX - centerClick.x;
            int dy = glY - centerClick.y;
            int radius = (int)sqrt(dx * dx + dy * dy);
            
            if (radius > 5) {  // Kiểm tra kích thước tối thiểu
                // Chọn màu ngẫu nhiên
                float r = (rand() % 100) / 100.0f + 0.3f;
                float g = (rand() % 100) / 100.0f + 0.3f;
                float b = (rand() % 100) / 100.0f + 0.3f;
                
                circles.push_back(Circle(centerClick, radius, r, g, b));
                
                std::cout << "Circle drawn - Center: (" << centerClick.x << ", " 
                         << centerClick.y << "), Radius: " << radius << "\n";
            } else {
                std::cout << "Circle too small. Please click farther from center.\n";
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
            circles.clear();
            hasCenterClick = false;
            centerClick = Point(-1, -1);
            std::cout << "All circles cleared.\n";
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
    
    std::cout << "=== MIDPOINT CIRCLE ALGORITHM ===\n";
    std::cout << "==================================\n";
    std::cout << "Thuật toán vẽ đường tròn Midpoint (Bresenham Circle)\n";
    std::cout << "Sử dụng tính đối xứng 8 phần của đường tròn\n\n";
    std::cout << "Công thức tham số quyết định:\n";
    std::cout << "- d = 1 - r (ban đầu)\n";
    std::cout << "- Nếu d < 0: chọn điểm E, d += 2x + 1\n";
    std::cout << "- Nếu d >= 0: chọn điểm SE, d += 2(x-y) + 1\n\n";
    std::cout << "Hướng dẫn:\n";
    std::cout << "- Click lần 1: Chọn tâm đường tròn\n";
    std::cout << "- Click lần 2: Xác định bán kính (khoảng cách từ tâm)\n";
    std::cout << "- Di chuyển chuột: Xem preview đường tròn\n";
    std::cout << "- Phím G: Bật/tắt lưới\n";
    std::cout << "- Phím C: Xóa tất cả đường tròn\n";
    std::cout << "- Phím ESC: Thoát\n";
    std::cout << "==================================\n\n";
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("LAB 1.2 - Midpoint Circle - Click to Draw");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
