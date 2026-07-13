#include "SceneEmitterManager.h"
#include <GamePlayParticle.h>
#include <GameOverParticle.h>
#include <TitleParticle.h>
#include <GameClearParticle.h>
#include <SceneManager.h>
#include <SceneName.h>

using namespace MyEngine;

namespace MyGame {

    // 静的メンバ変数の定義
    std::unique_ptr<SceneEmitterManager> SceneEmitterManager::instance = nullptr;

    // シングルトンインスタンスの取得
    SceneEmitterManager* SceneEmitterManager::GetInstance() {
        if (!instance) {
            instance = std::make_unique<SceneEmitterManager>();
        }
        return instance.get();
    }

    // 終了
    void SceneEmitterManager::Finalize() {
        Clear();
        // シーンマネージャのインスタンスの解放
        instance.reset();
    }

    void SceneEmitterManager::Initialize() {
        // 古いシーンのエミッターを破棄する
        Clear();

        // 現在のシーン状況を確認
        // SceneManagerが管理している実行中シーンから名前を取得する
        std::string currentScene = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();

        // シーン名に紐付いたパーティクルクラスをインスタンス化
        if (currentScene == SceneName::GAMEPLAY) {                          // ゲームプレイシーンのパーティクルエミッターを生成
            AddUI(SceneName::GAMEPLAY, std::make_unique<GamePlayParticle>());
        } else if (currentScene == SceneName::GAMEOVER) {                   // ゲームオーバーシーンのパーティクルエミッターを生成
            AddUI(SceneName::GAMEOVER, std::make_unique<GameOverParticle>());
        } else if (currentScene == SceneName::TITLE) {                      // タイトルシーンのパーティクルエミッターを生成
            AddUI(SceneName::TITLE, std::make_unique<TitleParticle>());
        } else if (currentScene == SceneName::GAMECLEAR) {                      // ゲームクリアシーンのパーティクルエミッターを生成
            AddUI(SceneName::GAMECLEAR, std::make_unique<GameClearParticle>());
        }

        // 生成したエミッターの個別初期化
        for (std::unique_ptr<SceneParticleBase>& emitter : emitterlist_) {
            emitter->Initialize();
        }
    }

    void SceneEmitterManager::Update() {
        // リストに登録されているエミッターを更新
        for (std::unique_ptr<SceneParticleBase>& emitter : emitterlist_) {
            emitter->Update();
        }
    }

    void SceneEmitterManager::AddUI(const std::string& sceneName, std::unique_ptr<SceneParticleBase> emitter) {
        // どのシーンに属しているかの情報をUI自身に持たせてリストに追加
        emitter->SetTargetSceneName(sceneName);
        emitterlist_.push_back(std::move(emitter));
    }

    void SceneEmitterManager::Clear() {
        // エミッターを消す前に、まずGPU側のパーティクルバッファやリクエストを完全に根こそぎクリアする
        // これにより、バッファ参照の迷子（浮遊カウンタ）を防ぎます。
        if (ParticleManager::GetInstance()) {
            ParticleManager::GetInstance()->ClearAll();
        }
        // 管理している全UIの破棄
        emitterlist_.clear();
    }
}