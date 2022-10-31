#pragma once

#include <iostream>
#include <sstream>
#include <functional>
#include <cassert>
#include <memory>
#include <cstdarg>

#include <set>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

namespace aux {

//-------------------------------------------------
class STR {
    std::string cfmt;
public:

    template <typename ...Args>
    static std::string cformat(const std::string& format, Args && ...args) 
    {
        auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);
        if (output.back() == '\0') 
            output.pop_back();
        return output;
    }

    static std::string embrace(std::string data, std::string brakets) 
    {
        auto prefix = brakets.size() > 0 ? std::string {brakets[0]} : "";
        auto suffix = brakets.size() > 1 ? std::string {brakets[1]} : "";
        return prefix + data + suffix;
    }

    template <class CONTAINER>
    std::string join(CONTAINER container, std::string separator = ", ", std::string brakets = "[]")
    {
        if(container.size() == 0)
            return "";
        std::ostringstream ss;
        for (const auto& item : container)
            ss << separator << (*this)(item);
        auto output = ss.str().substr(separator.size());
        return STR::embrace(output, brakets);
    }

    template <typename... Ts>
    std::string join(std::tuple<Ts...> const &theTuple, std::string separator = ", ", std::string brakets = "()")
    {
        std::stringstream ss;
        std::apply( [&](Ts const &...tupleArgs) {
                std::size_t n{0};
                ((ss << (*this)(tupleArgs) << (++n != sizeof...(Ts) ? separator : "")), ...);
            }, theTuple);
        return STR::embrace(ss.str(), brakets);
    }

    template<typename A, typename B>
    std::string join(std::pair<A, B> pair, std::string separator = ", ", std::string brakets = "()")
    {
        auto output = (*this)(pair.first) + separator + (*this)(pair.second);
        return STR::embrace(output, brakets);
    }

    //-----------------------------------------------------
    STR(std::string cfmt = ""): cfmt(cfmt){
    }

    // std::string operator()(std::string data) {
    //     return data;
    // }

    // std::string operator()(const char * data) {
    //     return std::string(data);
    // }

    template<typename A, typename B>
    std::string operator()(const std::pair<A, B>& pair) {
        return this->join(pair);
    }

    template <typename... Ts>
    std::string operator()(std::tuple<Ts...> const &theTuple) {
        return this->join(theTuple);
    }

    template <typename T>
    std::string operator()(const std::vector<T> &v) {
        return this->join(v);
    }

    template <typename T>
    std::string operator()(const std::list<T> &v) {
        return this->join(v);
    }

    template <typename T>
    std::string operator()(const std::set<T> &v) {
        return this->join(v);
    }

    template <typename T>
    std::string operator()(const std::unordered_set<T> &v) {
        return this->join(v);
    }

    template <typename K, typename T>
    std::string operator()(const std::map<K, T> &v) {
        return this->join(v);
    }

    template <typename K, typename T>
    std::string operator()(const std::unordered_map<K, T> &v) {
        return this->join(v);
    }

    template<typename PRINTABLE>
    std::string operator()(PRINTABLE data) {
        if (this->cfmt == "") {
            std::ostringstream ss;
            ss << data;
            return ss.str();
        } else {
            return STR::cformat(this->cfmt, data);
        }
    }

    //-----------------------------------------------------
    template<typename PRINTABLE>
    friend std::string operator|(PRINTABLE data, STR str) {
        return str(data);
    }
};

//-------------------------------------------------
struct CSTR {
    static std::string data;
    CSTR() {
    }
    const char * operator()(std::string content) {
        CSTR::data = content;
        return data.c_str();
    }
    friend const char * operator|(std::string content, CSTR cstr) {
        return cstr(content);
    }
};
std::string CSTR::data = "";

//-------------------------------------------------
template <typename PRINTABLE>
struct ASSERT {
    PRINTABLE expected;
    std::string label;
    ASSERT(PRINTABLE expected, std::string label = "") {
        this->expected = expected;
        this->label = label;
    }

    PRINTABLE operator()(PRINTABLE received){

        if (received != expected) {
            std::cout << "\n----------label------------\n" << this->label 
                      << "\n---------received----------\n" << STR()(received) 
                      << "\n---------expected----------\n" << STR()(expected) 
                      << "\n---------------------------\n";
            exit(1);
        }
        return received;
    };

    friend PRINTABLE operator|(PRINTABLE data, ASSERT<PRINTABLE> ass){
        ass(data);
        return data;
    };
};

//-------------------------------------------------
//guarda uma função de um único parametro para ser executada em pipeline
template<typename FUNCTION> 
struct PIPE {
    FUNCTION fn;
    PIPE(FUNCTION fn) : fn(fn) {}

    PIPE& operator()() { 
        return *this; 
    }

    template<typename DATA>
    auto operator()(DATA data) { 
        return fn(data); 
    }

    template<typename DATA>
    friend auto operator|(DATA data, PIPE<FUNCTION> PIPE) {
        return PIPE(data);
    }
};

//-------------------------------------------------
//DEPRECATED
#define LAMBDA(x, fx)           [](auto x){ return fx; }
#define LAMBDAE(escopo, x, fx)  [escopo](auto x){ return fx; }

//-------------------------------------------------
#define FMAP(x, fx)             PIPE([] (auto x) { return fx; })
#define FMAPE(x, fx)            PIPE([&](auto x) { return fx; })
#define FMAPR(x, pre, ret)      PIPE([] (auto x) { pre; return ret; })

#define FMAP2(x, y, fxy)             [] (auto x, auto y) { return fxy; }
#define FMAPE2(x, y, fxy)            [&](auto x, auto y) { return fxy; }

//-------------------------------------------------
struct PRINT {
    std::string end;
    PRINT(std::string end = "\n") : end(end) {}

    template<typename DATA>
    DATA operator()(DATA data) {
        std::cout << STR()(data) << this->end;
        return data;
    }

    template<typename DATA>
    friend DATA operator|(DATA data, PRINT print) {
        return print(data);
    }
};

// -------------------------------------------------
struct JOIN {
    std::string separator;
    std::string brakets;

    JOIN(std::string separator = "", std::string brakets = "") : separator(separator), brakets(brakets) {
    }

    template <class CONTAINER>
    std::string operator()(CONTAINER container) {
        return STR().join(container, this->separator, this->brakets);
    }

    template <class CONTAINER>
    friend std::string operator|(CONTAINER container, JOIN join) {
        return join(container);
    }
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

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, MAP<FUNCTION> map){
        return map(container);
    };
};

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
struct NEWVECFROM {
    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        return aux;
    }

    template <typename K, typename T>
    auto operator()(std::map<K, T> container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }

    template <typename K, typename T>
    auto operator()(std::unordered_map<K, T> container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, NEWVECFROM newvec) {
        return newvec(container);
    }
};

//-------------------------------------------------
struct SLICE {
    int begin;
    int end;
    bool from_begin {false};
    bool to_end {false};
    SLICE() {
        this->from_begin = true;
        this->to_end = true;
    }
    SLICE(int begin) {
        this->begin = begin;
        this->from_begin = false;
        this->to_end = true;
    }
    SLICE(int begin, int end) {
        this->begin = begin;
        this->end = end;
        this->from_begin = false;
        this->to_end = false;
    }

    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto aux = container | NEWVECFROM();
        if (this->from_begin && this->to_end) {
            std::copy(container.begin(), container.end(), std::back_inserter(aux));
            return aux;
        }

        int size = container.size();
        int begin = 0;
        int end = size;
        if (!this->from_begin) {
            begin = this->begin;
            if (begin < 0)
                begin = size + begin;
            begin = std::min(begin, size);
        }
        if (!this->to_end) {
            end = this->end;
            if (end < 0)
                end = size + end;
            end = std::min(end, size);
        }

        std::copy(std::next(container.begin(), begin), std::next(container.begin(), end), std::back_inserter(aux));
        return aux;
    }

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, SLICE slice) {
        return slice(container);
    }
};

//-------------------------------------------------
struct TAKE {
    int n;
    TAKE(int n): n(n) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        if (n < 0)
            return container | SLICE(n);
        return container | SLICE(0, this->n);
    }

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, TAKE take){
        return take(container);
    };
};

//-------------------------------------------------
struct DROP {
    int n;
    DROP(int n): n(n) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        if (this->n < 0)
            return container | SLICE(0, this->n);
        return container | SLICE(this->n);
    }

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, DROP drop){
        return drop(container);
    };
};

//-------------------------------------------------
template<typename FUNCTION>
struct FILTER {
    FUNCTION fn;
    FILTER(FUNCTION fn) : fn(fn) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | NEWVECFROM();
        std::copy_if(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, FILTER<FUNCTION> filter){
        return filter(container);
    };
};

//-------------------------------------------------
struct SPLIT {
    char delimiter;
    SPLIT(char delimiter=' ') : delimiter(delimiter) {}

    std::vector<std::string> operator()(std::string str) {
        std::vector<std::string> aux;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter))
            aux.push_back(token);
        return aux;
    }

    friend std::vector<std::string> operator|(std::string str, SPLIT split) {
        return split(str);
    }
};

//-------------------------------------------------
template <typename READABLE>
struct STR2 {
    READABLE operator()(std::string value) {
        std::istringstream iss(value);
        READABLE aux;
        if (iss >> aux) {
            return aux;
        }
        std::cout << "fail: conversão de \"" << value << "\" " << std::endl;
        return aux;
    }

    friend READABLE operator|(std::string value, STR2<READABLE> str2) {
        return str2(value);
    }
};

//DEPRECATED --------------------------------------
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
        std::stringstream ss_token(token);
        ss_token >> val;
        if constexpr (sizeof...(Tail) == 0) // this was the last tuple value
            return std::tuple{val};
        else
            return std::tuple_cat(std::tuple{val}, tuple_read_impl<Tail...>(is, delimiter));
    }

    std::tuple<Types...> operator()(std::string content) {
        std::stringstream ss(content);
        return tuple_read_impl<Types...>(ss, this->delimiter);
    }

    friend std::tuple<Types...> operator|(const std::string& line, TUPLEFY<Types...> parse) {
        return parse(line);
    }
};

//-------------------------------------------------
// struct KEYS {
//     KEYS() {}
//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         return container | MAP([](auto x) { return x.first; });
//     };

//     template<typename CONTAINER>
//     friend auto operator|(CONTAINER container, KEYS keys){
//         return keys(container);
//     };
// };

//-------------------------------------------------
// struct REVERSE {
//     REVERSE() {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         auto aux = container | NEWVECFROM();
//         std::copy(container.rbegin(), container.rend(), std::back_inserter(aux));
//         return aux;
//     };

//     template<typename CONTAINER>
//     friend auto operator|(CONTAINER container, REVERSE reverse){
//         return reverse(container);
//     };
// };

//-------------------------------------------------
// struct SORT {
//     SORT() {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         auto aux = container | SLICE();
//         std::sort(aux.begin(), aux.end());
//         return aux;
//     };

//     template<typename CONTAINER>
//     friend auto operator|(CONTAINER container, SORT sort){
//         return sort(container);
//     };
// };

//-------------------------------------------------
template <typename FUNCTION>
struct SORTBY {
    FUNCTION fn;
    SORTBY(FUNCTION fn): fn(fn) {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | SLICE();
        std::sort(aux.begin(), aux.end(), this->fn);
        return aux;
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, SORTBY<FUNCTION> sortby){
        return sortby(container);
    };
};

// //-------------------------------------------------
// struct SHUFFLE {
//     SHUFFLE() {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         auto aux = container | SLICE();
//         std::random_shuffle(aux.begin(), aux.end());
//         return aux;
//     };

//     template<typename CONTAINER>
//     friend auto operator|(CONTAINER container, SHUFFLE shuffle){
//         return shuffle(container);
//     };
// };

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

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, FOLD<FUNCTION, ACC> fold){
        return fold(container);
    };
};

//-------------------------------------------------
template <typename FUNCTION>
struct FOLD1 {
    FUNCTION fn;
    FOLD1(FUNCTION fn) : fn(fn){}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto front = container.front();
        return container | SLICE(1) | FOLD(this->fn, front);
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, FOLD1<FUNCTION> fold){
        return fold(container);
    };
};

//-------------------------------------------------
// struct SUM {
//     SUM() {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         return container | FOLD1([](auto acc, auto x) { return acc + x; });
//     };

//     template<typename CONTAINER>
//     friend auto operator|(CONTAINER container, SUM sum){
//         return sum(container);
//     };
// };

//-------------------------------------------------
template<typename FUNCTION>
struct FOREACH {
    FUNCTION fn;
    FOREACH(FUNCTION fn) : fn(fn) {}
    
    template<typename CONTAINER>
    void operator()(CONTAINER container){
        std::for_each(container.begin(), container.end(), this->fn);
    };

    template<typename CONTAINER>
    friend void operator|(CONTAINER container, FOREACH<FUNCTION> foreach){
        foreach(container);
    };
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

    template<typename CONTAINER>
    friend int operator|(CONTAINER container, INDEXOF<VALUE> find){
        return find(container);
    };
};

//-------------------------------------------------
template <typename CONTAINER>
class ZIP {

    template<typename CONTAINER_A, typename CONTAINER_B>
    static auto join(CONTAINER_A A, CONTAINER_B B){
        auto fn = [](auto x) { return x; };
        using type_a = decltype(fn(*A.begin()));
        using type_b = decltype(fn(*B.begin()));
        std::vector<std::pair<type_a, type_b>> aux;

        auto ita = A.begin();
        auto itb = B.begin();
        while(ita != A.end() &&  itb != B.end()) {
            aux.push_back({*ita, *itb});
            ita++;
            itb++;
        }
        return aux;
    };

public:
    CONTAINER container;
    ZIP(CONTAINER container) : container(container) {}

    template<typename CONTAINER2>
    auto operator()(CONTAINER2 other){
        return ZIP::join(this->container, other);
    };

    template<typename CONTAINER2>
    friend auto operator|(CONTAINER2 container, ZIP<CONTAINER> zip){
        return ZIP::join(container, zip.container);
    };
};

//-------------------------------------------------
template <typename CONTAINER, typename FUNCTION>
class ZIPWITH {
    
    template<typename CONTAINER_A, typename CONTAINER_B, typename FNJOIN>
    static auto join(CONTAINER_A A, CONTAINER_B B, FNJOIN fnjoin){
        auto idcopy = [](auto x) { return x; };
        using type_out = decltype( fnjoin( idcopy(*A.begin()), idcopy(*B.begin()) ));
        std::vector<type_out> aux;

        auto ita = A.begin();
        auto itb = B.begin();
        while(ita != A.end() &&  itb != B.end()) {
            aux.push_back(fnjoin(*ita, *itb));
            ita++;
            itb++;
        }
        return aux;
    };


public:
    CONTAINER container;
    FUNCTION fn;
    ZIPWITH(CONTAINER container, FUNCTION fn) : container(container), fn(fn) {}

    template<typename CONTAINER2>
    auto operator()(CONTAINER2 other){
        return ZIPWITH::join(this->container, other, this->fn);
    };

    template<typename CONTAINER2>
    friend auto operator|(CONTAINER2 container, ZIPWITH<CONTAINER, FUNCTION> zipwith){
        return ZIPWITH::join(container, zipwith.container, zipwith.fn);
    };
};

template<typename... Args>
class FORMAT 
{
    std::tuple<Args...> args;

    std::vector<std::string> tuple_to_vector_str(const std::vector<std::string>& controls)
    {
        std::vector<std::string> result;
        std::apply
        (
            [&result, &controls](Args const&... tupleArgs)
            {
                int i = -1;
                ((result.push_back(aux::STR(controls[++i])(tupleArgs))), ...);
            }, this->args
        );
        return result;
    }

    std::pair<std::vector<std::string>, std::vector<std::string>> extract(std::string data)
    {
        data.append("_");
        auto lim = data.size() - 1;
        std::vector<std::string> texts = {""};
        std::vector<std::string> ctrls = {""};
        auto* destiny = &texts;

        for (size_t i = 0; i < lim;) {
            char c = data[i];
            if ((c == '{' && data[i + 1] == '{') || (c == '}' && data[i + 1] == '}'))
            {
                destiny->back().append(std::string(1, c));
                i += 2;
            }
            else if (c == '{') 
            {
                texts.push_back("");
                destiny = &ctrls;
                i++;
            }
            else if (c == '}') 
            {
                ctrls.push_back("");
                destiny = &texts;
                i++;
            }
            else
            {
                destiny->back().append(std::string(1,c));
                i++;
            }
        }
        while (texts.size() > ctrls.size())
            ctrls.push_back("");
        return {texts, ctrls};
    }

public:

    FORMAT(Args ...args): args(std::forward<Args>(args)...)
    {
    }

    std::string operator()(std::string fmt)
    {
        auto [texts, controls] = extract(fmt);
        auto vars = tuple_to_vector_str(controls);
        while(vars.size() < texts.size())
            vars.push_back("");
        return texts | aux::ZIPWITH(vars, FMAP2(x, y, x + y)) | aux::JOIN(); 
    }

    friend std::string operator| (std::string fmt, FORMAT<Args...> mesh) {
        return mesh(fmt);
    }
};

auto KEYS    = PIPE(MAP(FMAP(x, x.first)));
auto VALUES  = PIPE(MAP(FMAP(x, x.second)));
auto SORT    = PIPE([](auto v) { auto z = v | SLICE(); std::sort(z.begin(), z.end()); return z;});
auto REVERSE = PIPE([](auto v) { auto z = v | SLICE(); std::reverse(z.begin(), z.end()); return z;});
auto SHUFFLE = PIPE([](auto v) { auto z = v | SLICE(); std::random_shuffle(z.begin(), z.end()); return z;});
auto UNIQUE  = PIPE([](auto y) { auto v = y | SLICE(); std::sort(v.begin(), v.end()); v.erase(std::unique(v.begin(), v.end()), v.end()); return v;});
auto SUM     = FMAP(x, x | FOLD1(FMAP2(x, y, x + y)));


//-------------------------------------------------
template <typename DATA>
struct CC {
    std::string content;
    CC(DATA data) {
        content = STR()(data);
    }

    std::string operator()(std::string before) {
        return before + content;
    }

    friend std::string operator | (std::string before, CC cc) {
        return before + cc.content;
    }
};

//-------------------------------------------------


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


