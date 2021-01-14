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

bool run_op(const size_t bound, const std::vector<instruction_t>& instructions, registers_t& reg)
{
    auto& in = instructions[reg.r[bound]];

    if(in.op == 0){
        reg.r[in.i[2]] = reg.r[in.i[0]] + reg.r[in.i[1]];
    }
    else if(in.op == 1){
        reg.r[in.i[2]] = reg.r[in.i[0]] + in.i[1];
    }
    else if(in.op == 2){
        reg.r[in.i[2]] = reg.r[in.i[0]] * reg.r[in.i[1]];
    }
    else if(in.op == 3){
        reg.r[in.i[2]] = reg.r[in.i[0]] * in.i[1];
    }
    else if(in.op == 4){
        reg.r[in.i[2]] = reg.r[in.i[0]] & reg.r[in.i[1]];
    }
    else if(in.op == 5){
        reg.r[in.i[2]] = reg.r[in.i[0]] & in.i[1];
    }
    else if(in.op == 6){
        reg.r[in.i[2]] = reg.r[in.i[0]] | reg.r[in.i[1]];
    }
    else if(in.op == 7){
        reg.r[in.i[2]] = reg.r[in.i[0]] | in.i[1];
    }
    else if(in.op == 8){
        reg.r[in.i[2]] = reg.r[in.i[0]];
    }
    else if(in.op == 9){
        reg.r[in.i[2]] = in.i[0];
    }
    else if(in.op == 10){
        reg.r[in.i[2]] = in.i[0] > reg.r[in.i[1]];
    }
    else if(in.op == 11){
        reg.r[in.i[2]] = reg.r[in.i[0]] > in.i[1];
    }
    else if(in.op == 12){
        reg.r[in.i[2]] = reg.r[in.i[0]] > reg.r[in.i[1]];
    }
    else if(in.op == 13){
        reg.r[in.i[2]] = in.i[0] == reg.r[in.i[1]];
    }
    else if(in.op == 14){
        reg.r[in.i[2]] = reg.r[in.i[0]] == in.i[1];
    }
    else if(in.op == 15){
        reg.r[in.i[2]] = reg.r[in.i[0]] == reg.r[in.i[1]];
    }

    reg.r[bound]++;
    return reg.r[bound] < instructions.size();
}



void main() {

    auto input = parse_input("../src/day19/day19_input.txt");

    {
        registers_t regs = { 0, 0, 0, 0, 0, 0 };

        size_t bound = input.ip;
        while (run_op(bound, input.test_program, regs)) {

        }
        std::cout << "part1: " << regs.r[0] << std::endl;
    }

    {
        size_t sum = 0;
        size_t end = 10551326; // this is number that remains in reg4 after many runs, this is the number to work on
        for (size_t i=1; i<=end; ++i) {
            if ((end % i) == 0) {
                sum += i;
            }
        }
        std::cout << "part2: " << sum << std::endl;
    }

}
