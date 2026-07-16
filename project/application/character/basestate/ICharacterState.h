#pragma once
#include <memory>

namespace MyGame {
    // 前方宣言
    class BaseCharacter;

    /// <summary>
    /// キャラクターの行動ステート基底クラス
    /// 各ステートはこれを継承する
    /// </summary>
    class ICharacterState {
    public: // メンバ関数
        virtual ~ICharacterState() = default;
        /// <summary>
        /// ステート開始時に呼ばれる
        /// </summary>
        /// <param name="character">対象キャラクター</param>
        virtual void Enter(BaseCharacter& character) {}
        /// <summary>
        /// 毎フレーム更新
        /// </summary>
        /// <param name="character">対象キャラクター</param>
        virtual void Update(BaseCharacter& character) = 0;
        /// <summary>
        /// ステート終了時に呼ばれる
        /// </summary>
        /// <param name="character">対象キャラクター</param>
        virtual void Exit(BaseCharacter& character) {}
    };
}