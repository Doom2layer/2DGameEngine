#include "AssetManager.h"

#include "../Logger/Logger.h"
#include <SDL_image.h>


AssetManager::AssetManager(SDL_Renderer* InRenderer) : Renderer(InRenderer)
{
}

AssetManager::~AssetManager()
{
    ClearAsset();
    Logger::Log("AssetManager Destructor Called");
}

void AssetManager::AddTexture(const std::string& AssetID, const std::string& FilePath)
{
    SDL_Surface* Surface = IMG_Load(FilePath.c_str());
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    SDL_FreeSurface(Surface);
    
    // Add the texture to the map
    Textures.emplace(AssetID, Texture);
    Logger::Log("AssetManager::AddTexture New Texture Added with ID: " + AssetID);
}

void AssetManager::ClearAsset()
{
    for (std::pair<const std::string, SDL_Texture*>& Texture : Textures)
    {
          SDL_DestroyTexture(Texture.second);   
    }
    Textures.clear();
}

SDL_Texture* AssetManager::GetTexture(const std::string& AssetID) const
{
    auto It = Textures.find(AssetID);
    if (It == Textures.end())
    {
        Logger::Error("AssetManager::GetTexture texture not found: " + AssetID);
        return nullptr;
    }
    return It->second;
}
