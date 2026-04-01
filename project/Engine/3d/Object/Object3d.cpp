#include "Object3d.h"
#include <Object3dCommon.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <MatrixVector.h>
#include <ModelManager.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <numbers>
#include <CameraManager.h>
#include <ResourceFactory.h>

namespace MyEngine {

    using namespace MatrixVector;
    using namespace ResourceFactory;

    Vector3 Object3d::GetWorldPosition() const {
        Matrix4x4 worldMatrix = MatrixVector::MakeAffineMatrix(
            transform_.scale, transform_.rotate, transform_.translate
        );

        Vector3 worldPos = {
            worldMatrix.m[3][0],
            worldMatrix.m[3][1],
            worldMatrix.m[3][2]
        };

        return worldPos;
    }

    void Object3d::Initialize(Object3dCommon* object3dCommon) {
        // NULL検出
        assert(object3dCommon);
        // 引数で受け取ってメンバ変数に記録する
        this->object3dCommon = object3dCommon;
        this->camera = object3dCommon->GetDefaultCamera();
        // マテリアル用のリソースの生成、初期化
        MaterialGenerate();
        // WVP,World用のリソースの生成、初期化
        TransformationMatrixGenerate();
        // カメラリソースの生成、初期化
        CameraForGPUGenerate();
    }

    void Object3d::Update() {
        // ワールド行列の作成
        Matrix4x4 worldMatrix = MakeAftineMatrix(transform_.scale, transform_.rotate, transform_.translate);
        // ワールド・ビュー・プロジェクション行列
        Matrix4x4 worldViewProjectionMatrix;
        // カメラを CameraManager 経由で取得
        Camera* activeCamera = CameraManager::GetInstance()->GetActiveCamera();
        if (activeCamera) {
            const Matrix4x4& viewProjectionMatrix = activeCamera->GetViewProjectionMatrix();
            worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

            // カメラのワールド座標をGPU用に渡す
            cameraForGPUData->worldPosition = activeCamera->GetTranslate();
        } else {
            worldViewProjectionMatrix = worldMatrix;
            // カメラがない場合もデフォルト位置にしておく
            cameraForGPUData->worldPosition = { 0.0f, 0.0f, -1000.0f };
        }

        transformationMatrixData->WVP = worldViewProjectionMatrix;
        transformationMatrixData->World = worldMatrix;
        // WorldInverseTranspose行列を再計算
        transformationMatrixData->WorldInverseTranspose = InverseTranspose(worldMatrix);
    }

    void Object3d::Draw() {
        // 座標変化行列CBufferの場所を設定
        object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

        // カメラの場所を設定 
        object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());

        // 3Dモデルが割り当てられていれば描画する
        if (model) {
            // マテリアルCBufferの場所を設定
            object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
            model->Draw();
        }
    }

    void Object3d::MaterialGenerate() {
        // マテリアル用のリソース
        materialResource = CreateBufferResource(object3dCommon->GetDxCommon()->GetDevice(), sizeof(Material));
        // マテリアル用にデータを書き込むためのアドレスを取得
        materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
        // マテリアルデータの初期値を書き込む
        materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
        // SpriteはLightingしないでfalseを設定する
        materialData->endbleLighting = true;
        // 単位行列を書き込んでおく
        materialData->uvTransform = MakeIdentity4x4();
        // 光沢度を書き込む
        materialData->shinimess = 70;
    }

    void Object3d::TransformationMatrixGenerate() {
        // WVP,World用のリソースを作る。TransformationMatrixを用意する
        transformationMatrixResource = CreateBufferResource(object3dCommon->GetDxCommon()->GetDevice(), sizeof(TransformationMatrix));
        // データを書き込むためのアドレスを取得
        transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
        // 単位行列を書き込んでおく
        transformationMatrixData->WVP = MakeIdentity4x4();
        transformationMatrixData->World = MakeIdentity4x4();
        // WorldInverseTransposeを計算してセット
        transformationMatrixData->WorldInverseTranspose = InverseTranspose(transformationMatrixData->World);
    }

    void Object3d::CameraForGPUGenerate() {
        // カメラ用リソースを作る
        cameraResource = CreateBufferResource(object3dCommon->GetDxCommon()->GetDevice(), sizeof(Object3d::CameraForGPU));
        // 書き込むためのアドレスを取得
        cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData));
        // 単位行列を書き込んでおく
        if (camera) {
            cameraForGPUData->worldPosition = camera->GetTranslate();
        } else {
            // カメラがない場合デフォルト位置にしておく
            cameraForGPUData->worldPosition = { 0.0f, 0.0f, -100.0f };
        }
    }

    void Object3d::SetModel(const std::string& filePath) {
        // モデルを検索してセットする
        model = ModelManager::GetInstance()->FindModel(filePath);
    }

    void Object3d::SetMaterialColor(const Vector4& color) {

    }

    std::unique_ptr<Object3d> Object3d::Create(std::string filePath, Transform transform) {
        std::unique_ptr<Object3d> object3d = std::make_unique<Object3d>();
        // 初期化
        object3d->Initialize(Object3dCommon::GetInstance());
        // モデルを検索してセットする
        object3d->model = ModelManager::GetInstance()->FindModel(filePath);
        // 初期色をセット
        object3d->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        // 座標をセット
        object3d->transform_ = transform;
        return object3d;
    }

    void Object3d::DrawImGui(const std::string& name) {
#ifdef USE_IMGUI

        ImGui::PushID(this);

        //====================================================
        // Object Info
        //====================================================
        ImGui::Text("Object Info");
        ImGui::Separator(); {
            ImGui::Text("Name : %s", name.c_str());
            ImGui::Separator();

            //====================================================
            // Resource Section
            //====================================================
            if (ImGui::TreeNode("Resource")) {

                // 開発中エリア
                ImGui::BeginDisabled();
                ImGui::Text("Model : (Under Development)");
                ImGui::Text("Texture : (Under Development)");
                ImGui::EndDisabled();

                ImGui::Spacing();

                ImGui::BeginDisabled();
                ImGui::Button("Change Model");
                ImGui::SameLine();
                ImGui::Text(" (Under Development)");
                ImGui::EndDisabled();

                ImGui::TreePop();
            }

            ImGui::Separator();

            //====================================================
            // Transform
            //====================================================
            if (ImGui::TreeNode("Transform")) {

                ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
                ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
                ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);

                ImGui::TreePop();
            }

            ImGui::Separator();

            //====================================================
            // Material
            //====================================================
            if (ImGui::TreeNode("Material")) {

                ImGui::ColorEdit4("Color",
                    reinterpret_cast<float*>(&materialData->color));

                ImGui::DragFloat("Shininess",
                    &materialData->shinimess,
                    0.1f, 0.0f, 256.0f);

                ImGui::TreePop();
            }
        }

        ImGui::PopID();

#endif
    }
}