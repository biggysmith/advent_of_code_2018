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
#include <bitset>
#include <emmintrin.h>
#include <intrin.h>

struct pos_t{
    union{
        struct {
            int x,y,z,w;
        };
        __m128i data;
    };
};

bool operator<(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y,a.z,a.w) < std::tie(b.x,b.y,b.z,b.w);
}

bool operator==(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y,a.z,a.w) == std::tie(b.x,b.y,b.z,b.w);
}

std::vector<pos_t> parse_input(const std::string& file){
    std::vector<pos_t> ret;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("-?[0-9]+");
        std::sregex_token_iterator first{ line.begin(), line.end(), re }, last;
        std::vector<std::string> tokens{ first, last };

        ret.push_back({ std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]) });
    }

    return ret;
}

// why?!?
int manhatten_distance(const pos_t& a, const pos_t& b){
    //return abs(a.x-b.x) + abs(a.y-b.y) + abs(a.z-b.z) + abs(a.w-b.w);
    auto c = _mm_abs_epi32(_mm_sub_epi32(a.data,b.data));
    auto d = _mm_hadd_epi32(c,c);
    return  _mm_extract_epi32(_mm_hadd_epi32(d,d), 0);
}

using graph_t = std::map<pos_t,std::vector<pos_t>>;

void main() 
{
    auto input = parse_input("../src/day25/day25_input.txt");

    graph_t graph;

    for(auto& p0 : input){
        for(auto& p1 : input){
             if(manhatten_distance(p0,p1) <= 3){
                graph[p0].push_back(p1);
             }
        }
    }

    auto find_number_constellations = [&]
    {
        int constellations = 0;

        std::queue<pos_t> q;
        std::set<pos_t> visited;

        q.push(input.front());
        visited.insert(input.front());

        while(!q.empty()){
            auto curr = q.front();
            q.pop();

            for(auto& adj : graph[curr]){
                if(!visited.count(adj)){
                    q.push(adj);
                    visited.insert(adj);
                }
            }

            if(q.empty()){
                constellations++;

                for(auto& p : input){
                    if(!visited.count(p)){
                        q.push(p);
                        visited.insert(p);
                        break;
                    }
                }
            }
        }

        return constellations;

    };

    std::cout << "part1: " << find_number_constellations() << std::endl;
    std::cout << "part2: " << "Thanks Rudolf!" << std::endl;

}

