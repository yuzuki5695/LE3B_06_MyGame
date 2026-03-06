//#include "AssetPipeline.h"
//
//#include "ResourceScanner.h"
//#include "ManifestBuilder.h"
//#include "JsonExporter.h"
//
//#include <iostream>
//
//void AssetPipeline::Execute()
//{
//    // 1. ScanFile
//    ResourceScanner scanner(resourceDir_);
//    auto files = scanner.ScanFile();
//
//    // 2. Build
//    ManifestBuilder builder;
//    auto manifest = builder.Build(files);
//
//    // 3. Export
//    JsonExporter::Save(outputPath_, manifest);
//
//    std::cout << "Generated: " << outputPath_ << std::endl;
//}