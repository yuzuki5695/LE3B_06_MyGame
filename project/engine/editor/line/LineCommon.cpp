#include "LineCommon.h"
#include <ShaderCompiler.h>
#include <externals/DirectXTex/d3dx12.h>
#include <Matrix4x4.h>

namespace MyEngine {

	using Microsoft::WRL::ComPtr;

      // 静的メンバ変数の定義
    std::unique_ptr<LineCommon> LineCommon::instance = nullptr;

    // シングルトンインスタンスの取得
    LineCommon* LineCommon::GetInstance() {
        if (!instance) {
            instance = std::make_unique<LineCommon>();
        }
        return instance.get();
    }

    // 終了
    void LineCommon::Finalize() {
        instance.reset();  // `delete` 不要
    }

	void LineCommon::Initialize(DirectXCommon* dxCommon) {
		assert(dxCommon);
		// 引数を受け取ってメンバ変数に記録する
		dxCommon_ = dxCommon;
		// パイプラインの生成
		PipelineGenerate();
	}

    void LineCommon::Commondrawing() {
        dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
        dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
        dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

	void LineCommon::PipelineGenerate() {
		// === RootSignature ===
		CD3DX12_ROOT_PARAMETER rootParams[1];

		// viewProj行列をRootConstantsとして頂点シェーダに渡す
		rootParams[0].InitAsConstants(sizeof(Matrix4x4) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_ROOT_SIGNATURE_DESC rsDesc;

		// 入力アセンブラを使うRootSignatureとして設定する
		rsDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> rsBlob;
		ComPtr<ID3DBlob> errorBlob;

		// RootSignatureをシリアライズする
		HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob);

		// シリアライズに失敗した場合はエラー内容を出力する
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			assert(false);
		}

		// RootSignatureを作成する
		hr = dxCommon_->GetDevice()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		// === シェーダロード ===
		// ライン描画用の頂点シェーダとピクセルシェーダを読み込む
		Microsoft::WRL::ComPtr<IDxcBlob> vsBlob = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Debug/LineRenderer.VS.hlsl", L"vs_6_0");
		Microsoft::WRL::ComPtr<IDxcBlob> psBlob = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Debug/LineRenderer.PS.hlsl", L"ps_6_0");

		// === 頂点レイアウト ===
		// 1頂点につき、位置と色を持つ
		D3D12_INPUT_ELEMENT_DESC inputElems[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			  0, D3D12_APPEND_ALIGNED_ELEMENT,
			  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		// === 4) ラスタライザ / ブレンド / 深度 ===
		D3D12_RASTERIZER_DESC rastDesc{};
		rastDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rastDesc.CullMode = D3D12_CULL_MODE_NONE;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthClipEnable = TRUE;

		// アルファブレンドを有効にする
		D3D12_BLEND_DESC blendDesc{};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// 深度テストを有効にして、3D空間上の位置関係に合わせる
		D3D12_DEPTH_STENCIL_DESC depthDesc{};
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//depthDesc.DepthEnable = FALSE;
		//depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthDesc.StencilEnable = FALSE;

		// === 5) PSO 設定 ===
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

		// RootSignatureとシェーダを設定する
		psoDesc.pRootSignature = rootSignature.Get();
		psoDesc.InputLayout = { inputElems, _countof(inputElems) };
		psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
		psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

		// 描画状態を設定する
		psoDesc.RasterizerState = rastDesc;
		psoDesc.BlendState = blendDesc;
		psoDesc.DepthStencilState = depthDesc;
		psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// ライン描画用のPRIMITIVETOPOLOGYTYPETRIANGLEにする
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// レンダーターゲットと深度フォーマットを設定する
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;

		// PSOを作成する
		hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));
	}
}