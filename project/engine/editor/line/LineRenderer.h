#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <Matrix4x4.h>
#include <Transform.h>
#include <memory>
#include <LineTypes.h>
#include <OBB.h>

namespace MyEngine {
    /// <summary>
    /// 
    /// </summary>
    class LineRenderer {
    public:
        struct DebugSettings {
            bool enable = true;                   // デバッグ表示ON/OFF
            bool isHit = false;                   // 衝突中か
            Vector3 size = { 1.5f,1.5f,1.5f };    // サイズ
            Vector4 color = { 0,0,1,1 };          // カラー(初期は緑)
            float thickness = 0.01f;              // ラインの太さ
        };

    private:
        static std::unique_ptr<LineRenderer> instance;

        LineRenderer(LineRenderer&) = delete;
        LineRenderer& operator=(LineRenderer&) = delete;
    public: // メンバ関数
        LineRenderer() = default;
        ~LineRenderer() = default;

        // シングルトンインスタンスの取得
        static LineRenderer* GetInstance();
        // 終了
        void Finalize();
        // 初期化
        void Initialize();
        // 描画処理
        void Draw(const Matrix4x4& viewProj);
        /// <summary>
        /// フレームの開始
        /// </summary>
        void BeginFrame();
        /// <summary>
        /// ラインの生成
        /// </summary>
        /// <param name="line"></param>
        void AddLine(const LineTypes::LineData& line);
        /// <summary>
        /// ライン生成(AABB)
        /// </summary>
        /// <param name="center"></param>
        /// <param name="size"></param>
        /// <param name="color"></param>
        void AddAABB(const Vector3& center, const Vector3& size, const Vector4& color);
        /// <summary>
        /// ライン生成(OBB)
        /// </summary>
        void AddOBB(const OBB& obb, const Vector4& color);
        /// <summary>
        /// imgui描画
        /// </summary>
        /// <param name="size"></param>
        void DrawImGui(Vector3& size);
    private:
        /// <summary>
        /// 頂点リソースの作成とデータの転送
        /// </summary>
        void CreateBuffer();

        static constexpr uint32_t kMaxLineCount = 8192;
        static constexpr uint32_t kVerticesPerLine = 6;
        static constexpr uint32_t kMaxVertexCount = kMaxLineCount * kVerticesPerLine;
    private:
        std::vector<LineTypes::LineData> lines_;
        uint32_t vertexIndex_ = 0;
        DebugSettings debug_;
        // バッファリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
        // バッファリソースの使い道を補足するバッファビュー
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        // バッファリソース内のデータを指すポインタ
        LineTypes::LineVertex* mappedVertices = nullptr;
    public:
        // getter
        float GetThickness() const { return debug_.thickness; }
        std::vector<LineTypes::LineData> GetLineData() { return lines_; }
        const DebugSettings& GetDebugSettings() const { return debug_; }
        // setter
        void SetThickness(float thickness) { debug_.thickness = thickness; }
        void SetSize(const Vector3& size) { debug_.size = size; }
        void SetColor(const Vector4& color) { debug_.color = color; }
        void SetShow(bool enable) { debug_.enable = enable; }
        void SetHit(bool hit) { debug_.isHit = hit; }
    };
}