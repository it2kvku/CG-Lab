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

// Cấu trúc lưu đoạn thẳng
struct Line {
    Point p1, p2;
    float r, g, b;  // Màu sắc
    Line(Point a, Point b, float red, float green, float blue) 
        : p1(a), p2(b), r(red), g(green), b(blue) {}
};

// Biến toàn cục
std::vector<Line> lines;
Point firstClick(-1, -1);
bool hasFirstClick = false;
int currentMode = 1;  // 1-4: 4 trường hợp hệ số góc

// Hàm vẽ một pixel
void putPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Thuật toán Bresenham tổng quát cho tất cả 8 octant (4 trường hợp hệ số góc)
void bresenhamLine(int x0, int y0, int x1, int y1) {
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    
    // Xử lý trường hợp dốc (steep)
    if (steep) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
    }
    
    // Đảm bảo vẽ từ trái sang phải
    if (x0 > x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    }

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int err = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        if (steep) 
            putPixel(y, x);  // Hoán đổi lại tọa độ
        else       
            putPixel(x, y);

        err -= dy;
        if (err < 0) { 
            y += ystep; 
            err += dx; 
        }
    }
}

// Xác định trường hợp của đoạn thẳng
int getLineCase(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    if (dx == 0) return 4;  // Đường thẳng đứng
    
    float slope = (float)dy / dx;
    
    if (slope >= 0) {
        if (slope < 1) return 1;   // 0 < m < 1 (octant 1)
        else return 2;             // m >= 1 (octant 2)
    } else {
        if (slope > -1) return 3;  // -1 < m < 0 (octant 8)
        else return 4;             // m <= -1 (octant 7)
    }
}

// Vẽ tất cả các đường
void drawAllLines() {
    glPointSize(2.0f);
    for (const Line& line : lines) {
        glColor3f(line.r, line.g, line.b);
        bresenhamLine(line.p1.x, line.p1.y, line.p2.x, line.p2.y);
    }
}

// Vẽ điểm click tạm thời
void drawTemporaryPoint() {
    if (hasFirstClick) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Màu vàng
        glPointSize(8.0f);
        putPixel(firstClick.x, firstClick.y);
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
    
    std::string info = "Mode: ";
    switch(currentMode) {
        case 1: info += "Case 1 (0 < m < 1)"; break;
        case 2: info += "Case 2 (m >= 1)"; break;
        case 3: info += "Case 3 (-1 < m < 0)"; break;
        case 4: info += "Case 4 (m <= -1 or vertical)"; break;
    }
    info += " | Lines: " + std::to_string(lines.size());
    info += " | Press 1-4 to filter, G: toggle grid, C: clear, ESC: quit";
    
    for (char c : info) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    if (hasFirstClick) {
        glRasterPos2f(10, H - 40);
        std::string msg = "First point selected. Click again to draw line.";
        for (char c : msg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
}

// Hiển thị
bool showGrid = true;
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (showGrid) {
        drawGrid();
    }
    
    drawAllLines();
    drawTemporaryPoint();
    drawInfo();
    
    glutSwapBuffers();
}

// Xử lý click chuột
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Chuyển đổi tọa độ (OpenGL có gốc ở dưới trái)
        int glX = x;
        int glY = H - y;
        
        if (!hasFirstClick) {
            // Lưu điểm đầu tiên
            firstClick = Point(glX, glY);
            hasFirstClick = true;
            std::cout << "First point: (" << glX << ", " << glY << ")\n";
        } else {
            // Vẽ đường từ điểm đầu đến điểm thứ hai
            Point secondClick(glX, glY);
            
            // Xác định trường hợp
            int lineCase = getLineCase(firstClick.x, firstClick.y, glX, glY);
            
            // Chọn màu theo trường hợp
            float r, g, b;
            switch(lineCase) {
                case 1: r = 1.0f; g = 0.0f; b = 0.0f; break;  // Đỏ
                case 2: r = 0.0f; g = 1.0f; b = 0.0f; break;  // Xanh lá
                case 3: r = 0.0f; g = 0.0f; b = 1.0f; break;  // Xanh dương
                case 4: r = 1.0f; g = 0.0f; b = 1.0f; break;  // Tím
            }
            
            lines.push_back(Line(firstClick, secondClick, r, g, b));
            
            std::cout << "Second point: (" << glX << ", " << glY << ")\n";
            std::cout << "Line drawn - Case " << lineCase << "\n";
            
            // Reset
            hasFirstClick = false;
            firstClick = Point(-1, -1);
        }
        
        glutPostRedisplay();
    }
}

// Xử lý phím
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '1':
        case '2':
        case '3':
        case '4':
            currentMode = key - '0';
            std::cout << "Mode changed to: Case " << currentMode << "\n";
            glutPostRedisplay();
            break;
        case 'g':
        case 'G':
            showGrid = !showGrid;
            std::cout << "Grid " << (showGrid ? "ON" : "OFF") << "\n";
            glutPostRedisplay();
            break;
        case 'c':
        case 'C':
            lines.clear();
            hasFirstClick = false;
            firstClick = Point(-1, -1);
            std::cout << "All lines cleared.\n";
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
    
    std::cout << "=== BRESENHAM LINE ALGORITHM - 4 CASES ===\n";
    std::cout << "==========================================\n";
    std::cout << "Thuật toán vẽ đoạn thẳng Bresenham\n";
    std::cout << "Xét 4 trường hợp hệ số góc:\n\n";
    std::cout << "Case 1 (RED):    0 < m < 1   (Octant 1)\n";
    std::cout << "Case 2 (GREEN):  m >= 1      (Octant 2)\n";
    std::cout << "Case 3 (BLUE):   -1 < m < 0  (Octant 8)\n";
    std::cout << "Case 4 (MAGENTA): m <= -1 or vertical (Octant 7)\n\n";
    std::cout << "Hướng dẫn:\n";
    std::cout << "- Click chuột 2 lần để vẽ đoạn thẳng\n";
    std::cout << "- Màu sắc tự động theo trường hợp hệ số góc\n";
    std::cout << "- Phím 1-4: Chọn mode (hiện tại chỉ hiển thị)\n";
    std::cout << "- Phím G: Bật/tắt lưới\n";
    std::cout << "- Phím C: Xóa tất cả đường\n";
    std::cout << "- Phím ESC: Thoát\n";
    std::cout << "==========================================\n\n";
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("LAB 1.1 - Bresenham Line (4 Cases) - Click to Draw");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
