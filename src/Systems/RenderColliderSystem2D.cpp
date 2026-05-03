#include "RenderColliderSystem2D.h"

#include "../Components/2DCircleColliderComponent.h"
#include "../Components/2DBoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

RenderColliderSystem2D::RenderColliderSystem2D()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<F2DBoxColliderComponent>();
}

void RenderColliderSystem2D::Update(SDL_Renderer* Renderer)
{
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255); // Red color for collider
    
    for (Entity InEntity : GetSystemEntities())
    {
        if (InEntity.HasComponent<F2DBoxColliderComponent>())
        {
            const FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
            const F2DBoxColliderComponent& Collider = InEntity.GetComponent<F2DBoxColliderComponent>();
        
            SDL_Rect ColliderRect{
                static_cast<int>(Transform.Position.x + Collider.Offset.x),
                static_cast<int>(Transform.Position.y + Collider.Offset.y),
                static_cast<int>(Collider.Width),
                static_cast<int>(Collider.Height)
            };
        
            SDL_RenderDrawRect(Renderer, &ColliderRect);
        }

        /*if (InEntity.HasComponent<F2DBoxColliderComponent>())
        {
            const FTransformComponent&         Transform = InEntity.GetComponent<FTransformComponent>();
            const F2DCircleColliderComponent&  Collider  = InEntity.GetComponent<F2DCircleColliderComponent>();

            DrawCircle(Renderer,
                static_cast<int>(Transform.Position.x + Collider.Offset.x),
                static_cast<int>(Transform.Position.y + Collider.Offset.y),
                static_cast<int>(Collider.Radius)
            );
        }*/
        
    }
}

/*void RenderColliderSystem2D::DrawCircle(SDL_Renderer* Renderer, int CenterX, int CenterY, int Radius)
{
    int X = Radius;
    int Y = 0;
    int Error = 0;

    while (X >= Y)
    {
        SDL_RenderDrawPoint(Renderer, CenterX + X, CenterY - Y);
        SDL_RenderDrawPoint(Renderer, CenterX + Y, CenterY - X);
        SDL_RenderDrawPoint(Renderer, CenterX - Y, CenterY - X);
        SDL_RenderDrawPoint(Renderer, CenterX - X, CenterY - Y);
        SDL_RenderDrawPoint(Renderer, CenterX - X, CenterY + Y);
        SDL_RenderDrawPoint(Renderer, CenterX - Y, CenterY + X);
        SDL_RenderDrawPoint(Renderer, CenterX + Y, CenterY + X);
        SDL_RenderDrawPoint(Renderer, CenterX + X, CenterY + Y);

        Y++;
        if (Error <= 0)
            Error += 2 * Y + 1;
        else
        {
            X--;
            Error += 2 * (Y - X) + 1;
        }
    }
}*/
