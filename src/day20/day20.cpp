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
#include <list>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void gotoxy(int x, int y) { 
    COORD pos = {(short)x, (short)y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

std::string parse_input(const std::string& file){
    std::ifstream file_stream(file);
    std::string line;
    std::getline(file_stream, line);
    return line;
}

struct pos_t{
    int x,y;
    int dist = 0;
};

bool operator<(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y) < std::tie(b.x,b.y);
}

struct grid_t{
    std::vector<char> data;
    int width;
    int height;

    char& val(pos_t pos){
        return data[pos.y*width+pos.x];
    }
};

void print(grid_t& grid){
    gotoxy(0,0);
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << grid.val({x,y});
        }   
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


using graph_t = std::map<pos_t,std::vector<pos_t>>;

void add_edge(graph_t& graph, const pos_t& n0, const pos_t& n1) {
    graph[n0].push_back(n1);
}

template<typename It>
void build_adjacency_graph(graph_t& graph, It& it, pos_t pos){
    pos_t start_pos = pos;
    while(*it++ != '$'){
        if(*it == 'N'){
            pos_t npos = {pos.x, pos.y-1};
            add_edge(graph, pos, npos);
            add_edge(graph, npos, pos);
            pos = npos;
        }
        else if(*it == 'E'){
            pos_t npos = {pos.x+1, pos.y};
            add_edge(graph, pos, npos);
            add_edge(graph, npos, pos);
            pos = npos;
        }
        else if(*it == 'S'){
            pos_t npos = {pos.x, pos.y+1};
            add_edge(graph, pos, npos);
            add_edge(graph, npos, pos);
            pos = npos;
        }
        else if(*it == 'W'){
            pos_t npos = {pos.x-1, pos.y};
            add_edge(graph, pos, npos);
            add_edge(graph, npos, pos);
            pos = npos;
        }
        else if(*it == '('){
            build_adjacency_graph(graph, it, pos);
        }
        else if(*it == ')'){
            return;
        }
        else if(*it == '|'){
            pos = start_pos;
        }
    }
}

void main() 
{
    auto input = parse_input("../src/day20/day20_input.txt");

    graph_t graph;
    build_adjacency_graph(graph, input.begin(), {0,0});

    std::queue<pos_t> q;
    std::set<pos_t> visited;

    int max_dist = INT_MIN;
    int dist_over_1000 = 0;

    q.push({0,0});

    // bfs time!
    while(!q.empty()) {
        pos_t current = q.front();
        q.pop();

        if(!visited.count(current)){
            visited.insert(current);
            max_dist = std::max(max_dist, current.dist);
            dist_over_1000 += current.dist >= 1000;
            for(auto& adj : graph[current]){
                q.push({adj.x, adj.y, current.dist+1});
            }
        }
    }

    std::cout << "part1: " << max_dist << std::endl;
    std::cout << "part2: " << dist_over_1000 << std::endl;
}