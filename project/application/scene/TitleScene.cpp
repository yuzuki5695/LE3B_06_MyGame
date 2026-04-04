#include "TitleScene.h"
#include <SceneManager.h>
#include <SceneName.h>
#include <Input.h>

using namespace MyEngine;

namespace MyGame {
	void TitleScene::Finalize() {

	}

	void TitleScene::Initialize() {

	}

	void TitleScene::Update() {
		if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
			SceneManager::GetInstance()->ChangeScene(SceneName::TITLE);
		}
	}

	void TitleScene::Draw() {

	}
}