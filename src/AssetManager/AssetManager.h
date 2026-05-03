#pragma once
#include <map>
#include <SDL_render.h>
#include <string>

class AssetManager
{
public:
    AssetManager(SDL_Renderer* InRenderer);
    ~AssetManager();
    
    void AddTexture(const std::string& AssetID, const std::string& FilePath);
    void ClearAsset();
    
    SDL_Texture* GetTexture(const std::string& AssetID) const;
    
private:
    std::map<std::string, SDL_Texture*> Textures;
    SDL_Renderer* Renderer;
        
};
