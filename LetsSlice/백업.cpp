// 2023. 11. 11 19:32 �ϼ�

#if 0
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

	fPoint pivot{ -0.3, 0.5f };
	tagRectangle* frg = new tagRectangle{ pivot };

	gVec.push_back(frg);

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
#endif

#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;
extern vector<Basis*> gFrag;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			//	cout << "������ �������� ����\n"; 
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			//	cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	gVecRemove();
	gVecPushBack();
	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pTri);

		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pRec);

		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();

	for (auto& obj : gFrag) {
		delete obj;
	}
	gFrag.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}

	for (auto& iter : gFrag) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();

	for (auto& element : gFrag)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	gVecIntersection();
	gFragIntersection();
}

void gVecIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	for (auto iter = gVec.begin(); iter != gVec.end();) {
		fPoint slice_point[2]{};
		int slice_index{};
		EdgeList* list{};
		GLint edge_num{};
		GLint ver_num{};
		GLfloat* vert{};

		int polygon_index[2]{};
		int index_cnt{};

		(*iter)->initCount();
		EdgeList* edgelist = (*iter)->getEdgeList();
		edge_num = (*iter)->getEdgeNum();
		ver_num = (*iter)->getVerNum();


		for (int i = 0; i < edge_num; ++i) {
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);

			if (intersection.x == 0.0 && intersection.y == 0.0) {
			}
			else {
				list = edgelist;
				(*iter)->countSlice();
				vert = (*iter)->getVertex();
				slice_point[slice_index].x = intersection.x;
				slice_point[slice_index].y = intersection.y;
				slice_index++;

				polygon_index[index_cnt++] = getIndex(edgelist[i].ep, vert, ver_num);

				if (slice_index == 2) {
					tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num, polygon_index[0],  polygon_index[1], vert, (*iter)->getfRGB() };
					gFrag.push_back(frag);
					tagFragment* frag2 = new tagFragment{ slice_point[1], slice_point[0], list, MINUS, edge_num, polygon_index[1], polygon_index[0], vert,(*iter)->getfRGB() };
					gFrag.push_back(frag2);
					iter = gVec.erase(iter);
					break;
				}
			}
		}

		if (slice_index < 2)
			++iter;
	}
}

void gFragIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };
	vector<tagFragment*> fragmentsToAdd; // �ӽ� ����

	for (auto iter = gFrag.begin(); iter != gFrag.end();) {
		fPoint slice_point[2]{};
		int slice_index{};
		EdgeList* list{};
		GLint edge_num{};
		GLint ver_num{};
		GLfloat* vert{};
		int polygon_index[2]{};
		int index_cnt{};

		(*iter)->initCount();
		EdgeList* edgelist = (*iter)->getEdgeList();
		edge_num = (*iter)->getEdgeNum();
		ver_num = (*iter)->getVerNum();


		for (int i = 0; i < edge_num; ++i) {
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);

			if (intersection.x == 0.0 && intersection.y == 0.0) {
			}
			else {
				list = edgelist;
				(*iter)->countSlice();
				vert = (*iter)->getVertex();
				slice_point[slice_index].x = intersection.x;
				slice_point[slice_index].y = intersection.y;
				slice_index++;

				polygon_index[index_cnt++] = getIndex(edgelist[i].ep, vert, ver_num);

				if (slice_index == 2) {
					tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num, polygon_index[0],  polygon_index[1], vert,(*iter)->getfRGB() };
					fragmentsToAdd.push_back(frag);
					tagFragment* frag2 = new tagFragment{ slice_point[1], slice_point[0], list, MINUS, edge_num, polygon_index[1], polygon_index[0], vert,(*iter)->getfRGB() };
					fragmentsToAdd.push_back(frag2);
					iter = gFrag.erase(iter);
					break;
				}
			}
		}

		if (slice_index < 2)
			++iter;
	}

	for (auto& fragment : fragmentsToAdd) {
		gFrag.push_back(fragment);
	}
}

int getIndex(const fPoint& edge_end_point, GLfloat* vertex, int vertex_size)
{
	int index{};
	for (int i = 0; i < vertex_size; ++i) {

		GLfloat* current = &vertex[i * 3];
		if (edge_end_point.x == current[0] && edge_end_point.y == current[1]) { // ���� ����� 1���� -> �ε� �Ҽ� == 
			break;
		}
		index = index + 3;
	}

	index /= 3; // ŵ
	return index;
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x - 0.5; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x - 0.5; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y - 0.5;
	else if (sp.y > ep.y)
		minXY.y = ep.y - 0.5;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x + 0.5;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x + 0.5;

	if (sp.y < ep.y)
		maxXY.y = ep.y + 0.5;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y + 0.5;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagTriangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "0 : sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "1: sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[0][0];
	edge[2].ep.y = m_vertex[0][1];
	//	cout << "2: sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";
}

void tagTriangle::initEdgeList()
{

}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}

void tagRectangle::initEdgeList()
{

}

//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagPentagon::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[7][0];
	edge[0].ep.y = m_vertex[7][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[7][0];
	edge[1].sp.y = m_vertex[7][1];
	edge[1].ep.x = m_vertex[8][0];
	edge[1].ep.y = m_vertex[8][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[8][0];
	edge[2].sp.y = m_vertex[8][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[2][0];
	edge[3].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
	edge[4].sp.x = m_vertex[2][0];
	edge[4].sp.y = m_vertex[2][1];
	edge[4].ep.x = m_vertex[0][0];
	edge[4].ep.y = m_vertex[0][1];
}

void tagPentagon::initEdgeList()
{

}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}

//========================================================================

bool tagFragment::isGone()
{
	if (m_vertex[0][1] <= -1.5f)
		return true;
	else
		return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	BasketMove();
	if (!bBasket) FreeFall();
	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);

}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLE_FAN, 0, edge_num);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < edge_num; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagFragment::UpdateEdgeList()
{
	for (int i = 0; i < edge_num - 1; ++i) {
		edge[i].sp.x = m_vertex[i][0];
		edge[i].sp.y = m_vertex[i][1];
		edge[i].ep.x = m_vertex[i + 1][0];
		edge[i].ep.y = m_vertex[i + 1][1];
	}
}

void tagFragment::initEdgeList()
{
	edge_num = 0;
	int i{};

	while (1)
	{
		if (m_vertex[i + 1][0] == 0. && m_vertex[i + 1][1] == 0.f) {
			break;
		}

		edge[i].sp.x = m_vertex[i][0];
		edge[i].sp.y = m_vertex[i][1];
		edge[i].ep.x = m_vertex[i + 1][0];
		edge[i].ep.y = m_vertex[i + 1][1];

		i++;
		edge_num++;
	}
	edge_num++;
	m_vertex[i + 1][0] = m_vertex[0][0];
	m_vertex[i + 1][1] = m_vertex[0][1];
}

int getEdgeStart(EdgeList* el, GLfloat* vert, int ver_st_index, int edge_num)
{
	int idx{};
	for (; idx < edge_num; ++idx) {
		if (el[idx].ep.x == vert[ver_st_index * 3] && el[idx].ep.y == vert[ver_st_index * 3 + 1]) {
			return idx;
		}
	}
	return idx;
}

void tagFragment::initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list, int ver_st_index, int ver_ed_index, GLfloat* vert, int edge_num)
{
	m_vertex[0][0] = sp.x;
	m_vertex[0][1] = sp.y;

	int indx{ 1 };
	int edge_idx = getEdgeStart(edge_list, vert, ver_st_index, edge_num);

	while (1)
	{
		if (edge_idx >= edge_num)
			edge_idx = 0;

		if (edge_list[edge_idx].ep.x == vert[ver_ed_index * 3] && edge_list[edge_idx].ep.y == vert[ver_ed_index * 3 + 1])
			break;

		m_vertex[indx][0] = edge_list[edge_idx].ep.x;
		m_vertex[indx][1] = edge_list[edge_idx].ep.y;

		edge_idx++;
		indx++;
	}
	m_vertex[indx][0] = ep.x;
	m_vertex[indx][1] = ep.y;

}

void tagFragment::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;

	for (int i = 0; i < edge_num; ++i) {
		m_vertex[i][0] += m_veloX * gTimeStep * m_sign;
		m_vertex[i][1] += m_veloY * gTimeStep;
	}
}

void tagFragment::BasketMove()
{
	if (OFF == bBasket)
		CheckBasket();
	else
	{
		Sign sign = gBasket->getSign();
		int i{};
		while (1)
		{
			if (m_vertex[i][0] == 0. && m_vertex[i][1] == 0.)
				break;
			m_vertex[i][0] += sign * 0.005f;
			i++;
		}
	}
}

void tagFragment::CheckBasket()
{
	fPoint basket_pos = gBasket->getfPivot();
	GLfloat basket_x_r = gBasket->get_x_radius();
	GLfloat basket_y_r = gBasket->get_y_radius();

	fPoint left_top{ basket_pos.x - basket_x_r, basket_pos.y + basket_y_r };
	fPoint right_down{ basket_pos.x + basket_x_r, basket_pos.y - basket_y_r };

	for (int i = 0; i < edge_num; ++i) {

		if (edge[i].sp.x + m_veloX * gTimeStep * m_sign >= left_top.x &&
			edge[i].sp.x + m_veloX * gTimeStep * m_sign <= right_down.x &&
			edge[i].sp.y + m_veloY * gTimeStep >= right_down.y &&
			edge[i].sp.y + m_veloY * gTimeStep <= left_top.y) {
			bBasket = ON;
			m_veloX = 0.f;
			m_veloY = 0.f;
			break;
		}
	}
}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			minXY.x = sp.x - 0.005f;
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

	//	cout << "edge minx,miny : " << minXY.x << ", " << minXY.y << '\n';
	return minXY;
}

fPoint getMaxXY(fPoint sp, fPoint ep)
{
	fPoint maxXY{};

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	//	cout << "edge maxx,maxy : " << maxXY.x << ", " << maxXY.y << '\n';

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	float det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;

	fPoint intersection = { x, y };

	if (isInRange(x, getMinXY(edge_start, edge_end).x, getMaxXY(edge_start, edge_end).x) && isInRange(y, getMinXY(edge_start, edge_end).y, getMaxXY(edge_start, edge_end).y)) {
		return intersection; // Out of range or no intersection
	}
	else
		return { 0,0 };
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {
	//	cout << "edge sp,ep: " << edge_start.x << ", " << edge_start.y << ", " << edge_end.x << ", " << edge_end.y << '\n';
	//	cout << "mouse sp,ep  : " << slice_start.x << ", " << slice_start.y << ", " << slice_end.x << ", " << slice_end.y << '\n';
	//	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}



#endif



//fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
//	double det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);
//
//	if (det == 0) {
//		return { 0, 0 }; // No intersection
//	}
//
//	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//
//	fPoint intersection = { x, y };
//	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';
//	cout << "fmin/max X : " << fmin(edge_start.x, edge_end.x) << ", " << fmax(edge_start.x, edge_end.x) << '\n';
//	cout << "fmin/max Y: " << fmin(edge_start.y, edge_end.y) << ", " << fmax(edge_start.y, edge_end.y) << '\n';
//
//
//	//	if (x >= fmin(edge_start.x, edge_end.x) && x <= fmax(edge_start.x, edge_end.x) &&
//	//		y >= fmin(edge_start.y, edge_end.y) && y <= fmax(edge_start.y, edge_end.y)) {
//	return intersection;  // �� ������ ������
//	//	}
//	//	else
//	//		return { 0,0 };
//}

#endif

#if 0

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual EdgeList* getEdgeList() = 0;
	virtual GLint getEdgeNum() = 0;
	virtual void initCount() = 0;
	virtual void countSlice() = 0;
	virtual GLfloat* getVertex() = 0;
	virtual GLint getVerNum() = 0;

	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
	fPoint getMinXYPoint();
	fPoint getMaxXYPoint();
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }
};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }
	Sign getSign() { return m_sign; }
};

//========================================================================

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};

	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[3]{};
	GLint    edge_num{ 3 };
	GLint	slice_point_count{};

public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagTriangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void countSlice() { slice_point_count++; }

	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagRectangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;

	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[5]{};
	GLint    edge_num{ 5 };
	GLint	slice_point_count{};

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagPentagon(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

//========================================================================

class tagFragment : public Basis
{
private:
	GLfloat m_vertex[15][3]{}; // vector ���ؽ�, �÷�, edge, edgenum
	GLfloat m_color[15][3]{};

	Sign    m_sign;
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[15]{};
	GLint    edge_num{};
	GLint	slice_point_count{};

	bool    bBasket{};

public:

	tagFragment(const fPoint& sp, const fPoint& ep, EdgeList* edge_list,
		Sign sign, GLint _edge_num, int ver_st_index, int ver_ed_index, GLfloat* vert, fRGB	_fRGB) : Basis()
	{
		m_fRGB = _fRGB;
		m_sign = sign; edge_num = _edge_num;;
		initVertex(sp, ep, edge_list, ver_st_index, ver_ed_index, vert, edge_num);
		initColor();
		initEdgeList();
		initBuffer();
	}
	void initVertex(const fPoint& sp, const fPoint& ep,
		EdgeList* edge_list, int ver_index, int ver_ed_index, GLfloat* vert, int edge_num);

	tagFragment(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	void FreeFall();
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }
	void BasketMove();
	void CheckBasket();
	void countSlice() { slice_point_count++; }
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

//========================================================================

bool isInRange(float val, float min, float max);
fPoint getIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4);
fPoint checkIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4, float xMin, float yMin, float xMax, float yMax);
int getIndex(const fPoint& end_point, GLfloat* vertex, int vertex_size);
void gFragIntersection();
void gVecIntersection();

void FindIntersection();

#endif
#endif


// 
// ---------------------

// 2023-11-10 
//main
#if 0
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
#endif

// Slice.cpp
#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	gVecRemove();
	gVecPushBack();
	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pTri);

		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pRec);

		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	//	Rotation();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagTriangle::Rotation()
{
	angle += 1.f;

	fPoint pivot = this->getfPivot();
	GLfloat dx1 = m_vertex[0][0] - pivot.x;
	GLfloat dy1 = m_vertex[0][1] - pivot.y;
	float radius = sqrt(dx1 * dx1 + dy1 * dy1);

	m_vertex[0][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[0][1] = pivot.y + radius * glm::sin(glm::radians(angle));

	GLfloat dx2 = m_vertex[1][0] - pivot.x;
	GLfloat dy2 = m_vertex[1][1] - pivot.y;
	radius = sqrt(dx2 * dx2 + dy2 * dy2);

	m_vertex[1][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[1][1] = pivot.y - radius * glm::sin(glm::radians(angle));

	GLfloat dx3 = m_vertex[2][0] - pivot.x;
	GLfloat dy3 = m_vertex[2][1] - pivot.y;
	radius = sqrt(dx3 * dx3 + dy3 * dy3);

	m_vertex[2][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[2][1] = pivot.y - radius * glm::sin(glm::radians(angle));
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}


//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}




#endif


#endif

// Slice.h
#if 0

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;
	GLfloat slope;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
};

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03 };
	float m_veloY{ 0.05 };

	float angle{ 0.0f };
	float rotationSpeed{ 1.0f };


public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void Rotation();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03 };
	float m_veloY{ 0.05 };

	EdgeList edge[4]{};
	int		 num_edge{ 5 };

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03 };
	float m_veloY{ 0.05 };

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

#endif
#endif


// ���� ���ϱ� ���� -> ��������Ʈ�� �ٱ��� �˻�
#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) {
		gSlicing->Draw();



	}
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	//	gVecRemove();
	//	gVecPushBack();
	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pTri);

		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pRec);

		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();

	fPoint sp = { -0.1, 0.1 };
	fPoint ep = { 0.1, 0.1 }; // ���� ����Ʈ�� ����

	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	tagFragment* temp = dynamic_cast<tagFragment*>(gVec[0]);

	fPoint intersection = checkIntersection(sp, ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);

	if (intersection.x == 0.0 && intersection.y == 0.0) {
		std::cout << "Lines do not intersect within the given range or one of the lines is out of range." << std::endl;
	}
	else {
		std::cout << "Lines intersect within the given range. Intersection point: (" << intersection.x << ", " << intersection.y << ")" << std::endl;
	}
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	//	Rotation();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagTriangle::Rotation()
{
	angle += 1.f;

	fPoint pivot = this->getfPivot();
	GLfloat dx1 = m_vertex[0][0] - pivot.x;
	GLfloat dy1 = m_vertex[0][1] - pivot.y;
	float radius = sqrt(dx1 * dx1 + dy1 * dy1);

	m_vertex[0][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[0][1] = pivot.y + radius * glm::sin(glm::radians(angle));

	GLfloat dx2 = m_vertex[1][0] - pivot.x;
	GLfloat dy2 = m_vertex[1][1] - pivot.y;
	radius = sqrt(dx2 * dx2 + dy2 * dy2);

	m_vertex[1][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[1][1] = pivot.y - radius * glm::sin(glm::radians(angle));

	GLfloat dx3 = m_vertex[2][0] - pivot.x;
	GLfloat dy3 = m_vertex[2][1] - pivot.y;
	radius = sqrt(dx3 * dx3 + dy3 * dy3);

	m_vertex[2][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[2][1] = pivot.y - radius * glm::sin(glm::radians(angle));
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}


//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}
//========================================================================

bool tagFragment::isGone()
{
	return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 1.0f;
		m_color[i][1] = 1.0f;
		m_color[i][2] = 0.0f;
	}
}

void tagFragment::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";

}

void tagFragment::initEdgeList()
{

}

//========================================================================


bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint p1_start, fPoint p1_end, fPoint p2_start, fPoint p2_end) {
	double det = (p1_start.x - p1_end.x) * (p2_start.y - p2_end.y) - (p1_start.y - p1_end.y) * (p2_start.x - p2_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((p1_start.x * p1_end.y - p1_start.y * p1_end.x) * (p2_start.x - p2_end.x) - (p1_start.x - p1_end.x) * (p2_start.x * p2_end.y - p2_start.y * p2_end.x)) / det;
	float y = ((p1_start.x * p1_end.y - p1_start.y * p1_end.x) * (p2_start.y - p2_end.y) - (p1_start.y - p1_end.y) * (p2_start.x * p2_end.y - p2_start.y * p2_end.x)) / det;

	fPoint intersection = { x, y };
	return intersection;
}

fPoint checkIntersection(fPoint p1_start, fPoint p1_end, fPoint p2_start, fPoint p2_end, float xMin, float yMin, float xMax, float yMax) {
	cout << "edge sp,ep: " << p1_start.x << ", " << p1_start.y << ", " << p1_end.x << ", " << p1_end.y << '\n';
	cout << "mouse sp,ep  : " << p2_start.x << ", " << p2_start.y << ", " << p2_end.x << ", " << p2_end.y << '\n';
	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(p1_start.x, xMin, xMax) || !isInRange(p1_start.y, yMin, yMax) || !isInRange(p1_end.x, xMin, xMax) || !isInRange(p1_end.y, yMin, yMax) ||
		!isInRange(p2_start.x, xMin, xMax) || !isInRange(p2_start.y, yMin, yMax) || !isInRange(p2_end.x, xMin, xMax) || !isInRange(p2_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(p1_start, p1_end, p2_start, p2_end);
	return intersection;
}

#if 1 
// �� �� ������ ����� ��ġ�� Ȯ���ϴ� �Լ�
double direction(fPoint pi, fPoint pj, fPoint pk) {
	return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
}

// �� ������ �����ϰų� ���� �������� Ȯ���ϴ� �Լ�
bool areLinesIntersectingOrTouching(EdgeList line1, EdgeList line2) {

	double d1 = direction(line2.sp, line2.ep, line1.sp);
	double d2 = direction(line2.sp, line2.ep, line1.ep);
	double d3 = direction(line1.sp, line1.ep, line2.sp);
	double d4 = direction(line1.sp, line1.ep, line2.ep);

	// ���� �ٸ� ���⿡ �ִ� ��� ���� �Ǵ� ����
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
		return true;
	}

	// ���� �ٸ� ���⿡ �ִ� ���, �ϳ��� ������ �ٸ� ������ ���弱 ���� ������ ����
	if ((d1 == 0 && direction(line1.sp, line1.ep, line2.sp) == 0) ||
		(d2 == 0 && direction(line1.sp, line1.ep, line2.ep) == 0) ||
		(d3 == 0 && direction(line2.sp, line2.ep, line1.sp) == 0) ||
		(d4 == 0 && direction(line2.sp, line2.ep, line1.ep) == 0)) {
		return true;
	}

	return false; // ���������� �ʰ� ���� �������� ����
}

#endif

#if 0
#include <iostream>

struct Point {
	double x;
	double y;
};

bool isInRange(double val, double min, double max) {
	return (val >= min && val <= max);
}

Point getIntersection(Point p1, Point p2, Point p3, Point p4) {
	double det = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	double x = ((p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x)) / det;
	double y = ((p1.x * p2.y - p1.y * p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x * p4.y - p3.y * p4.x)) / det;

	Point intersection = { x, y };
	return intersection;
}

Point checkIntersection(Point p1, Point p2, Point p3, Point p4, double xMin, double yMin, double xMax, double yMax) {
	if (!isInRange(p1.x, xMin, xMax) || !isInRange(p1.y, yMin, yMax) || !isInRange(p2.x, xMin, xMax) || !isInRange(p2.y, yMin, yMax) ||
		!isInRange(p3.x, xMin, xMax) || !isInRange(p3.y, yMin, yMax) || !isInRange(p4.x, xMin, xMax) || !isInRange(p4.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	Point intersection = getIntersection(p1, p2, p3, p4);
	return intersection;
}

int main() {
	Point p1, p2, p3, p4; // Line 1 and Line 2 points
	double xMin, yMin, xMax, yMax; // Range boundaries

	std::cout << "Enter start and end points of first line (x1, y1, x2, y2): ";
	std::cin >> p1.x >> p1.y >> p2.x >> p2.y;

	std::cout << "Enter start and end points of second line (x1, y1, x2, y2): ";
	std::cin >> p3.x >> p3.y >> p4.x >> p4.y;

	std::cout << "Enter x, y range boundaries (xMin, yMin, xMax, yMax): ";
	std::cin >> xMin >> yMin >> xMax >> yMax;

	Point intersection = checkIntersection(p1, p2, p3, p4, xMin, yMin, xMax, yMax);

	if (intersection.x == 0 && intersection.y == 0) {
		std::cout << "Lines do not intersect within the given range or one of the lines is out of range." << std::endl;
	}
	else {
		std::cout << "Lines intersect within the given range. Intersection point: (" << intersection.x << ", " << intersection.y << ")" << std::endl;
	}

	return 0;
}

#endif

#endif


#endif


// 21:06 ���� ����Ʈ�� �˻� �Ϸ� 
#if 0
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

	fPoint pivot{ -0.3, 0.0f };
	tagTriangle* frg = new tagTriangle{ pivot };

	gVec.push_back(frg);

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
#endif

#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	//	gVecRemove();
	//	gVecPushBack();
	//	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		//		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pTri);

		//		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pRec);

		//		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	for (auto iter = gVec.begin(); iter != gVec.end(); ++iter) {

		EdgeList* edgelist = (*iter)->getEdgeList();
		GLint num = (*iter)->getEdgeNum();

		for (int i = 0; i < num; ++i) {
			cout << i + 1 << "��°: ";
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);
			if (intersection.x == 0.0 && intersection.y == 0.0) {
				std::cout << "\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
			else {
				std::cout << "�浹 Intersection point: (" << intersection.x << ", " << intersection.y << ")\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
		}
	}
	/*



	tagFragment* temp = dynamic_cast<tagFragment*>(gVec[0]);
	EdgeList* edgelist = temp->getEdgeList();

	for (int i = 0; i < 4; ++i) {
		cout << i << "��°: ";
		fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);
		if (intersection.x == 0.0 && intersection.y == 0.0) {
			std::cout << "Lines do not intersect within the given range or one of the lines is out of range.\n" << std::endl;
			cout << "----------------------------------------------------------------------------------------------------\n";
		}
		else {
			std::cout << "Lines intersect within the given range. Intersection point: (" << intersection.x << ", " << intersection.y << ")\n" << std::endl;
			cout << "----------------------------------------------------------------------------------------------------\n";
		}
	}*/
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x - 0.5; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x - 0.5; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y - 0.5;
	else if (sp.y > ep.y)
		minXY.y = ep.y - 0.5;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x + 0.5;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x + 0.5;

	if (sp.y < ep.y)
		maxXY.y = ep.y + 0.5;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y + 0.5;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	//	Rotation();
	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagTriangle::Rotation()
{
	angle += 1.f;

	fPoint pivot = this->getfPivot();
	GLfloat dx1 = m_vertex[0][0] - pivot.x;
	GLfloat dy1 = m_vertex[0][1] - pivot.y;
	float radius = sqrt(dx1 * dx1 + dy1 * dy1);

	m_vertex[0][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[0][1] = pivot.y + radius * glm::sin(glm::radians(angle));

	GLfloat dx2 = m_vertex[1][0] - pivot.x;
	GLfloat dy2 = m_vertex[1][1] - pivot.y;
	radius = sqrt(dx2 * dx2 + dy2 * dy2);

	m_vertex[1][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[1][1] = pivot.y - radius * glm::sin(glm::radians(angle));

	GLfloat dx3 = m_vertex[2][0] - pivot.x;
	GLfloat dy3 = m_vertex[2][1] - pivot.y;
	radius = sqrt(dx3 * dx3 + dy3 * dy3);

	m_vertex[2][0] = pivot.x + radius * glm::cos(glm::radians(angle));
	m_vertex[2][1] = pivot.y - radius * glm::sin(glm::radians(angle));
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagTriangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "0 : sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "1: sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[0][0];
	edge[2].ep.y = m_vertex[0][1];
	//	cout << "2: sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";
}

void tagTriangle::initEdgeList()
{

}
//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}

void tagRectangle::initEdgeList()
{

}

//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagPentagon::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[7][0];
	edge[0].ep.y = m_vertex[7][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[7][0];
	edge[1].sp.y = m_vertex[7][1];
	edge[1].ep.x = m_vertex[8][0];
	edge[1].ep.y = m_vertex[8][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[8][0];
	edge[2].sp.y = m_vertex[8][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[2][0];
	edge[3].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
	edge[4].sp.x = m_vertex[2][0];
	edge[4].sp.y = m_vertex[2][1];
	edge[4].ep.x = m_vertex[0][0];
	edge[4].ep.y = m_vertex[0][1];
}

void tagPentagon::initEdgeList()
{

}
//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}
//========================================================================

bool tagFragment::isGone()
{
	return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 1.0f;
		m_color[i][1] = 1.0f;
		m_color[i][2] = 0.0f;
	}
}

void tagFragment::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";

}

void tagFragment::initEdgeList()
{

}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			minXY.x = sp.x - 0.005f;
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

	//	cout << "edge minx,miny : " << minXY.x << ", " << minXY.y << '\n';
	return minXY;
}

fPoint getMaxXY(fPoint sp, fPoint ep)
{
	fPoint maxXY{};

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	//	cout << "edge maxx,maxy : " << maxXY.x << ", " << maxXY.y << '\n';

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	float det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;

	fPoint intersection = { x, y };
	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';

	if (isInRange(x, getMinXY(edge_start, edge_end).x, getMaxXY(edge_start, edge_end).x) && isInRange(y, getMinXY(edge_start, edge_end).y, getMaxXY(edge_start, edge_end).y)) {
		return intersection; // Out of range or no intersection
	}
	else
		return { 0,0 };
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {
	//	cout << "edge sp,ep: " << edge_start.x << ", " << edge_start.y << ", " << edge_end.x << ", " << edge_end.y << '\n';
	//	cout << "mouse sp,ep  : " << slice_start.x << ", " << slice_start.y << ", " << slice_end.x << ", " << slice_end.y << '\n';
	//	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}



#endif



//fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
//	double det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);
//
//	if (det == 0) {
//		return { 0, 0 }; // No intersection
//	}
//
//	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//
//	fPoint intersection = { x, y };
//	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';
//	cout << "fmin/max X : " << fmin(edge_start.x, edge_end.x) << ", " << fmax(edge_start.x, edge_end.x) << '\n';
//	cout << "fmin/max Y: " << fmin(edge_start.y, edge_end.y) << ", " << fmax(edge_start.y, edge_end.y) << '\n';
//
//
//	//	if (x >= fmin(edge_start.x, edge_end.x) && x <= fmax(edge_start.x, edge_end.x) &&
//	//		y >= fmin(edge_start.y, edge_end.y) && y <= fmax(edge_start.y, edge_end.y)) {
//	return intersection;  // �� ������ ������
//	//	}
//	//	else
//	//		return { 0,0 };
//}

#endif

#if 0

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;

	fPoint min_range;
	fPoint max_range;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual EdgeList* getEdgeList() = 0;
	virtual GLint getEdgeNum() = 0;

	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
	fPoint getMinXYPoint();
	fPoint getMaxXYPoint();
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
};

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};

	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	float angle{ 0.0f };
	float rotationSpeed{ 1.0f };

	EdgeList edge[3]{};
	GLint    edge_num{ 3 };
	GLint	slice_point_count{};

public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagTriangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void Rotation();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagRectangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[5]{};
	GLint    edge_num{ 5 };
	GLint	slice_point_count{};

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagPentagon(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;

};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }

};

class tagFragment : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:

	tagFragment(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

//========================================================================

bool isInRange(float val, float min, float max);
fPoint getIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4);
fPoint checkIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4, float xMin, float yMin, float xMax, float yMax);

void FindIntersection();

#endif
#endif

// 04:04���� �����̽� - ���� ��ġ�� �˸� �� 
#if 0

#endif

#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;
extern vector<Basis*> gFrag;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	//	gVecRemove();
	//	gVecPushBack();
	//	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		//		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pTri);

		//		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pRec);

		//		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();

	for (auto& obj : gFrag) {
		delete obj;
	}
	gFrag.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}

	for (auto& iter : gFrag) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();

	for (auto& element : gFrag)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	fPoint slice_point[2]{};
	int slice_index{};
	EdgeList* list{};
	GLint edge_num{};

	for (auto iter = gVec.begin(); iter != gVec.end(); ++iter) {

		(*iter)->initCount();
		EdgeList* edgelist = (*iter)->getEdgeList();
		edge_num = (*iter)->getEdgeNum();

		for (int i = 0; i < edge_num; ++i) {
			cout << i << "��° [Edge index] : ";
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);
			if (intersection.x == 0.0 && intersection.y == 0.0) {
				std::cout << "\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
			else {
				list = edgelist;
				(*iter)->countSlice();
				slice_point[slice_index].x = intersection.x;
				slice_point[slice_index].y = intersection.y;
				slice_index++;
				std::cout << "�浹 Intersection point: (" << intersection.x << ", " << intersection.y << ")\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
		}
	}

	if (slice_index == 2) {
		// edge num -> �� �������� ���� [n * 3] 
		// sp ���� �� ��ġ  
		cout << "�����׸�Ʈ ����\n";
		tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num };
		gFrag.push_back(frag);

		tagFragment* frag2 = new tagFragment{ slice_point[1], slice_point[0], list, MINUS, edge_num };
		gFrag.push_back(frag2);
	}
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x - 0.5; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x - 0.5; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y - 0.5;
	else if (sp.y > ep.y)
		minXY.y = ep.y - 0.5;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x + 0.5;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x + 0.5;

	if (sp.y < ep.y)
		maxXY.y = ep.y + 0.5;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y + 0.5;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagTriangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "0 : sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "1: sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[0][0];
	edge[2].ep.y = m_vertex[0][1];
	//	cout << "2: sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";
}

void tagTriangle::initEdgeList()
{

}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}

void tagRectangle::initEdgeList()
{

}

//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagPentagon::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[7][0];
	edge[0].ep.y = m_vertex[7][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[7][0];
	edge[1].sp.y = m_vertex[7][1];
	edge[1].ep.x = m_vertex[8][0];
	edge[1].ep.y = m_vertex[8][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[8][0];
	edge[2].sp.y = m_vertex[8][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[2][0];
	edge[3].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
	edge[4].sp.x = m_vertex[2][0];
	edge[4].sp.y = m_vertex[2][1];
	edge[4].ep.x = m_vertex[0][0];
	edge[4].ep.y = m_vertex[0][1];
}

void tagPentagon::initEdgeList()
{

}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}

//========================================================================

bool tagFragment::isGone()
{
	if (m_vertex[0][1] <= -1.5f)
		return true;
	else
		return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	FreeFall();
	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < 4; ++i) {
		m_color[i][0] = 1.0f;
		m_color[i][1] = gColorUniform(gRandomEngine);
		m_color[i][2] = 0.0f;
	}
}

void tagFragment::UpdateEdgeList()
{
	m_vertex[0][0] = m_vertex[1][0];
	m_vertex[0][1] = m_vertex[1][1];

	m_vertex[1][0] = m_vertex[2][0];
	m_vertex[1][1] = m_vertex[2][1];

	m_vertex[2][0] = m_vertex[3][0];
	m_vertex[2][1] = m_vertex[3][1];
	// ------------------------------------------------------
	m_vertex[3][0] = m_vertex[0][0];
	m_vertex[3][1] = m_vertex[0][1];
}

void tagFragment::initEdgeList()
{

}

void tagFragment::initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list)
{
	m_vertex[0][0] = sp.x;
	m_vertex[0][1] = sp.y;

	m_vertex[1][0] = edge_list[1].sp.x;
	m_vertex[1][1] = edge_list[1].sp.y;

	m_vertex[2][0] = edge_list[2].sp.x;
	m_vertex[2][1] = edge_list[2].sp.y;
	// ------------------------------------------------------
	m_vertex[3][0] = ep.x;
	m_vertex[3][1] = ep.y;


}

void tagFragment::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;

	m_vertex[0][0] += m_veloX * gTimeStep * m_sign;
	m_vertex[0][1] += m_veloY * gTimeStep;

	m_vertex[1][0] += m_veloX * gTimeStep * m_sign;
	m_vertex[1][1] += m_veloY * gTimeStep;

	m_vertex[2][0] += m_veloX * gTimeStep * m_sign;
	m_vertex[2][1] += m_veloY * gTimeStep;
	// ------------------------------------------------------
	m_vertex[3][0] += m_veloX * gTimeStep * m_sign;
	m_vertex[3][1] += m_veloY * gTimeStep;
}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			minXY.x = sp.x - 0.005f;
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

	//	cout << "edge minx,miny : " << minXY.x << ", " << minXY.y << '\n';
	return minXY;
}

fPoint getMaxXY(fPoint sp, fPoint ep)
{
	fPoint maxXY{};

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	//	cout << "edge maxx,maxy : " << maxXY.x << ", " << maxXY.y << '\n';

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	float det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;

	fPoint intersection = { x, y };
	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';

	if (isInRange(x, getMinXY(edge_start, edge_end).x, getMaxXY(edge_start, edge_end).x) && isInRange(y, getMinXY(edge_start, edge_end).y, getMaxXY(edge_start, edge_end).y)) {
		return intersection; // Out of range or no intersection
	}
	else
		return { 0,0 };
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {
	//	cout << "edge sp,ep: " << edge_start.x << ", " << edge_start.y << ", " << edge_end.x << ", " << edge_end.y << '\n';
	//	cout << "mouse sp,ep  : " << slice_start.x << ", " << slice_start.y << ", " << slice_end.x << ", " << slice_end.y << '\n';
	//	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}



#endif



//fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
//	double det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);
//
//	if (det == 0) {
//		return { 0, 0 }; // No intersection
//	}
//
//	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//
//	fPoint intersection = { x, y };
//	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';
//	cout << "fmin/max X : " << fmin(edge_start.x, edge_end.x) << ", " << fmax(edge_start.x, edge_end.x) << '\n';
//	cout << "fmin/max Y: " << fmin(edge_start.y, edge_end.y) << ", " << fmax(edge_start.y, edge_end.y) << '\n';
//
//
//	//	if (x >= fmin(edge_start.x, edge_end.x) && x <= fmax(edge_start.x, edge_end.x) &&
//	//		y >= fmin(edge_start.y, edge_end.y) && y <= fmax(edge_start.y, edge_end.y)) {
//	return intersection;  // �� ������ ������
//	//	}
//	//	else
//	//		return { 0,0 };
//}

#endif

#if 0 

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;

	fPoint min_range;
	fPoint max_range;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual EdgeList* getEdgeList() = 0;
	virtual GLint getEdgeNum() = 0;
	virtual void initCount() = 0;
	virtual void countSlice() = 0;

	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
	fPoint getMinXYPoint();
	fPoint getMaxXYPoint();
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }

};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
};

//========================================================================

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};

	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[3]{};
	GLint    edge_num{ 3 };
	GLint	slice_point_count{};

public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagTriangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void countSlice() { slice_point_count++; }

	void initCount() { slice_point_count = 0; }
};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagRectangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;

	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }

};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[5]{};
	GLint    edge_num{ 5 };
	GLint	slice_point_count{};

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagPentagon(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }

};

//========================================================================

class tagFragment : public Basis
{
private:
	GLfloat m_vertex[4][3]{}; // vector ���ؽ�, �÷�, edge, edgenum
	GLfloat m_color[4][3]{};
	Sign    m_sign;
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:

	tagFragment(const fPoint& sp, const fPoint& ep, EdgeList* edge_list, Sign sign, GLint _edge_num) : Basis()
	{
		m_sign = sign;
		edge_num = _edge_num;;
		initVertex(sp, ep, edge_list);
		initColor();
		initEdgeList();
		initBuffer();
	}
	void initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list);

	tagFragment(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void FreeFall();
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

//========================================================================

bool isInRange(float val, float min, float max);
fPoint getIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4);
fPoint checkIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4, float xMin, float yMin, float xMax, float yMax);

void FindIntersection();

#endif
#endif

// 16:04 
#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;
extern vector<Basis*> gFrag;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	//	gVecRemove();
	//	gVecPushBack();
	//	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		//		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pTri);

		//		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pRec);

		//		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		//		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();

	for (auto& obj : gFrag) {
		delete obj;
	}
	gFrag.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}

	for (auto& iter : gFrag) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();

	for (auto& element : gFrag)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	for (auto iter = gVec.begin(); iter != gVec.end(); ++iter) {
		fPoint slice_point[2]{};
		int slice_index{};
		EdgeList* list{};
		GLint edge_num{};
		GLint ver_num{};
		GLfloat* vert{};

		int polygon_index[2]{};
		int index_cnt{};

		(*iter)->initCount();
		EdgeList* edgelist = (*iter)->getEdgeList();
		edge_num = (*iter)->getEdgeNum();
		ver_num = (*iter)->getVerNum();


		for (int i = 0; i < edge_num; ++i) {
			cout << i << "��° [Edge index] : ";
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);

			if (intersection.x == 0.0 && intersection.y == 0.0) {
				std::cout << "\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
			else {
				list = edgelist;
				(*iter)->countSlice();
				vert = (*iter)->getVertex();
				slice_point[slice_index].x = intersection.x;
				slice_point[slice_index].y = intersection.y;
				slice_index++;

				// �̰� ���� ���� ��ġ 
				polygon_index[index_cnt++] = getIndex(edgelist[i].ep, vert, ver_num);

				if (slice_index == 2) {
					// edge num -> �� �������� ���� [n * 3] 
					// sp ���� �� ��ġ  
					cout << "�����׸�Ʈ ����\n";
					tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num, polygon_index[0],  polygon_index[1], vert };
					gFrag.push_back(frag);

					//		tagFragment* frag2 = new tagFragment{slice_point[1], slice_point[0], list, MINUS, edge_num, polygon_index[1], polygon_index[0], vert};
					//		gFrag.push_back(frag2);
				}

				std::cout << "�浹 Intersection point: (" << intersection.x << ", " << intersection.y << ")\n" << std::endl;
				cout << "----------------------------------------------------------------------------------------------------\n";
			}
		}
	}
}

int getIndex(const fPoint& edge_end_point, GLfloat* vertex, int vertex_size)
{
	int index{};

	for (int i = 0; i < vertex_size; ++i) {

		GLfloat* current = &vertex[i * 3];

		cout << "end_x, end_y : " << edge_end_point.x << ", " << edge_end_point.y << '\n';
		cout << "cur_x, cur_y : " << current[0] << ", " << current[1] << '\n';

		if (edge_end_point.x == current[0] && edge_end_point.y == current[1]) { // ���� ����� 1���� -> �ε� �Ҽ� == 
			cout << "ã��\n";
			//assert(nullptr);
			break;
		}

		index = index + 3;
	}

	index /= 3; // ŵ

	return index;
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x - 0.5; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x - 0.5; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y - 0.5;
	else if (sp.y > ep.y)
		minXY.y = ep.y - 0.5;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x + 0.5;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x + 0.5;

	if (sp.y < ep.y)
		maxXY.y = ep.y + 0.5;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y + 0.5;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagTriangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "0 : sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "1: sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[0][0];
	edge[2].ep.y = m_vertex[0][1];
	//	cout << "2: sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";
}

void tagTriangle::initEdgeList()
{

}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}

void tagRectangle::initEdgeList()
{

}

//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagPentagon::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[7][0];
	edge[0].ep.y = m_vertex[7][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[7][0];
	edge[1].sp.y = m_vertex[7][1];
	edge[1].ep.x = m_vertex[8][0];
	edge[1].ep.y = m_vertex[8][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[8][0];
	edge[2].sp.y = m_vertex[8][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[2][0];
	edge[3].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
	edge[4].sp.x = m_vertex[2][0];
	edge[4].sp.y = m_vertex[2][1];
	edge[4].ep.x = m_vertex[0][0];
	edge[4].ep.y = m_vertex[0][1];
}

void tagPentagon::initEdgeList()
{

}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}

//========================================================================

bool tagFragment::isGone()
{
	if (m_vertex[0][1] <= -1.5f)
		return true;
	else
		return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	FreeFall();
	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);

}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLE_FAN, 0, edge_num);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < edge_num; ++i) {
		m_color[i][0] = 1.0f;
		m_color[i][1] = gColorUniform(gRandomEngine);
		m_color[i][2] = 0.0f;
	}
}

void tagFragment::UpdateEdgeList()
{
	// ����
	int i = 0;
	for (; i < edge_num - 1; ++i) {

		m_vertex[i][0] = m_vertex[i + 1][0];
		m_vertex[i][1] = m_vertex[i + 1][1];

	}
	m_vertex[edge_num - 1][0] = m_vertex[0][0];
	m_vertex[edge_num - 1][1] = m_vertex[0][1];

}

void tagFragment::initEdgeList()
{
	int i{ 0 };

	while (1)
	{
		if (m_vertex[i][0] == 0. && m_vertex[i][1] == 0.f) {
			break;
		}

		m_vertex[i][0] = m_vertex[i + 1][0];
		m_vertex[i][1] = m_vertex[i + 1][1];

		i++;
		edge_num++;
	}
	edge_num /= 2;
	m_vertex[i][0] = m_vertex[0][0];
	m_vertex[i][1] = m_vertex[0][1];
}

int getEdgeStart(EdgeList* el, GLfloat* vert, int ver_st_index, int edge_num)
{
	int idx{};
	for (; idx < edge_num; ++idx) {
		if (el[idx].ep.x == vert[ver_st_index * 3] && el[idx].ep.y == vert[ver_st_index * 3 + 1]) {
			return idx;
		}
	}
	return idx;
}

void tagFragment::initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list, int ver_st_index, int ver_ed_index, GLfloat* vert, int edge_num)
{
	m_vertex[0][0] = sp.x;
	m_vertex[0][1] = sp.y;

	int indx{ 1 };
	int edge_idx = getEdgeStart(edge_list, vert, ver_st_index, edge_num);

	while (edge_idx < edge_num)
	{
		//		if (  vert[ver_st_index * 3] == vert[ver_ed_index * 3] && vert[ver_st_index * 3 + 1] == vert[ver_ed_index * 3 + 1]) { // �̰Ŵ� Rec�� 4���� -> 5���� �ǳ�
		//			break;
		//		}

		if (edge_list[edge_idx].ep.x == vert[ver_ed_index * 3] && edge_list[edge_idx].ep.y == vert[ver_ed_index * 3 + 1])
			break;

		m_vertex[indx][0] = edge_list[edge_idx].ep.x;
		m_vertex[indx][1] = edge_list[edge_idx].ep.y;

		edge_idx++;
		indx++;
		ver_st_index++;
	}
	m_vertex[indx][0] = ep.x;
	m_vertex[indx][1] = ep.y;


	//	m_vertex[indx][0] = vert[ver_st_index * 3];
	//	m_vertex[indx][1] = vert[ver_st_index * 3 + 1];
}

void tagFragment::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;

	for (int i = 0; i < edge_num; ++i) {
		m_vertex[i][0] += m_veloX * gTimeStep * m_sign;
		m_vertex[i][1] += m_veloY * gTimeStep;
	}
}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			minXY.x = sp.x - 0.005f;
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

	//	cout << "edge minx,miny : " << minXY.x << ", " << minXY.y << '\n';
	return minXY;
}

fPoint getMaxXY(fPoint sp, fPoint ep)
{
	fPoint maxXY{};

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	//	cout << "edge maxx,maxy : " << maxXY.x << ", " << maxXY.y << '\n';

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	float det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;

	fPoint intersection = { x, y };
	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';

	if (isInRange(x, getMinXY(edge_start, edge_end).x, getMaxXY(edge_start, edge_end).x) && isInRange(y, getMinXY(edge_start, edge_end).y, getMaxXY(edge_start, edge_end).y)) {
		return intersection; // Out of range or no intersection
	}
	else
		return { 0,0 };
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {
	//	cout << "edge sp,ep: " << edge_start.x << ", " << edge_start.y << ", " << edge_end.x << ", " << edge_end.y << '\n';
	//	cout << "mouse sp,ep  : " << slice_start.x << ", " << slice_start.y << ", " << slice_end.x << ", " << slice_end.y << '\n';
	//	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}



#endif



//fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
//	double det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);
//
//	if (det == 0) {
//		return { 0, 0 }; // No intersection
//	}
//
//	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//
//	fPoint intersection = { x, y };
//	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';
//	cout << "fmin/max X : " << fmin(edge_start.x, edge_end.x) << ", " << fmax(edge_start.x, edge_end.x) << '\n';
//	cout << "fmin/max Y: " << fmin(edge_start.y, edge_end.y) << ", " << fmax(edge_start.y, edge_end.y) << '\n';
//
//
//	//	if (x >= fmin(edge_start.x, edge_end.x) && x <= fmax(edge_start.x, edge_end.x) &&
//	//		y >= fmin(edge_start.y, edge_end.y) && y <= fmax(edge_start.y, edge_end.y)) {
//	return intersection;  // �� ������ ������
//	//	}
//	//	else
//	//		return { 0,0 };
//}

#endif

#if 0 

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual EdgeList* getEdgeList() = 0;
	virtual GLint getEdgeNum() = 0;
	virtual void initCount() = 0;
	virtual void countSlice() = 0;
	virtual GLfloat* getVertex() = 0;
	virtual GLint getVerNum() = 0;

	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
	fPoint getMinXYPoint();
	fPoint getMaxXYPoint();
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }
};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

//========================================================================

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};

	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[3]{};
	GLint    edge_num{ 3 };
	GLint	slice_point_count{};

public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagTriangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void countSlice() { slice_point_count++; }

	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagRectangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;

	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[5]{};
	GLint    edge_num{ 5 };
	GLint	slice_point_count{};

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagPentagon(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

//========================================================================

class tagFragment : public Basis
{
private:
	GLfloat m_vertex[15][3]{}; // vector ���ؽ�, �÷�, edge, edgenum
	GLfloat m_color[15][3]{};

	Sign    m_sign;
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[15]{};
	GLint    edge_num{};
	GLint	slice_point_count{};

public:

	tagFragment(const fPoint& sp, const fPoint& ep, EdgeList* edge_list,
		Sign sign, GLint _edge_num, int ver_st_index, int ver_ed_index, GLfloat* vert) : Basis()
	{
		m_sign = sign; edge_num = _edge_num;;
		initVertex(sp, ep, edge_list, ver_st_index, ver_ed_index, vert, edge_num);
		initColor();
		//	initEdgeList();
		initBuffer();
	}
	void initVertex(const fPoint& sp, const fPoint& ep,
		EdgeList* edge_list, int ver_index, int ver_ed_index, GLfloat* vert, int edge_num);

	tagFragment(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	void FreeFall();
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

	void countSlice() { slice_point_count++; }
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

//========================================================================

bool isInRange(float val, float min, float max);
fPoint getIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4);
fPoint checkIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4, float xMin, float yMin, float xMax, float yMax);
int getIndex(const fPoint& end_point, GLfloat* vertex, int vertex_size);

void FindIntersection();

#endif
#endif


// ���Ǵ� �ϼ�
#if 0
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

	fPoint pivot{ -0.3, 0.0f };
	tagRectangle* frg = new tagRectangle{ pivot };

	gVec.push_back(frg);

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
#endif

#if 0

#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // ��¥ ���� �߻��� -> �� ���� �õ尪���� 
std::mt19937 gRandomEngine(gRandDevice()); // �˰��� + ��¥ ���� �õ� :: ��¥��¥ ���� ���� 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- �ҽ��ڵ� ���� ����
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- ���̴� ��ü
extern GLuint gShaderProgramID; //--- ���̴� ���α׷�

extern int gWidth;
extern int gHeight;

extern bool gToggle[(int)Toggle::END];

extern vector<Basis*> gVec;
extern tagBasket* gBasket;
extern tagSliceLine* gSlicing;
extern vector<Basis*> gFrag;

const float gGravity{ 0.0068f };
float gTimeStep{ 0.1f };
bool isUsingSlicing{};

//========================================================================

void ShowMenu()
{
	cout << "=========================================\n";
	cout << "����, ���� ���� ��ġ���� �ݴ뼆���� �缱 ���ƿ�\n";
	cout << "�ٱ��� �¿�� �����̴� ��\n";
	cout << "���콺 Ŭ��, �巡���Ͽ� �����̽��Ѵ�.(���� ������ �Ǿ� �Ʒ��� �ϰ�, �߷�)\n";
	cout << "������ �ٴ��� ���� ���δ�. �����̽� �� �ϸ� �ݴ������� �������.\n";
	cout << "=========================================\n";
	cout << "L: ���� ���: LINE/FILL\n";
	cout << "P: ��� ��� ON/OFF\n";
	cout << "+/- : ���ƿ��� �ӵ� �����ϱ�\n";
	cout << "r : �ʱ�ȭ\n";
	cout << "q: ���α׷� ����\n";
	cout << "=========================================\n";
}
GLvoid drawScene()
{
	glClearColor(0.5294f, 0.8078f, 0.9804f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gVecDraw();
	if (gSlicing) { gSlicing->Draw(); }
	if (gBasket) gBasket->Draw(); else assert(nullptr);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//========================================================================

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();
	gShaderProgramID = glCreateProgram();
	glAttachShader(gShaderProgramID, gVertexShader);
	glAttachShader(gShaderProgramID, gFragmentShader);
	glLinkProgram(gShaderProgramID);
	glDeleteShader(gVertexShader);
	glDeleteShader(gFragmentShader);
	glUseProgram(gShaderProgramID);
}
void make_vertexShaders()
{
	gVertexSource = filetobuf("vertex.glsl");
	gVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShader, 1, (const GLchar**)&gVertexSource, 0);
	glCompileShader(gVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gVertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	gFragmentSource = filetobuf("fragment.glsl");
	gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShader, 1, (const GLchar**)&gFragmentSource, 0);
	glCompileShader(gFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(gFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(gFragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

//========================================================================

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'L': case'l': setPolygonMode(); break;

	case 'p': case 'P': setPolygonPath(); break;

	case '+': gTimeStep *= 1.3f; break;

	case '-': gTimeStep /= 1.3f; break;

	case 'r': case'R':
		gTimeStep = 0.1f;
		gVecClear();
		initToggle();
		resetBasket();
		initBasket();
		resetSlicing();
		break;

	case 'q': case'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (gSlicing) { // ���� ������ ����� + �˻���� �ϰ� ������� 
			cout << "������ �������� ����\n";
			FindIntersection();

			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
			cout << "�����̽� ó�� ���� ����\n";
			isUsingSlicing = TRUE;
			fPoint start_mouse = convertDeviceXYtoOpenGLXY(x, y);
			gSlicing = new tagSliceLine{ start_mouse };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (isUsingSlicing) {
		fPoint end_mouse = convertDeviceXYtoOpenGLXY(x, y);
		gSlicing->setEndPoint(end_mouse);
		gSlicing->UpdatePivot();
	}
}

fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y)
{
	fPoint openGL_coordinate{}; // �������� ��ǥ��
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y���� ����, ������ �߽��� �������� x,y ���ְ�, ���� ũ�⸦ (1/�������߽�) ->�� ���ؼ� �ٿ��ش�. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	//	gVecRemove();
	//	gVecPushBack();
	WorldUpdate();
	glutPostRedisplay();				//ȭ�� �����
	glutTimerFunc(10, TimerFunction, 1); // �ٽ� ȣ�� 
}
//========================================================================

void setOffAllofToggle()
{
	for (auto& toggle_element : gToggle)
		toggle_element = OFF;
}

void initToggle()
{
	gToggle[(int)Toggle::FILL] = ON;
	gToggle[(int)Toggle::LINE] = OFF;
	gToggle[(int)Toggle::PATH] = OFF;
	glLineWidth(3.5);

}

void setPolygonMode()
{
	if (OFF == gToggle[(int)Toggle::LINE]) {
		gToggle[(int)Toggle::LINE] = ON;
		gToggle[(int)Toggle::FILL] = OFF;
	}
	else if (OFF == gToggle[(int)Toggle::FILL]) {
		gToggle[(int)Toggle::LINE] = OFF;
		gToggle[(int)Toggle::FILL] = ON;
	}
}

void setPolygonPath()
{
	if (OFF == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = ON;
	}
	else if (ON == gToggle[(int)Toggle::PATH]) {
		gToggle[(int)Toggle::PATH] = OFF;
	}
}

//========================================================================

void gVecPushBack()
{
	tagTriangle* pTri{};
	tagRectangle* pRec{};
	tagPentagon* pPen{};

	if (gVec.size() <= 5) {
		pTri = new tagTriangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pTri);

		pRec = new tagRectangle{ gSignUniform(gRandomEngine) };
		gVec.push_back(pRec);

		pPen = new tagPentagon{ gSignUniform(gRandomEngine) };
		gVec.push_back(pPen);
	}
}

void gVecClear()
{
	for (auto& obj : gVec) {
		delete obj;
	}
	gVec.clear();

	for (auto& obj : gFrag) {
		delete obj;
	}
	gFrag.clear();
}

void gVecDraw()
{
	for (auto& iter : gVec) {
		iter->Draw();
	}

	for (auto& iter : gFrag) {
		iter->Draw();
	}
}

void initBasket()
{
	fPoint pivot{ 0.0, -0.7f };
	gBasket = new tagBasket{ pivot };
}

void resetBasket()
{
	if (gBasket) {
		delete gBasket;
		cout << "�ٱ��� ����\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "���� ����\n";
	}
}

void gVecUpdate()
{
	for (auto& element : gVec)
		element->UpdatePivot();

	for (auto& element : gFrag)
		element->UpdatePivot();
}

void WorldUpdate()
{
	gBasket->UpdatePivot();
	gVecUpdate();
}

void gVecRemove()
{
	for (auto it = gVec.begin(); it != gVec.end(); ) {
		if ((*it)->isGone()) {
			//			cout << "Y:" << (*it)->getfPivot().y << "�� �Ҹ�Ǿ����ϴ�\n";
			delete* it;
			it = gVec.erase(it);
		}
		else {
			++it;
		}
	}
}

void FindIntersection()
{
	fPoint mouse_sp = gSlicing->getStartPoint();
	fPoint mouse_ep = gSlicing->getEndPoint();
	fPoint min{ gSlicing->getMinXYPoint() };
	fPoint max{ gSlicing->getMaxXYPoint() };

	for (auto iter = gVec.begin(); iter != gVec.end(); ++iter) {
		fPoint slice_point[2]{};
		int slice_index{};
		EdgeList* list{};
		GLint edge_num{};
		GLint ver_num{};
		GLfloat* vert{};

		int polygon_index[2]{};
		int index_cnt{};

		(*iter)->initCount();
		EdgeList* edgelist = (*iter)->getEdgeList();
		edge_num = (*iter)->getEdgeNum();
		ver_num = (*iter)->getVerNum();


		for (int i = 0; i < edge_num; ++i) {
			fPoint intersection = checkIntersection(edgelist[i].sp, edgelist[i].ep, mouse_sp, mouse_ep, min.x, min.y, max.x, max.y);

			if (intersection.x == 0.0 && intersection.y == 0.0) {
			}
			else {
				list = edgelist;
				(*iter)->countSlice();
				vert = (*iter)->getVertex();
				slice_point[slice_index].x = intersection.x;
				slice_point[slice_index].y = intersection.y;
				slice_index++;

				polygon_index[index_cnt++] = getIndex(edgelist[i].ep, vert, ver_num);

				if (slice_index == 2) {
					tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num, polygon_index[0],  polygon_index[1], vert };
					gFrag.push_back(frag);

					tagFragment* frag2 = new tagFragment{ slice_point[1], slice_point[0], list, MINUS, edge_num, polygon_index[1], polygon_index[0], vert };
					gFrag.push_back(frag2);
				}
			}
		}
	}
}

int getIndex(const fPoint& edge_end_point, GLfloat* vertex, int vertex_size)
{
	int index{};
	for (int i = 0; i < vertex_size; ++i) {

		GLfloat* current = &vertex[i * 3];
		if (edge_end_point.x == current[0] && edge_end_point.y == current[1]) { // ���� ����� 1���� -> �ε� �Ҽ� == 
			break;
		}
		index = index + 3;
	}

	index /= 3; // ŵ
	return index;
}

//========================================================================

void Basis::initfRGB()
{
	this->m_fRGB.fRed = gColorUniform(gRandomEngine);
	this->m_fRGB.fGreen = gColorUniform(gRandomEngine);
	this->m_fRGB.fBlue = gColorUniform(gRandomEngine);
}

//========================================================================

bool tagSliceLine::isGone()
{
	return false;
}

void tagSliceLine::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagSliceLine::UpdateVertex()
{
	m_vertex[0][0] = m_start.x;
	m_vertex[0][1] = m_start.y;

	m_vertex[1][0] = m_end.x;
	m_vertex[1][1] = m_end.y;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagSliceLine::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagSliceLine::UpdatePivot()
{

	UpdateVertex();
}

void tagSliceLine::initColor()
{
	m_color[0][0] = 1.0;
	m_color[0][1] = 0;
	m_color[0][2] = 0;

	m_color[1][0] = 0;
	m_color[1][1] = 0;
	m_color[1][2] = 0;
}

fPoint tagSliceLine::getMinXYPoint()
{
	fPoint minXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x <= ep.x)
		minXY.x = sp.x - 0.5; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x - 0.5; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y - 0.5;
	else if (sp.y > ep.y)
		minXY.y = ep.y - 0.5;


	return minXY;
}

fPoint tagSliceLine::getMaxXYPoint()
{
	fPoint maxXY{};

	fPoint sp{ getStartPoint() };
	fPoint ep{ getEndPoint() };

	if (sp.x < ep.x)
		maxXY.x = ep.x + 0.5;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x + 0.5;

	if (sp.y < ep.y)
		maxXY.y = ep.y + 0.5;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y + 0.5;

	return maxXY;
}

//========================================================================

void tagTriangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagTriangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x;         // ù ��° ���� (�ǹ� ��ġ)
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ������

	m_vertex[1][0] = pivot.x + m_x_radius;  // ������ ������
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ������
	m_vertex[2][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagTriangle::UpdatePivot()
{
	//	Rotation();
	FreeFall();
	UpdateVertex();
}

void tagTriangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagTriangle::initColor()
{
	m_color[0][0] = getfRGB().fRed;
	m_color[0][1] = getfRGB().fGreen;
	m_color[0][2] = getfRGB().fBlue;

	m_color[1][0] = getfRGB().fRed;
	m_color[1][1] = getfRGB().fGreen;
	m_color[1][2] = getfRGB().fBlue;

	m_color[2][0] = getfRGB().fRed;
	m_color[2][1] = getfRGB().fGreen;
	m_color[2][2] = getfRGB().fBlue;
}

void tagTriangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagTriangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagTriangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

bool tagTriangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagTriangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "0 : sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "1: sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[0][0];
	edge[2].ep.y = m_vertex[0][1];
	//	cout << "2: sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";
}

void tagTriangle::initEdgeList()
{

}

//========================================================================

bool tagRectangle::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagRectangle::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagRectangle::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagRectangle::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagRectangle::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagRectangle::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagRectangle::UpdatePivot()
{
	//	FreeFall();
	UpdateVertex();
}

void tagRectangle::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagRectangle::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagRectangle::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[1][0];
	edge[0].ep.y = m_vertex[1][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[1][0];
	edge[1].sp.y = m_vertex[1][1];
	edge[1].ep.x = m_vertex[2][0];
	edge[1].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[2][0];
	edge[2].sp.y = m_vertex[2][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[0][0];
	edge[3].ep.y = m_vertex[0][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
}

void tagRectangle::initEdgeList()
{

}

//========================================================================

bool tagPentagon::isGone()
{
	if (m_fpivot.y + m_y_radius <= -1.5f)
		return true;
	else
		return false;
}

void tagPentagon::setSign(int sign)
{
	if (sign == 1)
		m_sign = PLUS;
	else if (sign == 0)
		m_sign = MINUS;
}

void tagPentagon::initFallvalue()
{
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // ȭ�� ���� 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // ȭ�� ������
	m_fpivot.y = gHeightUniform(gRandomEngine);

	m_veloX = gXVzeroUniform(gRandomEngine);
	m_veloY = gYVzeroUniform(gRandomEngine);
}

void tagPentagon::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;
	m_fpivot.x += m_veloX * gTimeStep * m_sign;
	m_fpivot.y += m_veloY * gTimeStep;
}

void tagPentagon::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagPentagon::UpdateVertex()
{
	fPoint pivot = this->getfPivot();
	m_vertex[0][0] = pivot.x;
	m_vertex[0][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ�
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // ���� ��� - �ﰢ��1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // �߾�

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // �߾� 

	m_vertex[7][0] = pivot.x + m_x_radius;  // ���� ��� 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // ���� �ϴ� - �ﰢ��3
	m_vertex[8][1] = pivot.y - m_y_radius;

	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagPentagon::UpdatePivot()
{
	FreeFall();
	UpdateVertex();
}

void tagPentagon::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 9);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagPentagon::initColor()
{
	for (int i = 0; i < 9; ++i) {
		m_color[i][0] = getfRGB().fRed;
		m_color[i][1] = getfRGB().fGreen;
		m_color[i][2] = getfRGB().fBlue;
	}
}

void tagPentagon::UpdateEdgeList()
{
	edge[0].sp.x = m_vertex[0][0];
	edge[0].sp.y = m_vertex[0][1];
	edge[0].ep.x = m_vertex[7][0];
	edge[0].ep.y = m_vertex[7][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[0].sp.x << ", " << edge[0].sp.y << " ),  (" << edge[0].ep.x << ", " << edge[0].ep.y << ")\n";

	edge[1].sp.x = m_vertex[7][0];
	edge[1].sp.y = m_vertex[7][1];
	edge[1].ep.x = m_vertex[8][0];
	edge[1].ep.y = m_vertex[8][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[1].sp.x << ", " << edge[1].sp.y << " ),  (" << edge[1].ep.x << ", " << edge[1].ep.y << ")\n";

	edge[2].sp.x = m_vertex[8][0];
	edge[2].sp.y = m_vertex[8][1];
	edge[2].ep.x = m_vertex[5][0];
	edge[2].ep.y = m_vertex[5][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[2].sp.x << ", " << edge[2].sp.y << " ),  (" << edge[2].ep.x << ", " << edge[2].ep.y << ")\n";

	edge[3].sp.x = m_vertex[5][0];
	edge[3].sp.y = m_vertex[5][1];
	edge[3].ep.x = m_vertex[2][0];
	edge[3].ep.y = m_vertex[2][1];
	//	cout << "sp(x,y) ep(x,y) : " << "( " << edge[3].sp.x << ", " << edge[3].sp.y << " ),  (" << edge[3].ep.x << ", " << edge[3].ep.y << ")\n\n";
	edge[4].sp.x = m_vertex[2][0];
	edge[4].sp.y = m_vertex[2][1];
	edge[4].ep.x = m_vertex[0][0];
	edge[4].ep.y = m_vertex[0][1];
}

void tagPentagon::initEdgeList()
{

}

//========================================================================

bool tagBasket::isGone()
{
	return false;
}

void tagBasket::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagBasket::UpdateVertex()
{
	fPoint pivot = this->getfPivot();

	m_vertex[0][0] = pivot.x - m_x_radius;
	m_vertex[0][1] = pivot.y + m_y_radius;  // ���� ��� 

	m_vertex[1][0] = pivot.x + m_x_radius;  // ���� ���
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // ���� �ϴ� - �ﰢ��1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // ���� ��� 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // ���� �ϴ� 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // ���� �ϴ� - �ﰢ��2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);
}

void tagBasket::UpdatePivot()
{
	if (m_fpivot.x + m_x_radius > 1.0)
		m_sign *= MINUS;
	else if (m_fpivot.x - m_x_radius < -1.0)
		m_sign *= MINUS;

	m_fpivot.x += 0.005f * m_sign;
	UpdateVertex();
}

void tagBasket::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagBasket::initColor()
{
	for (int i = 0; i < 6; ++i) {
		m_color[i][0] = 0.0f;
		m_color[i][1] = 0.0f;
		m_color[i][2] = 1.0f;
	}
}

//========================================================================

bool tagFragment::isGone()
{
	if (m_vertex[0][1] <= -1.5f)
		return true;
	else
		return false;
}

void tagFragment::initBuffer()
{
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);

	glGenBuffers(1, &this->m_pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertex), this->m_vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &this->m_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_color), this->m_color, GL_STATIC_DRAW);
}

void tagFragment::UpdateVertex()
{
	FreeFall();
	UpdateEdgeList();

	glBindBuffer(GL_ARRAY_BUFFER, this->m_pos_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->m_vertex), this->m_vertex);

}

void tagFragment::UpdatePivot()
{
	UpdateVertex();
}

void tagFragment::Draw() const
{
	int PosLocation = glGetAttribLocation(gShaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(gShaderProgramID, "in_Color");

	glEnableVertexAttribArray(PosLocation); // Enable �ʼ�! ����ϰڴ� �ǹ�
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (ON == gToggle[(int)Toggle::FILL])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (ON == gToggle[(int)Toggle::LINE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLE_FAN, 0, edge_num);

	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < edge_num; ++i) {
		m_color[i][0] = 1.0f;
		m_color[i][1] = gColorUniform(gRandomEngine);
		m_color[i][2] = 0.0f;
	}
}

void tagFragment::UpdateEdgeList()
{
	for (int i = 0; i < edge_num - 1; ++i) {
		edge[i].sp.x = m_vertex[i][0];
		edge[i].sp.y = m_vertex[i][1];
		edge[i].ep.x = m_vertex[i + 1][0];
		edge[i].ep.y = m_vertex[i + 1][1];
	}
}

void tagFragment::initEdgeList()
{
	edge_num = 0;
	int i{};

	while (1)
	{
		if (m_vertex[i + 1][0] == 0. && m_vertex[i + 1][1] == 0.f) {
			break;
		}

		edge[i].sp.x = m_vertex[i][0];
		edge[i].sp.y = m_vertex[i][1];
		edge[i].ep.x = m_vertex[i + 1][0];
		edge[i].ep.y = m_vertex[i + 1][1];

		i++;
		edge_num++;
	}
	edge_num++;
	m_vertex[i + 1][0] = m_vertex[0][0];
	m_vertex[i + 1][1] = m_vertex[0][1];
}

int getEdgeStart(EdgeList* el, GLfloat* vert, int ver_st_index, int edge_num)
{
	int idx{};
	for (; idx < edge_num; ++idx) {
		if (el[idx].ep.x == vert[ver_st_index * 3] && el[idx].ep.y == vert[ver_st_index * 3 + 1]) {
			return idx;
		}
	}
	return idx;
}

void tagFragment::initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list, int ver_st_index, int ver_ed_index, GLfloat* vert, int edge_num)
{
	m_vertex[0][0] = sp.x;
	m_vertex[0][1] = sp.y;

	int indx{ 1 };
	int edge_idx = getEdgeStart(edge_list, vert, ver_st_index, edge_num);

	while (1)
	{
		if (edge_idx >= edge_num)
			edge_idx = 0;

		if (edge_list[edge_idx].ep.x == vert[ver_ed_index * 3] && edge_list[edge_idx].ep.y == vert[ver_ed_index * 3 + 1])
			break;

		m_vertex[indx][0] = edge_list[edge_idx].ep.x;
		m_vertex[indx][1] = edge_list[edge_idx].ep.y;

		edge_idx++;
		indx++;
	}
	m_vertex[indx][0] = ep.x;
	m_vertex[indx][1] = ep.y;

}

void tagFragment::FreeFall()
{
	m_veloY -= gGravity * gTimeStep;

	for (int i = 0; i < edge_num; ++i) {
		m_vertex[i][0] += m_veloX * gTimeStep * m_sign;
		m_vertex[i][1] += m_veloY * gTimeStep;
	}
}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp�� �� ������ ���� ����
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep�� �� ������ ������ ���� 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			minXY.x = sp.x - 0.005f;
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

	//	cout << "edge minx,miny : " << minXY.x << ", " << minXY.y << '\n';
	return minXY;
}

fPoint getMaxXY(fPoint sp, fPoint ep)
{
	fPoint maxXY{};

	if (sp.x < ep.x)
		maxXY.x = ep.x;
	else if (sp.x >= ep.x)
		maxXY.x = sp.x;

	if (sp.y < ep.y)
		maxXY.y = ep.y;
	else if (sp.y >= ep.y)
		maxXY.y = sp.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep�� �� ũ�� 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep�� �� ũ�� 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	//	cout << "edge maxx,maxy : " << maxXY.x << ", " << maxXY.y << '\n';

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	float det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);

	if (det == 0) {
		return { 0, 0 }; // No intersection
	}

	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;

	fPoint intersection = { x, y };

	if (isInRange(x, getMinXY(edge_start, edge_end).x, getMaxXY(edge_start, edge_end).x) && isInRange(y, getMinXY(edge_start, edge_end).y, getMaxXY(edge_start, edge_end).y)) {
		return intersection; // Out of range or no intersection
	}
	else
		return { 0,0 };
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {
	//	cout << "edge sp,ep: " << edge_start.x << ", " << edge_start.y << ", " << edge_end.x << ", " << edge_end.y << '\n';
	//	cout << "mouse sp,ep  : " << slice_start.x << ", " << slice_start.y << ", " << slice_end.x << ", " << slice_end.y << '\n';
	//	cout << "minx,miny, maxX, maxY : " << xMin << ", " << yMin << ", " << xMax << ", " << yMax << '\n';


	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}



#endif



//fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
//	double det = (edge_start.x - edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x - slice_end.x);
//
//	if (det == 0) {
//		return { 0, 0 }; // No intersection
//	}
//
//	float x = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.x - slice_end.x) - (edge_start.x - edge_end.x) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//	float y = ((edge_start.x * edge_end.y - edge_start.y * edge_end.x) * (slice_start.y - slice_end.y) - (edge_start.y - edge_end.y) * (slice_start.x * slice_end.y - slice_start.y * slice_end.x)) / det;
//
//	fPoint intersection = { x, y };
//	cout << "(x,y): " << intersection.x << ", " << intersection.y << '\n';
//	cout << "fmin/max X : " << fmin(edge_start.x, edge_end.x) << ", " << fmax(edge_start.x, edge_end.x) << '\n';
//	cout << "fmin/max Y: " << fmin(edge_start.y, edge_end.y) << ", " << fmax(edge_start.y, edge_end.y) << '\n';
//
//
//	//	if (x >= fmin(edge_start.x, edge_end.x) && x <= fmax(edge_start.x, edge_end.x) &&
//	//		y >= fmin(edge_start.y, edge_end.y) && y <= fmax(edge_start.y, edge_end.y)) {
//	return intersection;  // �� ������ ������
//	//	}
//	//	else
//	//		return { 0,0 };
//}

#endif

#if 0

#if 1
#pragma once

#include "usingInclude.h"
#define PI 3.141592
enum class Toggle
{
	LINE = 0,
	FILL = 1,
	PATH = 2,

	END = 10,
};

enum myBOOL
{
	OFF = 0,
	ON = 1,

	PLUS = 1,
	MINUS = -1,
};

struct fPoint
{
	float x;
	float y;
};

struct fRGB
{
	float fRed;
	float fGreen;
	float fBlue;
};

struct EdgeList
{
	fPoint sp;
	fPoint ep;
};

typedef int Sign;

//========================================================================

class Basis
{
protected:
	GLuint  m_vao{};
	GLuint  m_pos_vbo{};
	GLuint  m_color_vbo{};

	fPoint	m_fpivot{};
	fRGB	m_fRGB{};
	GLfloat m_x_radius{};
	GLfloat m_y_radius{};

public:
	Basis()
	{
		initfRGB();
	}

	Basis(const fPoint& pivot) {
		initPivot(pivot);
		initfRGB();
	}

public:
	virtual void UpdatePivot() = 0;
	virtual void Draw() const = 0;
	virtual void initBuffer() = 0;
	virtual bool isGone() = 0;
	virtual EdgeList* getEdgeList() = 0;
	virtual GLint getEdgeNum() = 0;
	virtual void initCount() = 0;
	virtual void countSlice() = 0;
	virtual GLfloat* getVertex() = 0;
	virtual GLint getVerNum() = 0;

	virtual ~Basis() {}

	void initfRGB();
	void initPivot(const fPoint& pivot) { m_fpivot = pivot; }

	fPoint getfPivot() { return m_fpivot; }
	fRGB getfRGB() { return m_fRGB; }

	GLfloat get_x_radius() { return m_x_radius; }
	void set_x_radius(float x) { m_x_radius = x; }
	GLfloat get_y_radius() { return m_y_radius; }
	void set_y_radius(float y) { m_y_radius = y; }
};

//========================================================================

class tagSliceLine : public Basis
{
private:
	GLfloat m_vertex[2][3]{};
	GLfloat m_color[2][3]{};

	fPoint m_start{};
	fPoint m_end{};

public:
	tagSliceLine()
	{}
	tagSliceLine(const fPoint& sp) : Basis()
	{
		setStartPoint(sp);
		setEndPoint(sp);
		UpdateVertex();
		initColor();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void initColor();
	void UpdateVertex();
	void UpdatePivot();
	void Draw() const;

	void setStartPoint(const fPoint& sp) { m_start = sp; }
	void setEndPoint(const fPoint& ep) { m_end = ep; }
	fPoint getStartPoint() { return m_start; }
	fPoint getEndPoint() { return m_end; }
	fPoint getMinXYPoint();
	fPoint getMaxXYPoint();
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }
};

class tagBasket : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign    m_sign;

public:

	tagBasket(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.2f);
		set_y_radius(0.03f);
		initColor();
		UpdateVertex();
		initBuffer();
	}

	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	EdgeList* getEdgeList() { return {}; }
	GLint getEdgeNum() { return {}; }
	void initCount() { }
	void countSlice() { }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

//========================================================================

class tagTriangle : public Basis
{
private:
	GLfloat m_vertex[3][3]{};
	GLfloat m_color[3][3]{};

	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[3]{};
	GLint    edge_num{ 3 };
	GLint	slice_point_count{};

public:
	tagTriangle()
	{}
	tagTriangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagTriangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void countSlice() { slice_point_count++; }

	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagRectangle : public Basis
{
private:
	GLfloat m_vertex[6][3]{};
	GLfloat m_color[6][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[4]{};
	GLint    edge_num{ 4 };
	GLint	slice_point_count{};

public:
	tagRectangle()
	{}
	tagRectangle(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagRectangle(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;

	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

class tagPentagon : public Basis
{
private:
	GLfloat m_vertex[9][3]{};
	GLfloat m_color[9][3]{};
	Sign m_sign{};
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[5]{};
	GLint    edge_num{ 5 };
	GLint	slice_point_count{};

public:
	tagPentagon()
	{}
	tagPentagon(int sign) : Basis()
	{
		setSign(sign);
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initFallvalue();
		UpdateVertex();
		initColor();
		initBuffer();
	}

	tagPentagon(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.12f);
		set_y_radius(0.12f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }

	void initEdgeList();
	void UpdateEdgeList();

	bool isGone();
	void setSign(int sign);
	void initFallvalue();
	void FreeFall();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	void countSlice() { slice_point_count++; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

};

//========================================================================

class tagFragment : public Basis
{
private:
	GLfloat m_vertex[15][3]{}; // vector ���ؽ�, �÷�, edge, edgenum
	GLfloat m_color[15][3]{};

	Sign    m_sign;
	float m_veloX{ 0.03f };
	float m_veloY{ 0.05f };

	EdgeList edge[15]{};
	GLint    edge_num{};
	GLint	slice_point_count{};

public:

	tagFragment(const fPoint& sp, const fPoint& ep, EdgeList* edge_list,
		Sign sign, GLint _edge_num, int ver_st_index, int ver_ed_index, GLfloat* vert) : Basis()
	{
		m_sign = sign; edge_num = _edge_num;;
		initVertex(sp, ep, edge_list, ver_st_index, ver_ed_index, vert, edge_num);
		initColor();
		initEdgeList();
		initBuffer();
	}
	void initVertex(const fPoint& sp, const fPoint& ep,
		EdgeList* edge_list, int ver_index, int ver_ed_index, GLfloat* vert, int edge_num);

	tagFragment(const fPoint& pivot) : Basis(pivot), m_sign{ PLUS }
	{
		set_x_radius(0.1f);
		set_y_radius(0.1f);
		initColor();
		UpdateVertex();
		initEdgeList();
		initBuffer();
	}

	EdgeList* getEdgeList() { return edge; }
	GLint getEdgeNum() { return edge_num; }
	GLfloat* getVertex() { return &m_vertex[0][0]; }

	void FreeFall();
	void initEdgeList();
	void UpdateEdgeList();
	bool isGone();
	void initBuffer();
	void UpdateVertex();
	void initColor();
	void UpdatePivot();
	void Draw() const;
	void initCount() { slice_point_count = 0; }
	GLint getVerNum() { return sizeof(m_vertex) / sizeof(m_vertex[0][0]); }

	void countSlice() { slice_point_count++; }
};
//========================================================================

void ShowMenu();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char*);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
fPoint convertDeviceXYtoOpenGLXY(int window_x, int window_y);
void TimerFunction(int value);

//========================================================================

void setOffAllofToggle();
void initToggle();
void setPolygonMode();
void setPolygonPath();

//========================================================================

void gVecPushBack();
void gVecClear();
void gVecDraw();
void WorldUpdate();
void gVecUpdate();
void gVecRemove();

//========================================================================

void resetSlicing();
void initBasket();
void resetBasket();

//========================================================================

bool isInRange(float val, float min, float max);
fPoint getIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4);
fPoint checkIntersection(fPoint p1, fPoint p2, fPoint p3, fPoint p4, float xMin, float yMin, float xMax, float yMax);
int getIndex(const fPoint& end_point, GLfloat* vertex, int vertex_size);

void FindIntersection();

#endif
#endif
