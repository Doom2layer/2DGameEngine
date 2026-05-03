#pragma once
#include "../ECS/ECS.h"
#include <SDL_stdinc.h>

struct FAnimationComponent : public Component<FAnimationComponent>
{
    int    NumberOfFrames;
    int    CurrentFrame;
    int    FrameSpeedRate;
    bool   bIsLoop;
    Uint32 StartTime;

    FAnimationComponent(int InNumberOfFrames = 1, int InFrameSpeedRate = 1, bool bInIsLoop = true)
        : NumberOfFrames(InNumberOfFrames)
        , CurrentFrame(1)
        , FrameSpeedRate(InFrameSpeedRate)
        , bIsLoop(bInIsLoop)
        , StartTime(0)
    {}
};