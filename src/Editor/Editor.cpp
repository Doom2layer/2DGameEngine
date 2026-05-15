#include "Editor.h"

#include <algorithm>
#include <vector>

#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "Panels/Inspector/InspectorPanel.h"
#include "Panels/ViewPort/ViewPortPanel.h"
#include "Panels/WorldOutliner/WorldOutlinerPanel.h"
#include <imgui/imgui.h>

Editor::~Editor() = default;

void Editor::Initialize(SDL_Renderer* renderer, SDL_Window* window, ECSManager* ecsManager)
{
	Renderer = renderer;
	Window = window;
	ECSManagerInstance = ecsManager;
	bIsInitialized = (Renderer != nullptr && Window != nullptr);
}

void Editor::Update()
{
	// Editor state updates can live here later.
}

void Editor::Render()
{
	if (!bIsInitialized)
	{
		return;
	}

	const ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
	const float ControlsHeight = 42.0f;
	const float OutlinerWidth = 250.0f;
	const float InspectorWidth = 300.0f;
	const float ViewportWidth = std::max(0.0f, DisplaySize.x - OutlinerWidth - InspectorWidth);
	const float ViewportHeight = std::max(0.0f, DisplaySize.y - ControlsHeight);

	if (!Viewport)
	{
		Viewport = std::make_unique<ViewPortPanel>();
	}
	if (!WorldOutliner)
	{
		WorldOutliner = std::make_unique<WorldOutlinerPanel>();
	}
	if (!Inspector)
	{
		Inspector = std::make_unique<InspectorPanel>();
	}

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(DisplaySize.x, ControlsHeight), ImGuiCond_FirstUseEver);
	ImGui::Begin("Editor Controls");
	if (ImGui::Button(CurrentPlayState == PlayState::Playing ? "Pause" : (CurrentPlayState == PlayState::Paused ? "Resume" : "Play")))
	{
		if (CurrentPlayState == PlayState::Playing)
		{
			SetPlayState(PlayState::Paused);
		}
		else
		{
			SetPlayState(PlayState::Playing);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		SetPlayState(PlayState::Stopped);
	}
	ImGui::SameLine();
	ImGui::Text("Mode: %s", CurrentPlayState == PlayState::Playing ? "Playing" : CurrentPlayState == PlayState::Paused ? "Paused" : "Stopped");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0.0f, ControlsHeight), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(OutlinerWidth, ViewportHeight), ImGuiCond_FirstUseEver);
	WorldOutliner->Render(*this);

	ImGui::SetNextWindowPos(ImVec2(DisplaySize.x - InspectorWidth, ControlsHeight), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(InspectorWidth, ViewportHeight), ImGuiCond_FirstUseEver);
	Inspector->Render(*this);

	ImGui::SetNextWindowPos(ImVec2(OutlinerWidth, ControlsHeight), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(ViewportWidth, ViewportHeight), ImGuiCond_FirstUseEver);
	Viewport->Render(*this);
}

void Editor::Shutdown()
{
	Renderer = nullptr;
	Window = nullptr;
	ECSManagerInstance = nullptr;
	ViewportTexture = nullptr;
	Viewport.reset();
	WorldOutliner.reset();
	Inspector.reset();
	bIsInitialized = false;
}

std::string Editor::GetSelectedEntityTag() const
{
	if (!HasSelectedEntity() || !ECSManagerInstance)
	{
		return {};
	}

	return ECSManagerInstance->GetEntityTag(GetSelectedEntity());
}

void Editor::SelectEntity(const Entity& InEntity)
{
	SelectedEntity = InEntity;
}

void Editor::SelectEntityAtScreenPoint(int ScreenX, int ScreenY)
{
	if (!ECSManagerInstance)
	{
		return;
	}

	struct FPickCandidate
	{
		Entity EntityInstance;
		SDL_Rect ScreenRect{};
		int Layer{ 0 };
		int ZIndex{ 0 };
	};

	std::vector<FPickCandidate> Candidates;
	for (const Entity& InEntity : ECSManagerInstance->GetAllEntities())
	{
		if (!InEntity.HasComponent<FTransformComponent>() || !InEntity.HasComponent<FSpriteComponent>())
		{
			continue;
		}

		const FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
		const FSpriteComponent& Sprite = InEntity.GetComponent<FSpriteComponent>();

		const float PickX = Transform.Position.x - static_cast<float>(Sprite.bIsFixed ? 0 : Camera.x);
		const float PickY = Transform.Position.y - static_cast<float>(Sprite.bIsFixed ? 0 : Camera.y);
		const float PickW = static_cast<float>(Sprite.SourceRectangle.w) * Transform.Scale.x;
		const float PickH = static_cast<float>(Sprite.SourceRectangle.h) * Transform.Scale.y;

		SDL_Rect ScreenRect
		{
			static_cast<int>(PickX),
			static_cast<int>(PickY),
			static_cast<int>(PickW),
			static_cast<int>(PickH)
		};

		Candidates.push_back({ InEntity, ScreenRect, static_cast<uint8_t>(Sprite.RenderLayer), Sprite.ZIndex });
	}

	std::sort(Candidates.begin(), Candidates.end(), [](const FPickCandidate& A, const FPickCandidate& B)
	{
		if (A.Layer != B.Layer)
		{
			return A.Layer < B.Layer;
		}
		return A.ZIndex < B.ZIndex;
	});

	for (auto It = Candidates.rbegin(); It != Candidates.rend(); ++It)
	{
		const SDL_Rect& Rect = It->ScreenRect;
		if (ScreenX >= Rect.x && ScreenX <= Rect.x + Rect.w && ScreenY >= Rect.y && ScreenY <= Rect.y + Rect.h)
		{
			SelectEntity(It->EntityInstance);
			return;
		}
	}

	ClearSelection();
}

bool Editor::TryGetSelectedEntityScreenRect(SDL_Rect& OutRect) const
{
	if (!HasSelectedEntity() || !ECSManagerInstance)
	{
		return false;
	}

	const Entity Selected = GetSelectedEntity();
	if (!Selected.HasComponent<FTransformComponent>() || !Selected.HasComponent<FSpriteComponent>())
	{
		return false;
	}

	const FTransformComponent& Transform = Selected.GetComponent<FTransformComponent>();
	const FSpriteComponent& Sprite = Selected.GetComponent<FSpriteComponent>();

	const float PickX = Transform.Position.x - static_cast<float>(Sprite.bIsFixed ? 0 : Camera.x);
	const float PickY = Transform.Position.y - static_cast<float>(Sprite.bIsFixed ? 0 : Camera.y);
	const float PickW = static_cast<float>(Sprite.SourceRectangle.w) * Transform.Scale.x;
	const float PickH = static_cast<float>(Sprite.SourceRectangle.h) * Transform.Scale.y;

	OutRect =
	{
		static_cast<int>(PickX),
		static_cast<int>(PickY),
		static_cast<int>(PickW),
		static_cast<int>(PickH)
	};

	return OutRect.w > 0 && OutRect.h > 0;
}

void Editor::ClearSelection()
{
	SelectedEntity.reset();
}

void Editor::RenameSelectedEntity(const std::string& NewName)
{
	if (!HasSelectedEntity() || !ECSManagerInstance)
	{
		return;
	}

	Entity Selected = GetSelectedEntity();
	ECSManagerInstance->RemoveEntityTag(Selected);

	if (!NewName.empty())
	{
		Selected.Tag(NewName);
	}
}

void Editor::SetPlayState(PlayState NewState)
{
	CurrentPlayState = NewState;
	if (CurrentPlayState == PlayState::Stopped)
	{
		ClearSelection();
	}
}


