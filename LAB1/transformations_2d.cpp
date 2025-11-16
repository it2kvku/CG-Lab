#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

// Định nghĩa cấu trúc một điểm 2D
typedef struct {
    float x, y;
} Point;

// --- Các điểm của tam giác gốc ---
Point A = {1.0, 1.0};
Point B = {4.0, 1.0};
Point C = {1.0, 5.0};

// --- Các điểm của tam giác sau phép tỉ lệ ---
Point A1, B1, C1;

// --- Các điểm của tam giác sau phép quay ---
Point A2, B2, C2;

// --- Các điểm của tam giác sau phép đối xứng ---
Point A3, B3, C3;

// Hằng số PI
const float PI = 3.14159265f;

// Hàm khởi tạo môi trường OpenGL
void init() {
    glClearColor(0.1, 0.1, 0.1, 1.0); // Nền màu xám đậm
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0); // Thiết lập hệ tọa độ 2D
    glMatrixMode(GL_MODELVIEW);
}

// 1. HÀM BIẾN ĐỔI: PHÉP TỈ LỆ
// Biến đổi 1 điểm 'p' qua phép tỉ lệ tâm 'center' với tỉ số sx, sy
Point scalePoint(Point p, Point center, float sx, float sy) {
    Point newP;
    // 1. Tịnh tiến về gốc tọa độ
    float tempX = p.x - center.x;
    float tempY = p.y - center.y;
    // 2. Tỉ lệ
    tempX *= sx;
    tempY *= sy;
    // 3. Tịnh tiến trở lại
    newP.x = tempX + center.x;
    newP.y = tempY + center.y;
    return newP;
}

// 2. HÀM BIẾN ĐỔI: PHÉP QUAY
// Biến đổi 1 điểm 'p' qua phép quay tâm 'center' một góc 'angleRad' (radian)
Point rotatePoint(Point p, Point center, float angleRad) {
    Point newP;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    // 1. Tịnh tiến về gốc tọa độ
    float tempX = p.x - center.x;
    float tempY = p.y - center.y;
    // 2. Quay
    float rotatedX = tempX * cosA - tempY * sinA;
    float rotatedY = tempX * sinA + tempY * cosA;
    // 3. Tịnh tiến trở lại
    newP.x = rotatedX + center.x;
    newP.y = rotatedY + center.y;
    return newP;
}

// 3. HÀM BIẾN ĐỔI: PHÉP ĐỐI XỨNG
// Biến đổi 1 điểm 'p' qua phép đối xứng trục y = x + 1
Point reflectPoint(Point p) {
    Point newP;
    // Công thức biến đổi cho đường thẳng y = x + 1 (hay x - y + 1 = 0) là:
    // x' = y - 1
    // y' = x + 1
    newP.x = p.y - 1.0f;
    newP.y = p.x + 1.0f;
    return newP;
}

// Hàm tiện ích để vẽ một tam giác với màu cho trước
void drawTriangle(Point p1, Point p2, Point p3, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
    glEnd();
}

// Hàm vẽ các trục và đường thẳng đối xứng
void drawAxesAndLine() {
    // Vẽ trục X và Y (màu xám)
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
        // Trục X
        glVertex2f(-10.0, 0.0);
        glVertex2f(10.0, 0.0);
        // Trục Y
        glVertex2f(0.0, -10.0);
        glVertex2f(0.0, 10.0);
    glEnd();

    // Vẽ đường thẳng đối xứng y = x + 1 (màu lam)
    glColor3f(0.0, 0.5, 1.0);
    glBegin(GL_LINES);
        // Khi x = -10, y = -9
        glVertex2f(-10.0, -9.0);
        // Khi x = 9, y = 10
        glVertex2f(9.0, 10.0);
    glEnd();
}


// Hàm hiển thị chính
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Xóa màn hình
    glLoadIdentity();             // Reset ma trận

    // Vẽ các trục và đường thẳng
    drawAxesAndLine();

    // 1. Vẽ tam giác GỐC (Đỏ)
    drawTriangle(A, B, C, 1.0, 0.0, 0.0);

    // 2. Vẽ tam giác TỈ LỆ (Lục)
    drawTriangle(A1, B1, C1, 0.0, 1.0, 0.0);

    // 3. Vẽ tam giác QUAY (Vàng)
    drawTriangle(A2, B2, C2, 1.0, 1.0, 0.0);

    // 4. Vẽ tam giác ĐỐI XỨNG (Tím)
    drawTriangle(A3, B3, C3, 1.0, 0.0, 1.0);

    glFlush(); // Đẩy buffer ra màn hình
}

int main(int argc, char** argv) {
    // --- TÍNH TOÁN CÁC PHÉP BIẾN ĐỔI ---
    // (Thực hiện một lần trước khi vào vòng lặp chính)

    // 1. Phép tỉ lệ: Tâm A, tỉ số sx=1.5, sy=2.0
    float sx = 1.5f, sy = 2.0f;
    A1 = scalePoint(A, A, sx, sy); // A1 sẽ trùng A
    B1 = scalePoint(B, A, sx, sy);
    C1 = scalePoint(C, A, sx, sy);

    // 2. Phép quay: Tâm A, góc 45 độ
    float angleRad = 45.0f * PI / 180.0f;
    A2 = rotatePoint(A, A, angleRad); // A2 sẽ trùng A
    B2 = rotatePoint(B, A, angleRad);
    C2 = rotatePoint(C, A, angleRad);

    // 3. Phép đối xứng: qua đường y = x + 1
    A3 = reflectPoint(A);
    B3 = reflectPoint(B);
    C3 = reflectPoint(C);

    // --- KHỞI TẠO GLUT ---
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bien doi 2D - Tam giac ABC");

    init(); // Gọi hàm khởi tạo

    glutDisplayFunc(display); // Đăng ký hàm 'display'
    glutMainLoop();           // Bắt đầu vòng lặp sự kiện

    return 0;
}