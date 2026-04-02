#include "Model.h"
#include <Object3d.h>
#include <MatrixVector.h>
#include <TextureManager.h>
#include <ResourceFactory.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace MyEngine::MatrixVector;
using namespace MyEngine::ResourceFactory;

namespace MyEngine {
    void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename) {
        // NULL検出
        assert(modelCommon);
        // 引数で受け取ってメンバ変数に記録する
        this->modelCommon = modelCommon;

        // 拡張子を取得して小文字化
        std::string extension = filename.substr(filename.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        // モデルデータ格納用
        bool isGLTF = false;

        // 拡張子で分岐
        if (extension == "obj") {
            modelData = LoadObjFile(directorypath, filename);
        } else if (extension == "gltf" || extension == "glb") {
            glTFModelData gltfData = LoadModelFile(directorypath, filename);

            // glTFモデルをModelDateに変換して統一（描画コードの共通化）
            modelData.vertices = gltfData.vertices;
            modelData.material = gltfData.material;
            isGLTF = true;
        } else {
            assert(false && "Unsupported model format (only .obj, .gltf, .glb are supported)");
        }
        // 頂点データの作成
        VertexDatacreation();

        // .objの参照しているテクスチャ読み込み
        TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
        // 読み込んだテクスチャの番号を取得
        modelData.material.textureindex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);
    }

    void Model::Draw() {
        // VertexBufferViewの設定
        modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        //SRVのDescriptortableの先頭を設定。２はrootParameter[2]である。
        //SRVを切り替えて画像を変えるS
        modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));

        // 描画！(今回は球)
        modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
    }

    void Model::VertexDatacreation() {

        // 関数化したResouceで作成
        vertexResource = CreateBufferResource(modelCommon->GetDxCommon()->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

        //頂点バッファビューを作成する
        // リソースの先頭のアドレスから使う
        vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
        // 使用するリソースのサイズはの頂点のサイズ
        vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
        // 1頂点当たりのサイズ
        vertexBufferView.StrideInBytes = sizeof(VertexData);

        // 頂点リソースにデータを書き込むためのアドレスを取得
        vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
        // 頂点データをリソースにコピー
        std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
    }

    MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
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
    
    void  Model::ProcessMesh(aiMesh* mesh, std::vector<VertexData>& vertices, bool flipX) {
        assert(mesh->HasNormals());          // 法線がないMeshは今回は非対応
        assert(mesh->HasTextureCoords(0));;  // TexcoordがないMeshは今回は非対応
        // Meshの中身(Face)を解析していく
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3);// 三角形のみサポート
            // Faceの中身(Vertex)を解析していく
            for (uint32_t i = 0; i < face.mNumIndices; ++i) {
                uint32_t vertexIndex = face.mIndices[i];
                // 頂点の各要素を取得
                aiVector3D& position = mesh->mVertices[vertexIndex];
                aiVector3D& normal = mesh->mNormals[vertexIndex];
                aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
                // 1頂点分のデータを構築してModelDateに追加していく
                VertexData vertex{};
                vertex.position = { -position.x,position.y,position.z,1.0f }; // 位置のx成分を反転
                vertex.normal = { -normal.x,normal.y,normal.z };              // 法線のx成分を反転 
                vertex.texcoord = { texcoord.x,texcoord.y };                  // テクスチャ座標のy成分を反転
                // 左手系対応
                if (flipX) {
                    // aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
                    vertex.position.x *= -1.0f;
                    vertex.normal.x *= -1.0f;
                }
                vertices.push_back(vertex);
            }
        }
    }

    ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
        // 1. 中で必要となる変数の宣言
        ModelData modelData; // 構築するModelDate
        // 2. Assimpでの読み込み
        Assimp::Importer importer;
        std::string filePath(directoryPath + "/" + filename); // ファイルを開く
        const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
        assert(scene->HasMeshes()); // メッシュがないのは対応しない

        // 3. Meshを解析する
        for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            // 全メッシュを共通関数で処理
            ProcessMesh(scene->mMeshes[meshIndex], modelData.vertices, true);
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

    Node Model::ReadNode(aiNode* node) {
        Node result;
        // ローカル変換行列取得（Assimpは列ベクトルなので転置）
        aiMatrix4x4 aiLocalMatrix = node->mTransformation;
        aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
        result.localMatrix.m[0][0] = aiLocalMatrix[0][0]; result.localMatrix.m[0][1] = aiLocalMatrix[0][1]; result.localMatrix.m[0][2] = aiLocalMatrix[0][2]; result.localMatrix.m[0][3] = aiLocalMatrix[0][3];
        result.localMatrix.m[1][0] = aiLocalMatrix[1][0]; result.localMatrix.m[1][1] = aiLocalMatrix[1][1]; result.localMatrix.m[1][2] = aiLocalMatrix[1][2]; result.localMatrix.m[1][3] = aiLocalMatrix[1][3];
        result.localMatrix.m[2][0] = aiLocalMatrix[2][0]; result.localMatrix.m[2][1] = aiLocalMatrix[2][1]; result.localMatrix.m[2][2] = aiLocalMatrix[2][2]; result.localMatrix.m[2][3] = aiLocalMatrix[2][3];
        result.localMatrix.m[3][0] = aiLocalMatrix[3][0]; result.localMatrix.m[3][1] = aiLocalMatrix[3][1]; result.localMatrix.m[3][2] = aiLocalMatrix[3][2]; result.localMatrix.m[3][3] = aiLocalMatrix[3][3];
        result.name = node->mName.C_Str(); // Node名を格納
        result.children.resize(node->mNumChildren); // 子ノード数だけ確保
        // 再帰的に子ノードを読み込む
        for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
            result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
        }
        return result;
    }

    glTFModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
        // 1. 中で必要となる変数の宣言
        glTFModelData modelData; // 構築するModelDate
        // 2. Assimpでの読み込み
        Assimp::Importer importer;
        std::string filePath = directoryPath + "/" + filename; // ファイルを開く
        const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
        assert(scene->HasMeshes()); // メッシュがないのは対応しない
         
        // 3. Meshを解析する
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            // 全メッシュを共通関数で処理
            ProcessMesh(scene->mMeshes[meshIndex], modelData.vertices, true);
        }
        
        // 4. Materialの解析
        for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
            aiMaterial* material = scene->mMaterials[materialIndex];
            if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
                aiString textureFilePath;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
                modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
            }
        }

        // 5. ノード階層構造を取得
        modelData.rootNode = ReadNode(scene->mRootNode);
        // 6. ModelDateを返す
        return modelData;
    }
}