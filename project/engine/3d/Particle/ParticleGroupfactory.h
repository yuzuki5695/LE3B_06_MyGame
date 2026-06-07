#pragma once
#include <string>
#include <memory>
#include <d3d12.h>
#include <wrl/client.h>
#include <Vector4.h> // 必要に応じて
#include <MatrixVector.h>
#include <ParticleModel.h>
#include <ParticleData.h>

namespace MyEngine {
    // マネージャ側で使う前方宣言
    class DirectXCommon;
    class SrvManager;

	class ParticleGroupfactory {
	private:
		static std::unique_ptr<ParticleGroupfactory> instance;

		ParticleGroupfactory(ParticleGroupfactory&) = delete;
		ParticleGroupfactory& operator=(ParticleGroupfactory&) = delete;
	public: // メンバ関数
		ParticleGroupfactory() = default;
		~ParticleGroupfactory() = default;

		// シングルトンインスタンスの取得
		static ParticleGroupfactory* GetInstance();
		// 終了
		void Finalize();

		// 戻り値：true = 既存のものを再利用・更新した（新規作成不要） / false = 既存になし（新規作成が必要）
		bool Initialize(const std::string& name, const std::string& textureFilepath, std::unordered_map<std::string, ParticleGroup>* particleGroups);

		// ─── 💡 【追加】グループを生成して初期化まで完了したオブジェクトを返す関数 ───
		ParticleGroup CreateNewGroup(DirectXCommon* dxCommon, SrvManager* srvManager, const std::string& filename, const std::string& textureFilepath, uint32_t maxInstanceCount);

	};
}