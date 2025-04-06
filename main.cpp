#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Vec2.h" 
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "Game.h"

int main()
{
    Game g("config.txt");
    g.run();
    return 0;
}