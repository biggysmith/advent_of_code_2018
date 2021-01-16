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
    size_t op;
    size_t i[3];
};

struct registers_t{
    size_t r[6];
};

bool operator==(const registers_t& a, const registers_t& b){
    return std::equal(std::begin(a.r), std::end(a.r), std::begin(b.r));
}

struct input_t{
    size_t ip;
    std::vector<instruction_t> test_program;
};

const std::vector<std::string> ops = {
    "addr",
    "addi",
    "mulr",
    "muli",
    "banr",
    "bani",
    "borr",
    "bori",
    "setr",
    "seti",
    "gtir",
    "gtri",
    "gtrr",
    "eqir",
    "eqri",
    "eqrr"
};

input_t parse_input(const std::string& file){
    input_t ret;

    int new_lines = 0;

    std::ifstream file_stream(file);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::regex re("[a-z]+|[0-9]+");
        std::sregex_token_iterator first{line.begin(), line.end(), re}, last;
        std::vector<std::string> tokens{first, last};

        if(tokens.size() > 2){
            auto it = std::find(ops.begin(), ops.end(), tokens[0]);
            ret.test_program.push_back({ (size_t)std::distance(ops.begin(), it), std::stoull(tokens[1]), std::stoull(tokens[2]), std::stoull(tokens[3]) });
        }else{
            ret.ip = std::stoull(tokens[1]);
        }
    }

    return ret;
}

void transpile(const size_t bound, const std::vector<instruction_t>& instructions, registers_t& reg)
{
    int i = 0;

    auto regstr = [&](size_t r){
        return std::string("reg.r[") + std::to_string(r) + "]";
    };

    while(true)
    {
        auto& in = instructions[i];

        if(in.op == 0){
            std::cout << "case " << i << ": " << "reg.r[" << in.i[2] << "]" << " = " << regstr(in.i[0]) << " + " << regstr(in.i[1]) << ";   break;" << std::endl;
        }
        else if(in.op == 1){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " + " << in.i[1] << ";   break;" << std::endl;
        }
        else if(in.op == 2){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " * " << regstr(in.i[1]) << ";   break;" << std::endl;
        }
        else if(in.op == 3){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " * " << in.i[1] << ";   break;" << std::endl;
        }
        else if(in.op == 4){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " & " << regstr(in.i[1]) << ";   break;" << std::endl;
        }
        else if(in.op == 5){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " & " << in.i[1] << ";   break;" << std::endl;
        }
        else if(in.op == 6){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " | " << regstr(in.i[1]) << ";   break;" << std::endl;
        }
        else if(in.op == 7){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " | " << in.i[1] << ";   break;" << std::endl;
        }
        else if(in.op == 8){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << ";   break;" << std::endl;
        }
        else if(in.op == 9){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << in.i[0] << ";   break;" << std::endl;
        }
        else if(in.op == 10){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << in.i[0] << " > " << regstr(in.i[1]) << " ? 1 : 0;   break;" << std::endl;
        }
        else if(in.op == 11){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " > " << in.i[1] << " ? 1 : 0;   break;" << std::endl;
        }
        else if(in.op == 12){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " > " << regstr(in.i[1]) << " ? 1 : 0;   break;" << std::endl;
        }
        else if(in.op == 13){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << in.i[0] << " == " << regstr(in.i[1]) << " ? 1 : 0;   break;" << std::endl;
        }
        else if(in.op == 14){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " == " << in.i[1] << " ? 1 : 0;   break;" << std::endl;
        }
        else if(in.op == 15){
            std::cout << "case " << i << ": " << regstr(in.i[2]) << " = " << regstr(in.i[0]) << " == " << regstr(in.i[1]) << " ? 1 : 0;   break;" << std::endl;
        }

        i++;
        if(i >= instructions.size()){
            break;
        }
    }
}



void main() {

    auto input = parse_input("../src/day21/day21_input.txt");

    {
        registers_t reg{ 0, 0, 0, 0, 0, 0 };
        transpile(input.ip, input.test_program, reg);
    }

    {
        registers_t reg { 0, 0, 0, 0, 0, 0 };
        size_t bound = input.ip;
        size_t current_reg_val;
        std::vector<size_t> reg_vals;

        bool halt = false;
        while(!halt){
            switch(reg.r[bound]){
                case 0: reg.r[1] = 123; break;
                case 1: reg.r[1] = reg.r[1] & 456; break;
                case 2: reg.r[1] = reg.r[1] == 72 ? 1 : 0; break;
                case 3: reg.r[5] = reg.r[1] + reg.r[5]; break;
                case 4: reg.r[5] = 0; break;
                case 5: reg.r[1] = 0; break;
                case 6: reg.r[4] = reg.r[1] | 65536; break;
                case 7: reg.r[1] = 12772194; break;
                case 8: reg.r[3] = reg.r[4] & 255; break;
                case 9: reg.r[1] = reg.r[1] + reg.r[3]; break;
                case 10: reg.r[1] = reg.r[1] & 16777215; break;
                case 11: reg.r[1] = reg.r[1] * 65899; break;
                case 12: reg.r[1] = reg.r[1] & 16777215; break;
                case 13: reg.r[3] = 256 > reg.r[4] ? 1 : 0; break;
                case 14: reg.r[5] = reg.r[3] + reg.r[5]; break;
                case 15: reg.r[5] = reg.r[5] + 1; break;
                case 16: reg.r[5] = 27; break;
                case 17: reg.r[3] = 0; break;
                case 18: reg.r[2] = reg.r[3] + 1; break;
                case 19: reg.r[2] = reg.r[2] * 256; break;
                case 20: reg.r[2] = reg.r[2] > reg.r[4] ? 1 : 0; break;
                case 21: reg.r[5] = reg.r[2] + reg.r[5]; break;
                case 22: reg.r[5] = reg.r[5] + 1; break;
                case 23: reg.r[5] = 25; break;
                case 24: reg.r[3] = reg.r[3] + 1; break;
                case 25: reg.r[5] = 17; break;
                case 26: reg.r[4] = reg.r[3]; break;
                case 27: reg.r[5] = 7; break;
                case 28: { 
                    // this is the only place reg 0 is used, and the program halts after after comparing reg1 to reg0
                    // we need to concentrate on reg1
                    reg.r[3] = reg.r[1] == reg.r[0] ? 1 : 0;  
                    if(std::find(reg_vals.begin(), reg_vals.end(), reg.r[1]) == reg_vals.end()){
                        reg_vals.push_back(reg.r[1]);
                        current_reg_val = reg.r[1];
                    }else{
                        // got a repeat, this must be the highest halting val
                        halt = true;
                    }
                } break;
                case 29: reg.r[5] = reg.r[3] + reg.r[5]; break;
                case 30: reg.r[5] = 5; break;
                default: break;
            };
            if(reg.r[bound] > 30){
                halt = true;
            }
            reg.r[bound]++;
        }

        std::cout << "part1: " << reg_vals.front() << std::endl;
        std::cout << "part2: " << current_reg_val << std::endl;
    }

}
