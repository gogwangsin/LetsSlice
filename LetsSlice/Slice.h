
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
	float m_veloX{0.03f};
	float m_veloY{0.05f};

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
	float m_veloX{ 0.03f};
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
	GLfloat m_vertex[15][3]{}; // vector 버텍스, 컬러, edge, edgenum
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
int getEdgeStart(EdgeList* el, GLfloat* vert, int ver_st_index, int edge_num);
void FindIntersection();

#endif