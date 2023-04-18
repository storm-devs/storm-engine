#pragma once

#include "link_describe.hpp"

#include <dx9render.h>
#include <entity.h>
#include <v_sound_service.h>

#include <string>

enum eSoundState
{
    SOUND_STARTING,
    SOUND_PLAYING,
    SOUND_STOPPED
};

struct ScreenScale
{
    float x = 1.f;
    float y = 1.f;
};

class LegacyDialog final : public Entity
{
  public:
    static VDX9RENDER *RenderService;

    ~LegacyDialog() noexcept override;

    bool Init() override;

    void ProcessStage(Stage stage, uint32_t delta) override;

    void Realize(uint32_t deltaTime);

    uint32_t AttributeChanged(ATTRIBUTES *attributes) override;

    uint64_t ProcessMessage(MESSAGE &msg) override;

  private:
    void LoadIni();

    void UpdateScreenSize();

    void CreateBackBuffers();
    void UpdateBackBuffers();
    void DrawBackground(size_t start, size_t count);

    void UpdateHeadModel(const std::string &headModelPath);
    void DrawHeadModel(uint32_t deltaTime);

    void SetAction(std::string action);

    void UpdateLinks();
    void DrawLinks();

    void UpdateDialogText();
    void DrawDialogText();

    void ProcessControls();

    void PlayTick();
    void Unfade();

    storm::dialog::DlgLinkDescribe linkDescribe_;

    std::vector<std::string> formattedDialogText_;
    std::string dialogText_;

    std::string characterName_;
    std::string headModelPath_;

    ScreenScale screenScale_{};

    VSoundService *soundService_ = nullptr;

    float fontScale_{};

    int32_t lineHeight_{};
    int32_t textureLines_{};

    int32_t mainFont_{};
    int32_t nameFont_{};
    int32_t subFont_{};

    int32_t interfaceTexture_{};

    int32_t backVertexBuffer_{};
    int32_t backIndexBuffer_{};

    int32_t fadeTime_{};

    entid_t headModel_{invalid_entity};

    std::string mood_ = "normal";

    int32_t soundState_ = SOUND_STOPPED;
    int32_t currentSound_ = 0;
    std::string soundName_;

    bool dialogNeedsUpdate_ = false;
    bool backNeedsUpdate_ = true;
};