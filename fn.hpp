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
        std::ostringstream ss;
        for (auto it = container.begin(); it != container.end(); ++it) {
            ss << (it == container.begin() ? "" : separator) << (*this)(*it);
        }
        auto output = ss.str();
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

// -------------------------------------------------
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
    friend auto operator|(CONTAINER container, SLICE slice) {
        return slice(container);
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
                ((result.push_back(STR(controls[++i])(tupleArgs))), ...);
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
        return STR().join(texts | ZIPWITH(vars, [](auto x, auto y) { return x + y;}), "", ""); 
    }

    friend std::string operator| (std::string fmt, FORMAT<Args...> mesh) {
        return mesh(fmt);
    }
};

//-------------------------------------------------
#define FMAP(x, fx)             PIPE([] (auto x) { return fx; })
#define FMAP2(x, y, fxy)             [] (auto x, auto y) { return fxy; }


template <class T>
auto IOTA(T init, int step = 1) {
    return PIPE([init, step](int size) mutable {
        std::vector<T> aux;
        for (int i = 0; i < size; i++) {
            aux.push_back(init);
            init += step;
        }
        return aux;
    });
}

auto KEYS() {
    return PIPE(MAP(FMAP(x, x.first)));
}

auto VALUES() {
    return PIPE(MAP(FMAP(x, x.second)));
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

auto CSTR() {
    return PIPE([](std::string content) { 
        static std::string data; 
        data = content; 
        return data.c_str(); 
    });
}

auto PRINT(std::string end = "\n") {
    return PIPE([end](auto data) {
        std::cout << STR()(data) << end;
        return data;
    });
}

template <class FUNCTION>
auto FILTER(FUNCTION fn) {
    return PIPE([fn](auto container) {
        auto aux = container | SLICE(0, 0);
        std::copy_if(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    });
};

auto SPLIT(char delimiter = ' ') {
    return PIPE([delimiter](std::string content) {
        std::vector<std::string> aux;
        std::string token;
        std::istringstream tokenStream(content);
        while (std::getline(tokenStream, token, delimiter))
            aux.push_back(token);
        return aux;
    });
};

auto JOIN(std::string separator = "", std::string brakets = "") {
    return PIPE([separator, brakets](auto container) {
        return STR().join(container, separator, brakets);
    });
};

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

template <typename FUNCTION>
auto FOREACH(FUNCTION fn) {
    return PIPE([fn](auto vet) {
        std::for_each(vet.begin(), vet.end(), fn);
    });
};

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
    return PIPE([](auto container){
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

} // namespace fn
