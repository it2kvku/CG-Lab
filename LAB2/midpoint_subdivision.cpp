#include <GL/glut.h>
#include <iostream>
#include <cmath>

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

// Độ chính xác epsilon
const float EPSILON = 0.01f;

// Kiểm tra điểm có nằm trong cửa sổ không
bool isInside(float x, float y) {
    return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
}

// Thuật toán xén chia nhị phân (Midpoint Subdivision)
bool midpointSubdivisionClip(float x1, float y1, float x2, float y2, 
                              float &clipX1, float &clipY1, 
                              float &clipX2, float &clipY2) {
    bool p1Inside = isInside(x1, y1);
    bool p2Inside = isInside(x2, y2);
    
    // Trường hợp 1: Cả hai điểm đều trong cửa sổ
    if (p1Inside && p2Inside) {
        clipX1 = x1; clipY1 = y1;
        clipX2 = x2; clipY2 = y2;
        return true;
    }
    
    // Trường hợp 2: Cả hai điểm đều ngoài cửa sổ (cùng phía)
    if (!p1Inside && !p2Inside) {
        // Kiểm tra xem có cùng phía không
        if ((x1 < xMin && x2 < xMin) || (x1 > xMax && x2 > xMax) ||
            (y1 < yMin && y2 < yMin) || (y1 > yMax && y2 > yMax)) {
            return false;
        }
    }
    
    // Trường hợp 3: Chia nhị phân
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);
    
    // Nếu đoạn thẳng đủ nhỏ
    if (length < EPSILON) {
        if (p1Inside) {
            clipX1 = clipX2 = x1;
            clipY1 = clipY2 = y1;
            return true;
        } else if (p2Inside) {
            clipX1 = clipX2 = x2;
            clipY1 = clipY2 = y2;
            return true;
        }
        return false;
    }
    
    // Tính điểm giữa
    float midX = (x1 + x2) / 2.0f;
    float midY = (y1 + y2) / 2.0f;
    
    // Đệ quy cho nửa đầu
    float tempX1, tempY1, tempX2, tempY2;
    bool firstHalf = midpointSubdivisionClip(x1, y1, midX, midY, 
                                              tempX1, tempY1, tempX2, tempY2);
    
    // Đệ quy cho nửa sau
    float temp2X1, temp2Y1, temp2X2, temp2Y2;
    bool secondHalf = midpointSubdivisionClip(midX, midY, x2, y2, 
                                               temp2X1, temp2Y1, temp2X2, temp2Y2);
    
    // Kết hợp kết quả
    if (firstHalf && secondHalf) {
        clipX1 = tempX1;
        clipY1 = tempY1;
        clipX2 = temp2X2;
        clipY2 = temp2Y2;
        return true;
    } else if (firstHalf) {
        clipX1 = tempX1;
        clipY1 = tempY1;
        clipX2 = tempX2;
        clipY2 = tempY2;
        return true;
    } else if (secondHalf) {
        clipX1 = temp2X1;
        clipY1 = temp2Y1;
        clipX2 = temp2X2;
        clipY2 = temp2Y2;
        return true;
    }
    
    return false;
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
    const char* msg = "Midpoint Subdivision - Click 2 diem de tao doan thang. Phim R: Reset";
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
            isClipped = midpointSubdivisionClip(x1, y1, x2, y2,
                                                 clippedX1, clippedY1, 
                                                 clippedX2, clippedY2);
            
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
    glutCreateWindow("Midpoint Subdivision Line Clipping");
    
    init();
    
    std::cout << "\n=== MIDPOINT SUBDIVISION LINE CLIPPING ===" << std::endl;
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
