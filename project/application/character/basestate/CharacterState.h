#pragma once
#include <memory>
#include <ICharacterState.h>

namespace MyGame {

    // 前方宣言
    class BaseCharacter;

    /// <summary>
    /// キャラクターの現在の状態（State）を管理・実行するコンテナクラス。
    /// ステートの切り替え（遷移）と、毎フレームの更新処理の委譲を責務とする。
    /// </summary>
    class CharacterState {
	public: // メンバ関数
        /// <summary>
        /// 現在のステートを終了し、新しいステートへ遷移させる。
        /// </summary>
        /// <param name="character">状態変更の対象となるキャラクター参照</param>
        /// <param name="nextState">次に遷移するステートの所有権付きポインタ</param>
        void ChangeState(BaseCharacter& character, std::unique_ptr<ICharacterState> nextState);
        /// <summary>
        /// 現在保持しているステートのUpdate処理を実行する。
        /// </summary>
        /// <param name="character">更新処理の対象となるキャラクター参照</param>
        void Update(BaseCharacter& character);
	private: // メンバ変数
        // 現在実行中のステート（Strategy/Stateパターン）
        std::unique_ptr<ICharacterState> currentState_;
	public: // アクセッサ
		ICharacterState* GetCurrentState() const { return currentState_.get(); }
    };
}