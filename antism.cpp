#include "application.h"

int main()
{
	Application* win = new Application(1280, 720, "Antism");
	win->run(); 

	return 0;
}