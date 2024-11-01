#if 1

#include "usingInclude.h"
#include "Slice.h"


GLchar* gVertexSource;
GLchar* gFragmentSource; //--- 소스코드 저장 변수
GLuint gVertexShader;
GLuint gFragmentShader; //--- 세이더 객체
GLuint gShaderProgramID; //--- 셰이더 프로그램

int gWidth{ 700 };
int gHeight{ 700 };

bool gToggle[(int)Toggle::END];

vector<Basis*> gVec;
vector<Basis*> gFrag;
tagBasket* gBasket;
tagSliceLine* gSlicing;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	// 기본 설정
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(gWidth, gHeight);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();

	// 쉐이더 프로그램과 버퍼 초기화
	make_shaderProgram();
	ShowMenu();
	initToggle();
	initBasket();

	// => 콜백함수 설정 
	glutMouseFunc(Mouse);						 // 마우스 입력 
	glutMotionFunc(Motion);						 // 마우스 누른 채 움직일 때
	glutKeyboardFunc(Keyboard);					 // 키보드 입력 콜백 함수
	glutTimerFunc(10, TimerFunction, 1);		 // 타이머 설정, 0.1초마다, 함수호출, 콜백 함수에 1 전달
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

#endif