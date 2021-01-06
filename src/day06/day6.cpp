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
#include <omp.h>

struct pos_t{
    int x,y;
};

std::vector<pos_t> parse_input(const std::string& file){
    std::vector<pos_t> input;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        size_t comma = line.find(',');
        input.push_back({ std::stoi(line.substr(0, comma)), std::stoi(line.substr(comma+1)) });
    }

    return input;
}

int manhatten_distance(const pos_t& a,const pos_t& b){
    return std::abs(a.x-b.x) + std:: abs(a.y-b.y); 
}

void main()
{
    auto input = parse_input("../src/day06/day6_input.txt");

    auto max_pos_x = *std::max_element(input.begin(), input.end(), [&](auto& a, auto& b){ return a.x < b.x; });
    auto max_pos_y = *std::max_element(input.begin(), input.end(), [&](auto& a, auto& b){ return a.y < b.y; });

    int width  = max_pos_x.x+1;
    int height = max_pos_y.y+1;
    std::vector<int> space(width*height, 0);

    std::set<int> finites;
    std::generate_n(std::inserter(finites, finites.end()), input.size(), [&]{ return (int)finites.size()+1; }); // std::iota doesnt work with set 

    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){

            pos_t pos {x,y};

            std::vector<int> distances;
            std::transform(input.begin(), input.end(), std::back_inserter(distances), [&](auto& a){
                return manhatten_distance(pos,a);
            });

            auto it = std::min_element(distances.begin(), distances.end());

            if(std::count(distances.begin(), distances.end(), *it) == 1){
                space[y*width+x] = (int)std::distance(distances.begin(), it) + 1;
            }

            if(x==0 || x==width-1 || y==0 || y==height-1){
                finites.erase(space[y*width+x]);
            }

        }
    }

    int max_area = INT_MIN;
    for(auto s : finites){
        max_area = std::max(max_area, (int)std::count(space.begin(), space.end(), s));
    }

    std::cout << "part1: " << max_area << std::endl;



    std::fill(space.begin(), space.end(), 0);

    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){

            pos_t pos {x,y};

            size_t total_distance = std::accumulate(input.begin(), input.end(), 0ULL, [&](auto& a, auto& b){
                return a + manhatten_distance(pos,b);
            });

            if(total_distance < 10000){
                space[y*width+x] = 1;
            }

        }
    }

    std::cout << "part2: " << std::count(space.begin(), space.end(), 1) << std::endl;



}