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

struct pos_t{
    int64_t x,y,z,r;
};

bool operator==(const pos_t& a, const pos_t& b){
    return std::tie(a.x,a.y,a.z,a.r) == std::tie(b.x,b.y,b.z,b.r);
}

pos_t operator+(const pos_t& a, const pos_t& b){
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

pos_t operator-(const pos_t& a, const pos_t& b){
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

pos_t operator*(const pos_t& a, const pos_t& b){
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

struct box_t{
    pos_t mn;
    pos_t mx;
};

struct queue_t{
    int64_t in_range;
    int64_t size;
    int64_t dist_to_zero;
    box_t box;
};

bool operator<(const queue_t& a, const queue_t& b){
    return std::tie(a.in_range,a.size,a.dist_to_zero) < std::tie(b.in_range,b.size,b.dist_to_zero);
}



std::vector<pos_t> parse_input(const std::string& file){
    std::vector<pos_t> ret;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("-?[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        ret.push_back({ std::stoll(tokens[0]), std::stoll(tokens[1]), std::stoll(tokens[2]), std::stoll(tokens[3]) });
    }

    return ret;
}

int64_t manhatten_dist(const pos_t& a, const pos_t& b){
    return abs(b.x-a.x) + abs(b.y-a.y) + abs(b.z-a.z);
}

int64_t next_power_of_two(int64_t v){
    int64_t ret = 1;
    while(ret <= v){
        ret *= 2;
    }
    return ret;
}

bool octant_intersect(const box_t& box, const pos_t& pos){
    int64_t r = (manhatten_dist(pos, box.mn) + manhatten_dist(pos, box.mx) - manhatten_dist(box.mx, box.mn)) / 2;
    return r <= pos.r;
}

void main() 
{
    auto input = parse_input("../src/day23/day23_input.txt");

    auto max_it = std::max_element(input.begin(), input.end(), [&](auto& a, auto& b){
        return a.r < b.r;
    });

    size_t cnt = std::count_if(input.begin(), input.end(), [&](auto& a){
        return manhatten_dist(a, *max_it) <= max_it->r;
    });

    std::cout << "part1: " << cnt << std::endl;



    int64_t min_x = INT_MAX;
    int64_t min_y = INT_MAX;
    int64_t min_z = INT_MAX;

    int64_t max_x = INT_MIN;
    int64_t max_y = INT_MIN;
    int64_t max_z = INT_MIN;

    for(auto& pos : input){
        min_x = std::min(min_x, pos.x-pos.r);
        min_y = std::min(min_y, pos.y-pos.r);
        min_z = std::min(min_z, pos.z-pos.r);

        max_x = std::max(max_x, pos.x+pos.r);
        max_y = std::max(max_y, pos.y+pos.r);
        max_z = std::max(max_z, pos.z+pos.r);
    }

    max_x = std::max(std::abs(min_x), max_x);
    max_y = std::max(std::abs(min_y), max_y);
    max_z = std::max(std::abs(min_z), max_z);

    int64_t max_extent = std::max(max_x,std::max(max_y,max_z));
    int64_t start_size = next_power_of_two(max_extent);

    pos_t zero {0,0,0};
    pos_t extent { max_extent, max_extent, max_extent };
    box_t box { zero-extent, zero+extent };


    std::priority_queue<queue_t> q;
    q.push({ (int)input.size(), max_extent*2, max_extent, box });

    while(!q.empty()){
        auto curr = q.top();
        q.pop();

        if(curr.size == 1){
            std::cout << "part2: " << curr.dist_to_zero << std::endl;
            break;
        }

        pos_t half = { curr.size>>1, curr.size>>1, curr.size>>1 };
        for(int i=0; i<8; ++i){
            std::bitset<3> bits(i);
            pos_t offset = { bits[0], bits[1], bits[2] };
            pos_t mn = curr.box.mn + half*offset;
            pos_t mx = mn + half;
            box_t box = { mn, mx };
            int64_t in_range = std::accumulate(input.begin(),input.end(),0,[&](auto& a,auto& b){
                return a + octant_intersect(box, b);
            });
            q.push({ in_range, half.x, manhatten_dist(mn, zero), box });
        }


    }
}

