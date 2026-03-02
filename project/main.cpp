#include "MyGame.h"
#include <TimeSystem.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { 
	TimeSystem::Initialize();   // 時間計測開始

	Framework* game = new MyGame();

	game->Run();

	delete game;

	return 0;
}
