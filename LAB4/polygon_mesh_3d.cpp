#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Cấu trúc điểm 3D
struct Point3D {
    float x, y, z, w;
    Point3D() : x(0), y(0), z(0), w(1) {}
    Point3D(float x, float y, float z) : x(x), y(y), z(z), w(1) {}
};

// Cấu trúc mặt đa giác
struct Face {
    std::vector<int> vertices;
};

// Dữ liệu đối tượng
std::vector<Point3D> vertices;
std::vector<Face> faces;

// Tham số quan sát
float theta = 30.0f;    // Góc quay quanh trục Y
float phi = 20.0f;      // Góc quay quanh trục X
float R = 8.0f;         // Khoảng cách quan sát
float D = 2.0f;         // Hệ số zoom

// Chế độ chiếu
bool usePerspective = true;

// Góc quay trực tiếp quanh các trục (cho 4.1)
float rotX = 0.0f;
float rotY = 0.0f;
float rotZ = 0.0f;

// Chế độ điều khiển
enum ControlMode {
    MODE_VIEW_MATRIX,    // Điều khiển bằng ma trận quan sát (4.2)
    MODE_DIRECT_ROTATION // Điều khiển quay trực tiếp (4.1)
};
ControlMode currentMode = MODE_VIEW_MATRIX;

// Hàm đọc dữ liệu từ file
bool loadMeshData(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Khong the mo file: " << filename << std::endl;
        return false;
    }
    
    int numVertices;
    file >> numVertices;
    
    vertices.clear();
    for (int i = 0; i < numVertices; i++) {
        float x, y, z;
        file >> x >> y >> z;
        vertices.push_back(Point3D(x, y, z));
    }
    
    int numFaces;
    file >> numFaces;
    
    faces.clear();
    for (int i = 0; i < numFaces; i++) {
        int numVerticesInFace;
        file >> numVerticesInFace;
        
        Face face;
        for (int j = 0; j < numVerticesInFace; j++) {
            int vertexIndex;
            file >> vertexIndex;
            face.vertices.push_back(vertexIndex);
        }
        faces.push_back(face);
    }
    
    file.close();
    
    std::cout << "Da doc " << vertices.size() << " dinh va " << faces.size() << " mat" << std::endl;
    return true;
}

// Nhân ma trận 4x4 với điểm
Point3D multiplyMatrixPoint(float matrix[4][4], Point3D p) {
    Point3D result;
    result.x = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2] * p.z + matrix[0][3] * p.w;
    result.y = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2] * p.z + matrix[1][3] * p.w;
    result.z = matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2] * p.z + matrix[2][3] * p.w;
    result.w = matrix[3][0] * p.x + matrix[3][1] * p.y + matrix[3][2] * p.z + matrix[3][3] * p.w;
    
    if (result.w != 0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
        result.w = 1.0f;
    }
    
    return result;
}

// Nhân 2 ma trận 4x4
void multiplyMatrix(float result[4][4], float a[4][4], float b[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Tính ma trận biến đổi quan sát (dùng cho chế độ 4.2)
void calculateViewMatrix(float matrix[4][4]) {
    float thetaRad = theta * M_PI / 180.0f;
    float phiRad = phi * M_PI / 180.0f;
    
    float cosTheta = cos(thetaRad);
    float sinTheta = sin(thetaRad);
    float cosPhi = cos(phiRad);
    float sinPhi = sin(phiRad);
    
    // Ma trận biến đổi quan sát T
    matrix[0][0] = -sinTheta;
    matrix[0][1] = -cosTheta * cosPhi;
    matrix[0][2] = -cosTheta * sinPhi;
    matrix[0][3] = 0;
    
    matrix[1][0] = cosTheta;
    matrix[1][1] = -sinTheta * cosPhi;
    matrix[1][2] = -sinTheta * sinPhi;
    matrix[1][3] = 0;
    
    matrix[2][0] = 0;
    matrix[2][1] = sinPhi;
    matrix[2][2] = -cosPhi;
    matrix[2][3] = R;
    
    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;
}

// Ma trận phóng to/thu nhỏ
void calculateScaleMatrix(float matrix[4][4], float scale) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    matrix[0][0] = scale;
    matrix[1][1] = scale;
    matrix[2][2] = scale;
}

// Ma trận quay quanh trục X
void calculateRotationX(float matrix[4][4], float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    matrix[1][1] = c;  matrix[1][2] = -s;
    matrix[2][1] = s;  matrix[2][2] = c;
}

// Ma trận quay quanh trục Y
void calculateRotationY(float matrix[4][4], float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    matrix[0][0] = c;  matrix[0][2] = s;
    matrix[2][0] = -s; matrix[2][2] = c;
}

// Ma trận quay quanh trục Z
void calculateRotationZ(float matrix[4][4], float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    matrix[0][0] = c;  matrix[0][1] = -s;
    matrix[1][0] = s;  matrix[1][1] = c;
}

// Forward declaration
void reshape(int w, int h);

// Vẽ hệ trục tọa độ
void drawAxes() {
    glLineWidth(2.0f);
    
    // Trục X - màu đỏ
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);
    glEnd();
    
    // Trục Y - màu xanh lá
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);
    glEnd();
    
    // Trục Z - màu xanh dương
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
    
    glLineWidth(1.0f);
}

// Vẽ đối tượng
void drawObject() {
    // Vẽ các cạnh
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.5f);
    
    for (size_t i = 0; i < faces.size(); i++) {
        glBegin(GL_LINE_LOOP);
        for (size_t j = 0; j < faces[i].vertices.size(); j++) {
            int idx = faces[i].vertices[j];
            glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
        }
        glEnd();
    }
    
    // Vẽ các đỉnh
    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < vertices.size(); i++) {
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    }
    glEnd();
    glPointSize(1.0f);
    glLineWidth(1.0f);
}

// Hiển thị
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Thiết lập camera
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
    
    if (currentMode == MODE_VIEW_MATRIX) {
        // Chế độ 4.2: Dùng ma trận quan sát
        glPushMatrix();
        
        // Tính và áp dụng các ma trận biến đổi
        float viewMatrix[4][4];
        calculateViewMatrix(viewMatrix);
        
        float scaleMatrix[4][4];
        calculateScaleMatrix(scaleMatrix, D);
        
        float finalMatrix[4][4];
        multiplyMatrix(finalMatrix, scaleMatrix, viewMatrix);
        
        // Chuyển sang dạng OpenGL (column-major)
        float glMatrix[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                glMatrix[j * 4 + i] = finalMatrix[i][j];
            }
        }
        
        glMultMatrixf(glMatrix);
        
        drawAxes();
        drawObject();
        
        glPopMatrix();
        
    } else {
        // Chế độ 4.1: Quay trực tiếp quanh các trục
        glPushMatrix();
        
        glScalef(D, D, D);
        
        // Áp dụng các phép quay
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        glRotatef(rotZ, 0, 0, 1);
        
        drawAxes();
        drawObject();
        
        glPopMatrix();
    }
    
    // Hiển thị thông tin
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 0.0f);
    
    std::stringstream ss;
    if (currentMode == MODE_VIEW_MATRIX) {
        ss << "CHE DO: Ma tran quan sat (4.2)";
    } else {
        ss << "CHE DO: Quay truc tiep (4.1)";
    }
    
    glRasterPos2f(10, 580);
    std::string str = ss.str();
    for (size_t i = 0; i < str.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }
    
    ss.str("");
    if (currentMode == MODE_VIEW_MATRIX) {
        ss << "Theta=" << theta << ", Phi=" << phi << ", R=" << R << ", D=" << D;
    } else {
        ss << "RotX=" << rotX << ", RotY=" << rotY << ", RotZ=" << rotZ << ", D=" << D;
    }
    
    glRasterPos2f(10, 560);
    str = ss.str();
    for (size_t i = 0; i < str.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }
    
    glRasterPos2f(10, 540);
    std::string proj = usePerspective ? "Phep chieu: PHOI CANH" : "Phep chieu: SONG SONG";
    for (size_t i = 0; i < proj.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, proj[i]);
    }
    
    glRasterPos2f(10, 520);
    const char* help = "M: Doi che do | P: Doi phep chieu | Phim mui ten, +/-, Q/W/E/A/S/D";
    for (const char* c = help; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glutSwapBuffers();
}

// Xử lý phím đặc biệt
void specialKeys(int key, int x, int y) {
    if (currentMode == MODE_VIEW_MATRIX) {
        switch (key) {
            case GLUT_KEY_UP:    phi += 5.0f; break;
            case GLUT_KEY_DOWN:  phi -= 5.0f; break;
            case GLUT_KEY_LEFT:  theta -= 5.0f; break;
            case GLUT_KEY_RIGHT: theta += 5.0f; break;
        }
    } else {
        switch (key) {
            case GLUT_KEY_UP:    rotX += 5.0f; break;
            case GLUT_KEY_DOWN:  rotX -= 5.0f; break;
            case GLUT_KEY_LEFT:  rotY -= 5.0f; break;
            case GLUT_KEY_RIGHT: rotY += 5.0f; break;
        }
    }
    glutPostRedisplay();
}

// Xử lý bàn phím
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+':
        case '=':
            D += 0.1f;
            break;
        case '-':
        case '_':
            D -= 0.1f;
            if (D < 0.1f) D = 0.1f;
            break;
        case 'p':
        case 'P':
            usePerspective = !usePerspective;
            reshape(800, 600);
            break;
        case 'm':
        case 'M':
            currentMode = (currentMode == MODE_VIEW_MATRIX) ? MODE_DIRECT_ROTATION : MODE_VIEW_MATRIX;
            std::cout << "Chuyen sang che do: " << (currentMode == MODE_VIEW_MATRIX ? "Ma tran quan sat" : "Quay truc tiep") << std::endl;
            break;
        case 'q':
        case 'Q':
            if (currentMode == MODE_DIRECT_ROTATION) rotZ += 5.0f;
            else R += 0.5f;
            break;
        case 'e':
        case 'E':
            if (currentMode == MODE_DIRECT_ROTATION) rotZ -= 5.0f;
            else R -= 0.5f;
            break;
        case 'w':
        case 'W':
            if (currentMode == MODE_VIEW_MATRIX) R += 0.5f;
            break;
        case 's':
        case 'S':
            if (currentMode == MODE_VIEW_MATRIX) R -= 0.5f;
            break;
        case 'r':
        case 'R':
            theta = 30.0f;
            phi = 20.0f;
            R = 8.0f;
            D = 2.0f;
            rotX = rotY = rotZ = 0.0f;
            std::cout << "Reset!" << std::endl;
            break;
        case 27:
            exit(0);
    }
    glutPostRedisplay();
}

// Xử lý chuột
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            if (currentMode == MODE_VIEW_MATRIX) {
                R -= 0.5f;
            } else {
                D += 0.1f;
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (currentMode == MODE_VIEW_MATRIX) {
                R += 0.5f;
            } else {
                D -= 0.1f;
                if (D < 0.1f) D = 0.1f;
            }
        }
        glutPostRedisplay();
    }
}

// Thay đổi kích thước cửa sổ
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if (usePerspective) {
        gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
    } else {
        float aspect = (float)w / (float)h;
        glOrtho(-5 * aspect, 5 * aspect, -5, 5, 0.1, 100.0);
    }
    
    glMatrixMode(GL_MODELVIEW);
}

// Khởi tạo
void init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("LAB 4 - Quan Sat Vat The 3D");
    
    init();
    
    if (!loadMeshData("data.txt")) {
        std::cerr << "Loi doc file data.txt!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== LAB 4 - QUAN SAT VAT THE 3D ===" << std::endl;
    std::cout << "\n4.1. QUAY TRUC TIEP QUANH CAC TRUC (MODE_DIRECT_ROTATION):" << std::endl;
    std::cout << "- Phim mui ten: Quay quanh truc X, Y" << std::endl;
    std::cout << "- Phim Q/E: Quay quanh truc Z" << std::endl;
    std::cout << "- Phim +/-: Phong to / thu nho" << std::endl;
    std::cout << "\n4.2. MA TRAN BIEN DOI QUAN SAT (MODE_VIEW_MATRIX):" << std::endl;
    std::cout << "- Phim mui ten: Thay doi theta, phi" << std::endl;
    std::cout << "- Phim W/S: Thay doi khoang cach quan sat R" << std::endl;
    std::cout << "- Phim +/-: Phong to / thu nho (D)" << std::endl;
    std::cout << "- Chuot trai/phai: Dieu chinh R" << std::endl;
    std::cout << "\nCHUNG:" << std::endl;
    std::cout << "- Phim M: Chuyen doi giua 2 che do" << std::endl;
    std::cout << "- Phim P: Doi phep chieu (Song song / Phoi canh)" << std::endl;
    std::cout << "- Phim R: Reset" << std::endl;
    std::cout << "- Phim ESC: Thoat\n" << std::endl;
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    
    glutMainLoop();
    return 0;
}
