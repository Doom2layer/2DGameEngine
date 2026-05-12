#pragma once
#include <map>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>

class AssetManager
{
public:
    AssetManager(SDL_Renderer* InRenderer);
    ~AssetManager();
    
    void ClearAsset();
    
    void AddTexture(const std::string& AssetID, const std::string& FilePath);
    SDL_Texture* GetTexture(const std::string& AssetID) const;
    
    void AddFont(const std::string& AssetID, const std::string& FilePath, int FontSize);
    TTF_Font* GetFont(const std::string& AssetID) const;
    
private:
    std::map<std::string, SDL_Texture*> Textures;
    std::map<std::string, TTF_Font*> Fonts;
    SDL_Renderer* Renderer;
        
};
