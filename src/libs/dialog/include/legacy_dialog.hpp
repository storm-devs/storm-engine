#pragma once

#include <dx9render.h>
#include <entity.h>

class LegacyDialog final : Entity
{
  public:
    static VDX9RENDER *RenderService;

    ~LegacyDialog() noexcept override;

    bool Init() override;

    void ProcessStage(Stage stage, uint32_t delta) override;

    void Realize(uint32_t deltaTime);

  private:
    int32_t mainFont_{};
    int32_t nameFont_{};
    int32_t subFont_{};
};