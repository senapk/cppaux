#pragma once

#include <iostream>
#include <sstream>
#include <vector>

namespace aux {
    template <class T>
    T to(std::string data) {
        T value;
        std::stringstream(data) >> value;
        return value;
    }

    template <>
    bool to(std::string data) {
        return data == "true";
    }

    template <class T>
    std::vector<T> to_vet(std::string data, char delimiter = ',') {
        std::istringstream is(data.substr(1, data.size() - 2));
        std::vector<T> output;
        std::string token;
        while(std::getline(is, token, delimiter))
            output.push_back(aux::to<T>(token));
        return output;
    }
}

