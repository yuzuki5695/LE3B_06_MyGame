#pragma once

namespace Editor {
    // 言語設定の列挙体
    enum class Language {
        Japanese, // 日本語
        English   // 英語
        // 将来的に French, Chinese 等を追加可能
    };

    // メッセージIDの列挙体
    enum class MessageId {
		EngineInitialized,   // エンジンの初期化が完了したときのメッセージ
		SaveCompleted,       // システムの重要なイベントを表すメッセージ
		LangChanged,         //  言語設定が変更されたときのメッセージ
    };
}