
#if 1

#include "Slice.h"
#include "usingInclude.h"

#include <random>

std::random_device gRandDevice; // 진짜 난수 발생기 -> 이 값을 시드값으로 
std::mt19937 gRandomEngine(gRandDevice()); // 알고리즘 + 진짜 난수 시드 :: 진짜진짜 난수 생성 
std::uniform_real_distribution<float> gColorUniform{ 0.0f, 1.0f };
std::uniform_real_distribution<float> gHeightUniform{ -0.5f, 1.0f };
std::uniform_real_distribution<float> gXVzeroUniform{ 0.03f, 0.1f };
std::uniform_real_distribution<float> gYVzeroUniform{ 0.03f, 0.07f };
std::uniform_int_distribution<int> gSignUniform{ 0,1 };

extern GLchar* gVertexSource; //--- 소스코드 저장 변수
extern GLchar* gFragmentSource;
extern GLuint gVertexShader;
extern GLuint gFragmentShader; //--- 세이더 객체
extern GLuint gShaderProgramID; //--- 셰이더 프로그램

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
	cout << "=========================================================================\n";
	cout << "좌측, 우측 임의 위치에서 삼각형, 사각형, 오각형이 포물선 운동을 하면서 날아온다.\n";
	cout << "마우스 클릭, 드래그하여 도형을 슬라이스한다.(작은 조각이 되어 아래로 하강)\n";
	cout << "작은 조각을 담을 수 있는 바구니가 좌우로 움직이고 있다.\n";
	cout << "=========================================================================\n";
	cout << "L/l : 도형 모드: LINE/FILL\n";
//	cout << "P: 경로 출력 ON/OFF\n";
	cout << "+/- : 날아오는 속도 증감한다.\n";
	cout << "r/R : 게임을 초기화한다.\n";
	cout << "q/Q : 프로그램을 종료한다.\n";
	cout << "=========================================================================\n";
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
		std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
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
		std::cout << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
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
		if (gSlicing) { 
			FindIntersection();
			delete gSlicing;
			gSlicing = nullptr;
			isUsingSlicing = FALSE;
			return;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isUsingSlicing) {
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
	fPoint openGL_coordinate{}; // 오픈지엘 좌표계
	int w = gWidth;
	int h = gHeight;
	openGL_coordinate.x = (float)(window_x - (float)w / 2.0) * (float)((1.0 / (float)(w / 2.0)));
	openGL_coordinate.y = -(float)(window_y - (float)h / 2.0) * (float)((1.0 / (float)(h / 2.0)));
	// (-) y증가 방향, 윈도우 중심을 기준으로 x,y 빼주고, 범위 크기를 (1/윈도우중심) ->을 곱해서 줄여준다. 
	return openGL_coordinate;
}

void TimerFunction(int value)
{
	gVecRemove();
	gVecPushBack();
	WorldUpdate();
	glutPostRedisplay();				//화면 재출력
	glutTimerFunc(10, TimerFunction, 1); // 다시 호출 
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
		cout << "바구니 삭제\n";
	}
}

void resetSlicing()
{
	isUsingSlicing = FALSE;
	if (gSlicing) {
		delete gSlicing;
		cout << "선분 삭제\n";
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
//			cout << "Y:" << (*it)->getfPivot().y << "가 소멸되었습니다\n";
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
					tagFragment* frag = new tagFragment{ slice_point[0], slice_point[1], list, PLUS, edge_num, polygon_index[0],  polygon_index[1], vert, (*iter)->getfRGB()};
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
	vector<tagFragment*> fragmentsToAdd; // 임시 벡터

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
	for (int i = 0; i < vertex_size ; ++i) {
		
		GLfloat* current = &vertex[i * 3];
		if (edge_end_point.x == current[0] && edge_end_point.y == current[1]) { // 오류 생기면 1순위 -> 부동 소수 == 
			break;
		}
		index = index + 3;
	}
	
	index /= 3; // 킵
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
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
		minXY.x = sp.x -0.5; // sp가 더 작으면 왼쪽 작은
	else if (sp.x > ep.x)
		minXY.x = ep.x -0.5; // ep가 더 작으면 오른쪽 작은 

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

	m_vertex[0][0] = pivot.x;         // 첫 번째 정점 (피벗 위치)
	m_vertex[0][1] = pivot.y + m_y_radius;  // 위쪽 꼭지점

	m_vertex[1][0] = pivot.x + m_x_radius;  // 오른쪽 꼭지점
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // 왼쪽 꼭지점
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
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

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
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
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // 화면 왼편 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // 화면 우측편
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
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // 화면 왼편 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // 화면 우측편
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
	m_vertex[0][1] = pivot.y + m_y_radius;  // 좌측 상단 

	m_vertex[1][0] = pivot.x + m_x_radius;  // 우측 상단
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // 우측 하단 - 삼각형1 
	m_vertex[2][1] = pivot.y - m_y_radius;
// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // 좌측 상단 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // 우측 하단 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // 좌측 하단 - 삼각형2 
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
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

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
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
	if (PLUS == m_sign) m_fpivot.x = -1.0f - m_x_radius; // 화면 왼편 
	else if (MINUS == m_sign) m_fpivot.x = 1.0 + m_x_radius; // 화면 우측편
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
	m_vertex[0][1] = pivot.y + m_y_radius;  // 중앙

	m_vertex[1][0] = pivot.x - m_x_radius / 1.5;  // 우측 하단
	m_vertex[1][1] = pivot.y - m_y_radius;

	m_vertex[2][0] = pivot.x - m_x_radius;  // 좌측 상단 - 삼각형1 
	m_vertex[2][1] = pivot.y + m_y_radius / 3;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x;
	m_vertex[3][1] = pivot.y + m_y_radius;  // 중앙

	m_vertex[4][0] = pivot.x + m_x_radius / 1.5;  // 우측 하단 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius / 1.5;  // 좌측 하단 - 삼각형2 
	m_vertex[5][1] = pivot.y - m_y_radius;

	// ------------------------------------------------------
	m_vertex[6][0] = pivot.x;
	m_vertex[6][1] = pivot.y + m_y_radius;   // 중앙 

	m_vertex[7][0] = pivot.x + m_x_radius;  // 우측 상단 
	m_vertex[7][1] = pivot.y + m_y_radius / 3;

	m_vertex[8][0] = pivot.x + m_x_radius / 1.5;  // 좌측 하단 - 삼각형3
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
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

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
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
	m_vertex[0][1] = pivot.y + m_y_radius;  // 좌측 상단 

	m_vertex[1][0] = pivot.x + m_x_radius;  // 우측 상단
	m_vertex[1][1] = pivot.y + m_y_radius;

	m_vertex[2][0] = pivot.x + m_x_radius;  // 우측 하단 - 삼각형1 
	m_vertex[2][1] = pivot.y - m_y_radius;
	// ------------------------------------------------------
	m_vertex[3][0] = pivot.x - m_x_radius;  // 좌측 상단 
	m_vertex[3][1] = pivot.y + m_y_radius;

	m_vertex[4][0] = pivot.x + m_x_radius;  // 우측 하단 
	m_vertex[4][1] = pivot.y - m_y_radius;

	m_vertex[5][0] = pivot.x - m_x_radius;  // 좌측 하단 - 삼각형2 
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
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
	if( !bBasket ) FreeFall();
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

	glEnableVertexAttribArray(PosLocation); // Enable 필수! 사용하겠단 의미
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

	glDisableVertexAttribArray(PosLocation); // Disable 필수!
	glDisableVertexAttribArray(ColorLocation);
}

void tagFragment::initColor()
{
	for (int i = 0; i < 15 ; ++i) {
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
	int i {};

	while ( 1 )
	{		
		if ( m_vertex[i + 1][0] == 0. && m_vertex[i + 1][1] == 0.f) {
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

void tagFragment::initVertex(const fPoint& sp, const fPoint& ep, EdgeList* edge_list, int ver_st_index, int ver_ed_index, GLfloat* vert, int edge_num)
{
	m_vertex[0][0] = sp.x;
	m_vertex[0][1] = sp.y;

	int indx{1};
	int edge_idx = getEdgeStart(edge_list, vert, ver_st_index, edge_num);

	while ( 1 )
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
			if (m_vertex[i][0] == 0. && m_vertex[i][1] == 0.) break;
			m_vertex[i++][0] += sign * 0.005f;		
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
			break;
		}
	}
}

//========================================================================

fPoint getMinXY(fPoint sp, fPoint ep)
{
	fPoint minXY{};

	if (sp.x <= ep.x)
		minXY.x = sp.x; // sp가 더 작으면 왼쪽 작은
	else if (sp.x > ep.x)
		minXY.x = ep.x; // ep가 더 작으면 오른쪽 작은 

	if (sp.y <= ep.y)
		minXY.y = sp.y;
	else if (sp.y > ep.y)
		minXY.y = ep.y;

	if (abs(sp.x - ep.x) <= 0.005f) {
		if (sp.x - ep.x <= 0) { // ep가 더 크면 
			minXY.x = sp.x - 0.005f; 
		}
		else if (sp.x - ep.x > 0)
			minXY.x = ep.x - 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep가 더 크면 
			minXY.y = sp.y - 0.005f;
		}
		else if (sp.y - ep.y > 0)
			minXY.y = ep.y - 0.005f;
	}

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
		if (sp.x - ep.x <= 0) { // ep가 더 크면 
			maxXY.x = ep.x + 0.005f;
		}
		else if (sp.x - ep.x > 0)
			maxXY.x = sp.x + 0.005f;
	}

	if (abs(sp.y - ep.y) <= 0.005f) {
		if (sp.y - ep.y <= 0) { // ep가 더 크면 
			maxXY.y = ep.y + 0.005f;
		}
		else if (sp.y - ep.y > 0)
			maxXY.y = sp.y + 0.005f;
	}

	return maxXY;
}

bool isInRange(float val, float min, float max) {
	return (val >= min && val <= max);
}

fPoint getIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end) {
	// 행렬식 - 직선 교점 
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
	else {
		return { 0,0 };
	}
}

fPoint checkIntersection(fPoint edge_start, fPoint edge_end, fPoint slice_start, fPoint slice_end, float xMin, float yMin, float xMax, float yMax) {

	if (!isInRange(edge_start.x, xMin, xMax) || !isInRange(edge_start.y, yMin, yMax) || !isInRange(edge_end.x, xMin, xMax) || !isInRange(edge_end.y, yMin, yMax) ||
		!isInRange(slice_start.x, xMin, xMax) || !isInRange(slice_start.y, yMin, yMax) || !isInRange(slice_end.x, xMin, xMax) || !isInRange(slice_end.y, yMin, yMax)) {
		return { 0, 0 }; // Out of range or no intersection
	}

	fPoint intersection = getIntersection(edge_start, edge_end, slice_start, slice_end);
	return intersection;
}

int getEdgeStart(EdgeList* el, GLfloat* vert, int ver_st_index, int edge_num)
{
	int idx{};
	for (; idx < edge_num; ++idx) {
		if (el[idx].ep.x == vert[ver_st_index * 3] &&
			el[idx].ep.y == vert[ver_st_index * 3 + 1]) {
			return idx;
		}
	}
	return idx;
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
//	return intersection;  // 두 직선이 교차함
//	//	}
//	//	else
//	//		return { 0,0 };
//}
