#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<int> parse_input(const std::string& file){
    std::vector<int> input;

    std::ifstream file_stream(file);

    std::string line;
    while (std::getline(file_stream, line)) {
        input.push_back(std::stoi(line));
    }

    return input;
}

void main()
{
    auto input = parse_input("../src/day01/day1_input.txt");

    std::cout << "part1: " << std::accumulate(input.begin(), input.end(), 0) << std::endl;


    std::map<int,int> freq_map;
    int result_freq = 0;
    bool repeated = false;

    while(!repeated)
    {
        for(auto freq : input)
        {
            result_freq += freq;
            freq_map[result_freq]++;
            if(freq_map[result_freq] > 1){
                repeated = true;
                std::cout << "part2: " << result_freq << std::endl;
                break;
            }
        }
    }
}