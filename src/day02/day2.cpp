#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

std::vector<std::string> parse_input(const std::string& file){
    std::vector<std::string> input;

    std::ifstream file_stream(file);

    std::string line;
    while (std::getline(file_stream, line)) {
        input.push_back(line);
    }

    return input;
}

void main()
{
    auto input = parse_input("../src/day02/day2_input.txt");

    int count2 = 0;
    int count3 = 0;

    for(auto& s : input){

        std::map<char,int> char_map;
        for(auto c : s){
            char_map[c]++;
        }

        for(auto& [c,count] : char_map){
            if(count == 2){
                count2++;
                break;
            }
        }

        for(auto& [c,count] : char_map){
            if(count == 3){
                count3++;
                break;
            }
        }

    }

    std::cout << "part1: " << count2 << " * " << count3 << " = " << count2*count3 << std::endl;

    int min_mismatched = INT_MAX;
    std::string min_s0;
    std::string min_s1;

    for(auto& s0 : input){
        for(auto& s1 : input){

            if(s0 == s1){
                continue;
            }

            int mismatched = 0;
            for(int i=0; i<s0.size(); ++i){
                mismatched += s0[i] != s1[i];
            }

            if(mismatched < min_mismatched){
                min_mismatched = mismatched;
                min_s0 = s0;
                min_s1 = s1;
            }

        }
    }

    auto it = std::mismatch(min_s0.begin(), min_s0.end(), min_s1.begin());
    min_s0.erase(std::distance(min_s0.begin(), it.first), 1);

    std::cout << "part2: " << min_s0 << std::endl;

}