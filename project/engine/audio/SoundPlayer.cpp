#include "SoundPlayer.h"
#include <cassert>

namespace MyEngine {
    // 静的メンバ変数の定義
    std::unique_ptr<SoundPlayer> SoundPlayer::instance_ = nullptr;

    // シングルトンインスタンスの取得
    SoundPlayer* SoundPlayer::GetInstance() {
        if (!instance_) {
            instance_ = std::make_unique<SoundPlayer>();
        }
        return instance_.get();
    }

    // 終了
    void SoundPlayer::Finalize() {
        instance_.reset();
    }

    void SoundPlayer::Initialize(SoundLoader* soundLoader) {
        // NULL検出
        assert(soundLoader);
        // メンバ変数に記録
        this->soundLoader_ = soundLoader;
    }

    void SoundPlayer::SoundPlayWave(const SoundData& soundData, bool loop, float volume) {
        HRESULT result;

        result = soundLoader_->GetIXAudio2()->CreateSourceVoice(&psourcevoice_, &soundData.wfex);
        assert(SUCCEEDED(result));

        // 音量設定
        psourcevoice_->SetVolume(volume);
        assert(SUCCEEDED(result));

        XAUDIO2_BUFFER buf{};
        buf.pAudioData = soundData.pBuffer;
        buf.AudioBytes = soundData.bufferSize;

        if (loop) {
            buf.LoopCount = XAUDIO2_LOOP_INFINITE;
        } else {
            buf.Flags = XAUDIO2_END_OF_STREAM;
        }

        result = psourcevoice_->SubmitSourceBuffer(&buf);
        assert(SUCCEEDED(result));

        result = psourcevoice_->Start();
        assert(SUCCEEDED(result));
    }

    void SoundPlayer::SoundUnload(SoundData* soundData) {
        // バッファのメモリを解放
        delete[] soundData->pBuffer;
        soundData->pBuffer = 0;
        soundData->bufferSize = 0;
        soundData->wfex = {};
        if (psourcevoice_) {
            psourcevoice_->Stop(0);
            psourcevoice_->FlushSourceBuffers();
            psourcevoice_->DestroyVoice();
            psourcevoice_ = nullptr;
        }
    }
}