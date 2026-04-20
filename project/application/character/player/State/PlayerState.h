#pragma once
#include <ICharacterState.h>

namespace MyGame {
    /// <summary>
	/// プレイヤーの待機状態クラス
    /// </summary>
    class PlayerStateIdle : public ICharacterState {
	public: // メンバ関数
        void Update(BaseCharacter& character) override;
    };

    /// <summary>
	/// プレイヤーの移動状態クラス
    /// </summary>
    class PlayerStateMove : public ICharacterState {
	public: // メンバ関数
        void Update(BaseCharacter& character) override;
    };
 
    /// <summary>
	/// プレイヤーの死亡状態クラス
    /// </summary>
    class PlayerStateDead : public ICharacterState {
	public: // メンバ関数
        void Update(BaseCharacter& character) override;
    };
}