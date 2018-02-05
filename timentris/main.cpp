#include <map>
#include <memory>
#include "Configuration.h"
#include "GLWindow.h"
#include "Input.h"
#include "TimentrisGame.h"
#include "Render.h"

int main(int argc, char* argv[]) {
	GLWindowPtr glWindow( new GLWindow(Configuration::getScreenWidth(), Configuration::getScreenHeight(), Configuration::getIsFullscreen()) );
	RenderPtr render( new Render() );
	render->attachWindow(glWindow);
	TimentrisGamePtr game( new TimentrisGame() );
	game->attachRender(render);
	render->attachGame(game);
	InputPtr input( new Input() );
	input->attachGame(game);
	input->attachRender(render);
	render->attachInput(input);

	input->run();
	game->run();
	render->run(); 
	return 0;
}