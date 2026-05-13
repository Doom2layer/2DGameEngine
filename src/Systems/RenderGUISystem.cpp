#include "RenderGUISystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/2DBoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

void RenderGUISystem::Update(const std::unique_ptr<ECSManager>& ECSManagerInstance, const SDL_Rect& Camera)
{
    ImGui::NewFrame();
    
    if (ImGui::Begin("Spawn Enemies"))
    {
        static int EnemyPositionX       = 0;
        static int EnemyPositionY       = 0;
        static int EnemyScaleX          = 1;
        static int EnemyScaleY          = 1;
        static int EnemyVelocityX       = 0;
        static int EnemyVelocityY       = 0;
        static int EnemyHealth          = 100;
        static float EnemyRotation      = 0.0f;
        static float ProjectileAngle    = 0.0f;
        static float ProjectileSpeed    = 100.0f;
        static int ProjectileFireRate   = 10;
        static int ProjectileDuration   = 10;
        static int SelectedSpriteIndex  = 0;
        const char* Sprite[] = {"Tank-Image", "Truck-Image"};
        
        // section to input enemy sprite texture id
        if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
        {        
            ImGui::Combo("Sprite", &SelectedSpriteIndex, Sprite, IM_ARRAYSIZE(Sprite));
        }
        ImGui::Spacing();
        
        // section to input enemy transform values
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("Position X", &EnemyPositionX);
            ImGui::InputInt("Position Y", &EnemyPositionY);
            ImGui::SliderInt("Scale X", &EnemyScaleX, 1, 10);
            ImGui::SliderInt("Scale Y", &EnemyScaleY, 1, 10);
            ImGui::SliderAngle("Rotation", &EnemyRotation, 0, 360);
        }
        ImGui::Spacing();
        
        // Section to input enemy rigid body values
        if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("Velocity X", &EnemyVelocityX);
            ImGui::InputInt("Velocity Y", &EnemyVelocityY);
        }
        ImGui::Spacing();
        
        // Section to input enemy projectile emitter values
        if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderAngle("Projectile Angle (deg)", &ProjectileAngle, 0, 360);
            ImGui::InputFloat("Projectile Speed (px/sec)", &ProjectileSpeed, 10, 500);
            ImGui::InputInt("Fire Rate (sec)", &ProjectileFireRate);
            ImGui::InputInt("Duration (sec)", &ProjectileDuration);
        }
        ImGui::Spacing();
        
        // Section to input enemy health values
        if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderInt("%", &EnemyHealth, 0, 100);
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::CollapsingHeader("Spawn Enemy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::Button("Spawn"))
            {
                Entity Enemy = ECSManagerInstance->CreateEntity();
                Enemy.Group("Enemies");
                Enemy.AddComponent<FTransformComponent>(glm::vec2(EnemyPositionX, EnemyPositionY), glm::vec2(EnemyScaleX, EnemyScaleY), glm::degrees(EnemyRotation));
                Enemy.AddComponent<FRigidBodyComponent>(glm::vec2(EnemyVelocityX, EnemyVelocityY));
                Enemy.AddComponent<FSpriteComponent>(Sprite[SelectedSpriteIndex], 32, 32, 0, 0, ERenderLayer::Enemy, 0);
                Enemy.AddComponent<F2DBoxColliderComponent>(32, 32);
                Enemy.AddComponent<FProjectileEmitterComponent>(glm::vec2(cos(ProjectileAngle) * ProjectileSpeed, sin(ProjectileAngle) * ProjectileSpeed), ProjectileFireRate * 1000, ProjectileDuration * 1000, 10, false);
                Enemy.AddComponent<FHealthComponent>(EnemyHealth);
                
                // Reset all input values after we create a new enemy
                EnemyPositionX = EnemyPositionY = 0;
                EnemyScaleX = EnemyScaleY = 1;
                EnemyRotation = ProjectileAngle = 0;
                ProjectileFireRate = ProjectileDuration = 10;
                ProjectileSpeed = 100;
                EnemyHealth = 100;
            }
        }
    }
    ImGui::End();

    //Display a small overlay window to display the map position using the mouse
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.9f);
    if (ImGui::Begin("Map Position", NULL, WindowFlags))
    {
        ImGui::Text("Mouse Position: (x=%.1f, y=%.1f)", ImGui::GetIO().MousePos.x + Camera.x, ImGui::GetIO().MousePos.y + Camera.y);
    }
    ImGui::End();
    
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
}
