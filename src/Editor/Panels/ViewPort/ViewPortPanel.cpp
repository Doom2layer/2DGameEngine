#include "../../Editor.h"
#include "ViewPortPanel.h"
#include <imgui/imgui.h>

void ViewPortPanel::Render(Editor& EditorInstance)
{
	ImGui::Begin("Viewport");

	SDL_Texture* ViewportTexture = EditorInstance.GetViewportTexture();
	if (!ViewportTexture)
	{
		ImGui::TextUnformatted("No viewport texture available yet.");
		ImGui::End();
		return;
	}

	int TextureWidth = 0;
	int TextureHeight = 0;
	SDL_QueryTexture(ViewportTexture, nullptr, nullptr, &TextureWidth, &TextureHeight);

	ImVec2 AvailableSize = ImGui::GetContentRegionAvail();
	if (AvailableSize.x <= 0.0f || AvailableSize.y <= 0.0f || TextureWidth <= 0 || TextureHeight <= 0)
	{
		ImGui::TextUnformatted("Viewport is waiting for space or texture data.");
		ImGui::End();
		return;
	}

	const float TextureAspect = static_cast<float>(TextureWidth) / static_cast<float>(TextureHeight);
	float DrawWidth = AvailableSize.x;
	float DrawHeight = DrawWidth / TextureAspect;
	if (DrawHeight > AvailableSize.y)
	{
		DrawHeight = AvailableSize.y;
		DrawWidth = DrawHeight * TextureAspect;
	}

	const float OffsetX = (AvailableSize.x - DrawWidth) * 0.5f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + OffsetX);
	ImGui::Image(reinterpret_cast<void*>(ViewportTexture), ImVec2(DrawWidth, DrawHeight));

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		const ImVec2 ImageMin = ImGui::GetItemRectMin();
		const ImVec2 MousePos = ImGui::GetIO().MousePos;
		const ImVec2 LocalPos = ImVec2(MousePos.x - ImageMin.x, MousePos.y - ImageMin.y);

		if (LocalPos.x >= 0.0f && LocalPos.y >= 0.0f && LocalPos.x <= DrawWidth && LocalPos.y <= DrawHeight)
		{
			const float NormalizedX = LocalPos.x / DrawWidth;
			const float NormalizedY = LocalPos.y / DrawHeight;
			const int ScreenX = static_cast<int>(NormalizedX * static_cast<float>(TextureWidth));
			const int ScreenY = static_cast<int>(NormalizedY * static_cast<float>(TextureHeight));
			EditorInstance.SelectEntityAtScreenPoint(ScreenX, ScreenY);
		}
	}

	ImGui::End();
}

