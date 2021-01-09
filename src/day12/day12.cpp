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

struct note_t{
    std::string rule;
    char result;
};

struct state_t{
    std::string initial;
    std::vector<note_t> notes;
};

state_t parse_input(const std::string& file){
    state_t state;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        if(line == ""){
            
        }else if(line.find("initial state") != std::string::npos){
            state.initial = line.substr(15);
        }else{
            std::regex re("[.|#]+");
            std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
            std::vector<std::string> tokens{first, last};
            //input.push_back({ std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]) });
            state.notes.push_back( { tokens[0], tokens[1][0] } );
        }

        
    }

    return state;
}

void main()
{
    auto notes = parse_input("../src/day12/day12_input.txt");

    std::string result(400, '.');

    size_t offset = (result.size() - notes.initial.size()) / 2;
    std::copy(notes.initial.begin(), notes.initial.end(), result.begin()+offset);

    std::string result_copy(result);

    auto update = [&]{
        for(int i=2; i<result.size()-2; ++i){
            std::string sub = result.substr(i-2, 5);
            bool found = false;
            for(auto& s : notes.notes){
                if(s.rule == sub){
                    result_copy[i] = s.result;
                    found = true;
                }
            }
            if(!found){
                result_copy[i] = '.';
            }
        }

        std::swap(result, result_copy);
    };

    auto plant_string = [&]{
        return result.substr(result.find_first_of("#"), result.find_last_of("#")-result.find_first_of("#")+1);
    };

     auto plant_sum = [&]{
        int sum = 0;
        for(int i=0; i<result.size(); ++i){
            if(result[i] == '#'){
                sum += i-(int)offset;
            }
        }
        return sum;
    };

    for(int i=0; i<20; ++i){
        update();
    }

    std::cout << "part1: " << plant_sum() << std::endl;

    std::fill(result.begin(), result.end(), '.');
    std::copy(notes.initial.begin(), notes.initial.end(), result.begin()+offset);

    for(uint64_t i=0; i<50000000000; ++i){
        std::string pattern0 = plant_string();
        int sum0 = plant_sum();

        update();
        std::string pattern1 = plant_string();
        int sum1 = plant_sum();

        if(pattern0 == pattern1){
            std::cout << "part2: " << (50000000000-i-1)*(sum1-sum0)+sum1 << std::endl;
            return;
        }
    }

}