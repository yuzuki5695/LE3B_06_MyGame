#include "EventManager.h"

namespace MyGame {

    // シングルトン用インスタンス
    EventManager* EventManager::instance_ = nullptr;

    ///====================================================
    /// シングルトンインスタンスの取得
    ///====================================================
    EventManager* EventManager::GetInstance() {
        // まだインスタンスが生成されていなければ作成
        if (instance_ == nullptr) {
            instance_ = new EventManager;
        }
        return instance_;
    }

    ///====================================================
    /// 終了処理
    ///====================================================
    void EventManager::Finalize() {
        // インスタンスを削除してnullptrに戻す
        delete instance_;
        instance_ = nullptr;
    }
    ///====================================================		
    /// イベント開始
    ///====================================================
    void EventManager::EventStart(Event::EventState state) {
        switch (state) {
        case Event::EventState::GameStart:
            currentEvent_ = std::make_unique<StartEvent>();
            currentEvent_->Initialize();
            break;
        }
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void EventManager::Update() {
        if (currentEvent_) {
            currentEvent_->Update();

            if (currentEvent_->IsFinished()) {
                currentEvent_.reset();
            }
        }
    }
    ///====================================================
    /// 描画処理
    ///====================================================
    void EventManager::Draw2D() {
        if (currentEvent_) {
            currentEvent_->Draw2D();
        }
    }

    bool EventManager::IsActive() const {
        return currentEvent_ && !currentEvent_->IsFinished();
    }
}