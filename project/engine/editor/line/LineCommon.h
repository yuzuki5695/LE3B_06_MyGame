#pragma once
#include <DirectXCommon.h>

namespace MyEngine {
    /// <summary>
    /// 
    /// </summary>
    class LineCommon {
    private:
        static std::unique_ptr<LineCommon> instance;

        LineCommon(LineCommon&) = delete;
        LineCommon& operator=(LineCommon&) = delete;
    public: // メンバ関数
        LineCommon() = default;
        ~LineCommon() = default;

        // シングルトンインスタンスの取得
        static LineCommon* GetInstance();
        // 終了
        void Finalize();
        // 初期化
        void Initialize(DirectXCommon* dxCommon);
        // 共通描画設定
		void Commondrawing();
    private:
        void PipelineGenerate();
    private:
        DirectXCommon* dxCommon_ = nullptr;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
    public:
        // getter
        DirectXCommon* GetDxCommon() const { return  dxCommon_; }
    };
}