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

struct group_t{
    int size;
    int hit_points;
    int attack;
    int attack_initiative;
    int effective_power;
    int side;
    int id;
    std::string damage_type;
    std::vector<std::string> weak_to;
    std::vector<std::string> immune_to;
    group_t* target;
};

struct armys_t{
    std::vector<group_t> immune_system;
    std::vector<group_t> infection;
};


armys_t parse_input(const std::string& file){
    armys_t ret;
    std::vector<group_t>* group_list = nullptr;
    std::vector<std::string>* to = nullptr;

    int side = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {

        if(line == "Immune System:"){
            group_list = &ret.immune_system;
            side = 0;
            continue;
        }else if(line == "Infection:"){
            group_list = &ret.infection;
            side = 1;
            continue;
        }else if(line == ""){
            continue;
        }else{

            group_t group;

            std::string p0(line.begin(), line.begin()+line.find("points"));
            {
                std::regex re("[0-9]+");
                std::sregex_token_iterator first{ p0.begin(), p0.end(), re }, last;
                std::vector<std::string> tokens{ first, last };

                group.size = std::stoi(tokens[0]);
                group.hit_points = std::stoi(tokens[1]);
            }

            if(line.find("(") != std::string::npos)
            {
                std::string p1(line.begin()+line.find("(")+1, line.begin()+line.find(")"));
                {
                    std::regex re("[A-Za-z]+"); 
                    std::sregex_token_iterator first{ p1.begin(), p1.end(), re }, last;
                    std::vector<std::string> tokens{ first, last };

                    for(int i=0; i<tokens.size(); ++i){
                        if(tokens[i] == "weak"){
                            to = &group.weak_to;
                            i++;
                            continue;
                        }else if(tokens[i] == "immune"){
                            to = &group.immune_to;
                            i++;
                            continue;
                        }else{
                            to->push_back(tokens[i]);
                        }
                    }
                }
            }

            std::string p2(line.begin()+line.find("does")+1, line.end());
            {
                std::regex re("[A-Za-z]+|[0-9]+");
                std::sregex_token_iterator first{ p2.begin(), p2.end(), re }, last;
                std::vector<std::string> tokens{ first, last };

                group.attack = std::stoi(tokens[1]);
                group.damage_type = tokens[2];
                group.attack_initiative = std::stoi(tokens[6]);
            }

            group.side = side;
            group.id = (int)group_list->size()+1;
            group_list->push_back(group);
        }

    }

    return ret;
}

template<typename Container,typename Value>
auto contains(const Container& c,const Value& v){
    return std::find(c.begin(), c.end(), v) != c.end();
}

int effective_power(const group_t& group){
    return group.size * group.attack;
}

int effective_attack(const group_t& attacker,const group_t& defender){
    bool is_defending_immune  = contains(defender.immune_to, attacker.damage_type);
    bool is_defending_weak_to = contains(defender.weak_to, attacker.damage_type);

    int attack_potential;
    if(is_defending_immune){
        attack_potential = 0;
    }else if(is_defending_weak_to){
        attack_potential = effective_power(attacker)*2;
    }else{
        attack_potential = effective_power(attacker);
    }

    return attack_potential;
}



void main() 
{
    auto orig_input = parse_input("../src/day24/day24_input.txt");

    auto run = [&](int boost)
    {
        auto input = orig_input;

        for(auto& immune : input.immune_system){
            immune.attack += boost;
        }

        std::vector<group_t*> groups;

        auto target_selection = [&]()
        {
            groups.clear();

            std::vector<group_t*> immune_groups;
            std::vector<group_t*> infect_groups;

            for (auto& group : input.immune_system) {
                group.effective_power = effective_power(group);
                groups.push_back(&group);
                immune_groups.push_back(&group);
            }

            for (auto& group : input.infection) {
                group.effective_power = effective_power(group);
                groups.push_back(&group);
                infect_groups.push_back(&group);
            }

            std::sort(immune_groups.begin(), immune_groups.end(), [](auto& a, auto& b) {
                return std::tie(a->effective_power, a->attack_initiative) > std::tie(b->effective_power, b->attack_initiative);
            });

            std::sort(infect_groups.begin(), infect_groups.end(), [](auto& a, auto& b) {
                return std::tie(a->effective_power, a->attack_initiative) > std::tie(b->effective_power, b->attack_initiative);
            });

            auto choose = [](const std::vector<group_t*>& attackers, const std::vector<group_t*>& defenders)
            {
                std::vector<group_t*> targets;
                std::vector<group_t*> choosen_targets;
                for (auto* attacker : attackers) {
                    if (attacker->size == 0) {
                        continue;
                    }

                    std::vector<std::pair<int, group_t*>> targets;
                    for (auto* defender : defenders) {
                        if (defender->size == 0) {
                            continue;
                        }

                        if (contains(choosen_targets, defender)) {
                            continue;
                        }

                        int attack_potential = effective_attack(*attacker, *defender);

                        if (attack_potential) {
                            targets.push_back({ attack_potential, defender });
                        }
                    }

                    if (!targets.empty()) {
                        std::sort(targets.begin(), targets.end(), [](auto& a, auto& b) {
                            return std::tie(a.first, a.second->effective_power, a.second->attack_initiative) > std::tie(b.first, b.second->effective_power, b.second->attack_initiative);
                        });

                        attacker->target = targets.front().second;
                        choosen_targets.push_back(attacker->target);
                    }
                    else {
                        attacker->target = nullptr;
                    }
                }
            };

            choose(immune_groups, infect_groups);
            choose(infect_groups, immune_groups);
        };

        auto attacking = [&]()
        {
            std::sort(groups.begin(), groups.end(), [](auto& a, auto& b) {
                return a->attack_initiative > b->attack_initiative;
            });

            bool stale_mate = true;
            for (auto* group : groups) {
                if (group->size != 0 && group->target) {
                    int units_killed = effective_attack(*group, *group->target) / group->target->hit_points;
                    group->target->size = std::max(0, group->target->size - units_killed);
                    stale_mate = false;
                }
            }

            return stale_mate;
        };

        auto immune_units = [&] {
            return std::accumulate(input.immune_system.begin(), input.immune_system.end(), 0, [&](auto& a, auto& b) { return a + b.size; });
        };

        auto infection_units = [&] {
            return std::accumulate(input.infection.begin(), input.infection.end(), 0, [&](auto& a, auto& b) { return a + b.size; });
        };

        int stale_mate = false;

        while (immune_units() && infection_units() && !stale_mate) {
            target_selection();
            stale_mate = attacking();
        }

        return std::make_pair(immune_units(), infection_units());
    };

    auto [immune, infected] = run(0);
    std::cout << "part1: " << (immune ? immune : infected) << std::endl;

    int boost = 1;
    while(infected > 0){
        std::tie(immune, infected) = run(boost++);
    }

    std::tie(immune, infected) = run(boost-1);
    std::cout << "part2: " << (immune ? immune : infected) << std::endl;
}

