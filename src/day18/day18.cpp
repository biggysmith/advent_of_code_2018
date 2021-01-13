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

struct grid_t{
    std::vector<char> data;
    int width;
    int height;

    char& operator()(int x,int y){
        return data[y*width+x];
    }
};


grid_t parse_input(const std::string& file){
    grid_t ret;
    ret.height = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::copy(line.begin(), line.end(), std::back_inserter(ret.data));
        ret.width = (int)line.size();
        ret.height++;
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

size_t grid_hash(const grid_t& grid) {
    std::string_view view(grid.data.data(), grid.data.size());
    return std::hash<std::string_view>()(view);
}

void main() {

    auto orig_grid = parse_input("../src/day18/day18_input.txt");
    auto grid = orig_grid;
    auto grid_copy = grid;

    auto count = [&](int x, int y, int& trees, int& yards){
        if(x>=0 && x<grid.width && y>=0 && y<grid.height){
            char c = grid(x,y);
            trees += c == '|';
            yards += c == '#';
        }
    };

    auto update = [&]
    {
        #pragma omp parallel for
        for(int y=0; y<grid.height; ++y){
            for(int x=0; x<grid.width; ++x){
                
                int trees = 0;
                int yards = 0;

                count(x-1,y-1,trees,yards);
                count(x,y-1,trees,yards);
                count(x+1,y-1,trees,yards);

                count(x-1,y,trees,yards);
                //count(x,y,trees,yards);
                count(x+1,y,trees,yards);

                count(x-1,y+1,trees,yards);
                count(x,y+1,trees,yards);
                count(x+1,y+1,trees,yards);

                if(grid(x,y) == '.' && trees >= 3){
                    grid_copy(x, y) = '|';
                }
                else if(grid(x,y) == '|' && yards >= 3){
                    grid_copy(x, y) = '#';
                }
                else if(grid(x,y) == '#' && !(trees >= 1 && yards >= 1)){
                    grid_copy(x, y) = '.';
                }
                else{
                    grid_copy(x, y) = grid(x, y);
                }

            }
        }

        std::swap(grid, grid_copy);
    };

    auto score = [&]{
        size_t trees = std::count(grid.data.begin(), grid.data.end(), '|');
        size_t yards = std::count(grid.data.begin(), grid.data.end(), '#');
        return trees*yards;
    };

    for(size_t i=0; i<10; ++i){
        update();
    }

    std::cout << "part1: " << score() << std::endl;

    size_t start = -1;
    size_t end = -1;
    size_t width = -1;

    grid = orig_grid;
    std::vector<size_t> scores;

    std::vector<size_t> grid_hashes;
    grid_hashes.push_back(grid_hash(grid));

    for(size_t i=0; i<1'000'000'000; ++i){
        update();

        size_t s = grid_hash(grid);
        auto it = std::find(grid_hashes.begin(), grid_hashes.end(), s);
        if(it != grid_hashes.end() && start == -1){      
            start = std::distance(grid_hashes.begin(), it);
            end = i+1;
            width = end - start;
        }else{
            grid_hashes.push_back(s);
        }

        if(start != -1){
            scores.push_back(score());
        }

        if(scores.size() == width){
            size_t j = (1'000'000'000-end) % width;
            std::cout << "part2: " << scores[j] << std::endl;
            return;
        }
    }
    
}

