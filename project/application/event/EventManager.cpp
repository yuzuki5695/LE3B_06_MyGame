#include "EventManager.h"

// シングルトン用インスタンス
EventManager* EventManager::instance = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
EventManager* EventManager::GetInstance() {
    // まだインスタンスが生成されていなければ作成
    if (instance == nullptr) {
        instance = new EventManager;
    }
    return instance;
}

///====================================================
/// 終了処理
///====================================================
void EventManager::Finalize() {
    // インスタンスを削除してnullptrに戻す
    delete instance;
	instance = nullptr;   
}

void  EventManager::Initialize() {

}

///====================================================
/// 更新処理
///====================================================
void EventManager::Update() {

}
	
void  EventManager::Drawo3Dbject(){

}

void  EventManager::Draw2DSprite(){

}