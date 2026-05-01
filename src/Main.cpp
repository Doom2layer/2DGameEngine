#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) 
{

    Game GameInstance;
    
    GameInstance.Initialize();
    GameInstance.Run();
    GameInstance.Destroy();
    
    return 0;
}
