#pragma once
#include <BaseUI.h>

class TitleUI : public BaseUI {
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;

private: 
    std::unique_ptr<Sprite> ui_start_;
};
