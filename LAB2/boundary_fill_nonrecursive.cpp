#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <cmath>

// Cấu trúc điểm
struct Point {
    float x, y;
};

// Cấu trúc pixel
struct Pixel {
    int x, y;
};

// Lưu đa giác
std::vector<Point> polygon;
bool polygonComplete = false;

// Màu
float fillColor[3] = {1.0f, 0.5f, 0.0f};  // Màu tô (cam)
float boundaryColor[3] = {1.0f, 1.0f, 1.0f};  // Màu viền (trắng)
float backgroundColor[3] = {0.0f, 0.0f, 0.2f};  // Màu nền

// Hàm đọc màu pixel
void getPixelColor(int x, int y, float color[3]) {
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, color);
}

// Hàm so sánh màu
bool isSameColor(float c1[3], float c2[3]) {
    return (fabs(c1[0] - c2[0]) < 0.01f && 
            fabs(c1[1] - c2[1]) < 0.01f && 
            fabs(c1[2] - c2[2]) < 0.01f);
}

// Thuật toán tô màu theo đường biên - Không đệ quy (dùng stack)
void boundaryFillNonRecursive(int x, int y, float fill[3], float boundary[3]) {
    std::stack<Pixel> pixelStack;
    Pixel p;
    p.x = x;
    p.y = y;
    pixelStack.push(p);
    
    int count = 0;
    
    while (!pixelStack.empty()) {
        Pixel current = pixelStack.top();
        pixelStack.pop();
        
        float currentColor[3];
        getPixelColor(current.x, current.y, currentColor);
        
        // Nếu không phải màu viền và không phải màu tô
        if (!isSameColor(currentColor, boundary) && !isSameColor(currentColor, fill)) {
            // Tô màu pixel hiện tại
            glColor3fv(fill);
            glBegin(GL_POINTS);
            glVertex2i(current.x, current.y);
            glEnd();
            
            count++;
            if (count % 1000 == 0) {
                glFlush();  // Cập nhật màn hình sau mỗi 1000 pixel
            }
            
            // Thêm 4 pixel lân cận vào stack
            Pixel neighbor;
            
            // Bên phải
            neighbor.x = current.x + 1;
            neighbor.y = current.y;
            pixelStack.push(neighbor);
            
            // Bên trái
            neighbor.x = current.x - 1;
            neighbor.y = current.y;
            pixelStack.push(neighbor);
            
            // Bên trên
            neighbor.x = current.x;
            neighbor.y = current.y + 1;
            pixelStack.push(neighbor);
            
            // Bên dưới
            neighbor.x = current.x;
            neighbor.y = current.y - 1;
            pixelStack.push(neighbor);
        }
    }
    
    glFlush();
    std::cout << "Da to " << count << " pixels" << std::endl;
}

// Hàm vẽ đa giác
void drawPolygon() {
    if (polygon.size() < 2) return;
    
    glColor3fv(boundaryColor);
    glLineWidth(2.0f);
    
    if (polygonComplete) {
        glBegin(GL_LINE_LOOP);
    } else {
        glBegin(GL_LINE_STRIP);
    }
    
    for (size_t i = 0; i < polygon.size(); i++) {
        glVertex2f(polygon[i].x, polygon[i].y);
    }
    glEnd();
    
    // Vẽ các đỉnh
    glPointSize(6.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < polygon.size(); i++) {
        glVertex2f(polygon[i].x, polygon[i].y);
    }
    glEnd();
    glPointSize(1.0f);
}

// Hàm vẽ
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vẽ đa giác
    drawPolygon();
    
    // Hiển thị hướng dẫn
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 580);
    const char* msg = "Non-Recursive - Chuot trai: Them dinh. Phai: Dong. Giua: To mau. R: Reset";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

// Hàm đọc đa giác từ file
bool loadPolygonFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Khong the mo file: " << filename << std::endl;
        return false;
    }
    
    int n;
    file >> n;
    
    polygon.clear();
    for (int i = 0; i < n; i++) {
        Point p;
        file >> p.x >> p.y;
        polygon.push_back(p);
    }
    
    file.close();
    polygonComplete = true;
    
    std::cout << "Doc thanh cong " << n << " dinh tu file " << filename << std::endl;
    return true;
}

// Hàm xử lý click chuột
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        float mouseX = (float)x;
        float mouseY = (float)(600 - y);
        
        if (button == GLUT_LEFT_BUTTON) {
            // Thêm đỉnh
            if (!polygonComplete) {
                Point p;
                p.x = mouseX;
                p.y = mouseY;
                polygon.push_back(p);
                std::cout << "Them dinh " << polygon.size() << ": (" << p.x << ", " << p.y << ")" << std::endl;
                glutPostRedisplay();
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            // Đóng đa giác
            if (polygon.size() >= 3 && !polygonComplete) {
                polygonComplete = true;
                std::cout << "Da giac da dong voi " << polygon.size() << " dinh" << std::endl;
                glutPostRedisplay();
            }
        } else if (button == GLUT_MIDDLE_BUTTON || button == 3) {
            // Tô màu (chuột giữa hoặc scroll)
            if (polygonComplete) {
                std::cout << "Bat dau to mau (khong de quy) tai (" << mouseX << ", " << mouseY << ")..." << std::endl;
                boundaryFillNonRecursive((int)mouseX, (int)mouseY, fillColor, boundaryColor);
                std::cout << "Hoan thanh to mau!" << std::endl;
            } else {
                std::cout << "Vui long dong da giac truoc khi to mau!" << std::endl;
            }
        }
    }
}

// Hàm xử lý bàn phím
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        polygon.clear();
        polygonComplete = false;
        std::cout << "Reset!" << std::endl;
        glClear(GL_COLOR_BUFFER_BIT);
        glutPostRedisplay();
    } else if (key == 'l' || key == 'L') {
        // Load từ file
        if (loadPolygonFromFile("polygon_data.txt")) {
            glutPostRedisplay();
        }
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
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
    glPointSize(1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boundary Fill - Non-Recursive");
    
    init();
    
    std::cout << "\n=== BOUNDARY FILL - KHONG DE QUY (STACK) ===" << std::endl;
    std::cout << "Chuot trai: Them dinh da giac" << std::endl;
    std::cout << "Chuot phai: Dong da giac" << std::endl;
    std::cout << "Chuot giua: To mau" << std::endl;
    std::cout << "Phim L: Load da giac tu file polygon_data.txt" << std::endl;
    std::cout << "Phim R: Reset" << std::endl;
    std::cout << "Phim ESC: Thoat\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
