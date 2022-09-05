#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace aux {

    template<typename CONTAINER, typename LAMBDA>
    auto map(CONTAINER&& container, LAMBDA&& fn){
        std::vector<decltype(fn(*container.begin()))> aux;
        std::transform(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    };

    template<typename CONTAINER, typename LAMBDA>
    auto filter(CONTAINER&& container, LAMBDA&& pred){
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        std::copy_if(container.begin(), container.end(), std::back_inserter(aux), pred);
        return aux;
    };

    template<typename LAMBDA1, typename CONTAINER, typename LAMBDA2>
    auto clist(LAMBDA1&& fn, CONTAINER&& container, LAMBDA2&& pred){ 
        std::vector<decltype(fn(*container.begin()))> aux;
        for (auto&& elem : container)
            if (pred(elem))
                aux.push_back(fn(elem));
        return aux;
    };

    template<typename CONTAINER, typename LAMBDA, typename ELEM>
    auto foldl(CONTAINER&& container, LAMBDA&& fn, ELEM&& init){
        auto aux = init;
        for (auto&& elem : container)
            aux = fn(aux, elem);
        return aux;
    };

    template<typename CONTAINER, typename LAMBDA>
    auto foreach(CONTAINER&& container, LAMBDA&& fn){
        for (auto&& elem : container)
            fn(elem);
    };

    template<typename CONTAINER1, typename CONTAINER2>
    auto zip(CONTAINER1&& container1, CONTAINER2&& container2){
        std::vector<std::pair<decltype(*container1.begin()), decltype(*container2.begin())>> aux;
        auto it1 = container1.begin();
        auto it2 = container2.begin();
        while (it1 != container1.end() && it2 != container2.end())
            aux.push_back(std::make_pair(*it1++, *it2++));
        return aux;
    };

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

    std::vector<std::string> split(const std::string& line, char delimiter) {
        std::istringstream is(line);
        std::vector<std::string> output;
        std::string token;
        while(std::getline(is, token, delimiter))
            output.push_back(token);
        return output;
    }
}

