#include <AssetPipeline.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // パイプラインの生成  
    std::unique_ptr<AssetPipeline> pipeline = std::make_unique<AssetPipeline>();

    try {
        // 実行処理
        pipeline->Run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}