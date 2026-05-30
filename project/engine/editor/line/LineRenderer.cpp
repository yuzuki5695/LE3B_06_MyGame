#include "LineRenderer.h"
#include <LineCommon.h>
#include <MatrixVector.h>
#include <CameraManager.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

namespace MyEngine {

    using namespace MatrixVector;
    using namespace LineTypes;

    // 静的メンバ変数の定義
    std::unique_ptr<LineRenderer> LineRenderer::instance = nullptr;

    // シングルトンインスタンスの取得
    LineRenderer* LineRenderer::GetInstance() {
        if (!instance) {
            instance = std::make_unique<LineRenderer>();
        }
        return instance.get();
    }

    // 終了
    void LineRenderer::Finalize() {
        instance.reset();  // `delete` 不要
    }

    void LineRenderer::Initialize() {
#ifdef USE_IMGUI
        CreateBuffer();
#endif // USE_IMGUI
    }

    void LineRenderer::CreateBuffer() {
#ifdef USE_IMGUI
        const UINT size = sizeof(LineVertex) * kMaxVertexCount;

        // UploadHeapで作る
        D3D12_HEAP_PROPERTIES heapProps{};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        HRESULT hr = LineCommon::GetInstance()->GetDxCommon()->GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexResource)
        );

        assert(SUCCEEDED(hr));

        // Map
        vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertices));
        // VBView
        vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
        vertexBufferView.SizeInBytes = size;
        vertexBufferView.StrideInBytes = sizeof(LineVertex);
#endif // USE_IMGUI
    }

    void LineRenderer::Draw(const Matrix4x4& viewProj) {
#ifdef USE_IMGUI
        // 頂点がないなら描画しない
        if (vertexIndex_ == 0) {
            return;
        }

        LineCommon::GetInstance()->Commondrawing();

        // viewProjをRootConstantsに渡す
        LineCommon::GetInstance()->GetDxCommon()->GetCommandList()->SetGraphicsRoot32BitConstants(0, sizeof(Matrix4x4) / 4, &viewProj, 0);
        LineCommon::GetInstance()->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        LineCommon::GetInstance()->GetDxCommon()->GetCommandList()->DrawInstanced(vertexIndex_, 1, 0, 0);
#endif // USE_IMGUI
    }

    void LineRenderer::BeginFrame() {
#ifdef USE_IMGUI
        // 1フレーム分のライン情報をクリアする
        lines_.clear();
        vertexIndex_ = 0;
#endif // USE_IMGUI
    }

    void LineRenderer::AddLine(const LineData& line) {
#ifdef USE_IMGUI
        if (vertexIndex_ + kVerticesPerLine >= kMaxVertexCount) {
            return;
        }

        if (!CameraManager::GetInstance()->GetActiveCamera()) { return; }

        //========================
        // ライン方向
        //========================
        Vector3 lineDir = Normalize(line.end - line.start);
        //========================
        // カメラ前方向
        //========================
        Vector3 cameraForward = CameraManager::GetInstance()->GetActiveCamera()->GetForward();
        //========================
        // 板ポリ幅方向
        //========================
        Vector3 side = Normalize(Cross(cameraForward, lineDir));

        side *= debug_.thickness * 0.5f;

        //========================
        // 頂点生成
        //========================
        Vector3 v0 = line.start + side;
        Vector3 v1 = line.end + side;
        Vector3 v2 = line.start - side;
        Vector3 v3 = line.end - side;

        // triangle1
        mappedVertices[vertexIndex_++] = { v0,line.color };
        mappedVertices[vertexIndex_++] = { v1,line.color };
        mappedVertices[vertexIndex_++] = { v2,line.color };

        // triangle2
        mappedVertices[vertexIndex_++] = { v2,line.color };
        mappedVertices[vertexIndex_++] = { v1,line.color };
        mappedVertices[vertexIndex_++] = { v3,line.color };
#endif // USE_IMGUI
    }

    void LineRenderer::AddAABB(const Vector3& center, const Vector3& size, const Vector4& color) {
        float hx = size.x * 0.5f;
        float hy = size.y * 0.5f;
        float hz = size.z * 0.5f;

        Vector3 p[8] = {
            {center.x - hx, center.y - hy, center.z - hz},
            {center.x + hx, center.y - hy, center.z - hz},
            {center.x - hx, center.y + hy, center.z - hz},
            {center.x + hx, center.y + hy, center.z - hz},

            {center.x - hx, center.y - hy, center.z + hz},
            {center.x + hx, center.y - hy, center.z + hz},
            {center.x - hx, center.y + hy, center.z + hz},
            {center.x + hx, center.y + hy, center.z + hz},
        };

        auto add = [&](int a, int b) {AddLine({ p[a],p[b],color }); };

        // front
        add(0, 1);
        add(1, 3);
        add(3, 2);
        add(2, 0);

        // back
        add(4, 5);
        add(5, 7);
        add(7, 6);
        add(6, 4);

        // connect
        add(0, 4);
        add(1, 5);
        add(2, 6);
        add(3, 7);
    }

    void LineRenderer::AddOBB(const OBB& obb, const Vector4& color) {
#ifdef USE_IMGUI

        // 軸方向ベクトル（半サイズ反映）
        Vector3 x = obb.axis[0] * obb.halfSize.x;
        Vector3 y = obb.axis[1] * obb.halfSize.y;
        Vector3 z = obb.axis[2] * obb.halfSize.z;

        //=========================
        // OBBの8頂点生成
        //=========================
        Vector3 p[8];

        p[0] = obb.center - x - y - z;
        p[1] = obb.center + x - y - z;
        p[2] = obb.center - x + y - z;
        p[3] = obb.center + x + y - z;

        p[4] = obb.center - x - y + z;
        p[5] = obb.center + x - y + z;
        p[6] = obb.center - x + y + z;
        p[7] = obb.center + x + y + z;

        auto add = [&](int a, int b) {AddLine({ p[a], p[b], color });};

        //=========================
        // 前面
        //=========================
        add(0, 1);
        add(1, 3);
        add(3, 2);
        add(2, 0);

        //=========================
        // 背面
        //=========================
        add(4, 5);
        add(5, 7);
        add(7, 6);
        add(6, 4);

        //=========================
        // 接続辺
        //=========================
        add(0, 4);
        add(1, 5);
        add(2, 6);
        add(3, 7);

#endif
    }

    void LineRenderer::DrawImGui(Vector3& size) {
#ifdef USE_IMGUI
        if (ImGui::TreeNode("LineRenderer")) {

            //--------------------------------
            // Debug
            //--------------------------------
            ImGui::Checkbox("表示、非表示", &debug_.enable);                      // 描画の表示、非表示
            ImGui::DragFloat3("サイズ", &size.x, 0.01f, 0.1f, 20.0f);     // サイズ
            //ImGui::ColorEdit4("カラー", &debug_.color.x);                        // カラー
            ImGui::Separator();        
            ImGui::Text("当たり判定");
            if (debug_.isHit) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "状態 : ヒット中");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "状態 : 未ヒット");
            }
            //--------------------------------
            // Line
            //--------------------------------
            ImGui::DragFloat("ラインの太さ", &debug_.thickness, 0.0001f, 0.001f, 0.1f);       // 線の太さ調整
            //-------------------------------- 
            // Buffer Usage            
            //--------------------------------
            const float usageRate = static_cast<float>(vertexIndex_) / static_cast<float>(kMaxVertexCount);
            char overlay[128];
            sprintf_s(overlay, "%u / %u 頂点 (%.1f%%)", vertexIndex_, kMaxVertexCount, usageRate * 100.0f);
            ImGui::Text("頂点バッファ使用率");
            ImGui::ProgressBar(usageRate, ImVec2(-FLT_MIN, 0.0f), overlay);
            //--------------------------------
            // Info
            //--------------------------------
            ImGui::Text("ライン数 : %zu", lines_.size());
            ImGui::Text("頂点数 : %u", vertexIndex_);
            ImGui::TreePop();
        }
#endif // USE_IMGUI
    }
}