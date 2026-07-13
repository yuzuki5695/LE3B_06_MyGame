#include "ParticleCommon.h"
#include <Logger.h>
#include <StringUtility.h>
#include <ShaderCompiler.h>
#include <DsvManager.h>

using namespace Microsoft::WRL;

namespace MyEngine {
    // 静的メンバ変数の定義
    std::unique_ptr<ParticleCommon> ParticleCommon::instance_ = nullptr;

    // シングルトンインスタンスの取得
    ParticleCommon* ParticleCommon::GetInstance() {
        if (!instance_) {
            instance_ = std::make_unique<ParticleCommon>();
        }
        return instance_.get();
    }

    // 終了
    void ParticleCommon::Finalize() {
        instance_.reset();  // `delete` 不要
    }

    void ParticleCommon::Initialize(DirectXCommon* dxCommon, DsvManager* dsvManager) {
        assert(dxCommon);
        assert(dsvManager);
        // 引数を受け取ってメンバ変数に記録する
        dxCommon_ = dxCommon;
        dsvManager_ = dsvManager;
        // グラフィックスパイプラインの生成
        GraphicsPipelineGenerate();
        UpdatePipelineGenerate();
        SpawnPipelineGenerate();
    }

    void ParticleCommon::Commondrawing() {
        // RootSignatureを設定。PSOに設定しているけど別途設定が必要
        dxCommon_->GetCommandList()->SetGraphicsRootSignature(graphicsPipeline_.rootSignature.Get());
        dxCommon_->GetCommandList()->SetPipelineState(graphicsPipeline_.pipelineState.Get());
        // 形状を設定。PSOに設定しているものとはまた別。同じものを設定する
        dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void ParticleCommon::CommandUpdate() {
        // RootSignatureを設定。PSOに設定しているけど別途設定が必要
        dxCommon_->GetCommandList()->SetComputeRootSignature(updatePipeline_.rootSignature.Get());
        dxCommon_->GetCommandList()->SetPipelineState(updatePipeline_.pipelineState.Get());
    }

    void ParticleCommon::CommandSpawn() {
        // RootSignatureを設定。PSOに設定しているけど別途設定が必要 
        dxCommon_->GetCommandList()->SetComputeRootSignature(spawnPipeline_.rootSignature.Get());
        dxCommon_->GetCommandList()->SetPipelineState(spawnPipeline_.pipelineState.Get());
    }

    void ParticleCommon::CreateRootSignature(ID3D12Device* device, const D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>& rootSignature) {
        HRESULT hr;
        // シリアライズしてバイナリにする
        ComPtr <ID3DBlob> signatureBlob = nullptr;
        ComPtr <ID3DBlob> errorBlob = nullptr;
        hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (FAILED(hr)) {
            Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }
        // バイナリを元に作成
        hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));
    }

    void ParticleCommon::RootSignatureGenerate() {

        /*----------------------------------------------------------------------------------*/
        /*---------------------------------DescriptorRange作成-------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].BaseShaderRegister = 0;
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // Instancing作成
        D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
        descriptorRangeForInstancing[0].BaseShaderRegister = 0; // 0から始める
        descriptorRangeForInstancing[0].NumDescriptors = 1; // 数は1つ
        descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
        descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        /*----------------------------------------------------------------------------------*/
        /*---------------------------------RootParameter作成---------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_ROOT_PARAMETER rootParameters[4] = {};
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;// レジスタ番号0を使う

        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
        rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
        rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableで利用する数

        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//利用する数

        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// PixelShaderで使う
        rootParameters[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

        /*----------------------------------------------------------------------------------*/
        /*---------------------------------RootSignature作成---------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        descriptionRootSignature.pParameters = rootParameters;// ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);// 配列の長さ

        /*----------------------------------------------------------------------------------*/
        /*-----------------------------------Samplerの設定-----------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
        staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ	
        staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート		
        staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない		
        staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う	
        staticSamplers[0].ShaderRegister = 0;//レジスタ番号０を使う
        staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
        descriptionRootSignature.pStaticSamplers = staticSamplers;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

        // ルートシグネチャの作成
        CreateRootSignature(dxCommon_->GetDevice().Get(), descriptionRootSignature, graphicsPipeline_.rootSignature);
    }

    void ParticleCommon::GraphicsPipelineGenerate() {
        HRESULT hr;
        // ルートシグネチャの生成
        RootSignatureGenerate();

        /*----------------------------------------------------------------------------------*/
        /*---------------------------------InputLayout設定-----------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
        inputElementDescs[0].SemanticName = "POSITION";
        inputElementDescs[0].SemanticIndex = 0;
        inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[1].SemanticName = "TEXCOORD";
        inputElementDescs[1].SemanticIndex = 0;
        inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[2].SemanticName = "NORMAL";
        inputElementDescs[2].SemanticIndex = 0;
        inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElementDescs;
        inputLayoutDesc.NumElements = _countof(inputElementDescs);

        /*----------------------------------------------------------------------------------*/
        /*---------------------------------BlendStateの設定-----------------------------------*/
        /*----------------------------------------------------------------------------------*/
        D3D12_BLEND_DESC blendDesc{};
        //全ての色要素を書き込む
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;    // alpha も同様に
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        //===== RasterizerStateの設定を行う ======//   
        D3D12_RASTERIZER_DESC rasterizerDesc{};
        // 裏面(時計回り)を表示しない
        //rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        // 裏面(時計回り)を表紙する
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        // 三角形の中を塗りつぶす
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

        /*----------------------------------------------------------------------------------*/
        /*--------------------------------ShaderをCompile-----------------------------------*/
        /*----------------------------------------------------------------------------------*/
        ComPtr <IDxcBlob> vertexShaderBlob = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Particle/Particle.VS.hlsl", L"vs_6_0");
        assert(vertexShaderBlob != nullptr);
        ComPtr <IDxcBlob> pixelShaderBlob = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Particle/Particle.PS.hlsl", L"ps_6_0");
        assert(pixelShaderBlob != nullptr);

        /*-----------------------------------------------------------------------------------*/
        /*-------------------------------------PSO生成----------------------------------------*/
        /*-----------------------------------------------------------------------------------*/
        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
        graphicsPipelineStateDesc.pRootSignature = graphicsPipeline_.rootSignature.Get();   // RootSignature
        graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;    // InputLayout
        graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };  // VertexShader
        graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };    // PixelShader
        graphicsPipelineStateDesc.BlendState = blendDesc;// BlendState
        graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
        // 書き込むRTVの情報
        graphicsPipelineStateDesc.NumRenderTargets = 1;
        graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        // 利用するトポロジ(形状)のタイプ
        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        // どのように画面に色を打ち込むかの設定(気にしなくて良い)
        graphicsPipelineStateDesc.SampleDesc.Count = 1;
        graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        // DepthStencilの設定
        graphicsPipelineStateDesc.DepthStencilState = dsvManager_->GetDepthStencilDesc();
        graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // 実際に生成
        hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipeline_.pipelineState));
        assert(SUCCEEDED(hr));
    }
   
    void ParticleCommon::UpdateRootSignatureGenerate() {
        //---------------------------------
        // DescriptorRange
        //---------------------------------
        D3D12_DESCRIPTOR_RANGE descriptorRanges[3] = {};
        // u0 : Particle
        descriptorRanges[0].BaseShaderRegister = 0;
        descriptorRanges[0].NumDescriptors = 1;
        descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        // u1 : FreeList
        descriptorRanges[1].BaseShaderRegister = 1;
        descriptorRanges[1].NumDescriptors = 1;
        descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        // u2 : FreeCounter
        descriptorRanges[2].BaseShaderRegister = 2;
        descriptorRanges[2].NumDescriptors = 1;
        descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        //---------------------------------
        // RootParameter
        //---------------------------------
        D3D12_ROOT_PARAMETER rootParameters[5] = {};
        // root0 : u0 Particle
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
        rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
        // root1 : u1 FreeList
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
        rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
        // root2 : u2 FreeCounter
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
        // root3 : b0 ParticleInfo
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[3].Descriptor.ShaderRegister = 0;
        // root4 : b1 AttractInfo
        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[4].Descriptor.ShaderRegister = 1;
        // RootSignatureDesc
        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);

        // ルートシグネチャの作成
        CreateRootSignature(dxCommon_->GetDevice().Get(), desc, updatePipeline_.rootSignature);
    }

    void ParticleCommon::UpdatePipelineGenerate() {
        HRESULT hr;
        // ルートシグネチャの生成
        UpdateRootSignatureGenerate();
        // Shader Compile
        ComPtr<IDxcBlob> computeShaderBlob = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Particle/ParticleUpdate.CS.hlsl", L"cs_6_0");
        assert(computeShaderBlob != nullptr);
        // PSO Desc
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
        desc.pRootSignature = updatePipeline_.rootSignature.Get();
        desc.CS = { computeShaderBlob->GetBufferPointer(),computeShaderBlob->GetBufferSize() };
        // 実際に生成
        hr = dxCommon_->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&updatePipeline_.pipelineState));
        assert(SUCCEEDED(hr));
    }

    void ParticleCommon::SpawnRootSignatureGenerate() {
        D3D12_DESCRIPTOR_RANGE ranges[3] = {};
        // u0 : ParticleBuffer
        ranges[0].BaseShaderRegister = 0;
        ranges[0].NumDescriptors = 1;
        ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        // u1 : FreeList
        ranges[1].BaseShaderRegister = 1;
        ranges[1].NumDescriptors = 1;
        ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // u2 : FreeCounter
        ranges[2].BaseShaderRegister = 2;
        ranges[2].NumDescriptors = 1;
        ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


        D3D12_ROOT_PARAMETER rootParameters[5] = {};
        //========================= 
        // root0 : u0 Particle 
        //=========================
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
        rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
        //-----------------------------------------
        // u1 FreeList
        //-----------------------------------------
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[1].DescriptorTable.pDescriptorRanges = &ranges[1];
        rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
        //-----------------------------------------
        // u2 FreeCounter
        //-----------------------------------------
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &ranges[2];
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
        //-----------------------------------------
        // b0 GroupSpawnCB
        //-----------------------------------------
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[3].Descriptor.ShaderRegister = 0;
        //-----------------------------------------
        // t0 SpawnRequests
        //-----------------------------------------
        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[4].Descriptor.ShaderRegister = 0;

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);

        // ルートシグネチャの作成
        CreateRootSignature(dxCommon_->GetDevice().Get(), desc, spawnPipeline_.rootSignature);
    }

    void ParticleCommon::SpawnPipelineGenerate() {
        HRESULT hr;
        // ルートシグネチャの生成
        SpawnRootSignatureGenerate();
        // Shader Compile
        ComPtr<IDxcBlob>  shader = ShaderCompiler::GetInstance()->CompileShader(L"Resources/shaders/Particle/ParticleSpawn.CS.hlsl", L"cs_6_0");
        assert(shader != nullptr);
        // PSO Desc
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
        desc.pRootSignature = spawnPipeline_.rootSignature.Get();
        desc.CS = { shader->GetBufferPointer(), shader->GetBufferSize() };
        // 実際に生成
        hr = dxCommon_->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&spawnPipeline_.pipelineState));
        assert(SUCCEEDED(hr));
    }
}