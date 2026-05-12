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

void AssetManager::ClearAsset()
{
    for (std::pair<const std::string, SDL_Texture*>& Texture : Textures)
    {
          SDL_DestroyTexture(Texture.second);   
    }
    Textures.clear();
    
    for (std::pair<const std::string, TTF_Font*>& Font : Fonts)
    {
        TTF_CloseFont(Font.second);
    }
    Fonts.clear();   
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

SDL_Texture* AssetManager::GetTexture(const std::string& AssetID) const
{
    std::map<std::string, SDL_Texture*>::const_iterator It = Textures.find(AssetID);
    if (It == Textures.end())
    {
        Logger::Error("AssetManager::GetTexture texture not found: " + AssetID);
        return nullptr;
    }
    return It->second;
}

void AssetManager::AddFont(const std::string& AssetID, const std::string& FilePath, int FontSize)
{
    Fonts.emplace(AssetID, TTF_OpenFont(FilePath.c_str(), FontSize));
}

TTF_Font* AssetManager::GetFont(const std::string& AssetID) const
{
    std::map<std::string, TTF_Font*>::const_iterator It = Fonts.find(AssetID);
    if (It == Fonts.end())
    {
        Logger::Error("AssetManager::GetFont font not found: " + AssetID);
        return nullptr;
    }
    return It->second;   
}
