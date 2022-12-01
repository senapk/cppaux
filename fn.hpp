#pragma once

#include <type_traits>
#include <iostream>
#include <sstream>
#include <functional>
#include <cassert>
#include <memory>
#include <cstdarg>
#include <optional>

#include <set>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

namespace fn {

template<typename PRINTABLE>
std::string tostr(PRINTABLE data, std::string cfmt = "");

template <typename PRINTABLE>
PRINTABLE asserteq(PRINTABLE received, PRINTABLE expected, std::string label = "");

template <typename PRINTABLE>
auto ASSERTEQ(PRINTABLE expected, std::string label = "");

//-------------------------------------------------
//guarda uma função de um único parametro para ser executada em pipeline ou invocada diretamente
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

template <typename PRINTABLE>
PRINTABLE asserteq(PRINTABLE received, PRINTABLE expected, std::string label) {
    if (received != expected) {
        std::cout << "\n----------label------------\n" << label 
                    << "\n---------received----------\n" << tostr(received) 
                    << "\n---------expected----------\n" << tostr(expected) 
                    << "\n---------------------------\n";
        exit(1);
    }
    return received;
};

template <typename PRINTABLE>
auto ASSERTEQ(PRINTABLE expected, std::string label) {
    return PIPE([expected, label](PRINTABLE received) {
        return asserteq(received, expected, label);
    });
};


//-------------------------------------------------
#define FMAP(x, fx)             PIPE([] (auto x) { return fx; })
#define FMAP2(x, y, fxy)             [] (auto x, auto y) { return fxy; }

//-------------------------------------------------
class TOSTR {
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
        std::ostringstream ss;
        for (auto it = container.begin(); it != container.end(); ++it) {
            ss << (it == container.begin() ? "" : separator) << (*this)(*it);
        }
        auto output = ss.str();
        return TOSTR::embrace(output, brakets);
    }

    template <typename... Ts>
    std::string join(std::tuple<Ts...> const &theTuple, std::string separator = ", ", std::string brakets = "()")
    {
        std::stringstream ss;
        std::apply( [&](Ts const &...tupleArgs) {
                std::size_t n{0};
                ((ss << (*this)(tupleArgs) << (++n != sizeof...(Ts) ? separator : "")), ...);
            }, theTuple);
        return TOSTR::embrace(ss.str(), brakets);
    }

    template<typename A, typename B>
    std::string join(std::pair<A, B> pair, std::string separator = ", ", std::string brakets = "()")
    {
        auto output = (*this)(pair.first) + separator + (*this)(pair.second);
        return TOSTR::embrace(output, brakets);
    }

    //-----------------------------------------------------
    TOSTR(std::string cfmt = ""): cfmt(cfmt) {
    }

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

    template <typename T>
    std::string operator()(const std::optional<T> &v) {
        return v ? (*this)(*v) : "None";
    }

    template<typename PRINTABLE>
    std::string operator()(PRINTABLE data) {
        if (this->cfmt == "") {
            std::ostringstream ss;
            ss << data;
            return ss.str();
        } else {
            return TOSTR::cformat(this->cfmt, data);
        }
    }

    //-----------------------------------------------------
    template<typename PRINTABLE>
    friend std::string operator|(PRINTABLE data, TOSTR obj) {
        return obj(data);
    }
};

template<typename PRINTABLE>
std::string tostr(PRINTABLE data, std::string cfmt) {
    return TOSTR(cfmt)(data);
}

//-------------------------------------------------
class SLICE {
    int begin;
    int end;
    bool from_begin {false};
    bool to_end {false};

    template<typename CONTAINER>
    static auto new_vec_from(CONTAINER container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        return aux;
    }

    template <typename K, typename T>
    static auto new_vec_from(std::map<K, T> container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }

    template <typename K, typename T>
    static auto new_vec_from(std::unordered_map<K, T> container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }

public:
    SLICE(int begin = 0) {
        this->from_begin = begin == 0;
        this->begin = begin;
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
        auto aux = SLICE::new_vec_from(container);
        
        //empty container
        if (!this->from_begin && !this->to_end && (this->begin == this->end)) {
            return aux;
        }

        //full container
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
    friend auto operator|(CONTAINER container, SLICE obj) {
        return obj(container);
    }
};

template<typename CONTAINER>
auto slice(CONTAINER container, int begin = 0) {
    return SLICE(begin)(container);
}

template<typename CONTAINER>
auto slice(CONTAINER container, int begin, int end) {
    return SLICE(begin, end)(container);
}

// -------------------------------------------------
template<typename CONTAINER, typename FN>
auto map(CONTAINER container, FN fn) {
    std::vector<decltype(fn(*container.begin()))> aux;
    std::transform(container.begin(), container.end(), std::back_inserter(aux), fn);
    return aux;
}

template <class FUNCTION>
auto MAP(FUNCTION fn) {
    return PIPE([fn](auto container) {
        return map(container, fn);
    });
};

//-------------------------------------------------

template<typename CONTAINER, typename FN>
auto filter(CONTAINER container, FN fn) {
    auto aux = slice(container, 0, 0);
    for(auto& x : container) {
        if(fn(x))
            aux.push_back(x);
    }
    return aux;
}

template <class FUNCTION>
auto FILTER(FUNCTION fn) {
    return PIPE([fn](auto container) {
        return filter(container, fn);
    });
};

//-------------------------------------------------

template <typename READABLE>
READABLE strto(std::string value) {
    std::istringstream iss(value);
    READABLE aux;
    if (iss >> aux) {
        return aux;
    }
    std::cout << "fail: conversão de \"" << value << "\" " << std::endl;
    return aux;
}

template <class READABLE>
auto STRTO() {
    return PIPE([](std::string value) {
        return strto<READABLE>(value);
    });
};

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

template <typename... TS>
std::tuple<TS...> tuplefy(const std::string& line, char delimiter) {
    return TUPLEFY<TS...>(delimiter)(line);
}

//-------------------------------------------------

template<typename CONTAINER_A, typename CONTAINER_B>
auto zip(CONTAINER_A A, CONTAINER_B B) {
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

template<typename CONTAINER>
auto ZIP(CONTAINER B) {
    return PIPE([B](auto A) {
        return zip(A, B);
    });
};

//-------------------------------------------------

template<typename CONTAINER_A, typename CONTAINER_B, typename FNJOIN>
auto zipwith(CONTAINER_A A, CONTAINER_B B, FNJOIN fnjoin) {
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

template<typename CONTAINER, typename FNJOIN>
auto ZIPWITH(CONTAINER B, FNJOIN fn) {
    return PIPE([B, fn](auto A) {
        return zipwith(A, B, fn);
    });
};

//-------------------------------------------------

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
                ((result.push_back(TOSTR(controls[++i])(tupleArgs))), ...);
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
        return TOSTR().join(texts | ZIPWITH(vars, [](auto x, auto y) { return x + y;}), "", ""); 
    }

    friend std::string operator| (std::string fmt, FORMAT<Args...> mesh) {
        return mesh(fmt);
    }
};

template<typename... Args>
std::string format(std::string fmt, Args ...args) {
    return fmt | FORMAT<Args...>(args...);
}

//-------------------------------------------------

template <class T>
std::vector<T> range(T init, T end, int step = 1) {
    if (step == 0)
        throw std::invalid_argument("step cannot be zero");
    std::vector<T> aux;
    if (step > 0) {
        for (T i = init; i < end; i += step) {
            aux.push_back(i);
        }
    } else {
        for (T i = init; i > end; i += step) {
            aux.push_back(i);
        }
    }
    return aux;
}

template <class T>
std::vector<T> range(T end) {
    return range(0, end, 1);
}

//-------------------------------------------------

template <class T>
std::vector<T> iota(int size, T init, int step = 1) {
    if (step == 0)
        throw std::invalid_argument("step cannot be zero");
    std::vector<T> aux;
    for (int i = 0; i < size; i++) {
        aux.push_back(init);
        init += step;
    }
    return aux;
}

template <class T>
auto IOTA(T init, int step = 1) {
    return PIPE([init, step](int size) mutable {
        return iota(size, init, step);
    });
}


//-------------------------------------------------

const char * cstr(std::string content) {
    static std::string data; 
    data = content; 
    return data.c_str(); 
}

auto CSTR() {
    return PIPE([](std::string content) { 
        return cstr(content);
    });
}

//-------------------------------------------------

template <typename PRINTABLE>
PRINTABLE write(PRINTABLE data, std::string end = "\n") {
    std::cout << tostr(data) << end;
    return data;
}

auto WRITE(std::string end = "\n") {
    return PIPE([end](auto data) {
        return write(data, end);
    });
}

//-------------------------------------------------

std::vector<std::string> split(std::string content, char delimiter = ' ') {
    std::vector<std::string> aux;
    std::string token;
    std::istringstream tokenStream(content);
    while (std::getline(tokenStream, token, delimiter))
        aux.push_back(token);
    return aux;
}

auto SPLIT(char delimiter = ' ') {
    return PIPE([delimiter](std::string content) {
        return split(content, delimiter);
    });
};

//-------------------------------------------------

template <typename CONTAINER>
std::string join(CONTAINER container, std::string separator = "", std::string brakets = "") {
    return TOSTR().join(container, separator, brakets);
}

auto JOIN(std::string separator = "", std::string brakets = "") {
    return PIPE([separator, brakets](auto container) {
        return join(container, separator, brakets);
    });
};

} // namespace fn
