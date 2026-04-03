#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <AssetInfoLoader.h>

namespace AssetGen {
    /// <summary>
    /// C++コード（ヘッダファイル）の自動生成を担当するクラス
    /// </summary>
    class CodeGenerator {
	public: // メンバ関数

        struct Node {
            std::map<std::string, Node> children;
            std::vector<const AssetDefinition*> assets;
        };

        /// <summary>
        /// AssetInfoLoaderのデータから LoadResourceID.h を生成する
        /// </summary>
        /// <param name="assets">生成元となるアセット定義リスト</param>
        /// <param name="outputPath">出力するヘッダのパス</param>
        static void GenerateResourceIDHeader(const std::vector<AssetDefinition>& assets, const std::filesystem::path& outputPath);

        /// <summary>
        /// AssetDefinition のリストからカテゴリごとのツリー構造を構築する
        /// </summary>
        static std::map<std::string, Node> BuildCategoryTrees(const std::vector<AssetDefinition>& assets);

        /// <summary>
        /// ノードを再帰的に走査して namespace とアセット定義を書き出す
        /// </summary>
        static void WriteNode(std::ofstream& ofs, const Node& node, int indent);

        /// <summary>
        /// ヘッダファイルの本体を書き出す
        /// </summary>
        static void WriteHeader(std::ofstream& ofs, const std::map<std::string, Node>& categoryTrees);
    };
}