#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <cassert>
#include <memory>

namespace aux {

//-------------------------------------------------
template<typename FUNCTION>
struct PIPE {
    FUNCTION fn;
    PIPE(FUNCTION fn) : fn(fn) {}
    template<typename T>
    auto operator()(T data) { 
        return fn(data); 
    }
};

template<typename T, typename FUNCTION>
auto operator|(T data, PIPE<FUNCTION> PIPE) {
    return PIPE.fn(data);
}

//-------------------------------------------------
//DEPRECATED
#define LAMBDA(x, fx)           [](auto x){ return fx; }
#define LAMBDAE(escopo, x, fx)  [escopo](auto x){ return fx; }

//-------------------------------------------------

#define FMAP(x, fx)             PIPE([](auto x) { return fx; })
#define FMAPE(x, fx)             PIPE([&](auto x) { return fx; })

//-------------------------------------------------

std::vector<int> IOTA(int init, int end, int step = 1) {
    std::vector<int> aux;
    if (step > 0) {
        for (int i = init; i < end; i += step) {
            aux.push_back(i);
        }
    } else {
        for (int i = init; i > end; i += step) {
            aux.push_back(i);
        }
    }
    return aux;
}

//-------------------------------------------------


struct JOIN {
    std::string separator;
    JOIN(std::string separator = ", ") : separator(separator) {}

    template <class CONTAINER>
    std::string operator()(CONTAINER container) {
        if(container.size() == 0)
            return "";
        std::ostringstream ss;
        for (const auto& item : container)
            ss << separator << item;
        return ss.str().substr(separator.size());
    }
};

template <class CONTAINER>
std::string operator|(CONTAINER container, JOIN join) {
    return join(container);
}

//-------------------------------------------------
struct NEWVET {
    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        return aux;
    }
};

template<typename CONTAINER>
auto operator|(CONTAINER container, NEWVET newvet) {
    return newvet(container);
}

//-------------------------------------------------
struct COPY {
    int begin;
    int end;
    bool from_begin {false};
    bool to_end {false};
    COPY() {
        from_begin = true;
        to_end = true;
    }
    COPY(int begin) {
        this->begin = begin;
        from_begin = false;
        to_end = true;
    }
    COPY(int begin, int end) {
        this->begin = begin;
        this->end = end;
        from_begin = false;
        to_end = false;
    }

    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto aux = container | NEWVET();
        int size = container.size();
        int begin = 0;
        int end = size;
        if (!from_begin) {
            begin = this->begin;
            if (begin < 0)
                begin = size + begin;
            begin = std::min(begin, size);
        }
        if (!to_end) {
            end = this->end;
            if (end < 0)
                end = size + end;
            end = std::min(end, size);
        }

        std::copy(container.begin() + begin, container.begin() + end, std::back_inserter(aux));
        return aux;
    }

    std::string operator()(std::string data) {
        std::vector<char> vet(data.begin(), data.end());
        return (*this)(vet) | JOIN("");
    }
};

template<typename CONTAINER>
auto operator|(CONTAINER container, COPY copy) {
    return copy(container);
}

std::string operator|(std::string data, COPY copy) {
    return copy(data);
}

//-------------------------------------------------
struct TAKE {
    int n;
    TAKE(int n): n(n) {}
};
template<typename CONTAINER>
auto operator|(CONTAINER container, TAKE take){
    return container | COPY(0, take.n);
};

//-------------------------------------------------
struct DROP {
    int n;
    DROP(int n): n(n) {}
};
template<typename CONTAINER>
auto operator|(CONTAINER container, DROP drop){
    return container | COPY(drop.n);
};

//-------------------------------------------------

template<typename FUNCTION>
struct FILTER {
    FUNCTION fn;
    FILTER(FUNCTION fn) : fn(fn) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | NEWVET();
        std::copy_if(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    };
};

template<typename CONTAINER, typename FUNCTION>
auto operator|(CONTAINER container, FILTER<FUNCTION> filter){
    return filter(container);
};

//-------------------------------------------------

template<typename FUNCTION>
struct MAP {
    FUNCTION fn;
    MAP(FUNCTION fn) : fn(fn) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        std::vector<decltype(fn(*container.begin()))> aux;
        std::transform(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    };
};

template<typename CONTAINER, typename FUNCTION>
auto operator|(CONTAINER container, MAP<FUNCTION> map){
    return map(container);
};

//-------------------------------------------------
struct SPLIT {
    char delimiter;
    SPLIT(char delimiter) : delimiter(delimiter) {}

    std::vector<std::string> operator()(std::string str) {
        std::vector<std::string> aux;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter))
            aux.push_back(token);
        return aux;
    }
};

std::vector<std::string> operator|(std::string str, SPLIT split) {
    return split(str);
}

//-------------------------------------------------
template <typename T>
struct STR2 {
    T operator()(std::string value) {
        std::istringstream iss(value);
        T aux;
        if (iss >> aux) {
            return aux;
        }
        std::cout << "fail: conversão de \"" << value << "\" " << std::endl;
        return aux;
    }
};

template <typename T>
T operator|(std::string value, STR2<T> str2) {
    return str2(value);
}

//-------------------------------------------------
//DEPRECATED

template <typename T>
T to(std::string value) {
    return STR2<T>()(value);
}

//-------------------------------------------------

template <typename... Types>
struct TUPLEFY {
    char delimiter;
    TUPLEFY(char delimiter) : delimiter(delimiter) {}

    template<class Head, class... Tail>
    std::tuple<Head, Tail...> tuple_read_impl(std::istream& is, char delimiter) {
        Head val;
        std::string token;
        std::getline(is, token, delimiter);
        std::stringstream sstoken(token);
        sstoken >> val;
        if constexpr (sizeof...(Tail) == 0) // this was the last tuple value
            return std::tuple{val};
        else
            return std::tuple_cat(std::tuple{val}, tuple_read_impl<Tail...>(is, delimiter));
    }

    std::tuple<Types...> operator()(std::string content) {
        std::stringstream ss(content);
        return tuple_read_impl<Types...>(ss, this->delimiter);
    }
};

template <typename... Types>
std::tuple<Types...> operator|(const std::string& line, TUPLEFY<Types...> parse) {
    return parse(line);
}



//-------------------------------------------------
struct STR {
    std::string action;
    STR(std::string action = "") : action(action) {}

    template<typename DATA>
    std::string operator()(DATA data) {
        if (this->action == "") {
            std::ostringstream oss;
            oss << data;
            return oss.str();
        }
        char buffer[1000];
        sprintf(buffer, this->action.c_str(), data);
        return std::string(buffer);
    }
};

template<typename DATA>
std::string operator|(DATA data, STR fmt) {
    return fmt(data);
}

//-------------------------------------------------
struct FMT {
    std::string separator;
    std::string prefix;
    std::string suffix;
    FMT(std::string separator = ", ", std::string prefix = "[", std::string suffix = "]") :
        separator(separator), prefix(prefix), suffix(suffix) {}

    template <typename... Ts>
    std::string operator()(std::tuple<Ts...> const &theTuple) {
        std::stringstream ss;
        std::apply(
            [&](Ts const &...tupleArgs)
            {
                ss << this->prefix;
                std::size_t n{0};
                ((ss << tupleArgs << (++n != sizeof...(Ts) ? this->separator : "")), ...);
                ss << this->suffix;
            },
            theTuple);
        return ss.str();
    }

    template <class DATA>
    std::string operator()(std::vector<DATA> container) {
        return this->prefix + (container | JOIN(this->separator)) + this->suffix;
    }
};

template <typename... Ts>
std::string operator|(std::tuple<Ts...> const &theTuple, FMT fmt) {
    return fmt(theTuple);
}

template <class CONTAINER>
std::string operator|(CONTAINER container, FMT fmt) {
    return fmt(container);
}


//-------------------------------------------------

struct KEYS {
    KEYS() {}
    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        return container | MAP([](auto x) { return x.first; });
    };
};

template<typename CONTAINER>
auto operator|(CONTAINER container, KEYS keys){
    return keys(container);
};

//-------------------------------------------------

struct VALUES {
    VALUES() {}
    template<typename CONTAINER>

    auto operator()(CONTAINER container){
        return container | MAP([](auto x) { return x.second; });
    };
};
template<typename CONTAINER>
auto operator|(CONTAINER container, VALUES values){
    return values(container);
};

//-------------------------------------------------
struct REVERSE {
    REVERSE() {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | NEWVET();
        std::copy(container.rbegin(), container.rend(), std::back_inserter(aux));
        return aux;
    };
};

template<typename CONTAINER>
auto operator|(CONTAINER container, REVERSE reverse){
    return reverse(container);
};

//-------------------------------------------------
struct SORT {
    SORT() {}

    template<typename CONTAINER>
    CONTAINER operator()(CONTAINER container, SORT sort){
        auto aux = container | COPY();
        std::sort(aux.begin(), aux.end());
        return container;
    };
};

template<typename CONTAINER>
CONTAINER operator|(CONTAINER container, SORT){
    return container | SORT();
};

//-------------------------------------------------
template <typename FUNCTION>
struct SORTBY {
    FUNCTION fn;
    SORTBY(FUNCTION fn): fn(fn) {}

    template<typename CONTAINER>
    CONTAINER operator()(CONTAINER container){
        auto aux = container | COPY();
        std::sort(aux.begin(), aux.end(), this->fn);
        return aux;
    };
};

template<typename CONTAINER, typename FUNCTION>
CONTAINER operator|(CONTAINER container, SORTBY<FUNCTION> sortby){
    return sortby(container);
};

//-------------------------------------------------
struct SHUFFLE {
    SHUFFLE() {}

    template<typename CONTAINER>
    CONTAINER operator()(CONTAINER container){
        auto aux = container | COPY();
        std::random_shuffle(aux.begin(), aux.end());
        return aux;
    };
};

template<typename CONTAINER>
CONTAINER operator|(CONTAINER container, SHUFFLE shuffle){
    return shuffle(container);
};

//-------------------------------------------------
template <typename FUNCTION, typename ACC>
struct FOLD {
    FUNCTION fn;
    ACC acc;
    FOLD(FUNCTION fn, ACC acc) : fn(fn), acc(acc) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        ACC output = this->acc;
        for (const auto& item : container)
            output = this->fn(output, item);
        return output;
    };
};

template<typename CONTAINER, typename FUNCTION, typename ACC>
auto operator|(CONTAINER container, FOLD<FUNCTION, ACC> fold){
    return fold(container);
};
//-------------------------------------------------
struct SUM {
    SUM() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER container, SUM){
    return container | FOLD([](auto acc, auto x) { return acc + x; }, 0);
};

//-------------------------------------------------
template<typename FUNCTION>
struct FINDIF {
    FUNCTION fn;
    FINDIF(FUNCTION fn) : fn(fn) {}
};

template<typename CONTAINER, typename FUNCTION>
auto operator|(CONTAINER& container, FINDIF<FUNCTION> findif){
    return std::find_if(container.begin(), container.end(), findif.fn);
};

//-------------------------------------------------
template<typename FUNCTION>
struct FOREACH {
    FUNCTION fn;
    FOREACH(FUNCTION fn) : fn(fn) {}
};

template<typename CONTAINER, typename FUNCTION>
void operator|(CONTAINER& container, FOREACH<FUNCTION> foreach){
    std::for_each(container.begin(), container.end(), foreach.fn);
};

template<typename CONTAINER, typename FUNCTION>
void operator|(const CONTAINER& container, FOREACH<FUNCTION> foreach){
    std::for_each(container.begin(), container.end(), foreach.fn);
};

//-------------------------------------------------
template<typename VALUE>
struct FIND {
    VALUE value;
    FIND(VALUE value) : value(value) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER& container){
        return std::find(container.begin(), container.end(), value);
    };
};

template<typename CONTAINER, typename VALUE>
auto operator|(CONTAINER& container, FIND<VALUE> find){
    return find(container);
};

//-------------------------------------------------
template<typename VALUE>
struct INDEXOF {
    VALUE value;
    INDEXOF(VALUE value) : value(value) {}

    template<typename CONTAINER>
    int operator()(CONTAINER container){
        auto it = std::find(container.begin(), container.end(), value);
        if (it == container.end())
            return -1;
        return std::distance(container.begin(), it);
    };
};

template<typename CONTAINER, typename VALUE>
int operator|(CONTAINER container, INDEXOF<VALUE> find){
    return find(container);
};

//-------------------------------------------------
template <typename CONTAINER>
struct ZIP {
    CONTAINER container;
    ZIP(CONTAINER container) : container(container) {}

};

template<typename CONTAINER, typename CONTAINER2>
auto operator|(CONTAINER container, ZIP<CONTAINER2> zip){
    using type_a = decltype(*container.begin());
    using type_b = decltype(*zip.container.begin());
    std::vector<std::pair<type_a, type_b>> aux;

    auto ita = container.begin();
    auto itb = zip.container.begin();
    while(ita != container.end() &&  itb != zip.container.end()) {
        aux.push_back({*ita, *itb});
        ita++;
        itb++;
    }
    return aux;
};

//-------------------------------------------------
template <typename EXPECTED>
struct ASSERT {
    EXPECTED expected;
    std::string label;
    ASSERT(EXPECTED expected, std::string label = "") : expected(expected), label(label) {}

    template<typename DATA>
    DATA operator()(DATA data){
        auto received_str = data | STR();
        auto expected_str = expected | STR();

        if (received_str != expected_str) {
            std::cout << "ERROR: " << label 
                      << "\nexpected: " << expected_str 
                      << "\nreceived: " << received_str << '\n';
            exit(1);
        }
        return data;
    };
};

template<typename EXPECTED, typename DATA>
DATA operator|(DATA data, ASSERT<EXPECTED> ass){
    ass(data);
    return data;
};

//-------------------------------------------------

struct PRINT {
    std::string end;
    PRINT(std::string end = "\n") : end(end) {}

    template<typename DATA>
    DATA operator()(DATA data) {
        std::cout << data << this->end;
        return data;
    }
};

template<typename DATA>
DATA operator|(DATA data, PRINT print) {
    return print(data);
}

//-------------------------------------------------

struct STREAM {
    std::unique_ptr<std::stringstream> ss;
    STREAM() : ss(std::make_unique<std::stringstream>()) {}
};

struct COLLECT {
};

template <typename DATA>
STREAM operator|(STREAM stream, DATA data) {
    *stream.ss << data;
    return stream;
}

std::string operator|(STREAM stream, COLLECT) {
    return stream.ss->str();
}

using Chain = std::map<std::string, std::function<void()>>;
using Param = std::vector<std::string>;

void execute(Chain& chain, Param& ui) {
    while(true) {
        std::string line{}, cmd{};
        std::getline(std::cin, line);
        std::stringstream ss(line);
        ss >> cmd;
        ui = line | SPLIT(' ');
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



} // namespace aux


