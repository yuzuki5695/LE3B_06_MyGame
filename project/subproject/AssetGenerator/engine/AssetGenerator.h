#pragma once
#include <filesystem>
#include <vector>
#include <RuleLoader.h>

class AssetGenerator {
public:
    void Run(int argc, char* argv[]);

private:
    void Initialize(int argc, char* argv[]);
    void Execute();
    void Finalize();

private:
    std::filesystem::path resourceRoot_;
    std::filesystem::path outputDir_;
    std::filesystem::path rulesJsonPath_;

    std::vector<ExportRule> rules_;
};