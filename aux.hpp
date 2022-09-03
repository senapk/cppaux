#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <functional>
#include <map>
#include <iomanip>

#define FX(x_fn) []([[maybe_unused]] auto& x){ return x_fn; }
#define IDX [](auto& x){ return x; }

namespace aux {

    // retorna um vector com a transformaçao de cada elemento que passou no predicado
    auto clist(auto fn, const auto& container, auto predicate) {
        using output_type = decltype(fn(*container.begin()));
        std::vector<output_type> result;
        for (const auto& item : container)
            if (predicate(item))
                result.push_back(fn(item));
        return result;
    }


    // clist sem avaliação de predicado
    auto clist(auto fn, const auto& container) {
        return clist(fn, container, FX(true));
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------

    // aplica a função fn sobre cada elemento do container
    // depois junta os resultados em uma string
    // separando os elementos com o delimiter
    // auto fn = [](auto v){return *v;};
    // std::vector<int*> vi = {new int(1), new int(2), new int(3)};
    // std::cout << aux::join(fn, vi) << '\n';
    // std::cout << aux::join(FX(*x), vi) << '\n';

    std::string join(auto fn, const auto& container, const std::string& delimiter = ", ", const std::string &prefix = "[", const std::string &suffix = "]") {
        if(container.size() == 0)
            return "";
        std::ostringstream ss;
        for (const auto& item : container)
            ss << delimiter << fn(item);
        return prefix + ss.str().substr(delimiter.size()) + suffix;
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------

    // extrai um subvetor
    // extrai de início a fim, sem incluir o final
    // se não for passado o final, extrai até o final
    // se não for passado o início, extrai do início ao final
    // se o índice for negativo, conta a partir do final como no python e no js
    template <typename T>
    std::vector<T> slice(const std::vector<T>& container, int start = 0, int end = 0) {
        std::vector<T> result;
        if (end == 0)
            end = container.size();
        if (start < 0)
            start = container.size() + start;
        if (end < 0)
            end = container.size() + end;
        if (start >= end)
            return result;
        auto it_start = container.begin();
        auto it_end = container.begin();
        std::advance(it_start, start);
        std::advance(it_end, end);
        std::copy(it_start, it_end, std::back_inserter(result));
        return result;
    }

    // splits a line in a vector of strings using delimiter
    std::vector<std::string> split(const std::string& line, char delimiter) {
        std::istringstream is(line);
        std::vector<std::string> output;
        std::string token;
        while(std::getline(is, token, delimiter))
            output.push_back(token);
        return output;
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------

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

    template <class T>
    std::string fmt(std::vector<T> vet, std::string delimiter = ",") {
        return join(IDX, vet, delimiter, "[", "]");
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------


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
        auto vet_str = split(data.substr(1, data.size() - 2), delimiter);
        std::vector<T> vet;
        for (auto token : vet_str)
            vet.push_back(to<T>(token));
        return vet;
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------


    struct Show {
    };

    Show show;

    template <class T>
    Show& operator<<(Show& show, const T& data) {
        std::cout << fmt(data) << '\n';
        return show;
    }

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------


    using Chain = std::map<std::string, std::function<void()>>;
    using Param = std::vector<std::string>;

    void __shell(Chain& chain, Param& ui, bool on_moodle) {
        while(true) {
            std::string line{}, cmd{};
            if (!on_moodle)
                std::cout << "$";
            std::getline(std::cin, line);
            std::stringstream ss(line);
            ss >> cmd;
            ui = aux::split(line, ' ');
            if (on_moodle)
                std::cout << "$" << line << '\n';
            if (cmd == "end") {
                break;
            } else if (chain.count(cmd) != 0) {
                chain[cmd]();
            } else {
                std::cout << "fail: command not found\n";
            }
        }
    }

    void shell(Chain& chain, Param& ui) {
        __shell(chain, ui, false);
    }

    void execute(Chain& chain, Param& ui) {
        __shell(chain, ui, true);
    }
}