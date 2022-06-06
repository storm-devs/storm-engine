// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationImp
// --------------------------------------------------------------------------------------------
// Implementing the Animation interface
// ============================================================================================

#include "animation_imp.h"
#include "animation_service_imp.h"
#include "core.h"
#include "debug-trap.h"

//============================================================================================

// Animation Service Pointer
AnimationServiceImp *AnimationImp::aniService = nullptr;

// ============================================================================================
// Construction, destruction
// ============================================================================================

AnimationImp::AnimationImp(int32_t id, AnimationInfo *animationInfo)
{
    Assert(animationInfo);
    aniInfo = animationInfo;
    aniInfo->AddRef();
    thisID = id;
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
    {
        action[i].SetAnimation(this, i);
        timer[i].SetAnimation(this);
    }
    matrix = new CMatrix[aniInfo->NumBones()];
    memset(ae_listeners, 0, sizeof(ae_listeners));
    ae_listenersExt = nullptr;
    // Auto normalization
    isAutoNormalize = true;
    // Custom blending
    isUserBlend = true;
    // Procedural head look
    isControllableHead = false;
    headBoneIndex = 16;
    const char *dataStr = GetData("Head bone index");
    if (dataStr)
        headBoneIndex = atol(dataStr);
    customHeadAX = 0.0f;
    customHeadAY = 0.0f;
}

AnimationImp::~AnimationImp()
{
    aniInfo->RelRef();
    aniService->DeleteAnimation(this);
    delete[] matrix;
}

//--------------------------------------------------------------------------------------------
// Animation
//--------------------------------------------------------------------------------------------

// Access the action player
ActionPlayer &AnimationImp::Player(int32_t index)
{
    Assert(index >= 0 && index < ANI_MAX_ACTIONS);
    return action[index];
}

// Access the animation timer
AnimationTimer &AnimationImp::Timer(int32_t index)
{
    Assert(index >= 0 && index < ANI_MAX_ACTIONS);
    return timer[index];
}

// Events
// Set internal event
int32_t AnimationImp::SetEvent(AnimationEvent event, int32_t index, AnimationEventListener *ael)
{
    Assert(event < ae_numevents);
    if (!ael)
        return -1;
    for (int32_t i = 0; i < ANIIMP_MAXLISTENERS; i++)
        if (!ae_listeners[event][i])
        {
            ae_listeners[event][i] = ael;
            return (event << 16) | i;
        }
    return -1;
}

// Delete internal event
void AnimationImp::DelEvent(int32_t eventID)
{
    if (eventID < 0)
        return;
    const auto event = eventID >> 16;
    const auto lindex = eventID & 0xffff;
    Assert(event < ae_numevents);
    Assert(lindex < ANIIMP_MAXLISTENERS);
    Assert(ae_listeners[event][lindex]);
    ae_listeners[event][lindex] = nullptr;
}

// Set an external event handler
void AnimationImp::SetEventListener(AnimationEventListener *ael)
{
    ae_listenersExt = ael;
}

// Get the number of bones in a skeleton
int32_t AnimationImp::GetNumBones() const
{
    return aniInfo->NumBones();
}

// Get animation matrix for bone
CMatrix &AnimationImp::GetAnimationMatrix(int32_t iBone) const
{
    Assert(iBone >= 0 && iBone < aniInfo->NumBones());
    return matrix[iBone];
}

// Get custom data for animation
const char *AnimationImp::GetData(const char *dataName) const
{
    const auto &userData = aniInfo->GetUserData();
    const auto it = userData.find(dataName);
    return it != userData.end() ? it->second.c_str() : nullptr;
}

// Copy the state of one player to another
void AnimationImp::CopyPlayerState(int32_t indexSrc, int32_t indexDst, bool copyTimerState)
{
    Assert(indexSrc >= 0 && indexSrc < ANI_MAX_ACTIONS);
    Assert(indexDst >= 0 && indexDst < ANI_MAX_ACTIONS);
    if (indexSrc == indexDst)
        return;
    action[indexDst].CopyState(action[indexSrc]);
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
    {
        auto isInv = false;
        if (timer[i].IsUsedPlayer(indexSrc, &isInv))
        {
            timer[i].SetPlayer(indexDst, isInv);
        }
    }
}

// Get animation speed
float AnimationImp::GetFPS()
{
    return aniInfo->GetFPS();
}

// Set blending modes
// Automatic normalization of blending coefficients
bool AnimationImp::SetAutoNormalize(bool isNormalize)
{
    const auto b = isAutoNormalize;
    isAutoNormalize = isNormalize;
    return b;
}

bool AnimationImp::GetAutoNormalize()
{
    return isAutoNormalize;
}

// Allow custom blending coefficients in ActionPlayer
bool AnimationImp::UserBlend(bool isBlend)
{
    const auto b = isUserBlend;
    isUserBlend = isBlend;
    return b;
}

bool AnimationImp::IsUserBlend()
{
    return isUserBlend;
}

// Procedural head look
bool AnimationImp::HeadControl(bool isControllable)
{
    const auto b = isControllableHead;
    isControllableHead = isControllable;
    return b;
}

bool AnimationImp::IsControllableHead()
{
    return isControllableHead;
}

void AnimationImp::RotateHead(float x, float y)
{
    customHeadAX = x;
    customHeadAY = y;
}

//--------------------------------------------------------------------------------------------
// AnimationImp
//--------------------------------------------------------------------------------------------

// Take a step in time
void AnimationImp::Execute(int32_t dltTime)
{
    // execute animation
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
        action[i].Execute(dltTime);
    // execute the timers
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
        timer[i].Execute(dltTime);
    // Calculate animation matrices
    BuildAnimationMatrices();
}

// Calculate animation matrices
void AnimationImp::BuildAnimationMatrices()
{
    auto nFrames = aniInfo->GetAniNumFrames();
    auto nbones = aniInfo->NumBones();
    // see how many players are playing, calculate the current blending coefficients
    int32_t plCnt = 0;
    auto normBlend = 0.0f;
    for (int32_t i = 0; i < ANI_MAX_ACTIONS; i++)
        if (action[i].IsPlaying())
        {
            plCnt++;
            action[i].kBlendCurrent = action[i].TimerBlend();
            if (isUserBlend)
                action[i].kBlendCurrent *= action[i].Blend();
            normBlend += action[i].kBlendCurrent;
        }
    if (!plCnt)
        return;

    // Auto normalization
    if (normBlend != 0.0f)
        if (plCnt > 1)
        {
            normBlend = 1.0f / normBlend;

            auto frame0 = action[0].GetCurrentFrame();
            auto f0 = static_cast<int32_t>(frame0);
            auto ki0 = frame0 - static_cast<float>(f0);
            if (f0 >= nFrames)
            {
                f0 = nFrames - 1;
                ki0 = 0.0f;
            }

            auto frame1 = action[1].GetCurrentFrame();
            auto f1 = static_cast<int32_t>(frame1);
            auto ki1 = frame1 - static_cast<float>(f1);
            if (f1 >= nFrames)
            {
                f1 = nFrames - 1;
                ki1 = 0.0f;
            }

            auto kBlend = 1.0f - action[0].kBlendCurrent * normBlend;
            //-------------------------------------------------------------------------
            for (int32_t j = 0; j < nbones; j++)
            {
                auto &bn = aniInfo->GetBone(j);
                Matrix tmpMtx;
                CMatrix inmtx;
                Quaternion qt0, qt1, qt;
                bn.BlendFrame(f0, ki0, qt0);
                bn.BlendFrame(f1, ki1, qt1);
                qt.SLerp(qt0, qt1, kBlend);
                qt.GetMatrix(tmpMtx);
                inmtx = tmpMtx;
                inmtx.Pos() = bn.pos0;
                if (j == 0)
                {
                    auto p0 = bn.pos[f0] + ki0 * (bn.pos[f0 + 1] - bn.pos[f0]);
                    auto p1 = bn.pos[f1] + ki1 * (bn.pos[f1 + 1] - bn.pos[f1]);
                    inmtx.Pos() = p0 + kBlend * (p1 - p0);
                }

                // Procedural head look
                if (j == headBoneIndex && isControllableHead)
                {
                    inmtx.RotateX(customHeadAX);
                    inmtx.RotateY(customHeadAY);
                }

                if (bn.parent)
                    bn.matrix.EqMultiply(inmtx, CMatrix(bn.parent->matrix));
                else
                    bn.matrix = inmtx;
            }
        }
        else if (action[0].IsPlaying())
        {
            auto frame = action[0].GetCurrentFrame();
            auto f = static_cast<int32_t>(frame);
            auto ki = frame - static_cast<float>(f);
            if (f >= nFrames)
            {
                f = nFrames - 1;
                ki = 0.0f;
            }

            //-------------------------------------------------------------------------
            for (int32_t j = 0; j < nbones; j++)
            {
                auto &bn = aniInfo->GetBone(j);
                Matrix tmpMtx;
                CMatrix inmtx;
                Quaternion qt;
                bn.BlendFrame(f, ki, qt);
                qt.GetMatrix(tmpMtx);
                inmtx = tmpMtx;
                inmtx.Pos() = bn.pos0;
                if (j == 0)
                    inmtx.Pos() = bn.pos[f] + ki * (bn.pos[f + 1] - bn.pos[f]);

                // Procedural head look
                if (j == headBoneIndex && isControllableHead)
                {
                    inmtx.RotateX(customHeadAX);
                    inmtx.RotateY(customHeadAY);
                }

                if (bn.parent)
                    bn.matrix.EqMultiply(inmtx, CMatrix(bn.parent->matrix));
                else
                    bn.matrix = inmtx;
            }
        }
        else if (action[1].IsPlaying())
        {
            auto frame = action[1].GetCurrentFrame();
            auto f = static_cast<int32_t>(frame);
            auto ki = frame - static_cast<float>(f);
            if (f >= nFrames)
            {
                f = nFrames - 1;
                ki = 0.0f;
            }

            //-------------------------------------------------------------------------
            for (int32_t j = 0; j < nbones; j++)
            {
                auto &bn = aniInfo->GetBone(j);
                Matrix tmpMtx;
                CMatrix inmtx;
                Quaternion qt;
                bn.BlendFrame(f, ki, qt);
                qt.GetMatrix(tmpMtx);
                inmtx = tmpMtx;
                inmtx.Pos() = bn.pos0;
                if (j == 0)
                    inmtx.Pos() = bn.pos[f] + ki * (bn.pos[f + 1] - bn.pos[f]);

                // Procedural head look
                if (j == headBoneIndex && isControllableHead)
                {
                    inmtx.RotateX(customHeadAX);
                    inmtx.RotateY(customHeadAY);
                }

                if (bn.parent)
                    bn.matrix.EqMultiply(inmtx, CMatrix(bn.parent->matrix));
                else
                    bn.matrix = inmtx;
            }
        }
        else
        {
            core.Trace("AnimationImp::BuildAnimationMatrices -> Not support mode");
            psnip_trap();
            /*_asm int 3;*/
            //    float frame = 0.0f;
            //    for(int32_t j = 0; j < nbones; j++)
            //        aniInfo->GetBone(j).BlendFrame(frame);
        }
    for (int32_t j = 0; j < nbones; j++)
    {
        auto &bn = aniInfo->GetBone(j);
        matrix[j] = CMatrix(bn.start) * CMatrix(bn.matrix);
#ifdef _WIN32 // FIX_LINUX DirectXMath
        // inverse first column in advance
        matrix[j].matrix[0] = -matrix[j].matrix[0];
        matrix[j].matrix[4] = -matrix[j].matrix[4];
        matrix[j].matrix[8] = -matrix[j].matrix[8];
        matrix[j].matrix[12] = -matrix[j].matrix[12];
#endif
    }
}

// Events
// Send events
void AnimationImp::SendEvent(AnimationEvent event, int32_t index)
{
    for (int32_t i = 0; i < ANIIMP_MAXLISTENERS; i++)
    {
        if (ae_listeners[event][i])
        {
            ae_listeners[event][i]->Event(this, index, (event << 16) | i, event);
        }
    }
}
