#include "FadeManager.h"
#include <TextureManager.h>
#include <algorithm>

std::unique_ptr<FadeManager> FadeManager::instance = nullptr;

FadeManager* FadeManager::GetInstance() {
    if (!instance) instance = std::make_unique<FadeManager>();
    return instance.get();
}

void FadeManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
    dxCommon_ = dxCommon;
    srvManager_ = srvManager;



}

void FadeManager::Update() {


}


void FadeManager::Draw() {



}