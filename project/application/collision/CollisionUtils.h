#pragma once
#include <OBB.h>
#include <Transform.h>
#include <MatrixVector.h>

namespace MyGame {
    class CollisionUtils {
    public:
        /// <summary>
        /// Transform情報からOBBを生成する共通関数
        /// </summary>
        static MyEngine::OBB CreateOBB(const  MyEngine::Transform& transform, const  MyEngine::Vector3& sizeOffset = { 1.0f, 1.0f, 1.0f }) {
            MyEngine::OBB obb;
            // 中心座標
            obb.center = transform.translate;

            // オブジェクトによって倍率を変えたい場合は引数の倍率を変える
            obb.halfSize = { transform.scale.x * sizeOffset.x, transform.scale.y * sizeOffset.y, transform.scale.z * sizeOffset.z };
            // 回転行列の作成
            MyEngine::Matrix4x4 rotX = MyEngine::MatrixVector::MakeRotateXMatrix(transform.rotate.x);
            MyEngine::Matrix4x4 rotY = MyEngine::MatrixVector::MakeRotateYMatrix(transform.rotate.y);
            MyEngine::Matrix4x4 rotZ = MyEngine::MatrixVector::MakeRotateZMatrix(transform.rotate.z);

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