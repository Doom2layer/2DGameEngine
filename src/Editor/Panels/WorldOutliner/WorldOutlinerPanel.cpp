#include "WorldOutlinerPanel.h"

#include "../../Editor.h"
#include <imgui/imgui.h>

void WorldOutlinerPanel::Render(Editor& EditorInstance)
{
	ImGui::Begin("World Outliner");

	ECSManager* ECSManagerInstance = EditorInstance.GetECSManager();
	if (!ECSManagerInstance)
	{
		ImGui::TextUnformatted("No scene loaded.");
		ImGui::End();
		return;
	}

	const std::vector<Entity> Entities = ECSManagerInstance->GetAllEntities();
	if (Entities.empty())
	{
		ImGui::TextUnformatted("No entities found.");
		ImGui::End();
		return;
	}

	for (const Entity& InEntity : Entities)
	{
		std::string Label = ECSManagerInstance->GetEntityTag(InEntity);
		if (Label.empty())
		{
			Label = "Entity " + std::to_string(InEntity.GetID());
		}
		else
		{
			Label += " (" + std::to_string(InEntity.GetID()) + ")";
		}

		const bool bIsSelected = EditorInstance.HasSelectedEntity() && EditorInstance.GetSelectedEntity().GetID() == InEntity.GetID();
		if (ImGui::Selectable(Label.c_str(), bIsSelected))
		{
			EditorInstance.SelectEntity(InEntity);
		}
	}

	ImGui::End();
}

