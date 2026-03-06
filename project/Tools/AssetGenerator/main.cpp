#include <iostream>

#include "ResourceScanner.h"
#include "ManifestBuilder.h"
#include "JsonExporter.h"

int main()
{
    std::string resourceDir = "Resources";

    ResourceScanner scanner(resourceDir);

    auto files = scanner.Scan();

    ManifestBuilder builder;

    auto manifest = builder.Build(files);

    JsonExporter::Save("Resources/fest.json", manifest);

    std::cout << "Manifest generated\n";

    return 0;
}