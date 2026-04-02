#pragma once

namespace MyEngine {
    // ウィンドウの基底クラス
    class IEditorWindow {
    public:
        virtual ~IEditorWindow() = default;

        // 毎フレームの描画処理（派生クラスで実装する）
        virtual void Draw() = 0;

        // ウィンドウの名前（メニュー表示や識別に使用）
        virtual const char* GetName() const = 0;

        // 追加：メニュー項目を持ちたいウィンドウはこの関数をオーバーライドする
        virtual void OnMenuBar() {}

        // ウィンドウが開いているかどうかのフラグ
        bool isOpen = true;
    };
}