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
    return std::abs(a.x-b.x) + std::abs(a.y-b.y); 
}

void main()
{
    auto coords = parse_input("../src/day06/day6_input.txt");

    auto max_pos_x = *std::max_element(coords.begin(), coords.end(), [&](auto& a, auto& b){ return a.x < b.x; });
    auto max_pos_y = *std::max_element(coords.begin(), coords.end(), [&](auto& a, auto& b){ return a.y < b.y; });

    int width  = max_pos_x.x+1;
    int height = max_pos_y.y+1;

    std::map<int,int> region_map;

    int start = 1;
    std::set<int> finites;
    std::generate_n(std::inserter(finites, finites.end()), coords.size(), [&]{ return start++; }); 

    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){

            pos_t pos {x,y};

            std::vector<int> distances;
            std::transform(coords.begin(), coords.end(), std::back_inserter(distances), [&](auto& a){
                return manhatten_distance(pos,a);
            });

            auto it = std::min_element(distances.begin(), distances.end());
            int region = (int)std::distance(distances.begin(), it)+1;

            if(std::count(distances.begin(), distances.end(), *it) == 1){
                region_map[region]++;
            }

            if(x==0 || x==width-1 || y==0 || y==height-1){
                finites.erase(region);
            }

        }
    }

    int max_area = INT_MIN;
    for(auto s : finites){
        max_area = std::max(max_area, region_map[s]);
    }

    std::cout << "part1: " << max_area << std::endl;



    int count = 0;
    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){

            pos_t pos {x,y};

            size_t total_distance = std::accumulate(coords.begin(), coords.end(), 0ULL, [&](auto& a, auto& b){
                return a + manhatten_distance(pos,b);
            });

            if(total_distance < 10000){
                count++;
            }

        }
    }

    std::cout << "part2: " << count << std::endl;



}