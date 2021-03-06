﻿#pragma once

#include <Windows.h>
#include <iostream>
#include <fcntl.h>
#include <chrono>
#include <io.h>

namespace GColors {
	namespace Background {
		unsigned short int BLACK = 0x0000,
			DARK_BLUE = 0x0010,
			DARK_GREEN = 0x0020,
			DARK_CYAN = 0x0030,
			DARK_RED = 0x0040,
			DARK_MAGENTA = 0x0050,
			DARK_YELLOW = 0x0060,
			GREY = 0x0070,
			DARK_GREY = 0x0080,
			BLUE = 0x0090,
			GREEN = 0x00A0,
			CYAN = 0x00B0,
			RED = 0x00C0,
			MAGENTA = 0x00D0,
			YELLOW = 0x00E0,
			WHITE = 0x00F0;
	};

	namespace Foreground {
		unsigned short int BLACK = 0x0000,
			DARK_BLUE = 0x0001,
			DARK_GREEN = 0x0002,
			DARK_CYAN = 0x0003,
			DARK_RED = 0x0004,
			DARK_MAGENTA = 0x0005,
			DARK_YELLOW = 0x0006,
			GREY = 0x0007,
			DARK_GREY = 0x0008,
			BLUE = 0x0009,
			GREEN = 0x000A,
			CYAN = 0x000B,
			RED = 0x000C,
			MAGENTA = 0x000D,
			YELLOW = 0x000E,
			WHITE = 0x000F;
	};

	namespace Both {
		unsigned short int
			BLACK[2] = { 0x0000, 0x0000 },
			DARK_BLUE[2] = { 0x0001, 0x0010 },
			DARK_GREEN[2] = { 0x0002, 0x0020 },
			DARK_CYAN[2] = { 0x0003, 0x0030 },
			DARK_RED[2] = { 0x0004, 0x0040 },
			DARK_MAGENTA[2] = { 0x0005, 0x0050 },
			DARK_YELLOW[2] = { 0x0006, 0x0060 },
			GREY[2] = { 0x0007, 0x0070 },
			DARK_GREY[2] = { 0x0008, 0x0080 },
			BLUE[2] = { 0x0009, 0x0090 },
			GREEN[2] = { 0x000A, 0x00A0 },
			CYAN[2] = { 0x000B, 0x00B0 },
			RED[2] = { 0x000C, 0x00C0 },
			MAGENTA[2] = { 0x000D, 0x00D0 },
			YELLOW[2] = { 0x000E, 0x00E0 },
			WHITE[2] = { 0x000F, 0x00F0 };
	}
};

namespace GPixelTypes {
	wchar_t PIXEL_SOLID[1] = { 0x2588 },
		PIXEL_THREEQUARTERS[1] = { 0x2593 },
		PIXEL_HALF[1] = { 0x2592 },
		PIXEL_QUARTER[1] = { 0x2591 };
}

struct GPixel {
	wchar_t character[1];
	unsigned short int ForeGround;
	unsigned short int BackGround;
};

struct GVector {
	double x, y, z;

	GVector(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
};

GVector add(GVector a, GVector b) { GVector result(a.x + b.x, a.y + b.y, a.z + b.z); return result; }
GVector add(GVector a, double n) { GVector result(a.x + n, a.y + n, a.z + n); return result; }
GVector sub(GVector a, GVector b) { GVector result(a.x - b.x, a.y - b.y, a.z - b.z); return result; }
GVector sub(GVector a, double n) { GVector result(a.x - n, a.y - n, a.z - n); return result; }
GVector mult(GVector a, GVector b) { GVector result(a.x * b.x, a.y * b.y, a.z * b.z); return result; }
GVector mult(GVector a, double n) { GVector result(a.x * n, a.y * n, a.z * n); return result; }

class GEngine {
public:
	GEngine(short int argFontW, short int argFontH) {
		Console = GetConsoleWindow();
		StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		for (int i = 0; i < width * height; i++) {
			currentBuffer[i].character[0] = GPixelTypes::PIXEL_SOLID[0];
			currentBuffer[i].ForeGround = GColors::Foreground::WHITE;
			currentBuffer[i].BackGround = GColors::Background::WHITE;
		}

		setFont(argFontW, argFontH);
		setSize();
		disableMouseSelection();
		initUnicode16();
	}

	void setSize() {
		windowSize = { (short)width, (short)height };
		GetWindowRect(Console, &consoleDimensions);
		MoveWindow(Console, consoleDimensions.left, consoleDimensions.top, 1920, 1080, TRUE);
	}

	void setFont(short int argFontW, short int argFontH) {
		CONSOLE_FONT_INFOEX cf = { 0 };

		fontW = argFontW;
		fontH = argFontH;

		cf.cbSize = sizeof cf;
		cf.dwFontSize.X = fontW;
		cf.dwFontSize.Y = fontH;
		wcscpy_s(cf.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 0, &cf);
	}

	void disableMouseSelection() {
		DWORD prev_mode;

		GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
		SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prev_mode & ~ENABLE_QUICK_EDIT_MODE);
	}

	void initUnicode16() {
		_setmode(_fileno(stdout), _O_U16TEXT);
	};

	unsigned short int getWidth() {
		return width;
	}

	unsigned short int getHeight() {
		return height;
	}

	void run(void(*renderFunc)(), void(*updateFunc)(float elapsedTime)) {
		auto  start_time = std::chrono::system_clock::now();
		float elapsedTime = 0;

		while (1) {
			auto end_time = std::chrono::system_clock::now();
			std::chrono::duration<float> et = end_time - start_time;
			elapsedTime = et.count();

			start_time = std::chrono::system_clock::now();

			updateFunc(elapsedTime);
			renderFunc();

			COORD pos;
			DWORD nWritten;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int index = y * width + x;

					pos.X = x;
					pos.Y = y;

					if ((currentBuffer[index].character[0] != previousBuffer[index].character[0]) ||
						(currentBuffer[index].ForeGround != previousBuffer[index].ForeGround) ||
						(currentBuffer[index].BackGround != previousBuffer[index].BackGround))
					{
						SetConsoleCursorPosition(StdOutput, pos);
						SetConsoleTextAttribute(StdOutput, currentBuffer[index].BackGround | currentBuffer[index].ForeGround);
						WriteConsole(StdOutput, currentBuffer[index].character, 1, &nWritten, NULL);
						previousBuffer[index].character[0] = currentBuffer[index].character[0];
						previousBuffer[index].ForeGround = currentBuffer[index].ForeGround;
						previousBuffer[index].BackGround = currentBuffer[index].BackGround;
					}
				}
			}

			pos.X = width;
			pos.Y = height;

			SetConsoleCursorPosition(StdOutput, pos);
		}
	}

	void printPixel(short int x, short int y, unsigned short int color[]) {
		if (x >= 0 && y >= 0 && x < width && y < height) {
			currentBuffer[y * width + x].character[0] = GPixelTypes::PIXEL_SOLID[0];
			currentBuffer[y * width + x].BackGround = color[1];
			currentBuffer[y * width + x].ForeGround = color[0];
		}
	}

	void rectangle(short int x, short int y, unsigned short int w, unsigned short int h, unsigned short int color[]) {
		for (short int cx = x; cx < x + w; cx++) {
			for (short int cy = y; cy < y + h; cy++) {
				printPixel(cx, cy, color);
			}
		}
	}

	void square(short int x, short int y, unsigned short int s, unsigned short int color[]) {
		rectangle(x, y, s, s, color);
	}

	void background(unsigned short int color[]) {
		rectangle(0, 0, width, height, color);
	}

	void clear() {
		for (int i = 0; i < width * height; i++) {
			currentBuffer[i].character[0] = GPixelTypes::PIXEL_SOLID[0];
			currentBuffer[i].BackGround = GColors::Background::WHITE;
			currentBuffer[i].ForeGround = GColors::Foreground::WHITE;
		}
	}

	void circleOutline(short int gX, short int gY, unsigned short int radius, unsigned short int color[]) {
		short int x = radius - 1;
		short int y = 0;
		short int dx = 1;
		short int dy = 1;
		short int err = dx - (radius << 1);

		while (x >= y) {
			printPixel(gX + x, gY + y, color);
			printPixel(gX + y, gY + x, color);
			printPixel(gX - y, gY + x, color);
			printPixel(gX - x, gY + y, color);
			printPixel(gX - x, gY - y, color);
			printPixel(gX - y, gY - x, color);
			printPixel(gX + y, gY - x, color);
			printPixel(gX + x, gY - y, color);

			if (err <= 0) {
				y++;
				err += dy;
				dy += 2;
			}
			else {
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
	}

	void circleFill(short int gX, short int gY, unsigned short int radius, unsigned short int color[]) {
		while (radius > 0) {
			GEngine::circleOutline(gX, gY, radius, color);
			radius--;
		}
	}

	void drawString(short int gX, short int gY, std::string str, unsigned short int color[]) {
		for (char& c : str) {
			currentBuffer[gY * width + gX].character[0] = c;
			currentBuffer[gY * width + gX].ForeGround = color[0];
			currentBuffer[gY * width + gX].BackGround = color[1];
			gX++;
		}
	}

	// Credit https://github.com/ssloy/tinyrenderer/wiki/Lesson-1:-Bresenham%E2%80%99s-Line-Drawing-Algorithm

	void line(int x0, int y0, int x1, int y1, unsigned short int color[]) {
		bool steep = false;

		if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}

		if (x0 > x1) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int dx = x1 - x0;
		int dy = y1 - y0;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = y0;

		for (int x = x0; x <= x1; x++) {
			if (steep) {
				printPixel(y, x, color);
			}
			else {
				printPixel(x, y, color);
			}

			error2 += derror2;

			if (error2 > dx) {
				y += (y1 > y0 ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}

	// Credit https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling

	void triangle(GVector t0, GVector t1, GVector t2, unsigned short int color[]) {
		if (t0.y == t1.y && t0.y == t2.y) return;

		if (t0.y > t1.y) std::swap(t0, t1);
		if (t0.y > t2.y) std::swap(t0, t2);
		if (t1.y > t2.y) std::swap(t1, t2);

		int total_height = t2.y - t0.y;

		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1.y - t0.y || t1.y == t0.y;
			int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

			GVector A = add(t0, mult(sub(t2, t0), alpha));
			GVector B = second_half ? add(t1, mult(sub(t2, t1), beta)) : add(t0, mult(sub(t1, t0), beta));

			if (A.x > B.x) std::swap(A, B);

			for (int j = A.x; j <= B.x; j++) {
				printPixel(j, t0.y + i, color);
			}
		}
	}

private:
	HWND Console;
	RECT consoleDimensions;
	COORD windowSize;
	HANDLE StdOutput;

#ifdef WIDTH
	unsigned short int width = WIDTH, height = HEIGHT, fontW, fontH;
	GPixel currentBuffer[WIDTH * HEIGHT];
	GPixel previousBuffer[WIDTH * HEIGHT];
#else
	unsigned short int width = 300, height = 300, fontW, fontH;
	GPixel currentBuffer[300 * 300];
	GPixel previousBuffer[300 * 300];
#endif
};