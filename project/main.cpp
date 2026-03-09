#include "MyGame.h"
#include <TimeSystem.h>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { 
	TimeSystem::Initialize();   // 時間計測開始
	// ゲームの生成
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	// ゲームの実行
	game->Run();
	return 0;
}
