#include <vector>
#include <list>
#include <numeric>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>
#include <set>
#include <stack>
#include <omp.h>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void gotoxy(int x, int y) { 
    COORD pos = {(short)x, (short)y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

struct pos_t{
    int x,y;
};

bool operator==(const pos_t& a, const pos_t& b){
    return a.x==b.x && a.y==b.y;
}

struct cart_t{
    pos_t pos;
    char dir;
    char turn;
    //char prev;
};

struct grid_t{
    std::vector<char> data;
    int width;
    int height;
};

grid_t parse_input(const std::string& file){
    grid_t input;
    input.width = 0;
    input.height = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::copy(line.begin(), line.end(), std::back_inserter(input.data));  
        input.width = (int)line.size();
        input.height++;
    }

    return input;
}

bool is_dir(char d){
    return d=='<' || d=='^' || d=='>' || d=='v';
}

char next_dir(char d, int turn){
    if(d == '^'){
        if(turn == 0){
            return '<';
        }else if(turn == 1){
            return '^';
        }else if(turn == 2){
            return '>';
        }
    }else if(d == '>'){
        if(turn == 0){
            return '^';
        }else if(turn == 1){
            return '>';
        }else if(turn == 2){
            return 'v';
        }
    }else if(d == 'v'){
        if(turn == 0){
            return '>';
        }else if(turn == 1){
            return 'v';
        }else if(turn == 2){
            return '<';
        }
    }
    else if(d == '<'){
        if(turn == 0){
            return 'v';
        }else if(turn == 1){
            return '<';
        }else if(turn == 2){
            return '^';
        }
    }

    return '*';
}

std::vector<cart_t> find_carts(const grid_t& grid){
    std::vector<cart_t> carts;

    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            auto c = grid.data[y*grid.width+x];
            if(c == 'v' || c == '<' || c == '^' || c == '>'){
                carts.push_back( { {x, y},  c, 0 } );
            }
        }
    }

    return carts;
}

grid_t remove_carts(const grid_t& grid){
    grid_t new_grid = grid;

    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            auto c = grid.data[y*grid.width+x];
            if(c == 'v' || c == '^'){
                new_grid.data[y*grid.width+x] = '|';
            }else if(c == '<' || c == '>'){
                new_grid.data[y*grid.width+x] = '-';
            }
        }
    }

    return new_grid;
}

std::vector<pos_t> tick(grid_t& grid, const grid_t& orig_grid, std::vector<cart_t>& carts)
{
    auto idx = [&](const pos_t& p){
        return p.y*grid.width+p.x;
    };

    // get in proper xy order
    std::sort(carts.begin(), carts.end(), [&](auto& a, auto& b){
        return idx(a.pos) < idx(b.pos);
    });

    std::vector<int> crashes;

    for(int i=0; i<carts.size(); ++i)
    {
        if(std::find(crashes.begin(), crashes.end(), i) != crashes.end()){
            continue;
        }

        auto& cart = carts[i];

        grid.data[idx(cart.pos)] = orig_grid.data[idx(cart.pos)];

        switch(cart.dir){
            case '<':   cart.pos.x--;   break;
            case '^':   cart.pos.y--;   break;
            case '>':   cart.pos.x++;   break;
            case 'v':   cart.pos.y++;   break;
        }

        auto c = grid.data[idx(cart.pos)];

        if(c == '\\'){
            switch(cart.dir){
                case '<':   cart.dir = '^';   break;
                case '^':   cart.dir = '<';   break;
                case '>':   cart.dir = 'v';   break;
                case 'v':   cart.dir = '>';   break;
            }
        }else if(c == '/'){
            switch(cart.dir){
                case '<':   cart.dir = 'v';   break;
                case '^':   cart.dir = '>';   break;
                case '>':   cart.dir = '^';   break;
                case 'v':   cart.dir = '<';   break;
            }
        }else if(c == '+'){
            cart.dir = next_dir(cart.dir, cart.turn);
            cart.turn = (cart.turn+1) % 3;
        }

        if(is_dir(c)){
            // omg we got a crash!
            crashes.push_back(i);
            grid.data[idx(cart.pos)] = orig_grid.data[idx(cart.pos)];

            for(int j=0; j<carts.size(); ++j){
                if(j!=i && carts[j].pos == cart.pos){
                    crashes.push_back(j);
                    grid.data[idx(carts[j].pos)] = orig_grid.data[idx(carts[j].pos)];
                }
            }

        }else{
            grid.data[idx(cart.pos)] = cart.dir;
        }      
    }

    std::vector<pos_t> crash_pos;

    std::sort(crashes.begin(), crashes.end());
    for(int i=(int)crashes.size()-1; i >= 0; i--){
        auto crash = carts.begin() + crashes[i];
        crash_pos.push_back(crash->pos);
        carts.erase(crash);
    }
    
    return crash_pos;
}

void print(const grid_t& grid)
{
    gotoxy(0,0);
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << grid.data[y*grid.width+x];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void main()
{
    {
        auto grid = parse_input("../src/day13/day13_input.txt");
        auto orig_grid = remove_carts(grid);

        auto carts = find_carts(grid);
        auto orig_carts = carts;

        std::vector<pos_t> crash_sites;
        while (carts.size() == orig_carts.size()) {
            crash_sites = tick(grid, orig_grid, carts);
        }

        std::cout << "part1: " << crash_sites.front().x << "," << crash_sites.front().y << std::endl;
    }


    {
        auto grid = parse_input("../src/day13/day13_input.txt");
        auto orig_grid = remove_carts(grid);
        auto carts = find_carts(grid);

        while (carts.size() > 1) {
            tick(grid, orig_grid, carts);
        }

        std::cout << "part2: " << carts.front().pos.x << "," << carts.front().pos.y << std::endl;
    }



}