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

#include "Animation.h"
#include "AnimationInfo.h"
#include "vmodule_api.h"

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
    ~AnimationServiceImp();

    // Phase for running the animation
    virtual uint32_t RunSection();
    // Execution functions
    virtual void RunStart();
    virtual void RunEnd();
    // Create animation for the model, delete using "delete"
    virtual Animation *CreateAnimation(const char *animationName);

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
    long LoadAnimation(const char *animationName);
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
