#include "InspectorPanel.h"

#include "../../Editor.h"
#include "../../../Components/TransformComponent.h"
#include <imgui/imgui.h>
#include <array>
#include <cstdio>

void InspectorPanel::Render(Editor& EditorInstance)
{
	ImGui::Begin("Inspector");

	if (!EditorInstance.HasSelectedEntity())
	{
		ImGui::TextUnformatted("Select an entity from the World Outliner.");
		ImGui::End();
		return;
	}

	Entity SelectedEntity = EditorInstance.GetSelectedEntity();
	ECSManager* ECSManagerInstance = EditorInstance.GetECSManager();
	if (!ECSManagerInstance)
	{
		ImGui::TextUnformatted("No ECS scene is available.");
		ImGui::End();
		return;
	}

	ImGui::Text("Entity ID: %zu", SelectedEntity.GetID());

	static size_t LastEntityID = static_cast<size_t>(-1);
	static std::array<char, 128> NameBuffer{};
	if (LastEntityID != SelectedEntity.GetID())
	{
		LastEntityID = SelectedEntity.GetID();
		NameBuffer.fill('\0');
		std::string CurrentTag = ECSManagerInstance->GetEntityTag(SelectedEntity);
		(void)std::snprintf(NameBuffer.data(), NameBuffer.size(), "%s", CurrentTag.c_str());
	}

	ImGui::InputText("Name", NameBuffer.data(), NameBuffer.size());
	if (ImGui::Button("Apply Name"))
	{
		EditorInstance.RenameSelectedEntity(NameBuffer.data());
	}

	ImGui::Separator();

	if (SelectedEntity.HasComponent<FTransformComponent>())
	{
		FTransformComponent& Transform = SelectedEntity.GetComponent<FTransformComponent>();

		float Position[2] = { static_cast<float>(Transform.Position.x), static_cast<float>(Transform.Position.y) };
		float Scale[2] = { static_cast<float>(Transform.Scale.x), static_cast<float>(Transform.Scale.y) };
		float Rotation = static_cast<float>(Transform.Rotation);

		if (ImGui::InputFloat2("Position", Position))
		{
			Transform.Position.x = Position[0];
			Transform.Position.y = Position[1];
		}

		if (ImGui::InputFloat2("Scale", Scale))
		{
			Transform.Scale.x = Scale[0];
			Transform.Scale.y = Scale[1];
		}

		if (ImGui::InputFloat("Rotation", &Rotation))
		{
			Transform.Rotation = Rotation;
		}
	}
	else
	{
		ImGui::TextUnformatted("This entity does not have a Transform component yet.");
	}

	ImGui::End();
}

