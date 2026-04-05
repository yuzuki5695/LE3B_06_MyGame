#include "TitleScene.h"
#include <SceneManager.h>
#include <SceneName.h>
#include <Input.h>
#include <CameraManager.h>

using namespace MyEngine;

namespace MyGame {

	void TitleScene::Finalize() {}

	void TitleScene::Initialize() {	
		CameraManager::GetInstance()->Initialize(Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -50.0f }));	
	
	}

	void TitleScene::Update() {
		CameraManager::GetInstance()->Update();

		if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
			SceneManager::GetInstance()->ChangeScene(SceneName::GAMEPLAY);
		}
	}

	void TitleScene::Draw() {

	}
}