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

struct vec_t{
    int x,y;
};

vec_t operator*(const vec_t& a, int m){
    return {a.x*m, a.y*m};
}

vec_t& operator+=(vec_t& a, const vec_t& b){
    a.x += b.x;
    a.y += b.y;
    return a;
}

struct state_t{
    vec_t pos;
    vec_t vel;
};

std::vector<state_t> parse_input(const std::string& file){
    std::vector<state_t> input;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("-?[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};
        input.push_back({ std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]) });
    }

    return input;
}


void main()
{
    auto points = parse_input("../src/day10/day10_input.txt");

    int width = 65;
    int height = 11;
    std::vector<char> screen(width*height, '.');
    
    int seconds = 10905;

    vec_t min_pos = { INT_MAX, INT_MAX };
    vec_t max_pos = { INT_MIN, INT_MIN };

    for(auto& p : points){
        p.pos += p.vel * seconds;

        min_pos.x = std::min(min_pos.x, p.pos.x);
        min_pos.y = std::min(min_pos.y, p.pos.y);

        max_pos.x = std::max(max_pos.x, p.pos.x);
        max_pos.y = std::max(max_pos.y, p.pos.y);
    };


    for(auto& p : points){
        screen[(p.pos.y-min_pos.y)*width+(p.pos.x-min_pos.x)] = '#';
    }

    std::cout << "part1: " << std::endl;
    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){
            std::cout << screen[y*width+x];
        }
        std::cout << std::endl;
    }
    std::cout << "part2: " << seconds << std::endl;

}