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
    int x,y;
};

bool operator==(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y) == std::tie(b.x,b.y);
}

bool operator<(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y) < std::tie(b.x,b.y);
}

enum tool_enum_t { e_torch, e_climbing_gear, e_neither };

struct queue_t{
    pos_t pos;
    int dist;
    tool_enum_t tool;
};

bool operator>(const queue_t& a, const queue_t& b){
    return std::tie(a.dist,a.pos.x,a.pos.y,a.tool) > std::tie(b.dist,b.pos.x,b.pos.y,b.tool);
}

struct visited_t{
    pos_t pos;
    tool_enum_t tool;
};

bool operator<(const visited_t& a, const visited_t& b){
    return std::tie(a.pos.y,a.pos.x,a.tool) < std::tie(b.pos.y,b.pos.x,b.tool);
}


struct input_t{
    int depth;
    pos_t target;
};

struct grid_t{
    std::vector<int> data;
    int width;
    int height;

    int& operator()(pos_t p){
        return data[p.y*width+p.x];
    }
};

void print(grid_t& grid){
    gotoxy(0,0);
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << (char)grid({x,y});
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


input_t parse_input(const std::string& file){
    input_t ret;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        if(tokens.size() > 1){
            ret.target = { std::stoi(tokens[0]), std::stoi(tokens[1]) };
        }else{
            ret.depth = std::stoi(tokens[0]);
        }
    }

    return ret;
}

int geological_index(grid_t& erosion_grid, const pos_t& target, const pos_t& p, int depth){
    if(p == pos_t{0,0}){
        return 0;
    }else if(p == target){
        return 0;
    }else if(p.y == 0){
        return p.x * 16807;
    }else if(p.x == 0){
        return p.y * 48271;
    }else{
        return erosion_grid({p.x-1,p.y}) * erosion_grid({p.x,p.y-1});
    }
}

int erosion_level(grid_t& erosion_grid, const pos_t& target, const pos_t& p, int depth){
    int erosion = (geological_index(erosion_grid, target, p, depth) + depth) % 20183;
    erosion_grid(p) = erosion;
    if((erosion % 3) == 0){
        return '.';
    }else if((erosion % 3) == 1){
        return '=';
    }else if((erosion % 3) == 2){
        return '|';
    }else{
        throw std::exception("BOOM!");
    }   
}

int risk_level(grid_t& grid, const pos_t& pos, const pos_t& target){
    int risk = 0;
    for(int y=pos.x; y<=target.y; ++y){
        for(int x=pos.y; x<=target.x; ++x){
            char er = (char)grid({x,y});
            if(er == '.'){
                risk += 0;
            }else if(er == '='){
                risk += 1;
            }else if(er == '|'){
                risk += 2;
            }
        }
    }
    return risk;
}

bool traversable(grid_t& grid, const pos_t& pos){
    return pos.x >= 0 && pos.x < grid.width && pos.y >= 0 && pos.y < grid.height; 
}

int search(grid_t& grid, const pos_t& src, const pos_t& dst){
    std::priority_queue<queue_t, std::vector<queue_t>, std::greater<queue_t>> q;
    std::set<visited_t> visited;
    q.push({{0,0},0,e_torch});

    while(!q.empty()) {
        auto curr = q.top();
        q.pop();

        if(curr.pos == dst && curr.tool == e_torch){
            return curr.dist;
        }

        if(!visited.count({curr.pos,curr.tool}))
        {    
            visited.insert({curr.pos,curr.tool});

            auto push_next = [&](int dist, const pos_t& pos, tool_enum_t tool){
                if(traversable(grid, pos)) { 
                    char region = grid(pos);
                    if(region == '.' && (tool==e_torch || tool==e_climbing_gear) ||
                       region == '=' && (tool==e_climbing_gear || tool==e_neither) ||
                       region == '|' && (tool==e_torch || tool==e_neither)) 
                    { 
                        q.push({pos, curr.dist+dist, tool});
                    }
                }
            };

            push_next(7, curr.pos, e_torch);
            push_next(7, curr.pos, e_climbing_gear);
            push_next(7, curr.pos, e_neither);

            for(int i=0; i<4; i++) {
                pos_t npos = curr.pos;
                if(i == 0){
                    npos.y--;
                }else if(i == 1){
                    npos.x++;
                }else if(i == 2){
                    npos.y++;
                }else if(i == 3){
                    npos.x--;
                }

                push_next(1, npos, curr.tool);
            }

        }

    }

    return -1;
}


void main() {

    auto input = parse_input("../src/day22/day22_input.txt");

    int extra = 100;
    int width = input.target.x+extra+1;
    int height = input.target.y+extra+1;

    grid_t grid { std::vector<int>(width*height, '.'), width, height };
    grid_t erosion_grid { std::vector<int>(width*height, 0), width, height };

    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            grid({x,y}) = erosion_level(erosion_grid, input.target, {x,y}, input.depth);
        }
    }

    std::cout << "part1: " << risk_level(grid, {0,0}, input.target) << std::endl;
    std::cout << "part2: " << search(grid, {0,0}, input.target) << std::endl;
}

