#pragma once
#include <IMenuComponent.h>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace MyEngine {
    namespace Editor {
        /// <summary>
        /// エディタ上部のメニューバー管理クラス
        /// 各メニューコンポーネントを保持する責務を持つ
        /// </summary>
        class MenuBar {
        public: // メンバ関数
            /// <summary>
            /// メニューバー全体を描画する
            /// 登録済みの全メニューコンポーネントに対して
            /// Render を呼び出す。
            /// </summary>
            /// <param name="LT">
            /// 多言語文字列取得用ラムダ
            /// </param>
            void Render(const std::function<std::string(const std::string&)>& LT);
            
            /// <summary>
            /// メニューコンポーネントを追加する
            /// </summary>
            /// <type Param name="T">
            /// IMenuComponent を継承したメニュー型
            /// </type Param>
            /// <type Param name="Args">
            /// コンストラクタ引数型
            /// </type Param>
            /// <param name="args">
            /// メニュー生成時に渡す引数
            /// </param>
            /// <returns>
            /// 生成されたメニューオブジェクトのポインタ
            /// </returns>
            template<typename T, typename... Args>
            T* AddMenu(Args&&... args) {
                // IMenuComponent 継承チェック
                static_assert(std::is_base_of_v<IMenuComponent, T>, "T must inherit IMenuComponent");
                // メニュー生成
                std::unique_ptr<T> menu = std::make_unique<T>(std::forward<Args>(args)...);
                // 生ポインタを保持
                // （外部からアクセスするため）
                T* ptr = menu.get();
                // 管理リストへ追加
                menuComponents_.push_back(std::move(menu));
                return ptr;
            }
               
            /// <summary>
            /// 指定型のメニューを取得する
            /// </summary>
            /// <type Param name="T">
            /// 取得したいメニュー型
            /// </type Param>
            /// <returns>
            /// 見つかった場合はメニューポインタ、
            /// 見つからない場合は nullptr
            /// </returns>
            template<typename T>
            T* GetMenu() {
               // 登録済みメニューを走査
                for (const std::unique_ptr<IMenuComponent>& menu: menuComponents_) {
                    // 型変換
                    T* casted = dynamic_cast<T*>(menu.get());
                    // 成功したら返す
                    if (casted) {
                        return casted;
                    }
                }
                // 見つからなかったらスルー
                return nullptr;
            }
        private: // メンバ変数
            std::vector<std::unique_ptr<IMenuComponent>> menuComponents_;  /// 登録済みメニューコンポーネント
        };
    }
}