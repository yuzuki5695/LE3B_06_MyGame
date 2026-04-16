#pragma once
#include <Camera.h>
#include <unordered_map>
#include <memory>
#include <string>

namespace MyEngine {
    /// <summary>
    /// カメラセット構造体
    /// メインカメラ、サブカメラを管理する
    /// </summary>
    struct CameraSet {
        std::unique_ptr<Camera> mainCamera;                                      /// メインとなるカメラ実体
        std::unordered_map<std::string, std::unique_ptr<Camera>> subCameras;     /// サブカメラ群
        std::string activeSubCameraName;                                         /// 現在選択中のサブカメラ名 
		Camera* activeCamera = nullptr;                                          /// 現在有効なカメラのポインタ

        ///// <summary>
        //// サブカメラ登録
        ///// </summary>
        //void AddSubCamera(const std::string& name, std::unique_ptr<Camera> cam) { subCameras[name] = std::move(cam); }

        //// サブカメラを有効化
        //void SetActiveSubCamera(const std::string& name) {
        //    auto it = subCameras.find(name);
        //    if (it != subCameras.end()) {
        //        activeSubCameraName = name;
        //    }
        //}

        //// メインに戻す
        //void SetMainCamera() {
        //    activeSubCameraName.clear();
        //    activeCamera = mainCamera.get();
        //}

        //// サブカメラ中か判定
        //bool IsUsingSubCamera() const {
        //    return !activeSubCameraName.empty();
        //}
    };
}