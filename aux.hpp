#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <memory>

namespace aux {

//-------------------------------------------------

#define LAMBDA(escopo, x, fx) [escopo](auto x){ return fx; }

//-------------------------------------------------

std::vector<int> IOTA(int init, int end, int step = 1) {
    std::vector<int> aux;
    for (int i = init; i < end; i += step)
        aux.push_back(i);
    return aux;
}

//-------------------------------------------------
struct NEWVET {
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, NEWVET) {
    auto fn = [](auto x) {return x;}; 
    std::vector<decltype(fn(*container.begin()))> aux;
    return aux;
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
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, COPY copy) {
    auto aux = container | NEWVET();
    int size = container.size();
    int begin = 0;
    int end = size;
    if (!copy.from_begin) {
        begin = copy.begin;
        if (begin < 0)
            begin = size + begin;
        begin = std::min(begin, size);
    }
    if (!copy.to_end) {
        end = copy.end;
        if (end < 0)
            end = size + end;
        end = std::min(end, size);
    }

    std::copy(container.begin() + begin, container.begin() + end, std::back_inserter(aux));
    return aux;
}
//-------------------------------------------------
struct TAKE {
    int n;
    TAKE(int n): n(n) {}
};
template<typename CONTAINER>
auto operator|(CONTAINER&& container, TAKE take){
    return container | COPY(0, take.n);
};

//-------------------------------------------------
struct DROP {
    int n;
    DROP(int n): n(n) {}
};
template<typename CONTAINER>
auto operator|(CONTAINER&& container, DROP drop){
    return container | COPY(drop.n);
};

//-------------------------------------------------

template<typename LAMBDA>
struct FILTER {
    LAMBDA fn;
    FILTER(LAMBDA fn) : fn(fn) {}
};

template<typename CONTAINER, typename LAMBDA>
auto operator|(CONTAINER&& container, FILTER<LAMBDA> filter){
    auto aux = container | NEWVET();
    std::copy_if(container.begin(), container.end(), std::back_inserter(aux), filter.fn);
    return aux;
};
//-------------------------------------------------

template<typename LAMBDA>
struct MAP {
    LAMBDA fn;
    MAP(LAMBDA fn) : fn(fn) {}
};

template<typename CONTAINER, typename LAMBDA>
auto operator|(CONTAINER&& container, MAP<LAMBDA> map){
    std::vector<decltype(map.fn(*container.begin()))> aux;
    std::transform(container.begin(), container.end(), std::back_inserter(aux), map.fn);
    return aux;
};

//-------------------------------------------------
struct SPLIT {
    char delimiter;
    SPLIT(char delimiter) : delimiter(delimiter) {}
};

std::vector<std::string> operator|(std::string str, SPLIT split) {
    std::vector<std::string> aux;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, split.delimiter))
        aux.push_back(token);
    return aux;
}

//-------------------------------------------------
template <typename T>
struct STR2 {
};

template <typename T>
T operator|(std::string value, STR2<T>) {
    std::istringstream iss(value);
    T aux;
    if (iss >> aux) {
        return aux;
    }
    std::cout << "fail: conversão de \"" << value << "\" " << std::endl;
    return aux;
}

//-------------------------------------------------

template <typename... Args>
struct TUPLEFY {
    char delimiter;
    TUPLEFY(char delimiter) : delimiter(delimiter) {}
};

template <typename... Args>
auto operator|(const std::string& line, TUPLEFY<Args...> parse) {
    std::istringstream is(line);
    std::tuple<Args...> t;

    auto get_token = [] (std::istream& is, char delimiter, auto& value) {
        std::string line;
        std::getline(is, line, delimiter);
        std::istringstream ss(line);
        ss >> value;
    };

    std::apply(
        [&is, &parse, get_token](auto&&... args) {
            ((get_token(is, parse.delimiter, args)), ...);
        },t);
    return t;
}

//-------------------------------------------------
struct STR {
    std::string action;
    STR(std::string action = "") : action(action) {}
};

template<typename DATA>
std::string operator|(DATA data, STR fmt) {
    if (fmt.action == "") {
        std::ostringstream oss;
        oss << data;
        return oss.str();
    }
    char buffer[1000];
    sprintf(buffer, fmt.action.c_str(), data);
    return std::string(buffer);
}

//-------------------------------------------------

struct PRINT {
    std::string end;
    PRINT(std::string end = "\n") : end(end) {}
};

template<typename DATA>
void operator|(DATA&& data, PRINT print) {
    std::cout << data << print.end;
}
//-------------------------------------------------

struct JOIN {
    std::string delimiter;
    JOIN(std::string delimiter = ", ") : delimiter(delimiter) {}
};

template <class CONTAINER>
std::string operator|(CONTAINER&& container, JOIN join) {
    if(container.size() == 0)
        return "";
    std::ostringstream ss;
    for (const auto& item : container)
        ss << join.delimiter << item;
    return ss.str().substr(join.delimiter.size());
}

//-------------------------------------------------
struct FMT {
    std::string delimiter;
    std::string prefix;
    std::string suffix;
    FMT(std::string delimiter = ", ", std::string prefix = "[", std::string suffix = "]") :
        delimiter(delimiter), prefix(prefix), suffix(suffix) {}
};

template <class CONTAINER>
std::string operator|(CONTAINER&& container, FMT fmt) {
    return fmt.prefix + (container | JOIN(fmt.delimiter)) + fmt.suffix;
}
//-------------------------------------------------

struct KEYS {
    KEYS() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, [[maybe_unused]] KEYS keys){
    return container | MAP([](auto x) { return x.first; });
};

//-------------------------------------------------

struct VALUES {
    VALUES() {}
};
template<typename CONTAINER>
auto operator|(CONTAINER&& container, [[maybe_unused]] VALUES values){
    return container | MAP([](auto x) { return x.second; });
};

//-------------------------------------------------
struct REVERSE {
    REVERSE() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, REVERSE){
    auto aux = container | NEWVET();
    std::copy(container.rbegin(), container.rend(), std::back_inserter(aux));
    return aux;
};

//-------------------------------------------------
struct SORT {
    SORT() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, SORT){
    auto aux = container | COPY();
    std::sort(aux.begin(), aux.end());
    return aux;
};

//-------------------------------------------------
template <typename LAMBDA>
struct SORTBY {
    LAMBDA fn;
    SORTBY(LAMBDA fn): fn(fn) {}
};

template<typename CONTAINER, typename LAMBDA>
auto operator|(CONTAINER&& container, SORTBY<LAMBDA> sortby){
    auto aux = container | COPY();
    std::sort(aux.begin(), aux.end(), sortby.fn);
    return aux;
};

//-------------------------------------------------
struct SHUFFLE {
    SHUFFLE() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, SHUFFLE){
    auto aux = container | COPY();
    std::random_shuffle(aux.begin(), aux.end());
    return aux;
};

//-------------------------------------------------
template <typename LAMBDA, typename ACC>
struct FOLD {
    LAMBDA fn;
    ACC acc;
    FOLD(LAMBDA fn, ACC acc) : fn(fn), acc(acc) {}
};

template<typename CONTAINER, typename LAMBDA, typename ACC>
auto operator|(CONTAINER&& container, FOLD<LAMBDA, ACC> fold){
    ACC output = fold.acc;
    for (const auto& item : container)
        output = fold.fn(output, item);
    return output;
};
//-------------------------------------------------
struct SUM {
    SUM() {}
};

template<typename CONTAINER>
auto operator|(CONTAINER&& container, SUM){
    return container | FOLD([](auto acc, auto x) { return acc + x; }, 0);
};
//-------------------------------------------------
template<typename LAMBDA>
struct FINDIF {
    LAMBDA fn;
    FINDIF(LAMBDA fn) : fn(fn) {}
};

template<typename CONTAINER, typename LAMBDA>
auto operator|(CONTAINER&& container, FINDIF<LAMBDA> findif){
    return std::find_if(container.begin(), container.end(), findif.fn);
};

//-------------------------------------------------
template<typename VALUE>
struct FIND {
    VALUE value;
    FIND(VALUE value) : value(value) {}
};

template<typename CONTAINER, typename VALUE>
auto operator|(CONTAINER&& container, FIND<VALUE> find){
    return std::find(container.begin(), container.end(), find.value);
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
    while(ita != container.end() && itb != zip.container.end()) {
        aux.push_back({*ita, *itb});
        ita++;
        itb++;
    }
    return aux;
};

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

void __action(Chain& chain, Param& ui, bool on_moodle) {
    while(true) {
        std::string line{}, cmd{};
        if (!on_moodle)
            std::cout << "$";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        ss >> cmd;
        ui = line | SPLIT(' ');
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


} // namespace aux


