#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "print.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

struct v2 {
    float x;
    float y;

    float distance() {
        return sqrt(x * x + y * y);
    }

    v2(int a, int b) {
        x = a;
        y = b;
    }

    v2(float a, float b) {
        x = a;
        y = b;
    }

    v2() {

    }

    v2 makeunit() {
        v2 temp;
        float d = distance();
        temp.x = x / d;
        temp.y = y / d;
        return temp;
    }

    v2 operator -(v2 other) {
        v2 temp;
        temp.x = x - other.x;
        temp.y = y - other.y;
        return temp;
    }

    v2 operator +(v2 other) {
        v2 temp;
        temp.x = x + other.x;
        temp.y = y + other.y;
        return temp;
    }

    v2 operator /(float other) {
        v2 temp;
        temp.x = x / other;
        temp.y = y / other;
        return temp;
    }

    v2 operator *(float other) {
        v2 temp;
        temp.x = x * other;
        temp.y = y * other;
        return temp;
    }

    void operator +=(v2 other) {
        x += other.x;
        y += other.y;
    }

    void operator -=(v2 other) {
        x -= other.x;
        y -= other.y;
    }
};

struct tile {
    v2 pos;
    vector<int> gridpos;
    int value;
    bool clicked;
    bool marked;
    vector<v2> vertices;
    tile(vector<int> g, int v) {
        gridpos = g;
        value = v;
        clicked = false;
        marked = false;
        pos = { 200 + (g[0] - 1) * 50, 200 + (g[1] - 1) * 50 };
        vertices = { pos, {pos.x + 50, pos.y + 50} };
    }
};

bool emptytile(vector<int> target, set<vector<int>> minesgrid) {
    int x = target[0];
    int y = target[1];
    if (x > 0 and x < 9 and y > 0 and y < 9){
        if (minesgrid.find(target) == minesgrid.end()) {
            return true;
        }
        else { return false; }
    }
    else { return false; }
}

void definevalues(vector<tile>& tiles) {
    std::set<vector<int>> minesgrid;
    int index;
    vector<int> mine;
    srand(time(NULL));
    while (minesgrid.size() < 15) {

        index = rand() % 63;
        mine = tiles[index].gridpos;
        minesgrid.emplace(mine);
    }
    for (vector<int> mine : minesgrid) {
        for (tile& t : tiles) {
            if (mine == t.gridpos) {
                t.value = -1;
            }
        }
    }
    for (vector<int> mine : minesgrid) {
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vector<int> target = { mine[0] + x, mine[1] + y };
                if (emptytile(target, minesgrid)) {
                    for (tile& t : tiles) {
                        if (t.gridpos == target) {
                            t.value++;
                        }
                    }
                }
            }
        }
    }
}

void drawtiles(vector<tile> tiles, olc::PixelGameEngine* engine) {
    for (tile t : tiles) {
        engine->FillRect(t.pos.x, t.pos.y, 50, 50, olc::GREY);
        engine->DrawRect(t.pos.x, t.pos.y, 50, 50, olc::WHITE);
        if (t.clicked) {
            olc::Pixel color = (t.value == -1) ? olc::RED : olc::BLACK;
            string writeit = (t.value == -1) ? "*" : to_string(t.value);
            engine->DrawString(t.pos.x + 17, t.pos.y + 17, writeit, color, 2);
        }
        if (t.marked) {
            engine->DrawString(t.pos.x + 17, t.pos.y + 17, "M", olc::BLUE, 2);
        }
    }
}

bool mouseintile(v2 mousepos, tile t) {
    if (mousepos.x > t.pos.x and mousepos.x < t.vertices[1].x and mousepos.y > t.pos.y and mousepos.y < t.vertices[1].y) {
        return true;
    }
    else { return false; }
}

void creategame(vector<tile>& tiles) {
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            tiles.push_back(tile({ x, y }, 0));
        }
    }
    definevalues(tiles);
}

void revealzeros(vector<tile>& tiles, tile selected) {
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vector<int> target = { selected.gridpos[0] + x, selected.gridpos[1] + y };
            for (tile& t : tiles) {
                if (t.gridpos == target and t.value == 0) {
                    t.clicked = true;
                    //revealzeros(tiles, t);
                }
            }
        }
    }
}

struct table : olc::PixelGameEngine {
    vector<tile> tiles;
    bool OnUserCreate() {
        creategame(tiles);
        return true;
    }

    bool gameover = false;

    bool OnUserUpdate(float dt) {
        Clear(olc::BLACK);
        drawtiles(tiles, this);
        v2 mousepos = { GetMouseX(), GetMouseY() };
        if (gameover == false) {
            if (GetMouse(0).bPressed) {
                if (mousepos.x >= 200 and mousepos.x <= 600 and mousepos.y >= 200 and mousepos.y <= 600) {
                    for (tile& t : tiles) {
                        if (mouseintile(mousepos, t)) {
                            t.clicked = true;
                            if (t.value == -1) { gameover = true; }
                            if (t.value == 0) { revealzeros(tiles, t); }
                        }
                    }
                }
            }
            if (GetMouse(1).bPressed) {
                if (mousepos.x >= 200 and mousepos.x <= 600 and mousepos.y >= 200 and mousepos.y <= 600) {
                    for (tile& t : tiles) {
                        if (mouseintile(mousepos, t) and t.clicked == false) {
                            if (t.marked) { t.marked = false; }
                            else { t.marked = true; }
                        }
                    }
                }
            }
        }
        else {
            DrawString(200, 50, "You lost!\n\nPress R to reveal the board\n\nClick to restart", olc::WHITE, 2);
            if (GetKey(olc::R).bPressed) {
                for (tile& t : tiles) {
                    t.clicked = true;
                    t.marked = false;
                }
            }
            if (GetMouse(0).bPressed) {
                tiles.clear();
                creategame(tiles);
                gameover = false;
            }
        }
        return true;
    }

};

int main()
{
    table game;
    game.Construct(800, 800, 1, 1);
    game.Start();
}
