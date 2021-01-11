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

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void gotoxy(int x, int y) { 
    COORD pos = {(short)x, (short)y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

#define ELF 1
#define GOBLIN 2

struct pos_t {
    int x,y;
};

bool operator==(const pos_t& a, const pos_t& b) {
    return a.x == b.x && a.y == b.y;
}

bool operator<(const pos_t& a, const pos_t& b) {
    return a.y == b.y ? a.x < b.x : a.y < b.y;
}

struct grid_t{
    std::vector<int> data;
    int width;
    int height;

    int& operator()(int x,int y){ return data[y*width+x]; }
    int& operator()(const pos_t& p){ return data[p.y*width+p.x]; }
};


int manhatten_distance(const pos_t& a, const pos_t& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

struct unit_t {
    int type;
    pos_t pos;
    int attack_power;
    int hit_points;
};

bool diff(const unit_t& a, const unit_t& b) {
    return a.type != b.type;
}

bool no_nearby_enemies(std::vector<unit_t> &units, unit_t& unit){
    return std::none_of(units.begin(), units.end(), [&](auto& u) { 
        return diff(unit, u) && u.hit_points > 0; 
    });
}

bool attack(std::vector<unit_t> &units, unit_t& unit) 
{
    if (no_nearby_enemies(units, unit)) {
        return false;
    }

    std::vector<unit_t*> adjacent;
    for (auto& u : units) {
        if (diff(unit, u) && u.hit_points > 0 && manhatten_distance(u.pos, unit.pos) == 1) {
            adjacent.push_back(&u);
        }
    }

    if (adjacent.size() == 0) {
        return true;
    }

    std::sort(adjacent.begin(), adjacent.end(), [&](auto& a, auto& b) {
       if (a->hit_points == b->hit_points)
           return a->pos < b->pos;
       return a->hit_points < b->hit_points;
    });
    
    adjacent.front()->hit_points -= unit.attack_power;
    return true;
}

bool blocked(const std::vector<unit_t> &units, const pos_t& pos) {
    return std::any_of(units.begin(), units.end(), [&](auto& u) { 
        return u.pos == pos && u.hit_points > 0; 
    });
}

pos_t next_pos(pos_t pos, int i){
    if(i==0){
        pos.x--;
    }else if(i==1){
        pos.x++;
    }else if(i==2){
        pos.y--;
    }else if(i==3){
        pos.y++;
    }
    return pos;
}

void move(grid_t& grid, const std::vector<unit_t>& units, unit_t& unit) 
{
    auto bfs = [&](const pos_t& start){
        std::stack<pos_t> q;
        grid_t dist { std::vector<int>(grid.data.size(), -1), grid.width, grid.height };

        q.push(start);
        dist(start) = 0;

        while(!q.empty()) {
            pos_t current = q.top();
            q.pop();

            for (int i=0; i<4; ++i){
                pos_t pos = next_pos(current, i);

                if (grid(pos) == '.' && !blocked(units, pos) && (dist(pos) == -1 || dist(pos) > dist(current) + 1)) {
                    q.push(pos);
                    dist(pos) = dist(current) + 1;
                }
            }
        }

        return dist;
    };

    auto distances = bfs(unit.pos);
    
    std::vector<pos_t> enemy_neighbours;
    for (auto& u : units) {
        if (diff(u,unit) && u.hit_points > 0) {
            for (int i=0; i<4; ++i){
                pos_t pos = next_pos(u.pos, i);

                if(grid(pos) == '.' && !blocked(units, pos)) {
                    enemy_neighbours.push_back(pos); 
                }
            }
        }
    }

    int nearest_enemy_neighbour_dist = INT_MAX;
    for (auto& t : enemy_neighbours) {
        if (distances(t) != -1 && distances(t) < nearest_enemy_neighbour_dist) {
            nearest_enemy_neighbour_dist = distances(t);
        }
    }

    std::vector<pos_t> nearest_enemy_pos;
    for (pos_t t : enemy_neighbours) {
        if (distances(t) == nearest_enemy_neighbour_dist) {
            nearest_enemy_pos.push_back(t);
        }
    }

    if(nearest_enemy_pos.empty()) {
        return;
    }

    std::sort(nearest_enemy_pos.begin(), nearest_enemy_pos.end());
    pos_t enemy = nearest_enemy_pos[0];

    distances = bfs(enemy);

    std::vector<int> neighbour_dists;
    for (int i=0; i<4; ++i){
        pos_t pos = next_pos(unit.pos, i);
        neighbour_dists.push_back(distances(pos));
    }

    int min_dist = INT_MAX;
    for (int i : neighbour_dists){
        if (i != -1 && i < min_dist) {
            min_dist = i;
        }
    }

    std::vector<pos_t> potential_moves;
    for (int i=0; i<4; ++i){
        pos_t pos = next_pos(unit.pos, i);
        if (distances(pos) == min_dist) {
            potential_moves.push_back(pos);
        }
    }

    if(!potential_moves.empty()){
        std::sort(potential_moves.begin(), potential_moves.end());
        unit.pos = potential_moves.front();
    }

}

bool enemies_in_range(const std::vector<unit_t> &units, const unit_t& unit) {
    return std::any_of(units.begin(), units.end(), [&](auto& u) { 
        return diff(u,unit) && u.hit_points > 0 && manhatten_distance(u.pos,unit.pos) == 1;
    });
}

bool tick(grid_t& grid, std::vector<unit_t>& units) 
{
    std::sort(units.begin(), units.end(), [](unit_t a, unit_t b) { 
        return a.pos < b.pos; 
    });

    for (auto& unit : units) 
    {
        if (unit.hit_points <= 0) {
            continue;
        }

        if(!enemies_in_range(units, unit)){
            move(grid, units, unit);
        }

        if(!attack(units, unit)) {
            return false;
        }
    }

    return true;
}

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

std::vector<unit_t> find_units(grid_t& grid, int elf_attack_power){
    std::vector<unit_t> units;

    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            auto c = grid(x,y);
            if(c == 'G'){
                units.push_back( { GOBLIN, {x, y}, 3, 200 } );
                grid(x,y) = '.';
            }else if(c == 'E'){
                units.push_back( { ELF, {x, y}, elf_attack_power, 200 } );
                grid(x,y) = '.';
            }
        }
    }

    return units;
}

int count_units(const std::vector<unit_t>& units, int type){
    return (int)std::count_if(units.begin(), units.end(), [&](auto& a){
        return a.hit_points > 0 && a.type == type;
    });
}


void main() {

    auto input = parse_input("../src/day15/day15_input.txt");

    auto battle = [&](int elf_attack_power=3){
        auto grid = input;

        auto units = find_units(grid, elf_attack_power); // 23

        int round = 0;
        bool battling = true;
        while(battling){
            round += tick(grid, units);
            battling = count_units(units, ELF) && count_units(units, GOBLIN);
        }

        int score = std::accumulate(units.begin(), units.end(), 0, [&](auto& a, auto& b){
            return a + std::max(0, b.hit_points);
        });

        return std::make_tuple(count_units(units, ELF), count_units(units, GOBLIN), round*score);
    };

    auto [elfs, goblins, score] = battle();
    std::cout << "part1: " << score << std::endl;


    int num_orig_elfs = (int)std::count(input.data.begin(), input.data.end(), 'E');
    int power = 4;

    while(true){
        auto [elfs, goblins, score] = battle(power++);
        if(elfs == num_orig_elfs){
            std::cout << "part2: " << score << std::endl;
            break;
        }
    }
}

