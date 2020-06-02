#include "Chess3D.h"

int main(int argc, char* argv[])
{
	Hopper::Interface i(argc, argv);
	Chess3D game(argc, argv, &i, "Chess3D", 1920, 1080, true);
	while (!game.getWindowShouldClose())
	{
		game.update();
		game.render();	
	}
	return 0;
}