#pragma once
#include <BaseUI.h>
#include <GamePlayUI.h>

class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    void Initialize();
    void Update();
    void Draw();
    void Clear();

    void AddUI(const std::string& sceneName, std::unique_ptr<BaseUI> ui);
    void CreateAndAddUI(const std::string& sceneName);

private:
    std::vector<std::unique_ptr<BaseUI>> uiList_;
};