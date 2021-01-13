#include <iostream>
#include <ostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <numeric>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <regex>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void gotoxy(int x, int y) { 
    COORD pos = {(short)x, (short)y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

struct pos_t{
    int x, y;
};

struct vein_t{
    int x0; int x1;
    int y0; int y1;
};

using veins_t = std::vector<vein_t>;

struct grid_t{
    std::vector<char> data;
    int width;
    int height;

    char& operator()(int x,int y){
        return data[y*width+x];
    }
};


veins_t parse_input(const std::string& file){
    veins_t ret;

    int new_lines = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("[0-9]+|[a-z]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        if(tokens[0] == "x"){
            ret.push_back({ std::stoi(tokens[1]), std::stoi(tokens[1]), std::stoi(tokens[3]), std::stoi(tokens[4]) });
        }else if(tokens[0] == "y"){
            ret.push_back({ std::stoi(tokens[3]), std::stoi(tokens[4]), std::stoi(tokens[1]), std::stoi(tokens[1]) });
        }
    }

    return ret;
}


void print(const grid_t& grid){
    gotoxy(0,0);
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << grid.data[y*grid.width+x];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool traversable(grid_t& grid, int x, int y){
    return grid(x,y)!='#' && grid(x,y)!='~';
}

void process(grid_t& grid, int x, int y)
{
    if(y >= grid.height-1 || !traversable(grid,x,y)){
        return;
    }

    if(!traversable(grid,x,y+1)){
        int lx = x;
        while(traversable(grid,lx,y) && !traversable(grid,lx,y+1)){ // to left wall
            grid(lx--,y) = '|';
        }
        if(traversable(grid,lx,y+1)){
            process(grid,lx,y);
        }

        int rx = x+1;
        while(traversable(grid,rx,y) && !traversable(grid,rx,y+1)){ // to right wall
            grid(rx++,y) = '|';
        }
        if(traversable(grid,rx,y+1)){
            process(grid,rx,y);
        }

        if(grid(lx,y) == '#' && grid(rx,y) == '#') {
            int xx = lx+1;
            while(xx<rx){
                grid(xx++,y) = '~';
            }
		}
    }else if(grid(x,y) == '.'){
        grid(x,y) = '|';
		process(grid, x, y+1);
		if (grid(x,y+1) == '~') {
			process(grid, x, y);
		}
    }

}

void main() {

    auto input = parse_input("../src/day17/day17_input.txt");

    int min_x = INT_MAX;
    int min_y = INT_MAX;
    int max_x = INT_MIN;
    int max_y = INT_MIN;

    for(auto& vein : input){
        min_x = std::min(vein.x0, min_x);
        min_y = std::min(vein.y0, min_y);
        max_x = std::max(vein.x1, max_x);
        max_y = std::max(vein.y1, max_y);
    }

    int orig_min_y = min_y;

    min_y = 0;
    max_x += 1;
    max_y += 1;
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;

    grid_t grid = { std::vector<char>(width*height,'.'), width, height };

    for(auto& vein : input){
        for(int y=vein.y0; y<=vein.y1; ++y){
            for(int x=vein.x0; x<=vein.x1; ++x){
                grid.data[(y-min_y)*width+(x-min_x)] = '#';
            }
        }
    }

    process(grid, 500-min_x, 0);
    grid.data[500-min_x] = '+';

    size_t count_or = std::count_if(grid.data.begin()+orig_min_y*width, grid.data.end(), [](auto& c){
        return c=='|';
    }); 

    size_t count_sqwiggle = std::count_if(grid.data.begin()+orig_min_y*width, grid.data.end(), [](auto& c){
        return c=='~';
    });

    std::cout << "part1: " << count_or + count_sqwiggle << std::endl;
    std::cout << "part2: " << count_sqwiggle  << std::endl;
}

