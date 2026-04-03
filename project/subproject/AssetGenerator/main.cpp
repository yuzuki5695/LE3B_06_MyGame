#include <AssetGenerator.h>

using namespace AssetGen;

int main(int argc, char* argv[]) {
    // ジェネレーターの生成
    AssetGenerator app; 
    // 一連の処理を実行
    app.Run(argc, argv); 
    return 0;
}