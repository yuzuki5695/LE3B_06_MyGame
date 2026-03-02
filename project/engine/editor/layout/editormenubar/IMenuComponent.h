#pragma once
#include <functional>
#include <string>

/// <summary>
/// メニューバー内の各メニュー要素の共通インターフェース。
/// EditorMenuBar はこのインターフェースを通して
/// 各メニューを描画する。
/// </summary>
class IMenuComponent
{
public:
    /// <summary>
    /// 仮想デストラクタ
    /// ポリモーフィズム使用のため必須
    /// </summary>
    virtual ~IMenuComponent() = default;
    /// <summary>
    /// メニュー描画処理
    /// </summary>
    /// <param name="LT">
    /// 多言語テキスト取得用ラムダ。
    /// キー文字列を受け取り、現在の言語に対応した文字列を返す。
    /// </param>
    virtual void Render(const std::function<std::string(const std::string&)>& LT) = 0;
};