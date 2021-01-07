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
#include <array>
#include <unordered_map>

struct step_t{
    int x;
    int y;
};

std::vector<step_t> parse_input(const std::string& file){
    std::vector<step_t> input;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        input.push_back({ line[5]-65, line[36]-65 });
    }

    return input;
}

using graph_t = std::vector<std::vector<int>>;

std::vector<int> lexo_topological_sort(const graph_t& graph)
{
    std::vector<int> in_degree(graph.size(), 0);

    for (auto& nodes : graph) { 
        for(auto& node : nodes){
            in_degree[node]++; 
        }
    } 

    std::set<int> zero_in_degree_nodes; 
    for (int i = 0; i < graph.size(); i++) {
        if (in_degree[i] == 0) {
            zero_in_degree_nodes.insert(i); 
        }
    }
  
    std::vector<int> order; 

    while (!zero_in_degree_nodes.empty()) 
    { 
        int n = *zero_in_degree_nodes.begin();
        zero_in_degree_nodes.erase(n);
        order.push_back(n); 
  
        for(auto& node : graph[n]){
            in_degree[node]--;
            if (in_degree[node] == 0) {
                zero_in_degree_nodes.insert(node); 
            }
        }
    } 

    return order;
}

template<typename Range,typename Type>
bool contains(const Range& r,const Type& t){
    return std::find(std::begin(r), std::end(r), t) != std::end(r);
}


void main()
{
    auto steps = parse_input("../src/day07/day7_input.txt");

    std::set<int> nodes;
    for(auto s : steps){
        nodes.insert(s.x);
        nodes.insert(s.y);
    }

    graph_t graph(nodes.size());

    for(auto s : steps){
        graph[s.x].push_back(s.y);
    }

    auto sorted = lexo_topological_sort(graph);

    std::cout << "part1: ";
    for(auto s : sorted){
        std::cout << (char)(s+65) << "";
    }
    std::cout << std::endl;


    std::vector<std::vector<int>> nodes_map(nodes.size());
    for (auto s : steps) { 
        nodes_map[s.y].push_back(s.x);
    }

    int number_workers = 5;
    std::set<int> finished_workers;
    std::vector<int> workers(number_workers, 0);
    std::vector<int> available_workers;

    std::vector<int> node_time(nodes.size());
    for (auto n : nodes) {
        node_time[n] = 60+n;
    }

    int count = -1;

    do {
        for (auto& worker : workers){
            if (node_time[worker]-- == 0) {
                finished_workers.insert(worker);
                worker = 0;
            }
        }

        for (int n=0; n<nodes_map.size(); ++n) 
        {
            if (!contains(finished_workers, n) && !contains(workers, n)) 
            {
                bool all_finished = true;
                for(auto n2 : nodes_map[n]){
                    all_finished &= contains(finished_workers, n2);
                }
                if (all_finished) {
                    available_workers.push_back(n);
                }
            }
        }
        std::sort(available_workers.begin(), available_workers.end());

        for (auto& next : available_workers) {
            auto finished_worker = std::find(workers.begin(), workers.end(), 0);
            if (finished_worker != workers.end()) {
                *finished_worker = next;
                next = 0;
            }
        }
        available_workers.erase(std::remove(available_workers.begin(), available_workers.end(), 0), available_workers.end());

        count++;
    } 
    while (!available_workers.empty() || std::accumulate(workers.begin(), workers.end(), 0));

    std::cout << "part2: " << count << "\n";

}