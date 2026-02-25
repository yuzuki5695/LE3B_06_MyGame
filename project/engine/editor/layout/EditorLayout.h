#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include <SrvManager.h>

/// <summary>
/// エディタ全体のレイアウト・外観を管理するクラス
/// メニューバーやパネルの配置など、画面の枠組みを構築する。
/// </summary>
class EditorLayout {
public: // メンバ関数
    EditorLayout() = default;
    ~EditorLayout() = default;
    /// <summary>
    /// エディタ全体のレイアウトを描画するメイン関数
    /// </summary>
    /// <param name="srvmanager">SRV管理クラス</param>
    /// <param name="windows">表示対象のウィンドウリスト</param>
    void Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows);
private:  // 内部関数
    /// <summary>
    /// 上部メインメニューバーを表示する
    /// </summary>
    /// <param name="windows">ウィンドウリスト（表示切り替え用）</param>
    void ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows);
private: // メンバ変数

};