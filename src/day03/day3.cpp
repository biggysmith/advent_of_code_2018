#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

struct claim_t{
    int claim_id;
    int x;
    int y;
    int width;
    int height;
};

struct fabric_t{
    std::vector<char> cloth;
    int width;
    int height;
};

std::vector<claim_t> parse_input(const std::string& file){
    std::vector<claim_t> input;

    std::ifstream file_stream(file);

    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        input.push_back({ 
            std::stoi(tokens[0]), 
            std::stoi(tokens[1]), 
            std::stoi(tokens[2]), 
            std::stoi(tokens[3]), 
            std::stoi(tokens[4]) 
        });
    }

    return input;
}

void fill_claim(fabric_t& fabric, const claim_t& claim)
{
    for(int y=claim.y; y<claim.y+claim.height; ++y){
        for(int x=claim.x; x<claim.x+claim.width; ++x){
        
            char& c = fabric.cloth[y*fabric.width+x];
            if(c == '.'){
                c = '*';
            }else if(c == '*'){
                c = 'X';
            }

        }
    }
}

bool claim_overlaps(fabric_t& fabric, const claim_t& claim)
{
    for(int y=claim.y; y<claim.y+claim.height; ++y){
        for(int x=claim.x; x<claim.x+claim.width; ++x){
        
            if(fabric.cloth[y*fabric.width+x] == 'X'){
                return true;
            }

        }
    }
    return false;
}

void main()
{
    auto input = parse_input("../src/day03/day3_input.txt");

    fabric_t fabric;
    fabric.width = 1000;
    fabric.height = 1000;
    fabric.cloth.resize(fabric.width*fabric.height, '.');

    for(auto& c : input){
        fill_claim(fabric, c);
    }

    std::cout << "part1: " << std::count(fabric.cloth.begin(), fabric.cloth.end(), 'X') << std::endl;

    for(auto& c : input){
        if(!claim_overlaps(fabric, c)){
            std::cout << "part2: " << c.claim_id << std::endl;
            break;
        }
    }
}