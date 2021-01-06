#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>

auto time_to_point(const std::string& time_string){
    std::tm tm = {};
    std::stringstream ss(time_string);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    tm.tm_year = 100; // stupid epoch 
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

int get_minute(const std::chrono::system_clock::time_point& t){
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    tm local_tm = *localtime(&tt);
    return local_tm.tm_min;
}


struct record_t{
    std::string text;
    std::chrono::system_clock::time_point time;
};

struct event_t{
    int event;
    std::chrono::system_clock::time_point time;
};

std::vector<record_t> parse_input(const std::string& file){
    std::vector<record_t> input;

    std::ifstream file_stream(file);

    std::string line;
    while (std::getline(file_stream, line)) {
        input.push_back({ line.substr(19), time_to_point(line.substr(1, 16)) });

    }

    return input;
}


void main()
{
    auto input = parse_input("../src/day04/day4_input.txt");

    std::sort(input.begin(), input.end(), [](auto& a, auto& b){
        return a.time < b.time;
    });

    int current_guard = -1;
    std::map<int,std::vector<event_t>> timeline;

    for(auto& record : input){
        if(record.text.find("begins") != std::string::npos)
        {
            auto id_str = record.text.substr(record.text.find("#")+1, record.text.find(" begins")-6);
            current_guard = std::stoi(id_str);
            timeline[current_guard].push_back({ 0, record.time });
        }
        else if(record.text.find("wakes") != std::string::npos)
        {
            timeline[current_guard].push_back({ 1, record.time });
        }
        else if(record.text.find("asleep") != std::string::npos)
        {
            timeline[current_guard].push_back({ 2, record.time });
        }
    }

    std::map<int,std::chrono::system_clock::duration> guard_sleep_time;
    for(auto& [guard, events] : timeline){

        std::chrono::system_clock::duration time_asleep(0);
        for(int i=0; i<events.size(); ++i){
            if(events[i].event == 1){
                time_asleep += events[i].time - events[i-1].time;
            }
        }

        guard_sleep_time[guard] = time_asleep;
    }

    auto sleepiest_guard = std::max_element(guard_sleep_time.begin(), guard_sleep_time.end(), [](auto& a, auto& b){
        return a.second < b.second;
    });

    std::map<int,int> minute_map;
    auto& sleepiest_guard_events = timeline[sleepiest_guard->first];

    for(int i=0; i<sleepiest_guard_events.size(); ++i){
        if(sleepiest_guard_events[i].event == 1){
            int start = get_minute(sleepiest_guard_events[i-1].time);
            int end = get_minute(sleepiest_guard_events[i].time);
            for(int m=start; m<end; ++m){
                minute_map[m]++;
            } 
        }
    }

    auto sleepiest_minute = std::max_element(minute_map.begin(), minute_map.end(), [](auto& a, auto& b){
        return a.second < b.second;
    });

    int minutes = std::chrono::duration_cast<std::chrono::minutes>(sleepiest_guard->second).count();
    int most_minute = sleepiest_minute->first;

    std::cout << "guard " << sleepiest_guard->first << " sleeps " << minutes << " minutes, mostly at minute " << most_minute << std::endl;
    std::cout << "part1: " << sleepiest_guard->first << " * " <<  most_minute << " = " << sleepiest_guard->first *  most_minute << std::endl;

    int sleepiest_guard2 = -1;
    int sleepiest_minute2 = INT_MIN;
    int sleepiest_minute_count = INT_MIN;

    for(auto& [guard, events] : timeline)
    {
        std::map<int,int> guard_minute_map;

        for(int i=0; i<events.size(); ++i){
            if(events[i].event == 1){
                int start = get_minute(events[i-1].time);
                int end = get_minute(events[i].time);
                for(int m=start; m<end; ++m){
                    guard_minute_map[m]++;
                } 
            }
        }

        auto local_sleepiest_minute = std::max_element(guard_minute_map.begin(), guard_minute_map.end(), [](auto& a, auto& b){
            return a.second < b.second;
        });

        if(local_sleepiest_minute != guard_minute_map.end()){
            if(local_sleepiest_minute->second > sleepiest_minute_count){
                sleepiest_guard2 = guard;
                sleepiest_minute2 = local_sleepiest_minute->first;
                sleepiest_minute_count = local_sleepiest_minute->second;
            }
        }
    }

    std::cout << std::endl << "guard " << sleepiest_guard2 << " sleeps mostly at minute " << sleepiest_minute2 << std::endl;
    std::cout << "part2: " << sleepiest_guard2 << " * " <<  sleepiest_minute2 << " = " << sleepiest_guard2 *  sleepiest_minute2 << std::endl;

}