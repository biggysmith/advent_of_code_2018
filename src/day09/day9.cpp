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

std::pair<int,int> parse_input(const std::string& file){
    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};
        return std::make_pair(std::stoi(tokens[0]), std::stoi(tokens[1]));
    }

    return std::make_pair(0,0);
}

void print(const std::vector<int>& v,int current){
    for(int i=0; i<v.size(); ++i){
        if(i==current){
            std::cout << "(";
        }
        std::cout << v[i];   
        if(i==current){
            std::cout << ")";
        }
        std::cout << " ";   
    }
    std::cout << std::endl;
}

struct node_t{
    size_t val;
    node_t* prev;
    node_t* next;
};

void main()
{
    auto [num_players, num_points] = parse_input("../src/day09/day9_input.txt");

    auto play = [](int players, int points)
    {
        int player = 1;
        int marble = 1;
        std::vector<size_t> player_scores(players, 0);

        std::vector<node_t*> game_nodes(1);
        game_nodes[0] = new node_t;
        game_nodes[0]->val = 0;
        game_nodes[0]->prev = game_nodes[0];
        game_nodes[0]->next = game_nodes[0];

        node_t* current = game_nodes[0];

        for (int i = 0; i <= points; ++i) {
            if (marble % 23 == 0) {
                player_scores[player] += marble;
                for(int j=0; j<7; ++j){
                    current = current->prev;
                }

                player_scores[player] += current->next->val;
                current->next = current->next->next;
                current->next->prev = current;
            }
            else {
                for(int j=0; j<2; ++j){
                    current = current->next;
                }
                auto new_node = new node_t;
                new_node->val = marble;
                new_node->prev = current;
                new_node->next = current->next;

                current->next = new_node;
                current->next->next->prev = new_node;
            }

            marble++;
            player = (player + 1) % players;
        }

        for (auto n : game_nodes){
            delete n;
        } 

        return *std::max_element(player_scores.begin(), player_scores.end());
    };

    std::cout << "part1: " << play(num_players, num_points) << std::endl;
    std::cout << "part2: " << play(num_players, num_points*100) << std::endl;

}