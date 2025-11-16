#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

// Define GLUT_BUILDING_LIB to fix the entry point issue
#ifndef GLUT_BUILDING_LIB
#define GLUT_BUILDING_LIB
#endif

int W = 1000, H = 700;

// Enum cho loại đường cong
enum CurveType {
    PARABOLA,      // y = ax^2
    HYPERBOLA,     // xy = c
    SINE,          // y = A*sin(Bx)
    COSINE         // y = A*cos(Bx)
};

// Cấu trúc điểm
struct Point {
    float x, y;
    Point(float px = 0, float py = 0) : x(px), y(py) {}
};

// Cấu trúc đường cong
struct Curve {
    CurveType type;
    std::vector<Point> points;
    float r, g, b;  // Màu sắc
    float param1, param2;  // Tham số của đường cong
    
    Curve(CurveType t, float p1, float p2, float red, float green, float blue)
        : type(t), param1(p1), param2(p2), r(red), g(green), b(blue) {}
};

// Biến toàn cục
std::vector<Curve> curves;
CurveType currentCurveType = PARABOLA;
bool showGrid = true;
bool showAxis = true;

// Màu sắc cho từng loại đường cong
struct CurveColor {
    float r, g, b;
    CurveColor(float red, float green, float blue) : r(red), g(green), b(blue) {}
};

CurveColor parabolaColor(1.0f, 0.0f, 0.0f);    // Đỏ
CurveColor hyperbolaColor(0.0f, 1.0f, 0.0f);   // Xanh lá
CurveColor sineColor(0.0f, 0.0f, 1.0f);        // Xanh dương
CurveColor cosineColor(1.0f, 1.0f, 0.0f);      // Vàng

// Hàm vẽ một điểm
void drawPoint(float x, float y) {
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

// Tạo điểm cho đường Parabol: y = ax^2
void generateParabola(Curve& curve, float a) {
    curve.points.clear();
    float xMin = -W/2;
    float xMax = W/2;
    float step = 1.0f;
    
    for (float x = xMin; x <= xMax; x += step) {
        float y = a * x * x;
        if (std::abs(y) < H/2) {  // Giới hạn trong màn hình
            curve.points.push_back(Point(x, y));
        }
    }
}

// Tạo điểm cho đường Hyperbol: xy = c
void generateHyperbola(Curve& curve, float c) {
    curve.points.clear();
    float xMin = -W/2;
    float xMax = W/2;
    float step = 1.0f;
    
    // Nhánh 1: x > 0
    for (float x = 5.0f; x <= xMax; x += step) {
        float y = c / x;
        if (std::abs(y) < H/2) {
            curve.points.push_back(Point(x, y));
        }
    }
    
    // Nhánh 2: x < 0
    for (float x = -5.0f; x >= xMin; x -= step) {
        float y = c / x;
        if (std::abs(y) < H/2) {
            curve.points.push_back(Point(x, y));
        }
    }
}

// Tạo điểm cho đường Sin: y = A*sin(Bx)
void generateSine(Curve& curve, float amplitude, float frequency) {
    curve.points.clear();
    float xMin = -W/2;
    float xMax = W/2;
    float step = 0.5f;
    
    for (float x = xMin; x <= xMax; x += step) {
        float y = amplitude * sin(frequency * x * M_PI / 180.0f);
        curve.points.push_back(Point(x, y));
    }
}

// Tạo điểm cho đường Cos: y = A*cos(Bx)
void generateCosine(Curve& curve, float amplitude, float frequency) {
    curve.points.clear();
    float xMin = -W/2;
    float xMax = W/2;
    float step = 0.5f;
    
    for (float x = xMin; x <= xMax; x += step) {
        float y = amplitude * cos(frequency * x * M_PI / 180.0f);
        curve.points.push_back(Point(x, y));
    }
}

// Vẽ đường cong
void drawCurve(const Curve& curve) {
    glColor3f(curve.r, curve.g, curve.b);
    glPointSize(2.0f);
    
    for (const Point& p : curve.points) {
        // Chuyển đổi từ tọa độ logic sang tọa độ màn hình
        drawPoint(W/2 + p.x, H/2 + p.y);
    }
}

// Vẽ tất cả đường cong
void drawAllCurves() {
    for (const Curve& curve : curves) {
        drawCurve(curve);
    }
}

// Vẽ grid
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
}

// Vẽ trục tọa độ
void drawAxis() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    // Trục X
    glVertex2i(0, H/2);
    glVertex2i(W, H/2);
    
    // Trục Y
    glVertex2i(W/2, 0);
    glVertex2i(W/2, H);
    glEnd();
    
    // Vẽ mũi tên trục X
    glBegin(GL_TRIANGLES);
    glVertex2i(W - 10, H/2);
    glVertex2i(W - 20, H/2 - 5);
    glVertex2i(W - 20, H/2 + 5);
    glEnd();
    
    // Vẽ mũi tên trục Y
    glBegin(GL_TRIANGLES);
    glVertex2i(W/2, 10);
    glVertex2i(W/2 - 5, 20);
    glVertex2i(W/2 + 5, 20);
    glEnd();
    
    // Nhãn trục
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(W - 30, H/2 + 15);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
    
    glRasterPos2f(W/2 + 10, 15);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
}

// Hiển thị thông tin
void drawInfo() {
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Thông tin phía trên
    glRasterPos2f(10, H - 20);
    std::string info = "Current Mode: ";
    switch(currentCurveType) {
        case PARABOLA:  info += "PARABOLA (y = ax^2)"; break;
        case HYPERBOLA: info += "HYPERBOLA (xy = c)"; break;
        case SINE:      info += "SINE (y = A*sin(Bx))"; break;
        case COSINE:    info += "COSINE (y = A*cos(Bx))"; break;
    }
    info += " | Curves: " + std::to_string(curves.size());
    
    for (char c : info) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // Hướng dẫn
    glRasterPos2f(10, H - 40);
    std::string guide = "Press 1:Parabola 2:Hyperbola 3:Sine 4:Cosine | SPACE:Add curve | G:Grid A:Axis C:Clear ESC:Quit";
    for (char c : guide) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    // Legend (chú thích màu)
    int legendY = H - 80;
    glColor3f(parabolaColor.r, parabolaColor.g, parabolaColor.b);
    glRasterPos2f(10, legendY);
    std::string legend1 = "RED: Parabola";
    for (char c : legend1) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    glColor3f(hyperbolaColor.r, hyperbolaColor.g, hyperbolaColor.b);
    glRasterPos2f(120, legendY);
    std::string legend2 = "GREEN: Hyperbola";
    for (char c : legend2) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    glColor3f(sineColor.r, sineColor.g, sineColor.b);
    glRasterPos2f(260, legendY);
    std::string legend3 = "BLUE: Sine";
    for (char c : legend3) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    glColor3f(cosineColor.r, cosineColor.g, cosineColor.b);
    glRasterPos2f(360, legendY);
    std::string legend4 = "YELLOW: Cosine";
    for (char c : legend4) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
}

// Hiển thị
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (showGrid) {
        drawGrid();
    }
    
    if (showAxis) {
        drawAxis();
    }
    
    drawAllCurves();
    drawInfo();
    
    glutSwapBuffers();
}

// Thêm đường cong mới
void addCurve() {
    Curve newCurve(currentCurveType, 0, 0, 1.0f, 1.0f, 1.0f);
    
    static float parabolaA = 0.001f;
    static float hyperbolaC = 5000.0f;
    static float amplitude = 100.0f;
    static float frequency = 1.0f;
    
    switch(currentCurveType) {
        case PARABOLA:
            newCurve.param1 = parabolaA;
            newCurve.r = parabolaColor.r;
            newCurve.g = parabolaColor.g;
            newCurve.b = parabolaColor.b;
            generateParabola(newCurve, parabolaA);
            parabolaA += 0.0005f;
            std::cout << "Added Parabola: y = " << newCurve.param1 << "x^2\n";
            break;
            
        case HYPERBOLA:
            newCurve.param1 = hyperbolaC;
            newCurve.r = hyperbolaColor.r;
            newCurve.g = hyperbolaColor.g;
            newCurve.b = hyperbolaColor.b;
            generateHyperbola(newCurve, hyperbolaC);
            hyperbolaC += 2000.0f;
            std::cout << "Added Hyperbola: xy = " << newCurve.param1 << "\n";
            break;
            
        case SINE:
            newCurve.param1 = amplitude;
            newCurve.param2 = frequency;
            newCurve.r = sineColor.r;
            newCurve.g = sineColor.g;
            newCurve.b = sineColor.b;
            generateSine(newCurve, amplitude, frequency);
            amplitude += 20.0f;
            std::cout << "Added Sine: y = " << newCurve.param1 << "*sin(" << newCurve.param2 << "x)\n";
            break;
            
        case COSINE:
            newCurve.param1 = amplitude;
            newCurve.param2 = frequency;
            newCurve.r = cosineColor.r;
            newCurve.g = cosineColor.g;
            newCurve.b = cosineColor.b;
            generateCosine(newCurve, amplitude, frequency);
            frequency += 0.5f;
            std::cout << "Added Cosine: y = " << newCurve.param1 << "*cos(" << newCurve.param2 << "x)\n";
            break;
    }
    
    curves.push_back(newCurve);
    glutPostRedisplay();
}

// Xử lý phím
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '1':
            currentCurveType = PARABOLA;
            std::cout << "Mode: PARABOLA\n";
            glutPostRedisplay();
            break;
        case '2':
            currentCurveType = HYPERBOLA;
            std::cout << "Mode: HYPERBOLA\n";
            glutPostRedisplay();
            break;
        case '3':
            currentCurveType = SINE;
            std::cout << "Mode: SINE\n";
            glutPostRedisplay();
            break;
        case '4':
            currentCurveType = COSINE;
            std::cout << "Mode: COSINE\n";
            glutPostRedisplay();
            break;
        case ' ':  // Space
            addCurve();
            break;
        case 'g':
        case 'G':
            showGrid = !showGrid;
            std::cout << "Grid " << (showGrid ? "ON" : "OFF") << "\n";
            glutPostRedisplay();
            break;
        case 'a':
        case 'A':
            showAxis = !showAxis;
            std::cout << "Axis " << (showAxis ? "ON" : "OFF") << "\n";
            glutPostRedisplay();
            break;
        case 'c':
        case 'C':
            curves.clear();
            std::cout << "All curves cleared.\n";
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
    
    std::cout << "=== DRAWING CURVES - LAB 1.4 ===\n";
    std::cout << "=================================\n";
    std::cout << "Vẽ các đường cong toán học:\n\n";
    std::cout << "1. PARABOLA (Parabol):\n";
    std::cout << "   Công thức: y = ax^2\n";
    std::cout << "   Màu: ĐỎ (RED)\n\n";
    std::cout << "2. HYPERBOLA (Hyperbol):\n";
    std::cout << "   Công thức: xy = c\n";
    std::cout << "   Màu: XANH LÁ (GREEN)\n\n";
    std::cout << "3. SINE (Sin):\n";
    std::cout << "   Công thức: y = A*sin(Bx)\n";
    std::cout << "   Màu: XANH DƯƠNG (BLUE)\n\n";
    std::cout << "4. COSINE (Cos):\n";
    std::cout << "   Công thức: y = A*cos(Bx)\n";
    std::cout << "   Màu: VÀNG (YELLOW)\n\n";
    std::cout << "Điều khiển:\n";
    std::cout << "- Phím 1: Chọn mode Parabola\n";
    std::cout << "- Phím 2: Chọn mode Hyperbola\n";
    std::cout << "- Phím 3: Chọn mode Sine\n";
    std::cout << "- Phím 4: Chọn mode Cosine\n";
    std::cout << "- SPACE: Thêm đường cong mới\n";
    std::cout << "- Phím G: Bật/tắt lưới\n";
    std::cout << "- Phím A: Bật/tắt trục tọa độ\n";
    std::cout << "- Phím C: Xóa tất cả đường cong\n";
    std::cout << "- Phím ESC: Thoát\n";
    std::cout << "=================================\n\n";
    
    // Thêm một số đường cong mẫu
    std::cout << "Adding sample curves...\n";
    currentCurveType = PARABOLA;
    addCurve();
    currentCurveType = SINE;
    addCurve();
    currentCurveType = COSINE;
    addCurve();
    currentCurveType = HYPERBOLA;
    addCurve();
    
    currentCurveType = PARABOLA;  // Reset về mode đầu tiên
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("LAB 1.4 - Drawing Curves (Parabola, Hyperbola, Sin, Cos)");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
