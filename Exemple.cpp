#include "GEngine.h"
#include <conio.h>

const int PW = 3;
const int PH = (int)(70 / 5);

GEngine engine(150, 70, 5, 5);
GVector p1(0, engine.getHeight() / 2 - PH / 2, 0),
p2(engine.getWidth() - PW, engine.getHeight() / 2 - PH / 2, 0),
ball(engine.getWidth() / 2, engine.getHeight() / 2, 0);

int ballVX = -20;
int ballVY = 15;
int started = 0;

void update(float elapsedTime) {
	if (started == 1) {
		GVector applied(0, 0, 0);
		applied.x = ballVX;
		applied.y = ballVY;
		applied.mult(elapsedTime);
		ball.add(applied);

		if (ball.y <= 0 || ball.y + PW >= engine.getHeight()) ballVY *= -1;
		if (ball.y <= 0) ball.y = 0;
		if (ball.y + PW >= engine.getHeight()) ball.y = engine.getHeight() - PW;


		if (ball.x + PW <= PW && ball.x >= -2 && ball.y - PW >= p1.y && ball.y + PW <= p1.y + PH) {
			ballVX *= -1;
			ball.x  = PW;
		}

		if (ball.x + PW >= p2.x && ball.x <= engine.getWidth() + 2 && ball.y - PW >= p2.y && ball.y + PW <= p2.y + PH) {
			ballVX *= -1;
			ball.x  = p2.x - PW;
		}
	}

	if (engine.isKeyPressed('A') == 1) {
		p1.y -= 0.5;
		started = 1;
	} else if (engine.isKeyPressed('D') == 1) {
		p1.y += 0.5;
		started = 1;
	}

	if (engine.isKeyPressed('E') == 1) {
		p2.y -= 0.5;
		started = 1;
	}
	else if (engine.isKeyPressed('F') == 1) {
		p2.y += 0.5;
		started = 1;
	}
}

void render() {
	engine.background(GColors::Both::BLACK);
	
	engine.rectangleFill(p1.x, p1.y, PW, PH, GColors::Both::WHITE);
	engine.rectangleFill(p2.x, p2.y, PW, PH, GColors::Both::WHITE);
	engine.rectangleFill(ball.x, ball.y, PW, PW, GColors::Both::WHITE);
}

int main() {
	engine.run(&render, &update);
}