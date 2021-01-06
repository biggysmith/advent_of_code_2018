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

std::vector<char> parse_input(const std::string& file){
    std::vector<char> input;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        input.insert(input.end(), line.begin(), line.end());
    }

    return input;
}

size_t react(std::vector<char> polymer){
    size_t new_size = 0;
    while (polymer.size() != new_size) {
        new_size = polymer.size();
        auto it0 = polymer.begin();
        auto it1 = std::next(it0);
        auto end = polymer.end();
        for (; it1 < end; ++it0, ++it1) {
            if (tolower(*it0) == tolower(*it1) && islower(*it0) != islower(*it1)) {
                polymer.erase(it0, std::next(it1));
                end = polymer.end();
            }
        }
    }
    return polymer.size();
}

void main()
{
    auto input = parse_input("../src/day05/day5_input.txt");

    std::cout << "part1: " << react(input) << std::endl;


    std::set<char> unit_types;
    std::copy_if(input.begin(), input.end(), std::inserter(unit_types, unit_types.begin()), islower);


    std::vector<size_t> min_polymers(omp_get_max_threads(), INT_MAX);

    #pragma omp parallel for
    for(int i=0; i<unit_types.size(); ++i){
        std::vector<char> polymer;
        char unit = *std::next(unit_types.begin(), i);
        std::copy_if(input.begin(), input.end(), std::back_inserter(polymer), [&](auto c){
            return tolower(c) != unit;
        });

        size_t new_size = react(polymer);
        if(new_size < min_polymers[omp_get_thread_num()]){
            min_polymers[omp_get_thread_num()] = new_size;
        }
    }

    std::cout << "part2: " << *std::min_element(min_polymers.begin(), min_polymers.end()) << std::endl;


}