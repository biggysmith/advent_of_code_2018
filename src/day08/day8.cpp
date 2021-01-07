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

std::vector<int> parse_input(const std::string& file){
    std::vector<int> input;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line, ' ')) {
        input.push_back(std::stoi(line));
    }

    return input;
}

int sum_meta_data(const std::vector<int>& tree, int& pos){
    int sum = 0;

    int num_childs = tree[pos++];
    int num_metas = tree[pos++];

    for(int i=0; i<num_childs; ++i){
        sum += sum_meta_data(tree,pos);
    }

    for(int i=0; i<num_metas; ++i){
        sum += tree[pos++];
    }

    return sum;
}

int sum_meta_data(const std::vector<int>& tree){
    int pos = 0;
    return sum_meta_data(tree, pos);
}



int sum_child_data(const std::vector<int>& tree, int& pos){
    int sum = 0;

    int num_childs = tree[pos++];
    int num_metas = tree[pos++];

    std::vector<int> child_sums(num_childs);
    for(auto& child_sum : child_sums){
        child_sum = sum_child_data(tree,pos);
    }

    if(num_childs == 0){
        for(int i=0; i<num_metas; ++i){
            sum += tree[pos++];
        }
    }else{
        for(int i=0; i<num_metas; ++i, ++pos){
            if(tree[pos] <= num_childs) {
                sum += child_sums[tree[pos]-1];
            }
        }
    }

    return sum;
}

int sum_child_data(const std::vector<int>& tree){
    int pos = 0;
    return sum_child_data(tree, pos);
}


void main()
{
    auto tree = parse_input("../src/day08/day8_input.txt");

    std::cout << "part1: " << sum_meta_data(tree) << std::endl;

    std::cout << "part2: " << sum_child_data(tree) << std::endl;
}