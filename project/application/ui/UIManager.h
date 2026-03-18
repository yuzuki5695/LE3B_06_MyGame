#pragma once
#include <BaseUI.h>
#include <vector>
#include <string>
#include <memory>

/// <summary>
/// 全シーンのUIを統括管理するマネージャクラス（シングルトン）
/// シーンの切り替えを検知し、そのシーンに必要なUIクラスの生成・更新・描画を自動で行う。
/// </summary>
class UIManager {
private: // シングルトンインスタンス
    static std::unique_ptr<UIManager> instance;

    // シングルトン設計のためコピーを禁止
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
public:// メンバ関数
    UIManager() = default;
    ~UIManager() = default;
    /// <summary>
    /// シングルトンインスタンスの取得
    /// </summary>
    /// <returns>UIManagerのポインタ</returns>
    static UIManager* GetInstance();
    /// <summary>
    /// インスタンスの破棄
    /// </summary>
    void Finalize();
    /// <summary>
    /// 現在のシーンに応じたUIの再構築
    /// SceneManagerから現在のシーン名を取得し、古いUIを破棄して新しいUIを生成する。
    /// </summary>
    void Initialize();
    /// <summary>
    /// UIの更新処理
    /// リストに登録されている（＝現在のシーン用の）全UIのUpdateを呼び出す。
    /// </summary>
    void Update();
    /// <summary>
    /// UIの描画処理
    /// リストに登録されている全UIのDrawを呼び出す。通常は透明なスプライト等の重なりを考慮し最後に呼ぶ。
    /// </summary>
    void Draw();
    /// <summary>
    /// 内部リストへのUI追加
    /// </summary>
    /// <param name="sceneName">そのUIが所属するシーン名</param>
    /// <param name="ui">UIインスタンスの所有権</param>
    void AddUI(const std::string& sceneName, std::unique_ptr<BaseUI> ui);
    /// <summary>
    /// 管理している全UIの破棄
    /// </summary>
    void Clear();
private: // メンバ変数
    // 現在のシーンで有効なUIインスタンスを保持するリスト
    std::vector<std::unique_ptr<BaseUI>> uiList_;
public:
    // UIManager.h 内の public メンバに追加
    template <typename T>
    T* GetUI() {
        for (auto& ui : uiList_) {
            // dynamic_cast で指定した型 (T) に変換できるか試行
            T* target = dynamic_cast<T*>(ui.get());
            if (target) {
                return target;
            }
        }
        return nullptr;
    }
};