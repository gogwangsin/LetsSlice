#if 0
#include <GL/glew.h>
#include <GL/glut.h>

GLuint vboID; // VBO ID

void createVBO() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,  // 첫 번째 정점
        0.5f, 0.0f, 0.0f,  // 두 번째 정점
        0.0f, 0.5f, 0.0f,  // 세 번째 정점
        -0.5f, 0.0f, 0.0f, // 네 번째 정점
        0.0f, -0.5f, 0.0f, // 다섯 번째 정점
    };

    glGenBuffers(1, &vboID); // VBO 생성
    glBindBuffer(GL_ARRAY_BUFFER, vboID); // VBO를 바인딩
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 데이터를 GPU로 전송
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 5); // 삼각형 팬 그리기

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 바인딩 해제

    glFlush();
}

int main(int argc, char** argv) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutInit(&argc, argv);
    glutCreateWindow("VBO Triangle Fan");
    glewInit();

    createVBO(); // VBO 생성

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
    // 점이 직선 위에 있는지 확인
    double x1 = line.start.x;
    double y1 = line.start.y;
    double x2 = line.end.x;
    double y2 = line.end.y;
    double x = point.x;
    double y = point.y;

    double d1 = (x - x1) * (y2 - y1);
    double d2 = (y - y1) * (x2 - x1);

    if (std::abs(d1 - d2) < std::numeric_limits<double>::epsilon()) {
        // 두 값이 거의 같으면 점이 직선 위에 있음
        return true;
    }

    return false;
}

bool IsCollisionWithSquare(const Line& line, const Point& squareCenter, double squareSize) {
    // 정사각형 네 변의 끝점을 계산
    Point topLeft = { squareCenter.x - squareSize / 2, squareCenter.y + squareSize / 2 };
    Point topRight = { squareCenter.x + squareSize / 2, squareCenter.y + squareSize / 2 };
    Point bottomLeft = { squareCenter.x - squareSize / 2, squareCenter.y - squareSize / 2 };
    Point bottomRight = { squareCenter.x + squareSize / 2, squareCenter.y - squareSize / 2 };

    // 직선과 정사각형의 네 변 각각에 대해 충돌을 확인
    if (IsPointOnLine(line, topLeft) || IsPointOnLine(line, topRight) ||
        IsPointOnLine(line, bottomLeft) || IsPointOnLine(line, bottomRight)) {
        return true;
    }

    return false;
}

int main() {
    Line line = { {0.0, 1.0}, {1.0, 0.0} }; // 예시 직선
    Point squareCenter = { 0.5, 0.5 };     // 정사각형 중심 좌표
    double squareSize = 1.0;             // 정사각형 변의 길이

    if (IsCollisionWithSquare(line, squareCenter, squareSize)) {
        std::cout << "직선과 정사각형이 충돌합니다." << std::endl;
    }
    else {
        std::cout << "직선과 정사각형이 충돌하지 않습니다." << std::endl;
    }

    return 0;
}
#endif