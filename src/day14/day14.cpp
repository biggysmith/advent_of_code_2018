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

std::string tail(const std::string& str, int n) {
    if (n >= str.size()) { 
        return str; 
    }
    return str.substr(str.size()-n);
}

void main()
{
    std::string input = "047801";
    std::string recipes = "37";

    int elf0_current = 0;
    int elf1_current = 1;

    do{
        int new_recipes = recipes[elf0_current]-'0' + recipes[elf1_current]-'0';
        recipes = recipes.append(std::to_string(new_recipes));
        elf0_current = (elf0_current + (recipes[elf0_current]-'0') + 1) % recipes.size();
        elf1_current = (elf1_current + (recipes[elf1_current]-'0') + 1) % recipes.size();
    }
    while(tail(recipes, input.size()+1).find(input) == std::string::npos);

    std::cout << "part1: " << recipes.substr(std::stoi(input), 10) << std::endl;

    std::cout << "part2: " << recipes.find(input) << std::endl;
}