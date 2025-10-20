#pragma once
#include <OBB.h>
#include<Transform.h>

class CollisionManager {
public:
    /// <summary>
    /// 2つのOBBが衝突しているか判定（SAT）
    /// </summary>
    static bool CheckOBB(const OBB& a, const OBB& b);

    /// <summary>
    /// 任意のゲームオブジェクト同士の衝突をチェックする
    /// </summary>
    template<typename T1, typename T2>
    static bool CheckCollision(const T1& objA, const T2& objB) {
        return CheckOBB(objA.GetOBB(), objB.GetOBB());
    }

    OBB GetBulletOBB(Transform transform,Vector3 scalar) const;

    
    bool IsOBBIntersect(const OBB& a, const OBB& b);

};