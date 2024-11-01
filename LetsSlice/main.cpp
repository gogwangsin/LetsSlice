#if 1

#include "usingInclude.h"
#include "Slice.h"


GLchar* gVertexSource;
GLchar* gFragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint gVertexShader;
GLuint gFragmentShader; //--- ���̴� ��ü
GLuint gShaderProgramID; //--- ���̴� ���α׷�

int gWidth{ 700 };
int gHeight{ 700 };

bool gToggle[(int)Toggle::END];

vector<Basis*> gVec;
vector<Basis*> gFrag;
tagBasket* gBasket;
tagSliceLine* gSlicing;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	// �⺻ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(gWidth, gHeight);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();

	// ���̴� ���α׷��� ���� �ʱ�ȭ
	make_shaderProgram();
	ShowMenu();
	initToggle();
	initBasket();

	// => �ݹ��Լ� ���� 
	glutMouseFunc(Mouse);						 // ���콺 �Է� 
	glutMotionFunc(Motion);						 // ���콺 ���� ä ������ ��
	glutKeyboardFunc(Keyboard);					 // Ű���� �Է� �ݹ� �Լ�
	glutTimerFunc(10, TimerFunction, 1);		 // Ÿ�̸� ����, 0.1�ʸ���, �Լ�ȣ��, �ݹ� �Լ��� 1 ����
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

#endif