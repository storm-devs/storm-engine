// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationServiceImp
// --------------------------------------------------------------------------------------------
// Animation service for creating AnimationManager objects
// ============================================================================================

#pragma once

#include "animation.h"
#include "animation_info.h"
#include "v_module_api.h"

//============================================================================================

class INIFILE;
class AnimationImp;

class AnimationServiceImp final : public AnimationService
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AnimationServiceImp();
    ~AnimationServiceImp() override;

    // Phase for running the animation
    uint32_t RunSection() override;
    // Execution functions
    void RunStart() override;
    void RunEnd() override;
    // Create animation for the model, delete using "delete"
    Animation *CreateAnimation(const char *animationName) override;

    // --------------------------------------------------------------------------------------------
    // Functions for Animation
    // --------------------------------------------------------------------------------------------
    // Remove animation (called from destructor)
    void DeleteAnimation(AnimationImp *ani);
    // Event
    void Event(const char *eventName);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // load animation
    int32_t LoadAnimation(const char *animationName);
    // Load user data from the current section
    void LoadUserData(INIFILE *ani, const char *sectionName, std::unordered_map<std::string, std::string> &data,
                      const char *animationName);
    // load AN
    bool LoadAN(const char *fname, AnimationInfo *info);

    std::vector<AnimationInfo *> ainfo;
    std::vector<AnimationImp *> animations;

    static char key[1024];
};

//============================================================================================
