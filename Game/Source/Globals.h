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
	Point(int xx, int yy) { x = xx; y = yy; }
	Point(float xx, float yy) { x = xx; y = yy; }
	Point(double xx, double yy) { x = xx; y = yy; }

	int x = 0, y = 0;

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

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE false
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true