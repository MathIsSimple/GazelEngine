#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <iostream>
#include <fcntl.h>
#include <chrono>
#include <io.h>

namespace GColors {
	namespace Background {
		unsigned short int BLACK = 0x0000,
			DARK_BLUE    = 0x0010,
			DARK_GREEN   = 0x0020,
			DARK_CYAN    = 0x0030,
			DARK_RED     = 0x0040,
			DARK_MAGENTA = 0x0050,
			DARK_YELLOW  = 0x0060,
			GREY         = 0x0070,
			DARK_GREY    = 0x0080,
			BLUE         = 0x0090,
			GREEN        = 0x00A0,
			CYAN         = 0x00B0,
			RED          = 0x00C0,
			MAGENTA      = 0x00D0,
			YELLOW       = 0x00E0,
			WHITE        = 0x00F0;
	};

	namespace Foreground {
		unsigned short int BLACK = 0x0000,
			DARK_BLUE    = 0x0001,
			DARK_GREEN   = 0x0002,
			DARK_CYAN    = 0x0003,
			DARK_RED     = 0x0004,
			DARK_MAGENTA = 0x0005,
			DARK_YELLOW  = 0x0006,
			GREY         = 0x0007,
			DARK_GREY    = 0x0008,
			BLUE         = 0x0009,
			GREEN        = 0x000A,
			CYAN         = 0x000B,
			RED          = 0x000C,
			MAGENTA      = 0x000D,
			YELLOW       = 0x000E,
			WHITE        = 0x000F;
	};

	namespace Both {
		unsigned short int BLACK[2] = { 0x0000, 0x0000 },
			DARK_BLUE[2]    = { 0x0001, 0x0010 },
			DARK_GREEN[2]   = { 0x0002, 0x0020 },
			DARK_CYAN[2]    = { 0x0003, 0x0030 },
			DARK_RED[2]     = { 0x0004, 0x0040 },
			DARK_MAGENTA[2] = { 0x0005, 0x0050 },
			DARK_YELLOW[2]  = { 0x0006, 0x0060 },
			GREY[2]         = { 0x0007, 0x0070 },
			DARK_GREY[2]    = { 0x0008, 0x0080 },
			BLUE[2]         = { 0x0009, 0x0090 },
			GREEN[2]        = { 0x000A, 0x00A0 },
			CYAN[2]         = { 0x000B, 0x00B0 },
			RED[2]          = { 0x000C, 0x00C0 },
			MAGENTA[2]      = { 0x000D, 0x00D0 },
			YELLOW[2]       = { 0x000E, 0x00E0 },
			WHITE[2]        = { 0x000F, 0x00F0 };
	}
};

namespace GPixelTypes {
	wchar_t PIXEL_SOLID[1]         = { 0x2588 },
		    PIXEL_THREEQUARTERS[1] = { 0x2593 },
		    PIXEL_HALF[1]          = { 0x2592 },
		    PIXEL_QUARTER[1]       = { 0x2591 };
}

struct GPixel {
	wchar_t character[1];
	unsigned short int ForeGround;
	unsigned short int BackGround;
};

class GVector {
	public:
		double x, y, z;

		GVector(double gX, double gY, double gZ) {
			x = gX;
			y = gY;
			z = gZ;
		}

		void add(GVector v2) {
			x += v2.x;
			y += v2.y;
			z += v2.z;
		}

		void sub(GVector v2) {
			x -= v2.x;
			y -= v2.y;
			z -= v2.z;
		}

		void mult(GVector v2) {
			x *= v2.x;
			y *= v2.y;
			z *= v2.z;
		}

		void div(GVector v2) {
			x /= v2.x;
			y /= v2.y;
			z /= v2.z;
		}

		void add(double n) {
			x += n;
			y += n;
			z += n;
		}

		void sub(double n) {
			x -= n;
			y -= n;
			z -= n;
		}

		void mult(double n) {
			x *= n;
			y *= n;
			z *= n;
		}

		void div(double n) {
			x /= n;
			y /= n;
			z /= n;
		}
};

class GEngine {
	public:
		GEngine(short int argWidth, short int argHeight, short int argFontW, short int argFontH) {
			width  = argWidth;
			height = argHeight;

			currentBuffer  = (GPixel*) std::malloc(width * height * sizeof(GPixel));
			previousBuffer = (GPixel*) std::malloc(width * height * sizeof(GPixel));

			Console   = GetConsoleWindow();
			StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

			for (int i = 0; i < width * height; i++) {
				currentBuffer[i].character[0] = GPixelTypes::PIXEL_SOLID[0];
				currentBuffer[i].ForeGround   = GColors::Foreground::WHITE;
				currentBuffer[i].BackGround   = GColors::Background::WHITE;
			}

			setFont(argFontW, argFontH);
			
			windowSize = { (short) width, (short) height };
			GetWindowRect(Console, &consoleDimensions);
			MoveWindow(Console, consoleDimensions.left, consoleDimensions.top, 1920, 1080, TRUE);

			DWORD prev_mode;

			GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
			SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prev_mode & ~ENABLE_QUICK_EDIT_MODE);

			_setmode(_fileno(stdout), _O_U16TEXT);
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

				renderFunc();
				if (started == 1) updateFunc(elapsedTime);

				COORD pos;
				DWORD nWritten;

				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						int index = y * width + x;

						pos.X = x;
						pos.Y = y;

						if ((currentBuffer[index].character[0] != previousBuffer[index].character[0]) ||
							(currentBuffer[index].ForeGround   != previousBuffer[index].ForeGround) ||
							(currentBuffer[index].BackGround   != previousBuffer[index].BackGround))
						{
							SetConsoleCursorPosition(StdOutput, pos);
							SetConsoleTextAttribute(StdOutput, currentBuffer[index].BackGround | currentBuffer[index].ForeGround);
							WriteConsole(StdOutput, currentBuffer[index].character, 1, &nWritten, NULL);
							previousBuffer[index].character[0] = currentBuffer[index].character[0];
							previousBuffer[index].ForeGround   = currentBuffer[index].ForeGround;
							previousBuffer[index].BackGround   = currentBuffer[index].BackGround;
						}
					}
				}

				pos.X = width;
				pos.Y = height;

				SetConsoleCursorPosition(StdOutput, pos);
				started = 1;
			}
		}

		double degToRad(double angle) {
			return (angle * 3.1415926 / 180);
		}

		void printPixel(short int x, short int y, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			if (x >= 0 && y >= 0 && x < width && y < height) {
				currentBuffer[y * width + x].character[0] = pixelType;
				currentBuffer[y * width + x].BackGround   = color[1];
				currentBuffer[y * width + x].ForeGround   = color[0];
			}
		}

		void rectangleFill(short int x, short int y, unsigned short int w, unsigned short int h, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			for (short int cx = x; cx < x + w; cx++) {
				for (short int cy = y; cy < y + h; cy++) {
					printPixel(cx, cy, color, pixelType);
				}
			}
		}

		void rectangleOutline(short int x, short int y, unsigned short int w, unsigned short int h, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			for (short int cx = x; cx < x + w; cx++) { printPixel(cx, y, color, pixelType); printPixel(cx, y + h, color, pixelType); }
			for (short int cy = y; cy < y + h + 1; cy++) { printPixel(x, cy, color, pixelType); printPixel(x + w, cy, color, pixelType); }
		}

		void squareFill(short int x, short int y, unsigned short int s, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			rectangleFill(x, y, s, s, color, pixelType);
		}

		void squareOutline(short int x, short int y, unsigned short int s, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			rectangleOutline(x, y, s, s, color, pixelType);
		}

		void background(unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			rectangleFill(0, 0, width, height, color, pixelType);
		}

		void clear() {
			for (int i = 0; i < width * height; i++) {
				currentBuffer[i].character[0] = GPixelTypes::PIXEL_SOLID[0];
				currentBuffer[i].BackGround   = GColors::Background::WHITE;
				currentBuffer[i].ForeGround   = GColors::Foreground::WHITE;
			}
		}

		void circleOutline(short int gX, short int gY, unsigned short int radius, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			short int x   = radius - 1;
			short int y   = 0;
			short int dx  = 1;
			short int dy  = 1;
			short int err = dx - (radius << 1);

			while (x >= y) {
				printPixel(gX + x, gY + y, color, pixelType);
				printPixel(gX + y, gY + x, color, pixelType);
				printPixel(gX - y, gY + x, color, pixelType);
				printPixel(gX - x, gY + y, color, pixelType);
				printPixel(gX - x, gY - y, color, pixelType);
				printPixel(gX - y, gY - x, color, pixelType);
				printPixel(gX + y, gY - x, color, pixelType);
				printPixel(gX + x, gY - y, color, pixelType);

				if (err <= 0) {
					y++;
					err += dy;
					dy += 2;
				} else {
					x--;
					dx += 2;
					err += dx - (radius << 1);
				}
			}
		}

		void circleFill(short int gX, short int gY, unsigned short int radius, unsigned short int color[], wchar_t pixelType = *GPixelTypes::PIXEL_SOLID) {
			while (radius > 0) {
				circleOutline(gX, gY, radius, color, pixelType);
				radius--;
			}
		}

		void drawString(short int gX, short int gY, std::string str, unsigned short int color[]) {
			for (char& c : str) {
				currentBuffer[gY * width + gX].character[0] = c;
				currentBuffer[gY * width + gX].ForeGround   = color[0];
				currentBuffer[gY * width + gX].BackGround   = color[1];
				gX++;
			}
		}

		unsigned short int isKeyPressed(char key) {
			if (GetKeyState(key) & 0x8000) return 1;
			return 0;
		}

	private:
		HWND Console;
		RECT consoleDimensions;
		COORD windowSize;
		HANDLE StdOutput;

		unsigned short int width, height;
		unsigned short int fontW, fontH;
		unsigned short int started = 0;

		GPixel * currentBuffer;
		GPixel * previousBuffer;
};
#else
#error you are not on windows
#endif