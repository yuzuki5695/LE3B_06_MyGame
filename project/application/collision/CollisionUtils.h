#pragma once
#include <OBB.h>
#include <MatrixVector.h>
#include <Object3d.h>

namespace MyGame {
    /// <summary>
	/// 衝突判定に関する共通関数をまとめたクラス
    /// </summary>
    class CollisionUtils {
    public:
        /// <summary>
        /// Object3d情報からOBBを生成する共通関数
        /// </summary>
        static MyEngine::OBB CreateOBB(const MyEngine::Object3d* object, const MyEngine::Vector3& sizeOffset = { 1.0f, 1.0f, 1.0f }) {
            MyEngine::OBB obb;
            // 中心座標
            obb.center = object->GetTransform().translate;     
            MyEngine::Vector3 size = object->GetModel()->GetSize();
            // オブジェクトによって倍率を変えたい場合は引数の倍率を変える
            obb.halfSize = {
                size.x * object->GetTransform().scale.x * sizeOffset.x * 0.5f,
                size.y * object->GetTransform().scale.y * sizeOffset.y * 0.5f,
                size.z * object->GetTransform().scale.z * sizeOffset.z * 0.5f
            };
            // 回転行列の作成
            MyEngine::Matrix4x4 rotX = MyEngine::MatrixVector::MakeRotateXMatrix(object->GetTransform().rotate.x);
            MyEngine::Matrix4x4 rotY = MyEngine::MatrixVector::MakeRotateYMatrix(object->GetTransform().rotate.y);
            MyEngine::Matrix4x4 rotZ = MyEngine::MatrixVector::MakeRotateZMatrix(object->GetTransform().rotate.z);

            // Z→X→Y の順で合成
            MyEngine::Matrix4x4 rotMat = MyEngine::MatrixVector::Multiply(MyEngine::MatrixVector::Multiply(rotZ, rotX), rotY);

            // 各軸を抽出
            obb.axis[0] = MyEngine::MatrixVector::Normalize(MyEngine::MatrixVector::Multiply4x4x3(rotMat, { 1, 0, 0 }));
            obb.axis[1] = MyEngine::MatrixVector::Normalize(MyEngine::MatrixVector::Multiply4x4x3(rotMat, { 0, 1, 0 }));
            obb.axis[2] = MyEngine::MatrixVector::Normalize(MyEngine::MatrixVector::Multiply4x4x3(rotMat, { 0, 0, 1 }));

            return obb;
        }
    };
}