#include "application.h"
#include "config.h"

int main()
{
	Config::genDefaults();
	Application* win = new Application(Config::applicationWidth, Config::applicationHeight, Config::vsync, "Antism");
	win->run(); 
	return 0;
}