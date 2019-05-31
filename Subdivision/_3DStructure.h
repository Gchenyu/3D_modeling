#pragma once
#ifndef _3DStructure_H_
#define _3DStructure_H_
/**************************总依赖*********************************/
//_3DStructure.h的依赖
#include <cmath>
#include <assert.h>
#include<iostream>


//Display.h的依赖
#include <GL/glut.h>
#include<GL/glu.h>
#include <stdio.h>
#include <windows.h>

//MeshStructure.h的依赖
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>
/****************************************************************/
using namespace std;

/**************************三维向量结构定义*********************************/
class _3DStructure{
public:
	float x, y, z;
	_3DStructure(float _x = 0.0, float _y = 0.0, float _z = 0.0) :x(_x), y(_y), z(_z) {}
	float Length() { return sqrt(x * x + y * y + z * z); }
	void display() {
		cout << "x=" << x << " y=" << y << " z=" << z << "\n";
	}
};



/********************************向量基础运算，运算符符重载****************************************/

_3DStructure operator+(const _3DStructure& v1, const _3DStructure& v2) {
	return _3DStructure(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}


_3DStructure operator-(const _3DStructure& v1, const _3DStructure& v2) {
	return _3DStructure(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}


_3DStructure operator-(const _3DStructure& v) {
	return _3DStructure(-v.x, -v.y, -v.z);
}

_3DStructure operator*(const _3DStructure& v, float l) {
	return _3DStructure(v.x*l, v.y*l, v.z*l);
}
_3DStructure operator*(float l, const _3DStructure & v) {
	return _3DStructure(v.x*l, v.y*l, v.z*l);
}

_3DStructure operator/(const _3DStructure& v, float l) {
	return _3DStructure(v.x / l, v.y / l, v.z / l);
}

//点乘
float operator^(const _3DStructure& v1, const _3DStructure& v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

//叉乘
_3DStructure operator*(const _3DStructure& v1, const _3DStructure& v2) {
	return _3DStructure(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}




#endif