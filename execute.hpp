#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using Chain = std::map<std::string, std::function<void()>>;
using Param = std::vector<std::string>;

std::vector<std::string> split(std::string const& s, char delim = ' ') {
    std::vector<std::string> v;
    std::istringstream iss {s};
    std::string token;
    while (std::getline(iss, token, delim)) {
        v.push_back(token);
    }
    return v;
}

double operator+(std::string const& s) {
    double d {};
    std::istringstream iss {s};
    if (iss >> d) {
        return d;
    }
    std::cout << "Error: (" << s << ") is not a number.\n";
    return 0;
} 

void execute(Chain& chain, Param& ui) {
    while(true) {
        std::string line{}, cmd{};
        std::getline(std::cin, line);
        std::stringstream ss(line);
        ss >> cmd;
        ui = split(line, ' ');
        std::cout << "$" << line << '\n';
        if (cmd == "end") {
            break;
        } else if (chain.count(cmd) != 0) {
            try {
                chain[cmd]();
            } catch (std::string& e) {
                std::cout << e << '\n';
            }
        } else {
            std::cout << "fail: command not found\n";
        }
    }
}

