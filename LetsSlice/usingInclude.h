#pragma once
//"#pragma once" 지시문은 헤더 파일의 내용이 이미 한 번 
// 포함되었다면 해당 헤더 파일을 다시 포함하지
// 않도록 컴파일러에 지시합니다. 
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <cmath>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;