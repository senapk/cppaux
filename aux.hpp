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
template <typename PRINTABLE>
struct ASSERT {
    std::string expected;
    std::string label;
    ASSERT(PRINTABLE expected, std::string label = "") {
        this->expected = this->to_string(expected);
        this->label = label;
    }

    template<typename DATA>
    std::string to_string(DATA data) {
        std::ostringstream oss;
        oss << data;
        return oss.str();
    }

    template<typename DATA>
    DATA operator()(DATA data){
        auto received = this->to_string(data);

        if (received != expected) {
            std::cout << "ERROR: " << this->label 
                      << "\nreceived: " << received 
                      << "\nexpected: " << expected 
                      << '\n';
            exit(1);
        }
        return data;
    };

    template<typename DATA>
    friend DATA operator|(DATA data, ASSERT<PRINTABLE> ass){
        ass(data);
        return data;
    };
};

//-------------------------------------------------
template<typename FUNCTION> 
struct PIPE {
    FUNCTION fn;
    PIPE(FUNCTION fn) : fn(fn) {}

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
#define FMAP(x, fx)             PIPE([](auto x) { return fx; })
#define FMAPE(x, fx)             PIPE([&](auto x) { return fx; })

//-------------------------------------------------
struct STR {
    std::string format;
    STR(std::string format = "") : format(format) {}

    template<typename PRINTABLE>
    std::string operator()(PRINTABLE data) {
        if (this->format == "") {
            std::ostringstream oss;
            oss << data;
            return oss.str();
        }
        char buffer[1000];
        sprintf(buffer, this->format.c_str(), data);
        return std::string(buffer);
    }

    template<typename PRINTABLE>
    friend std::string operator|(PRINTABLE data, STR fmt) {
        return fmt(data);
    }
};

// -------------------------------------------------
struct JOIN {
    std::string separator;
    std::string prefix = "";
    std::string suffix = "";

    JOIN(std::string separator = "", std::string prefix_suffix = "") : separator(separator) {
        if (prefix_suffix == "")
            return;
        if (prefix_suffix.size() > 0)
            this->prefix = prefix_suffix.substr(0, 1);
        if (prefix_suffix.size() > 1)
            this->suffix = prefix_suffix.substr(1, 1);
    }

    template <class PRINTABLE_CONTAINER>
    std::string operator()(PRINTABLE_CONTAINER container) {
        if(container.size() == 0)
            return "";
        std::ostringstream ss;
        for (const auto& item : container)
            ss << separator << item;
        return prefix + ss.str().substr(separator.size()) + suffix;
    }

    template <class PRINTABLE1, class PRINTABLE2>
    std::string operator()(std::pair<PRINTABLE1, PRINTABLE2> pair) {
        return prefix + STR()(pair.first) + separator + STR()(pair.second) + suffix;
    }


    template <typename... Ts>
    std::string operator()(std::tuple<Ts...> const &theTuple) {
        std::stringstream ss;
        std::apply( [&](Ts const &...tupleArgs) {
                std::size_t n{0};
                ((ss << tupleArgs << (++n != sizeof...(Ts) ? this->separator : "")), ...);
            }, theTuple);
        return prefix + ss.str() + suffix;
    }

    template <class PRINTABLE_CONTAINER>
    friend std::string operator|(PRINTABLE_CONTAINER container, JOIN join) {
        return join(container);
    }

    template <typename... Ts>
    friend std::string operator|(std::tuple<Ts...> const &theTuple, JOIN join) {
        return join(theTuple);
    }

    template <class PRINTABLE1, class PRINTABLE2>
    friend std::string operator|(std::pair<PRINTABLE1, PRINTABLE2> pair, JOIN join) {
        return join(pair);
    }
};

//-------------------------------------------------
struct FMT {
    std::string separator;
    std::string prefix_suffix;
    FMT(std::string separator = ", ", std::string prefix_suffix = "[]") : 
        separator(separator), prefix_suffix(prefix_suffix) {
    }

    template <class CONTAINER_PRINTABLE>
    std::string operator()(CONTAINER_PRINTABLE container) {
        return JOIN(this->separator, this->prefix_suffix)(container);
    }
};

template <class CONTAINER_PRINTABLE>
std::string operator|(CONTAINER_PRINTABLE container, FMT fmt) {
    return fmt(container);
}

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

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, NEWVECFROM newvec) {
        return newvec(container);
    }
};

//-------------------------------------------------
struct VECTORIZE {
    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        std::transform(container.begin(), container.end(), std::back_inserter(aux), fn);
        return aux;
    }

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, VECTORIZE vectorize) {
        return vectorize(container);
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

struct KEYS {
    KEYS() {}
    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        return container | MAP([](auto x) { return x.first; });
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, KEYS keys){
        return keys(container);
    };
};


//-------------------------------------------------

struct VALUES {
    VALUES() {}
    template<typename CONTAINER>

    auto operator()(CONTAINER container){
        return container | MAP([](auto x) { return x.second; });
    };
    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, VALUES values){
        return values(container);
    };
};

//-------------------------------------------------
struct REVERSE {
    REVERSE() {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | NEWVECFROM();
        std::copy(container.rbegin(), container.rend(), std::back_inserter(aux));
        return aux;
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, REVERSE reverse){
        return reverse(container);
    };
};


//-------------------------------------------------
struct SORT {
    SORT() {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | SLICE();
        std::sort(aux.begin(), aux.end());
        return aux;
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, SORT sort){
        return sort(container);
    };
};


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


//-------------------------------------------------
struct SHUFFLE {
    SHUFFLE() {}

    template<typename CONTAINER>
    auto operator()(CONTAINER container){
        auto aux = container | SLICE();
        std::random_shuffle(aux.begin(), aux.end());
        return aux;
    };

    template<typename CONTAINER>
    friend auto operator|(CONTAINER container, SHUFFLE shuffle){
        return shuffle(container);
    };
};


// //-------------------------------------------------
// template <typename FUNCTION, typename ACC>
// struct FOLD {
//     FUNCTION fn;
//     ACC acc;
//     FOLD(FUNCTION fn, ACC acc) : fn(fn), acc(acc) {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER container){
//         ACC output = this->acc;
//         for (const auto& item : container)
//             output = this->fn(output, item);
//         return output;
//     };
// };

// template<typename CONTAINER, typename FUNCTION, typename ACC>
// auto operator|(CONTAINER container, FOLD<FUNCTION, ACC> fold){
//     return fold(container);
// };
// //-------------------------------------------------
// struct SUM {
//     SUM() {}
// };

// template<typename CONTAINER>
// auto operator|(CONTAINER container, SUM){
//     return container | FOLD([](auto acc, auto x) { return acc + x; }, 0);
// };

// //-------------------------------------------------
// template<typename FUNCTION>
// struct FINDIF {
//     FUNCTION fn;
//     FINDIF(FUNCTION fn) : fn(fn) {}
// };

// template<typename CONTAINER, typename FUNCTION>
// auto operator|(CONTAINER& container, FINDIF<FUNCTION> findif){
//     return std::find_if(container.begin(), container.end(), findif.fn);
// };

// //-------------------------------------------------
// template<typename FUNCTION>
// struct FOREACH {
//     FUNCTION fn;
//     FOREACH(FUNCTION fn) : fn(fn) {}
// };

// template<typename CONTAINER, typename FUNCTION>
// void operator|(CONTAINER& container, FOREACH<FUNCTION> foreach){
//     std::for_each(container.begin(), container.end(), foreach.fn);
// };

// template<typename CONTAINER, typename FUNCTION>
// void operator|(const CONTAINER& container, FOREACH<FUNCTION> foreach){
//     std::for_each(container.begin(), container.end(), foreach.fn);
// };

// //-------------------------------------------------
// template<typename VALUE>
// struct FIND {
//     VALUE value;
//     FIND(VALUE value) : value(value) {}

//     template<typename CONTAINER>
//     auto operator()(CONTAINER& container){
//         return std::find(container.begin(), container.end(), value);
//     };
// };

// template<typename CONTAINER, typename VALUE>
// auto operator|(CONTAINER& container, FIND<VALUE> find){
//     return find(container);
// };

// //-------------------------------------------------
// template<typename VALUE>
// struct INDEXOF {
//     VALUE value;
//     INDEXOF(VALUE value) : value(value) {}

//     template<typename CONTAINER>
//     int operator()(CONTAINER container){
//         auto it = std::find(container.begin(), container.end(), value);
//         if (it == container.end())
//             return -1;
//         return std::distance(container.begin(), it);
//     };
// };

// template<typename CONTAINER, typename VALUE>
// int operator|(CONTAINER container, INDEXOF<VALUE> find){
//     return find(container);
// };

// //-------------------------------------------------
// template <typename CONTAINER>
// struct ZIP {
//     CONTAINER container;
//     ZIP(CONTAINER container) : container(container) {}

// };

// template<typename CONTAINER, typename CONTAINER2>
// auto operator|(CONTAINER container, ZIP<CONTAINER2> zip){
//     using type_a = decltype(*container.begin());
//     using type_b = decltype(*zip.container.begin());
//     std::vector<std::pair<type_a, type_b>> aux;

//     auto ita = container.begin();
//     auto itb = zip.container.begin();
//     while(ita != container.end() &&  itb != zip.container.end()) {
//         aux.push_back({*ita, *itb});
//         ita++;
//         itb++;
//     }
//     return aux;
// };

// //-------------------------------------------------

// struct PRINT {
//     std::string end;
//     PRINT(std::string end = "\n") : end(end) {}

//     template<typename DATA>
//     DATA operator()(DATA data) {
//         std::cout << data << this->end;
//         return data;
//     }
// };

// template<typename DATA>
// DATA operator|(DATA data, PRINT print) {
//     return print(data);
// }

// //-------------------------------------------------

// struct STREAM {
//     std::unique_ptr<std::stringstream> ss;
//     STREAM() : ss(std::make_unique<std::stringstream>()) {}
// };

// struct COLLECT {
// };

// template <typename DATA>
// STREAM operator|(STREAM stream, DATA data) {
//     *stream.ss << data;
//     return stream;
// }

// std::string operator|(STREAM stream, COLLECT) {
//     return stream.ss->str();
// }

// using Chain = std::map<std::string, std::function<void()>>;
// using Param = std::vector<std::string>;

// void execute(Chain& chain, Param& ui) {
//     while(true) {
//         std::string line{}, cmd{};
//         std::getline(std::cin, line);
//         std::stringstream ss(line);
//         ss >> cmd;
//         ui = line | SPLIT(' ');
//         std::cout << "$" << line << '\n';
//         if (cmd == "end") {
//             break;
//         } else if (chain.count(cmd) != 0) {
//             try {
//                 chain[cmd]();
//             } catch (std::string& e) {
//                 std::cout << e << '\n';
//             }
//         } else {
//             std::cout << "fail: command not found\n";
//         }
//     }
// }



} // namespace aux


