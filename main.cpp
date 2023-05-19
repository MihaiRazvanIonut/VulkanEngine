#include "Application.hpp"

int main() {

	Application* CyanCrate = new Application(true, 640, 480);

	CyanCrate->runApplication();
	delete CyanCrate;

	return 0;
}