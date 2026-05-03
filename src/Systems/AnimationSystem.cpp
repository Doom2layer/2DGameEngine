#include "AnimationSystem.h"

#include <SDL_timer.h>

#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"

AnimationSystem::AnimationSystem()
{
    RequireComponent<FSpriteComponent>();   
    RequireComponent<FAnimationComponent>();
}

void AnimationSystem::AddEntityToSystem(Entity InEntity)
{
    System::AddEntityToSystem(InEntity);
    // We set the start time for the animation when the entity is added to the system. This ensures that the animation starts from the correct time when the entity is first processed in the Update method.
    InEntity.GetComponent<FAnimationComponent>().StartTime = SDL_GetTicks();
}

void AnimationSystem::Update()
{
    // we get the current time in milliseconds using SDL_GetTicks(), which returns the number of milliseconds since the SDL library was initialized. This value is used to calculate the current frame of the animation based on the elapsed time since the animation started.
    const Uint32 CurrentTime = SDL_GetTicks();
    
    for (const Entity& InEntity : GetSystemEntities())
    {
        
        FAnimationComponent& Animation = InEntity.GetComponent<FAnimationComponent>();
        FSpriteComponent&    Sprite    = InEntity.GetComponent<FSpriteComponent>();

        // we set the start time once on the first update
        if (Animation.StartTime == 0)
        {
            Animation.StartTime = SDL_GetTicks();
        }

        // we calculate the new frame based on the current time, the start time of the animation, the frame speed rate, and the total number of frames in the animation. The formula used is:
        const int NewFrame = ((CurrentTime - Animation.StartTime) * Animation.FrameSpeedRate / 1000) % Animation.NumberOfFrames;
        
        // we only update the CurrentFrame and the SourceRectangle if the NewFrame is different from the current frame to avoid unnecessary updates and potential performance issues.
        if (NewFrame != Animation.CurrentFrame)
        {
            Animation.CurrentFrame       = NewFrame;
            Sprite.SourceRectangle.x     = NewFrame * Sprite.SourceRectangle.w;
        }
    }
}
