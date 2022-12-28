#pragma once
#include "fn.hpp"

namespace fn {

auto KEYS() {
    return PIPE(MAP(FNT(x, x.first)));
}

auto VALUES() {
    return PIPE(MAP(FNT(x, x.second)));
}

auto SORT() {
    return PIPE([](auto v) { 
        auto z = v | SLICE(); 
        std::sort(z.begin(), z.end()); 
        return z;
    });
}

auto REVERSE() {
    return PIPE([](auto v) { 
        auto z = v | SLICE(); 
        std::reverse(z.begin(), z.end()); 
        return z;
    });
}

auto SHUFFLE() {
    return PIPE([](auto v) { 
        auto z = v | SLICE(); 
        std::random_shuffle(z.begin(), z.end()); 
        return z;
    });
}

auto UNIQUE() {
    return PIPE([](auto y) { 
        auto v = y | SLICE();
        std::sort(v.begin(), v.end()); 
        v.erase(std::unique(v.begin(), v.end()), v.end()); 
        return v;
    });
}

auto TAKE(int n) {
    return PIPE([n](auto container) {
        if (n < 0)
            return container | SLICE(n);
        return container | SLICE(0, n);
    });
}

auto DROP(int n) {
    return PIPE([n](auto container) {
        if (n < 0)
            return container | SLICE(0, n);
        return container | SLICE(n);
    });
}

template <typename FUNCTION>
auto SORTBY(FUNCTION fn) {
    return PIPE([fn](auto vet) {
        auto aux = vet | SLICE();
        std::sort(aux.begin(), aux.end(), fn);
        return aux;
    });
};

template <typename CONTAINER>
auto CAT(CONTAINER other) {
    return PIPE([other](CONTAINER left) {
        for (auto x : other)
            left.push_back(x);
        return left; 
    });
}

template <typename DATA>
auto INDEXOF(DATA value) {
    return PIPE([value](auto vet) {
        int index = 0;
        for (auto it = vet.begin(); it != vet.end(); it++, index++) {
            if (*it == value)
                return index;
        } 
        return -1;
    });
};

template <class FUNCTION, class DATA>
auto FOLD(FUNCTION fn, DATA acc) {
    return PIPE([fn, acc](auto vet) mutable {
        for(const auto& item : vet)
            acc = fn(acc, item);
        return acc;
    });
}

auto SUM() {
    return PIPE([](auto container) {
        return container | FOLD([](auto x, auto y) {return x + y;}, 0);
    });
}

template <class FUNCTION>
auto FINDIF(FUNCTION fn) {
    return PIPE([fn](auto container) {
        auto it = std::find_if(container.begin(), container.end(), fn);
        auto identity = [](auto x) { return x; };
        return it == container.end() ? std::nullopt : std::make_optional(identity(*it));
    });
}

template <class FUNCTION>
auto FOLD1(FUNCTION fn) {
    return PIPE([fn](auto vet) {
        auto first = vet.front();
        auto tail = vet | SLICE(1);
        return tail | FOLD(fn, first);
    });
}

}