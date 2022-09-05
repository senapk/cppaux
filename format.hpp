#pragma once

#include <iostream> //string
#include <sstream>  //stringstream
#include <vector>   //vector
#include <iomanip>  //setprecision
#include <functional>

namespace aux {

    template<typename T>
    std::string fmt(T value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template <>
    std::string fmt(bool value) {
        return value ? "true" : "false";
    }

    template <>
    std::string fmt(double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value;
        return ss.str();
    }

    template <>
    std::string fmt(float value) {
        return fmt((double) value);
    }

    template <typename K, typename T>
    std::string fmt(std::pair<K,T> value) {
        return fmt(value.first) + ":" + fmt(value.second);
    }

    template <class T>
    std::string fmt(std::vector<T> vet, std::string sep = ", ") {
        std::string prefix = "[";
        std::string suffix = "]";
        if(vet.size() == 0)
            return prefix + suffix;
        std::stringstream ss;
        for (const auto& item : vet)
            ss << sep << fmt(item);
        return prefix + ss.str().substr(sep.size()) + suffix;
    }

    struct Show {
    };

    Show show;

    template <class T>
    Show& operator<<(Show& show, T&& data) {
        std::cout << fmt(data) << '\n';
        return show;
    }
}

