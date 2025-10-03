#include "Model.h"
#include "Object3d.h"
#include "MatrixVector.h"
#include "TextureManager.h"
#include <ResourceFactory.h>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

using namespace MatrixVector;
using namespace ResourceFactory;

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename) {
    // NULL検出
    assert(modelCommon);
    // 引数で受け取ってメンバ変数に記録する
    this->modelCommon = modelCommon;

    // モデル読み込み
    modelDate = LoadObjFile(directorypath, filename);

    // 頂点データの作成
    VertexDatacreation();
    // マテリアルの生成、初期化
    MaterialGenerate();

    // .objの参照しているテクスチャ読み込み
    TextureManager::GetInstance()->LoadTexture(modelDate.material.textureFilePath);
    // 読み込んだテクスチャの番号を取得
    modelDate.material.textureindex = TextureManager::GetInstance()->GetSrvIndex(modelDate.material.textureFilePath);
}

void Model::Draw() {
    // VertexBufferViewの設定
    modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    // マテリアルCBufferの場所を設定
    modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    //SRVのDescriptortableの先頭を設定。２はrootParameter[2]である。
    //SRVを切り替えて画像を変えるS
    modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelDate.material.textureFilePath));

    // 描画！(今回は球)
    modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelDate.vertices.size()), 1, 0, 0);
}

void Model::VertexDatacreation() {

    // 関数化したResouceで作成
    vertexResoruce = CreateBufferResource(modelCommon->GetDxCommon()->GetDevice(), sizeof(VertexData) * modelDate.vertices.size());

    //頂点バッファビューを作成する
    // リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResoruce->GetGPUVirtualAddress();
    // 使用するリソースのサイズはの頂点のサイズ
    vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelDate.vertices.size());
    // 1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // 頂点リソースにデータを書き込むためのアドレスを取得
    vertexResoruce->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    // 頂点データをリソースにコピー
    std::memcpy(vertexData, modelDate.vertices.data(), sizeof(VertexData) * modelDate.vertices.size());
}

void Model::MaterialGenerate() {
    // マテリアル用のリソース
    materialResource = CreateBufferResource(modelCommon->GetDxCommon()->GetDevice(), sizeof(Material));
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


MaterialDate Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    // 1. 中で必要となる変数の宣言
    MaterialDate materialDate; // 構築するMaterialDate
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

ModelDate Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    // 1. 中で必要となる変数の宣言
    ModelDate modelDate; // 構築するModelDate
    // 2. Assimpでの読み込み
    Assimp::Importer importer;
    std::string filePath(directoryPath + "/" + filename); // ファイルを開く
    const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

    // 3. Meshを解析する
    for (int32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
        assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応
        // Meshの中身(Face)を解析していく
        for (int32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); // 三角形のみサポート
            // Faceの中身(Vertex)を解析していく
            for (int32_t elememt = 0; elememt < face.mNumIndices; ++elememt) {
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
                modelDate.vertices.push_back(vertex);
            }
        }
    }

    // 4. Materialの解析
    for (int32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelDate.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}  
    }
    
    // 5. ModelDateを返す
    return modelDate;
}