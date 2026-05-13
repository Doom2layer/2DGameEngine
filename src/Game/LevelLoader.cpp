#include "./LevelLoader.h"
#include "./Game.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/2DBoxColliderComponent.h"
#include "../Components/PlayerControllerComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"
#include <fstream>
#include <string>
#include <sol/sol.hpp>

LevelLoader::LevelLoader() {
    Logger::Log("LevelLoader constructor called!");    
}

LevelLoader::~LevelLoader() {
    Logger::Log("LevelLoader destructor called!");    
}

void LevelLoader::LoadLevel(sol::state& LuaState, const std::unique_ptr<ECSManager>& ECSManager, const std::unique_ptr<AssetManager>& AssetManager, int LevelNumber) {
    // This checks the syntax of our script, but it does not execute the script
    
    LuaState.new_enum<ERenderLayer>("ERenderLayer",
    {
        { "Background", ERenderLayer::Background },
        { "Vegetation",  ERenderLayer::Vegetation },
        { "Obstacle",    ERenderLayer::Obstacle },
        { "Enemy",       ERenderLayer::Enemy },
        { "Player",      ERenderLayer::Player },
        { "UI",          ERenderLayer::UI }
    });

    
    sol::load_result script = LuaState.load_file("./assets/scripts/Level" + std::to_string(LevelNumber) + ".lua");
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Error("Error loading the lua script: " + errorMessage);
        return;
    }

    // Executes the script using the Sol state
    LuaState.script_file("./assets/scripts/Level" + std::to_string(LevelNumber) + ".lua");

    // Read the big table for the current level
    sol::table level = LuaState["Level"];

    ////////////////////////////////////////////////////////////////////////////
    // Read the level assets
    ////////////////////////////////////////////////////////////////////////////
    sol::table assets = level["assets"];

    int i = 0;
    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        if (assetType == "texture") {
            AssetManager->AddTexture(assetId, asset["file"]);
            Logger::Log("A new texture asset was added to the asset store, id: " + assetId);
        }
        if (assetType == "font") {
            AssetManager->AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("A new font asset was added to the asset store, id: " + assetId);
        }
        i++;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Read the level tilemap information
    ////////////////////////////////////////////////////////////////////////////
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    ERenderLayer mapRenderLayer = static_cast<ERenderLayer>(map["render_layer"]);
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = ECSManager->CreateEntity();
            tile.AddComponent<FTransformComponent>(glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)), glm::vec2(mapScale, mapScale), 0.0);
            tile.AddComponent<FSpriteComponent>(mapTextureAssetId, tileSize, tileSize, srcRectX, srcRectY, mapRenderLayer, 0, false);
        }
    }
    mapFile.close();
    Game::MapWidth = mapNumCols * tileSize * mapScale;
    Game::MapHeight = mapNumRows * tileSize * mapScale;

    ////////////////////////////////////////////////////////////////////////////
    // Read the level entities and their components
    ////////////////////////////////////////////////////////////////////////////
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = ECSManager->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<FTransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<FRigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<FSpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0),
                    entity["components"]["sprite"]["render_layer"].get_or(ERenderLayer::Background),
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false)
                );
                
                const FSpriteComponent& DbgSprite = newEntity.GetComponent<FSpriteComponent>();
                Logger::Log("Entity " + std::to_string(newEntity.GetID()) + 
                            " RenderLayer: " + std::to_string(static_cast<uint8_t>(DbgSprite.RenderLayer)) +
                            " ZIndex: " + std::to_string(DbgSprite.ZIndex));
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<FAnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<F2DBoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }
            
            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<FHealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }
            
            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<FProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<FCameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<FPlayerControllerComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }
        }
        i++;
    }

    /*
    // Adding assets to the asset manager
    AssetManagerInstance->AddTexture("Chopper-Image", "./assets/images/chopper-spritesheet.png");
    AssetManagerInstance->AddTexture("Tank-Image", "./assets/images/tank-panther-right.png");
    AssetManagerInstance->AddTexture("Truck-Image", "./assets/images/truck-ford-right.png");
    AssetManagerInstance->AddTexture("Tree-Image", "./assets/images/tree.png");   
    AssetManagerInstance->AddTexture("Jungle-Tilemap-Image", "./assets/tilemaps/jungle.png");
    AssetManagerInstance->AddTexture("Radar-Image", "./assets/images/radar.png");
    AssetManagerInstance->AddTexture("Bullet-Image", "./assets/images/bullet.png");
    AssetManagerInstance->AddFont("Charriot-Font", "./assets/fonts/charriot.ttf", 24);
    AssetManagerInstance->AddFont("Arial-Font", "./assets/fonts/arial.ttf", 24);
    
    
    //Load the tile map
    constexpr int TileSize =32;
    constexpr double TileScale = 2;
    constexpr int MapNumberColumns = 25;
    constexpr int MapNumberRows = 20;
    std::fstream MapFile;
    MapFile.open("./assets/tilemaps/jungle.map");
    if (!MapFile.is_open())
    {
        Logger::Error("Failed to open map file.");
        return;
    }
    
    for (int y = 0; y < MapNumberRows; y++)
    {
        for (int x = 0; x < MapNumberColumns; x++)
        {
            char TileType;
            MapFile.get(TileType);
            int SourceRectY = (TileType - '0') * TileSize;
            MapFile.get(TileType);
            int SourceRectX = (TileType - '0') * TileSize;
            MapFile.ignore();
            
            Entity Tile = ECSManagerInstance->CreateEntity();
            Tile.Group("Tiles");
            Tile.AddComponent<FTransformComponent>(glm::vec2(x * TileSize * TileScale, y * TileSize * TileScale), glm::vec2(TileScale, TileScale), 0.0f);
            Tile.AddComponent<FSpriteComponent>("Jungle-Tilemap-Image", TileSize, TileSize, SourceRectX, SourceRectY, ERenderLayer::Background, 0, false);
        }
    }
    MapFile.close();    
    Game::MapWidth = MapNumberColumns * TileSize * TileScale;
    Game::MapHeight = MapNumberRows * TileSize * TileScale;
    
    //Create an entity and Add Some Components to the entity
    Entity Chopper = ECSManagerInstance->CreateEntity();
    Chopper.Tag("Player");
    Chopper.AddComponent<FTransformComponent>(glm::vec2(10.0f, 100.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Chopper.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Chopper.AddComponent<FSpriteComponent>("Chopper-Image", 32, 32, 0, 0, ERenderLayer::Player, 0);
    Chopper.AddComponent<F2DBoxColliderComponent>(32, 32);
    Chopper.AddComponent<FAnimationComponent>(2, 15, true);
    Chopper.AddComponent<FPlayerControllerComponent>(glm::vec2(0.0f, -80.0f), glm::vec2(80.0f, 0.0f), glm::vec2(0.0f, 80.0f), glm::vec2(-80.0f, 0.0f));
    Chopper.AddComponent<FCameraFollowComponent>();
    Chopper.AddComponent<FHealthComponent>(100);
    Chopper.AddComponent<FProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
    
    
    Entity Radar = ECSManagerInstance->CreateEntity();
    Radar.AddComponent<FTransformComponent>(glm::vec2(Game::WindowWidth - 74.0f, 10), glm::vec2(1.0f, 1.0f), 0.0f);
    Radar.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Radar.AddComponent<FSpriteComponent>("Radar-Image", 64, 64, 0, 0, ERenderLayer::UI, 0, true);
    Radar.AddComponent<FAnimationComponent>(8, 5, true);
    
    Entity Tank = ECSManagerInstance->CreateEntity();
    Tank.Group("Enemies");
    Tank.AddComponent<FTransformComponent>(glm::vec2(500.0f, 500.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Tank.AddComponent<FRigidBodyComponent>(glm::vec2(50.0f, 0.0f));
    Tank.AddComponent<FSpriteComponent>("Tank-Image", 32, 32, 0, 0, ERenderLayer::Enemy, 0);
    Tank.AddComponent<F2DBoxColliderComponent>(32, 32);
    Tank.AddComponent<FProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 3000, 10, false);
    Tank.AddComponent<FHealthComponent>(100);
    
    Entity Truck = ECSManagerInstance->CreateEntity();
    Truck.Group("Enemies");
    Truck.AddComponent<FTransformComponent>(glm::vec2(120.0f, 500.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Truck.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Truck.AddComponent<FSpriteComponent>("Truck-Image", 32, 32, 0, 0, ERenderLayer::Enemy, 0);
    Truck.AddComponent<F2DBoxColliderComponent>(32, 32);
    Truck.AddComponent<FProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 5000, 10, false);
    Truck.AddComponent<FHealthComponent>(100);
    
    Entity TreeA = ECSManagerInstance->CreateEntity();
    TreeA.Group("Obstacles");
    TreeA.AddComponent<FTransformComponent>(glm::vec2(600.0f, 495.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    TreeA.AddComponent<FSpriteComponent>("Tree-Image", 16, 32, 0, 0, ERenderLayer::Obstacle, 0);
    TreeA.AddComponent<F2DBoxColliderComponent>(16, 32);

    Entity TreeB = ECSManagerInstance->CreateEntity();
    TreeB.Group("Obstacles");
    TreeB.AddComponent<FTransformComponent>(glm::vec2(400.0f, 495.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    TreeB.AddComponent<FSpriteComponent>("Tree-Image", 16, 32, 0, 0, ERenderLayer::Obstacle, 0);
    TreeB.AddComponent<F2DBoxColliderComponent>(16, 32);
    
    Entity Label = ECSManagerInstance->CreateEntity();
    Label.AddComponent<FTextLabelComponent>(glm::vec2(Game::WindowWidth / 2 - 40, 10.0f), "Hello World!", "Charriot-Font", SDL_Color{255, 255, 255}, true);
    */
    
}
