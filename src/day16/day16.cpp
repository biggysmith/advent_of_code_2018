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

struct instruction_t{
    int i[4];
};

struct registers_t{
    int r[4];
};

bool operator==(const registers_t& a, const registers_t& b){
    return a.r[0] == b.r[0] && a.r[1] == b.r[1] && a.r[2] == b.r[2] && a.r[3] == b.r[3];
}

struct sample_t{
    registers_t before;
    instruction_t instruction;
    registers_t after;
};

struct input_t{
    std::vector<sample_t> samples;
    std::vector<instruction_t> test_program;
};

input_t parse_input(const std::string& file){
    input_t ret;

    int new_lines = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        if(new_lines >= 2){
            break;
        }

        if(line == ""){
            new_lines++;
            continue;
        }

        std::regex re("[0-9]+|[A-Za-z]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        if(tokens.size() == 5){
            if(tokens[0] == "Before"){
                ret.samples.push_back(sample_t{});
                ret.samples.back().before = { std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4]) };
            }else if(tokens[0] == "After"){
                ret.samples.back().after = { std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4]) };
            }
        }

        if(tokens.size() == 4){
            ret.samples.back().instruction = { std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]) };
            new_lines = 0;
        }
    }

    while (std::getline(file_stream, line)) {
        std::regex re("[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        ret.test_program.push_back({ std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]) });
    }

    return ret;
}

void run_op(const std::string& opcode, const instruction_t& in, registers_t& reg){
    if(opcode == "addr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] + reg.r[in.i[2]];
    }
    else if(opcode == "addi"){
        reg.r[in.i[3]] = reg.r[in.i[1]] + in.i[2];
    }
    else if(opcode == "mulr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] * reg.r[in.i[2]];
    }
    else if(opcode == "muli"){
        reg.r[in.i[3]] = reg.r[in.i[1]] * in.i[2];
    }
    else if(opcode == "banr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] & reg.r[in.i[2]];
    }
    else if(opcode == "bani"){
        reg.r[in.i[3]] = reg.r[in.i[1]] & in.i[2];
    }
    else if(opcode == "bonr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] | reg.r[in.i[2]];
    }
    else if(opcode == "boni"){
        reg.r[in.i[3]] = reg.r[in.i[1]] | in.i[2];
    }
    else if(opcode == "setr"){
        reg.r[in.i[3]] = reg.r[in.i[1]];
    }
    else if(opcode == "seti"){
        reg.r[in.i[3]] = in.i[1];
    }
    else if(opcode == "gtir"){
        reg.r[in.i[3]] = in.i[1] > reg.r[in.i[2]];
    }
    else if(opcode == "gtri"){
        reg.r[in.i[3]] = reg.r[in.i[1]] > in.i[2];
    }
    else if(opcode == "gtrr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] > reg.r[in.i[2]];
    }
    else if(opcode == "eqir"){
        reg.r[in.i[3]] = in.i[1] == reg.r[in.i[2]];
    }
    else if(opcode == "eqri"){
        reg.r[in.i[3]] = reg.r[in.i[1]] == in.i[2];
    }
    else if(opcode == "eqrr"){
        reg.r[in.i[3]] = reg.r[in.i[1]] == reg.r[in.i[2]];
    }
}

 const std::vector<std::string> ops = {
    "addr",
    "addi",
    "mulr",
    "muli",
    "banr",
    "bani",
    "bonr",
    "boni",
    "setr",
    "seti",
    "gtir",
    "gtri",
    "gtrr",
    "eqir",
    "eqri",
    "eqrr"
};

int valid_samples(const std::vector<sample_t>& samples){

    int sum = 0;
    for(auto& sample : samples){
        int valid = 0;
        for(auto& op : ops){
            auto regs = sample.before;
            run_op(op, sample.instruction, regs);
            valid += regs == sample.after;
        }
        sum += valid >= 3;
    }

    return sum;
}

std::map<int,std::set<int>> map_samples(const std::vector<sample_t>& samples){
    std::map<int,std::set<int>> op_map;

    for(auto& sample : samples){
        for(int i=0; i<ops.size(); ++i){
            auto regs = sample.before;
            run_op(ops[i], sample.instruction, regs);
            if(regs == sample.after){
                op_map[i].insert(sample.instruction.i[0]);
            }
        }
    }

    return op_map;
}

void main() {

    auto input = parse_input("../src/day16/day16_input.txt");

    std::cout << "part1: " << valid_samples(input.samples) << std::endl;


    std::vector<char> found_rule(ops.size(),0);
    auto op_map = map_samples(input.samples);

    auto solved = [&]{
        return std::all_of(op_map.begin(),op_map.end(),[](auto& s){ 
            return s.second.size()<=1; 
        });
    };

    auto remove_rule = [&](int rn){
        for(int r=0; r<found_rule.size(); ++r){
            if(!found_rule[r]){
                op_map[r].erase(rn);
            }
        }
    };

    auto refresh = [&](){
        for(int c=0; c<ops.size(); ++c){
            if(!found_rule[c] && op_map[c].size()==1){
                found_rule[c] = true;
                remove_rule(*op_map[c].begin());
                //return;
            }
        }
    };

    while(!solved()){
        refresh();
    }

    std::vector<std::string> code_to_op(ops.size());
    for(int r=0; r<ops.size(); ++r){
        code_to_op[*op_map[r].begin()] = ops[r];
    }

    registers_t regs = { 0,0,0,0 };
    for(auto& in : input.test_program){
        run_op(code_to_op[in.i[0]], in, regs);
    }

    std::cout << "part2: " << regs.r[0] << std::endl;

}

