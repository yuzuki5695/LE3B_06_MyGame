#include "SettingWindow.h"
#include "LogManager.h"
#include <externals/imgui/imgui.h>

SettingWindow::SettingWindow() {
    isOpen = false; // 設定画面は最初閉じているのが一般的
}

void SettingWindow::OnMenuBar() {
    // "Options" メニューの中に項目を追加
    if (ImGui::BeginMenu(MessageService::GetText("Editor.Menu.Options").c_str())) {
        if (ImGui::MenuItem(MessageService::GetText("Editor.Menu.Settings").c_str())) {
            isOpen = true; // 設定ウィンドウを開く
        }
        ImGui::EndMenu();
    }
}

void SettingWindow::Draw() {
    if (!isOpen) return;

    ImGui::Begin(MessageService::GetText("Editor.Settings.Title").c_str(), &isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::Text("%s", MessageService::GetText("Editor.Settings.Language").c_str());

        //// 現在の言語設定を取得
        //Editor::Language current = MessageService::GetLanguage();

        //// 日本語
        //if (ImGui::RadioButton(MessageService::GetText("Editor.Settings.Japanese").c_str(), current == Editor::Language::Japanese)) {
        //    SetLanguage(Editor::Language::Japanese);
        //}

        //ImGui::SameLine();

        //// 英語
        //if (ImGui::RadioButton(MessageService::GetText("Editor.Settings.English").c_str(), current == Editor::Language::English)) {
        //    SetLanguage(Editor::Language::English);
        //}

        //ImGui::Separator();

        //if (ImGui::Button(MessageService::GetText("Editor.Settings.Close").c_str())) {
        //    isOpen = false;
        //}
    }
    ImGui::End();
}

void SettingWindow::SetLanguage(Editor::Language lang) {
    MessageService::SetLanguage(lang);
    // ログに通知
    LogManager::GetInstance()->AddLog(Editor::MessageId::LangChanged);
}