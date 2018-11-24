#include "GEngine.h"
#include <conio.h>

GEngine engine(10, 10);

void update(float elapsedTime) {
	
}

void render() {
	engine.background(GColors::Both::WHITE);
	engine.rectangle(0, 0, 40, 40, GColors::Both::RED);
	engine.circleFill(40, 40, 40, GColors::Both::GREEN);

	unsigned short int color[2] = {GColors::Foreground::CYAN, GColors::Background::DARK_MAGENTA};

	engine.drawString(0, 0, std::string("Hello"), color);
}

int main() {
	engine.run(&render, &update);
}