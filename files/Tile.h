#pragma once
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class Tile
{
    Sprite background;
    Sprite foreground;
    Sprite icon;
    Sprite flagicon;


    bool isdebug;

public:
    vector<Tile*> neighbors;
    int numbombs;
    bool isbomb;
    bool isflagged;
    bool isrevealed;

    Tile(map<string,Texture> &textures, float xpos, float ypos);
    void setBombState(bool bombstate);
    void updateIcon(map<string, Texture> &textures);

    bool toggleFlag();
    void toggleDebug();
    bool reveal(int &counter);
    void draw(RenderWindow &w);
};

Tile::Tile(map<string,Texture> &textures, float xpos, float ypos)
{
    background.setPosition(xpos, ypos);
    foreground.setPosition(xpos, ypos);
    flagicon.setPosition(xpos, ypos);
    icon.setPosition(xpos, ypos);

    background.setTexture(textures["revealed"]);
    foreground.setTexture(textures["hidden"]);
    flagicon.setTexture(textures["flag"]);

    flagicon.setColor(Color(255, 255, 255, 0));
    isbomb = false;
    isrevealed = false;
    isflagged = false;
    isdebug = false;
}


void Tile::setBombState(bool bombstate)
{
    isbomb = bombstate;
}


void Tile::updateIcon(map<string, Texture> &textures)
{
    numbombs = 0;
    for (int c = 0; c < 8; c++)
    {
        if (neighbors[c] != nullptr && neighbors[c]->isbomb)
        {
            numbombs++;
        }
    }
    if (isbomb)
        icon.setTexture(textures["bomb"]);
    else
    {
        switch (numbombs)
        {
            case 1: icon.setTexture(textures["one"]); break;
            case 2: icon.setTexture(textures["two"]); break;
            case 3: icon.setTexture(textures["three"]); break;
            case 4: icon.setTexture(textures["four"]); break;
            case 5: icon.setTexture(textures["five"]); break;
            case 6: icon.setTexture(textures["six"]); break;
            case 7: icon.setTexture(textures["seven"]); break;
            case 8: icon.setTexture(textures["eight"]); break;
            default: icon.setTexture(textures["revealed"]); break;
        }
    }
    icon.setColor(Color(255, 255, 255, 0));
    flagicon.setColor(Color(255, 255, 255, 0));
    foreground.setColor(Color(255, 255, 255, 255));
    isrevealed = false;
    isflagged = false;
    isdebug = false;
}


bool Tile::toggleFlag()
{
    if (!isrevealed)
    {
        if (isflagged)
        {
            isflagged = false;
            flagicon.setColor(Color(255, 255, 255, 0));
        }
        else
        {
            isflagged = true;
            flagicon.setColor(Color(255, 255, 255, 255));
        }
    }
    return isflagged;
}


void Tile::toggleDebug()
{
    if (isbomb && !isrevealed)
    {
        if (isdebug)
        {
            isdebug = false;
            icon.setColor(Color(255, 255, 255, 0));
        }
        else
        {
            isdebug = true;
            icon.setColor(Color(255, 255, 255, 255));
        }
    }
}


bool Tile::reveal(int &counter)
{
    if (!isrevealed && !isflagged)
    {
        isrevealed = true;
        counter++;
        foreground.setColor(Color(255, 255, 255, 0));
        icon.setColor(Color(255, 255, 255, 255));
        if (numbombs == 0 && !isbomb)
        {
            for (int c = 0; c < 8; c++)
            {
                if (neighbors[c] != nullptr && !(neighbors[c]->isbomb))
                    neighbors[c]->reveal(counter);
            }
        }
    }
    return isbomb;
}


void Tile::draw(RenderWindow &w)
{
    w.draw(background);
    w.draw(foreground);
    w.draw(icon);
    w.draw(flagicon);
}