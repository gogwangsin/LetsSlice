#if 0
#include <GL/glew.h>
#include <GL/glut.h>

GLuint vboID; // VBO ID

void createVBO() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,  // ù ��° ����
        0.5f, 0.0f, 0.0f,  // �� ��° ����
        0.0f, 0.5f, 0.0f,  // �� ��° ����
        -0.5f, 0.0f, 0.0f, // �� ��° ����
        0.0f, -0.5f, 0.0f, // �ټ� ��° ����
    };

    glGenBuffers(1, &vboID); // VBO ����
    glBindBuffer(GL_ARRAY_BUFFER, vboID); // VBO�� ���ε�
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // �����͸� GPU�� ����
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 5); // �ﰢ�� �� �׸���

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // ���ε� ����

    glFlush();
}

int main(int argc, char** argv) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutInit(&argc, argv);
    glutCreateWindow("VBO Triangle Fan");
    glewInit();

    createVBO(); // VBO ����

    glutInitWindowSize(400, 400);
    glutDisplayFunc(draw);
    glutMainLoop();
    return 0;
}
#endif
#if 0
#include <iostream>
#include <limits>

struct Point {
    double x;
    double y;
};

struct Line {
    Point start;
    Point end;
};

bool IsPointOnLine(const Line& line, const Point& point) {
    // ���� ���� ���� �ִ��� Ȯ��
    double x1 = line.start.x;
    double y1 = line.start.y;
    double x2 = line.end.x;
    double y2 = line.end.y;
    double x = point.x;
    double y = point.y;

    double d1 = (x - x1) * (y2 - y1);
    double d2 = (y - y1) * (x2 - x1);

    if (std::abs(d1 - d2) < std::numeric_limits<double>::epsilon()) {
        // �� ���� ���� ������ ���� ���� ���� ����
        return true;
    }

    return false;
}

bool IsCollisionWithSquare(const Line& line, const Point& squareCenter, double squareSize) {
    // ���簢�� �� ���� ������ ���
    Point topLeft = { squareCenter.x - squareSize / 2, squareCenter.y + squareSize / 2 };
    Point topRight = { squareCenter.x + squareSize / 2, squareCenter.y + squareSize / 2 };
    Point bottomLeft = { squareCenter.x - squareSize / 2, squareCenter.y - squareSize / 2 };
    Point bottomRight = { squareCenter.x + squareSize / 2, squareCenter.y - squareSize / 2 };

    // ������ ���簢���� �� �� ������ ���� �浹�� Ȯ��
    if (IsPointOnLine(line, topLeft) || IsPointOnLine(line, topRight) ||
        IsPointOnLine(line, bottomLeft) || IsPointOnLine(line, bottomRight)) {
        return true;
    }

    return false;
}

int main() {
    Line line = { {0.0, 1.0}, {1.0, 0.0} }; // ���� ����
    Point squareCenter = { 0.5, 0.5 };     // ���簢�� �߽� ��ǥ
    double squareSize = 1.0;             // ���簢�� ���� ����

    if (IsCollisionWithSquare(line, squareCenter, squareSize)) {
        std::cout << "������ ���簢���� �浹�մϴ�." << std::endl;
    }
    else {
        std::cout << "������ ���簢���� �浹���� �ʽ��ϴ�." << std::endl;
    }

    return 0;
}
#endif