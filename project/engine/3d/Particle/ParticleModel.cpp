#include "ParticleModel.h"
#include <MatrixVector.h>
#include <cassert>
#include <ModelManager.h>
#include <TextureManager.h>
#include <numbers>
#include <externals/DirectXTex/d3dx12.h>
#include <Object3dCommon.h>
#include <ResourceFactory.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace Microsoft::WRL;
using namespace MyEngine::MatrixVector;
using namespace MyEngine::PrimitiveGenerator;
using namespace MyEngine::ResourceFactory;

namespace MyEngine {

    void ParticleModel::Initialize(DirectXCommon* birectxcommon, const std::string& filename) {
        // NULL検出
        assert(birectxcommon);
        // メンバ変数に記録
        this->dxCommon_ = birectxcommon;
        // マテリアルの生成と初期化
        MaterialGenerate();        
        modelData = LoadObjFile("Resources", filename);
        CreateVertexBuffer();
        std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
        //.objの参照しているテクスチャ読み込み
        TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
        // 読み込んだテクスチャの番号を取得
        modelData.material.textureindex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);
    }

    void ParticleModel::Draw() {
        // VertexBufferView を設定
        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        // マテリアル用の定数バッファを設定
        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    }

    void ParticleModel::CreateVertexBuffer() {
        // 関数化したResouceで作成
        vertexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
        //頂点バッファビューを作成する
        // リソースの先頭のアドレスから使う
        vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
        // 使用するリソースのサイズはの頂点のサイズ
        vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
        // 1頂点当たりのサイズ
        vertexBufferView.StrideInBytes = sizeof(VertexData);
        // 頂点リソースにデータを書き込むためのアドレスを取得
        vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    }

    void ParticleModel::MaterialGenerate() {
        // マテリアル用のリソース
        materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
        // マテリアル用にデータを書き込むためのアドレスを取得
        materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
        // マテリアルデータの初期値を書き込む
        materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
        materialData->enableLighting = true;
        materialData->uvTransform = MakeIdentity4x4();
    }

    MaterialData ParticleModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
        // 1. 中で必要となる変数の宣言
        MaterialData materialDate; // 構築するMaterialDate
        std::string line; // ファイルから読んだ1行を格納するもの
        std::ifstream file(directoryPath + "/" + filename); // 2.ファイルを開く
        assert(file.is_open()); // とりあえず開けなかったら止める
        // 3. 実際にファイルを読み、MaterialDateを構築していく
        while (std::getline(file, line)) {
            std::string identifier;
            std::istringstream s(line);
            s >> identifier;

            // identifierの応じた処理
            if (identifier == "map_Kd") {
                std::string textureFilename;
                s >> textureFilename;
                // 連結してファイルパスにする
                materialDate.textureFilePath = directoryPath + "/" + textureFilename;
            }
        }
        return materialDate;
    }

    ModelData ParticleModel::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
        // 1. 中で必要となる変数の宣言
        ModelData modelData; // 構築するModelDate
        // 2. Assimpでの読み込み
        Assimp::Importer importer;
        std::string filePath(directoryPath + "/" + filename); // ファイルを開く
        const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
        assert(scene->HasMeshes()); // メッシュがないのは対応しない

        // 3. Meshを解析する
        for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            aiMesh* mesh = scene->mMeshes[meshIndex];
            assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
            assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応
            // Meshの中身(Face)を解析していく
            for (size_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
                aiFace& face = mesh->mFaces[faceIndex];
                assert(face.mNumIndices == 3); // 三角形のみサポート
                // Faceの中身(Vertex)を解析していく
                for (size_t elememt = 0; elememt < face.mNumIndices; ++elememt) {
                    uint32_t vertexIndex = face.mIndices[elememt];
                    // 頂点の各要素を取得
                    aiVector3D& position = mesh->mVertices[vertexIndex];
                    aiVector3D& normal = mesh->mNormals[vertexIndex];
                    aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
                    // 1頂点分のデータを構築してModelDateに追加していく
                    VertexData vertex;
                    vertex.position = { -position.x,position.y,position.z,1.0f };// 位置のx成分を反転
                    vertex.normal = { -normal.x,normal.y,normal.z };// 法線のx成分を反転
                    vertex.texcoord = { texcoord.x,texcoord.y };// テクスチャ座標のy成分を反転
                    // aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
                    vertex.position.x *= -1.0f;
                    vertex.normal.x *= -1.0f;
                    modelData.vertices.push_back(vertex);
                }
            }
        }
        // 4. Materialの解析
        for (size_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
            aiMaterial* material = scene->mMaterials[materialIndex];
            if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
                aiString textureFilePath;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
                modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
            }
        }

        // 5. ModelDateを返す
        return modelData;
    }
}