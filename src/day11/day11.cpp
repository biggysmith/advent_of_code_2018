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

int power_level(int x,int y,int serial){
    int rack_id = x+10;
    int power_lvl = rack_id * y;
    power_lvl += serial;
    power_lvl *= rack_id;
    power_lvl = (power_lvl/100) % 10;
    power_lvl -= 5;
    return power_lvl;
}

void main()
{
    int serial = 5177;

    int width = 300;
    int height = 300;
    int depth = 300;
    std::vector<int> grid(width*height, 0);

    for(int y=0; y<height; ++y){
        for(int x=0; x<width; ++x){     
            grid[y*width+x] = power_level(x+1,y+1,serial);
        }
    }

    {
        int max_total = INT_MIN;
        int max_x = INT_MIN;
        int max_y = INT_MIN;
    
        for(int y=0; y<height-3; ++y){
            for(int x=0; x<width-3; ++x){
    
                int total_power = 0;
                for(int y2=y; y2<y+3; ++y2){
                    for(int x2=x; x2<x+3; ++x2){
                        total_power += grid[y2*width+x2];
                    }
                }
    
                if(total_power > max_total){
                    max_total = total_power;
                    max_x = x+1;
                    max_y = y+1;
                }
    
            }
        }
    
        std::cout << "part1: " << max_x << "," << max_y << std::endl; 
    }

    {
        struct max_t{
            int max_total = INT_MIN;
            int max_x = INT_MIN;
            int max_y = INT_MIN;
            int max_z = INT_MIN;
        };

        std::vector<max_t> maxes(omp_get_max_threads());
    
        #pragma omp parallel for
        for(int z=1; z<=depth; ++z){
            for(int y=0; y<height-z; ++y){
                for(int x=0; x<width-z; ++x){
    
                    int total_power = 0;
                    for(int y2=y; y2<y+z; ++y2){
                        for(int x2=x; x2<x+z; ++x2){
                            total_power += grid[y2*width+x2];
                        }
                    }
                   
                    if(total_power > maxes[omp_get_thread_num()].max_total){
                        maxes[omp_get_thread_num()].max_total = total_power;
                        maxes[omp_get_thread_num()].max_x = x+1;
                        maxes[omp_get_thread_num()].max_y = y+1;
                        maxes[omp_get_thread_num()].max_z = z;
                    }
    
                }
            }
        }

        max_t final_mx;
        for(auto& mx : maxes){
            if(mx.max_total > final_mx.max_total){
                final_mx.max_total = mx.max_total;
                final_mx.max_x = mx.max_x;
                final_mx.max_y = mx.max_y;
                final_mx.max_z = mx.max_z;
            }
        }
    
        std::cout << "part2: " << final_mx.max_x << "," << final_mx.max_y << "," << final_mx.max_z << std::endl; 
    }
        

}