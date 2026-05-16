#include "SceneMenu.h"
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <SceneManager.h>
#include <SceneFactory.h>

namespace MyEngine {
    void SceneMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
		// 「Scene」メニュー開始
        if (!ImGui::BeginMenu(LT("Menu.Scene").c_str())) {
            return;
        }
        // シーンマネージャ取得
        SceneManager* sceneManager = SceneManager::GetInstance();
		// シーンファクトリー取得
        AbstractSceneFactory* factory = sceneManager->GetSceneFactory();

        if (factory) {
			// シーン名のリストを取得
            const std::vector<std::string> scenes = factory->GetSceneNames();
			// 現在のシーン名を取得
            const std::string currentScene = sceneManager->GetCurrentSceneName();
			// シーン名のリストをループしてメニューアイテムを作成
            for (const std::string& sceneName : scenes) {
				// 現在のシーンと同じかどうかを判定して、チェックマークを表示
                bool selected = (sceneName == currentScene);
                // Localization取得
                std::string label = LT("Scene." + sceneName);
				// メニューアイテムを作成。選択されたとき、現在のシーンと同じでなければシーンを切り替える
                if (ImGui::MenuItem(label.c_str(), nullptr, selected)) {
					// シーン切り替え
                    if (!selected) {
                        sceneManager->ChangeScene(sceneName);
                    }
                }
            }
        }
		//  Sceneメニュー終了
        ImGui::EndMenu();
#endif
    }
}