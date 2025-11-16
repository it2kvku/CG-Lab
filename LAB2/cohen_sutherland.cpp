#include <GL/glut.h>
#include <iostream>
#include <cmath>

// Định nghĩa các vùng cho Cohen-Sutherland
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// Tọa độ cửa sổ xén
float xMin = 150, yMin = 150, xMax = 400, yMax = 350;

// Điểm đầu và cuối của đoạn thẳng
float x1 = -1, y1 = -1, x2 = -1, y2 = -1;
int clickCount = 0;

// Lưu đoạn thẳng gốc và đoạn thẳng sau khi xén
float origX1, origY1, origX2, origY2;
float clippedX1, clippedY1, clippedX2, clippedY2;
bool isClipped = false;
bool hasValidLine = false;

// Hàm tính mã vùng cho một điểm
int computeRegionCode(float x, float y) {
    int code = INSIDE;
    
    if (x < xMin)       code |= LEFT;
    else if (x > xMax)  code |= RIGHT;
    if (y < yMin)       code |= BOTTOM;
    else if (y > yMax)  code |= TOP;
    
    return code;
}

// Thuật toán Cohen-Sutherland
bool cohenSutherlandClip(float &x1, float &y1, float &x2, float &y2) {
    int code1 = computeRegionCode(x1, y1);
    int code2 = computeRegionCode(x2, y2);
    bool accept = false;
    
    while (true) {
        if ((code1 == 0) && (code2 == 0)) {
            // Cả hai điểm nằm trong cửa sổ
            accept = true;
            break;
        } else if (code1 & code2) {
            // Cả hai điểm nằm cùng một phía bên ngoài
            break;
        } else {
            // Một số phần của đoạn thẳng nằm trong cửa sổ
            float x, y;
            
            // Chọn điểm nằm ngoài cửa sổ
            int codeOut = code1 ? code1 : code2;
            
            // Tìm giao điểm với cạnh của cửa sổ
            if (codeOut & TOP) {
                x = x1 + (x2 - x1) * (yMax - y1) / (y2 - y1);
                y = yMax;
            } else if (codeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (yMin - y1) / (y2 - y1);
                y = yMin;
            } else if (codeOut & RIGHT) {
                y = y1 + (y2 - y1) * (xMax - x1) / (x2 - x1);
                x = xMax;
            } else if (codeOut & LEFT) {
                y = y1 + (y2 - y1) * (xMin - x1) / (x2 - x1);
                x = xMin;
            }
            
            // Thay thế điểm nằm ngoài bằng giao điểm
            if (codeOut == code1) {
                x1 = x;
                y1 = y;
                code1 = computeRegionCode(x1, y1);
            } else {
                x2 = x;
                y2 = y;
                code2 = computeRegionCode(x2, y2);
            }
        }
    }
    
    return accept;
}

// Hàm vẽ
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vẽ cửa sổ xén (màu trắng)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(xMin, yMin);
    glVertex2f(xMax, yMin);
    glVertex2f(xMax, yMax);
    glVertex2f(xMin, yMax);
    glEnd();
    
    // Hiển thị hướng dẫn
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 580);
    const char* msg = "Click 2 diem de tao doan thang. Phim R: Reset";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Vẽ đoạn thẳng gốc (màu đỏ - nét đứt)
    if (hasValidLine) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0xAAAA);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(origX1, origY1);
        glVertex2f(origX2, origY2);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
        
        // Vẽ đoạn thẳng sau khi xén (màu xanh lá - nét liền)
        if (isClipped) {
            glLineWidth(2.0f);
            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f(clippedX1, clippedY1);
            glVertex2f(clippedX2, clippedY2);
            glEnd();
            glLineWidth(1.0f);
        }
    }
    
    // Vẽ các điểm đã click
    if (clickCount >= 1) {
        glPointSize(6.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(x1, y1);
        if (clickCount >= 2) {
            glVertex2f(x2, y2);
        }
        glEnd();
        glPointSize(1.0f);
    }
    
    glutSwapBuffers();
}

// Hàm xử lý click chuột
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Chuyển đổi tọa độ (y đảo ngược trong OpenGL)
        float mouseX = (float)x;
        float mouseY = (float)(600 - y);
        
        if (clickCount == 0) {
            x1 = mouseX;
            y1 = mouseY;
            clickCount = 1;
            std::cout << "Diem 1: (" << x1 << ", " << y1 << ")" << std::endl;
        } else if (clickCount == 1) {
            x2 = mouseX;
            y2 = mouseY;
            clickCount = 2;
            std::cout << "Diem 2: (" << x2 << ", " << y2 << ")" << std::endl;
            
            // Lưu đoạn thẳng gốc
            origX1 = x1; origY1 = y1;
            origX2 = x2; origY2 = y2;
            hasValidLine = true;
            
            // Thực hiện xén
            clippedX1 = x1; clippedY1 = y1;
            clippedX2 = x2; clippedY2 = y2;
            
            isClipped = cohenSutherlandClip(clippedX1, clippedY1, clippedX2, clippedY2);
            
            if (isClipped) {
                std::cout << "Doan thang sau khi xen: (" << clippedX1 << ", " << clippedY1 
                         << ") - (" << clippedX2 << ", " << clippedY2 << ")" << std::endl;
            } else {
                std::cout << "Doan thang bi loai bo hoan toan!" << std::endl;
            }
        }
        
        glutPostRedisplay();
    }
}

// Hàm xử lý bàn phím
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        // Reset
        x1 = y1 = x2 = y2 = -1;
        clickCount = 0;
        hasValidLine = false;
        isClipped = false;
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
    glutCreateWindow("Cohen-Sutherland Line Clipping");
    
    init();
    
    std::cout << "\n=== COHEN-SUTHERLAND LINE CLIPPING ===" << std::endl;
    std::cout << "Click chuot trai 2 lan de tao doan thang" << std::endl;
    std::cout << "Doan thang goc: mau do (net dut)" << std::endl;
    std::cout << "Doan thang sau khi xen: mau xanh la (net lien)" << std::endl;
    std::cout << "Phim R: Reset" << std::endl;
    std::cout << "Phim ESC: Thoat\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
