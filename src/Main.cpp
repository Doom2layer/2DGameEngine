#include <iostream>
#include "Game/Game.h"

int main(int argc, char* argv[]) 
{

    Game GameInstance;
    
    GameInstance.Initialize();
    GameInstance.Run();
    GameInstance.Destroy();
    
    return 0;
}
