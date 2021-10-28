#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

#define OUTPUT_LOG_LIMIT 200

void log(const char file[], int line, const char* format, ...);

std::string GetOutputText(int index);

int GetOutputSize();

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct Point
{
	Point() {}
	Point(int xx, int yy) { x = float(xx); y = float(yy); }
	Point(float xx, float yy) { x = xx; y = yy; }
	Point(double xx, double yy) { x = xx; y = yy; }

	float x = 0, y = 0;

	bool operator==(Point b) { return (x == b.x && y == b.y); }
	bool operator==(int i) { return (x == i || y == i); }
	bool operator!=(Point b) { return !(x == b.x && y == b.y); }
	bool operator!=(int i) { return !(x == i && y == i); }

	void operator+=(int i) { x += i; y += i; }
	void operator-=(int i) { x -= i; y -= i; }
	void operator*=(int i) { x *= i; y *= i; }
	void operator/=(int i) { if (i == 0) return; x /= i; y /= i; }

	void operator+=(Point i) { x += i.x; y += i.y; }
	void operator-=(Point i) { x -= i.x; y -= i.y; }
	void operator*=(Point i) { x *= i.x; y *= i.y; }
	void operator/=(Point i) { if (i == 0) return; x /= i.x; y /= i.y; }

	bool operator<(int i) { return (x < i && y < i); }
	bool operator<=(int i) { return (x <= i && y <= i); }
	bool operator>(int i) { return (x > i && y > i); }
	bool operator>=(int i) { return (x >= i && y >= i); }

	bool operator<(Point b) { return (x < b.x && y < b.y); }
	bool operator<=(Point b) { return (x <= b.x && y <= b.y); }
	bool operator>(Point b) { return (x > b.x && y > b.y); }
	bool operator>=(Point b) { return (x >= b.x && y >= b.y); }


};

struct Point3D
{
	Point3D() {}
	Point3D(int xx, int yy, int zz) { x = xx; y = yy; z = zz; }
	Point3D(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	Point3D(double xx, double yy, double zz) { x = xx; y = yy; z = zz; }

	float x = 0, y = 0, z = 0;

	bool operator==(Point3D b) { return (x == b.x && y == b.y && z == b.z); }
	bool operator==(int i) { return (x == i || y == i || z == i); }
	bool operator!=(Point3D b) { return !(x == b.x && y == b.y && z == b.z); }
	bool operator!=(int i) { return !(x == i && y == i && z == i); }

	void operator+=(int i) { x += i; y += i; z += i; }
	void operator-=(int i) { x -= i; y -= i; z -= i; }
	void operator*=(int i) { x *= i; y *= i; z *= i; }
	void operator/=(int i) { if (i == 0) return; x /= i; y /= i; z /= i;}

	void operator+=(Point3D i) { x += i.x; y += i.y; z += i.z; }
	void operator-=(Point3D i) { x -= i.x; y -= i.y; z -= i.z; }
	void operator*=(Point3D i) { x *= i.x; y *= i.y; z *= i.z; }
	void operator/=(Point3D i) { if (i == 0) return; x /= i.x; y /= i.y; z /= i.z; }

	bool operator<(int i) { return (x < i && y < i && z < i); }
	bool operator<=(int i) { return (x <= i && y <= i && z <= i); }
	bool operator>(int i) { return (x > i && y > i && z > i); }
	bool operator>=(int i) { return (x >= i && y >= i && z >= i); }

	bool operator<(Point3D b) { return (x < b.x && y < b.y && z < b.z); }
	bool operator<=(Point3D b) { return (x <= b.x && y <= b.y && z <= b.z); }
	bool operator>(Point3D b) { return (x > b.x && y > b.y && z > b.z); }
	bool operator>=(Point3D b) { return (x >= b.x && y >= b.y && z >= b.z); }


};

class Rotation
{
public:

	Rotation(float anglex, float planeXx, float planeYx, float planeZx)
	{
		angle = anglex;
		planeX = planeXx;
		planeY = planeYx;
		planeZ = planeZx;
	}

	float angle = 0.0f;
	float planeX = 0;
	float planeY = 0;
	float planeZ = 0;
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE false
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true