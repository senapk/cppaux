#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>

namespace aux {
    using Chain = std::map<std::string, std::function<void()>>;
    using Param = std::vector<std::string>;

    std::vector<std::string> __split(const std::string& line, char delimiter) {
        std::istringstream is(line);
        std::vector<std::string> output;
        std::string token;
        while(std::getline(is, token, delimiter))
            output.push_back(token);
        return output;
    };

    void __action(Chain& chain, Param& ui, bool on_moodle) {
        while(true) {
            std::string line{}, cmd{};
            if (!on_moodle)
                std::cout << "$";
            std::getline(std::cin, line);
            std::stringstream ss(line);
            ss >> cmd;
            ui = __split(line, ' ');
            if (on_moodle)
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

    void shell(Chain& chain, Param& ui) {
        __action(chain, ui, false);
    }

    void execute(Chain& chain, Param& ui) {
        __action(chain, ui, true);
    }
}

