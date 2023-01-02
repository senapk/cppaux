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

// template<typename PRINTABLE>
// std::string tostr(PRINTABLE data, std::string cfmt = "");
// auto        TOSTR(                std::string cfmt = "");

// template <typename PRINTABLE> 
// PRINTABLE VERIFY(PRINTABLE received, PRINTABLE expected, std::string label = "");
// template <typename PRINTABLE> 
// auto      VERIFY(                    PRINTABLE expected, std::string label = "");

// template<typename CONTAINER>
// auto slice(CONTAINER container, int begin = 0);
// auto SLICE(                     int begin = 0);

// template<typename CONTAINER>
// auto slice(CONTAINER container, int begin, int end);
// auto SLICE(                     int begin, int end);

// template<typename CONTAINER, typename FUNCTION>
// auto map(CONTAINER container, FUNCTION fn);
// template <typename FUNCTION>
// auto MAP(                     FUNCTION fn);

// template<typename CONTAINER, typename FUNCTION>
// auto filter(CONTAINER container, FUNCTION fn);
// template <typename FUNCTION> 
// auto FILTER(                     FUNCTION fn);

// template <typename READABLE>
// READABLE strto(std::string value);
// template <typename READABLE>
// auto     STRTO();

// double number(std::string value);
// auto   NUMBER();

// template <typename... TS>
// std::tuple<TS...> unpack(const std::string& line, char delimiter);
// template <typename... TS>
// auto              UNPACK(                         char delimiter);

// template<typename CONTAINER_A, typename CONTAINER_B>
// auto zip(CONTAINER_A A, CONTAINER_B B);
// template<typename CONTAINER_B>
// auto ZIP(               CONTAINER_B B);

// template<typename CONTAINER_A, typename CONTAINER_B, typename FNJOIN>
// auto zipwith(CONTAINER_A A, CONTAINER_B B, FNJOIN fnjoin);
// template<typename CONTAINER_B, typename FNJOIN>
// auto ZIPWITH(               CONTAINER_B B, FNJOIN fnjoin);

// template<typename... Args>
// std::string format(std::string fmt, Args ...args);
// template<typename... Args>
// auto        FORMAT(                 Args ...args);

// std::vector<int> range(int init, int end, int step = 1);
// auto             RANGE(          int end, int step = 1);

// std::vector<int> range(int end);
// auto             RANGE(       );

// template <typename PRINTABLE>
// PRINTABLE write(PRINTABLE data, std::string end = "\n");
// auto      WRITE(                std::string end = "\n");

// std::vector<std::string> split(std::string content, char delimiter = ' ');
// auto                     SPLIT(                     char delimiter = ' ');

// template <typename CONTAINER>
// std::string join(CONTAINER container, std::string separator = "", std::string brakets = "");
// auto        JOIN(                     std::string separator = "", std::string brakets = "");

// std::string input(std::istream & is = std::cin);
// auto        INPUT();

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------

//[[pipe]]
/**
 * @brief Functor para criação de funções Pipeline.
 * 
 * PIPE é um functor, ou seja, uma struct que após instanciada, funciona como uma função.
 * Ela é construída passando uma função que recebe um único parâmetro qualquer.
 * O PIPE então guarda essa função para que possa ser executada em pipeline ou invocada diretamente.
 * 
 * @param fn função a ser guardada
 * @note https://github.com/senapk/cppaux/#pipe
 */
template<typename FUNCTION> 
struct PIPE
//[[pipe]]]
{
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
template<typename PRINTABLE>
std::string tostr(PRINTABLE data, std::string cfmt = ""); 

struct __STRAUX {
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

    static std::string embrace(std::string data, std::string brackets) 
    {
        auto prefix = brackets.size() > 0 ? std::string {brackets[0]} : "";
        auto suffix = brackets.size() > 1 ? std::string {brackets[1]} : "";
        return prefix + data + suffix;
    }

    template <typename CONTAINER>
    static std::string join(std::string fmt, CONTAINER container, std::string separator = ", ", std::string brackets = "[]")
    {
        std::ostringstream ss;
        for (auto it = container.begin(); it != container.end(); ++it) {
            ss << (it == container.begin() ? "" : separator) << tostr(*it, fmt);
        }
        auto output = ss.str();
        return __STRAUX::embrace(output, brackets);
    }

    template <typename... Ts>
    static std::string join(std::string fmt, std::tuple<Ts...> const &theTuple, std::string separator = ", ", std::string brackets = "()")
    {
        std::stringstream ss;
        std::apply( [&](Ts const &...tupleArgs) {
                std::size_t n{0};
                ((ss << tostr(tupleArgs, fmt) << (++n != sizeof...(Ts) ? separator : "")), ...);
            }, theTuple);
        return __STRAUX::embrace(ss.str(), brackets);
    }

    template<typename A, typename B>
    static std::string join(std::string fmt, std::pair<A, B> pair, std::string separator = ", ", std::string brackets = "()")
    {
        auto output = tostr(pair.first, fmt) + separator + tostr(pair.second, fmt);
        return __STRAUX::embrace(output, brackets);
    }
};

//-------------------------------------------------

//[[join]]
/**
 * @brief Transforma um container, par ou tupla em string separando os elementos pelo separador e envolvendo com os brakets.
 *
 * Se os elementos não forem strings, eles serão transformados em string utilizando a função tostr
 * Exemplo:
 * join(std::vector<int>{1, 2, 3}, "-", "<>") | WRITE(); // <1-2-3>
 * join(range(10)) | WRITE(); // 0123456789 
 */
template <typename CONTAINER>
std::string join(CONTAINER container, std::string separator = "", std::string brakets = "")
//[[join]]
{
    return __STRAUX().join("", container, separator, brakets);
}

// Transforma um container, par ou tupla em string, separando os elementos pelo separador e envolvendo com os brakets
// Se os elementos não forem strings, eles serão transformados em string utilizando a função TOSTR
// Exemplo:
// std::vector<int>{1, 2, 3} | JOIN("-", "<>") | WRITE(); // <1-2-3>
// range(10) | JOIN() | WRITE(); // 0123456789
inline auto JOIN(std::string separator = "", std::string brakets = "") {
    return PIPE([separator, brakets](auto container) {
        return join(container, separator, brakets);
    });
};

//-------------------------------------------------

template <typename DATA>
struct __TOSTR{
    std::string apply(DATA data, std::string fmt = "") {
        if (fmt == "") {
            std::ostringstream ss;
            ss << data;
            return ss.str();
        } else {
            return __STRAUX::cformat(fmt, data);
        }
    }
};

//specialization for bool
template <>
struct __TOSTR<bool> {
    std::string apply(bool t, std::string fmt = "") {
        (void) fmt;
        return t ? "true" : "false";
    }
};

//specialization for string
template <>
struct __TOSTR<std::string> {
    std::string apply(std::string v, std::string fmt = "") {
        if (fmt.size() > 0) {
            static std::string dummy;
            dummy = v;
            return __STRAUX::cformat(fmt, v.c_str());
        }
        return v;
    }
};

//specialization for pair
template <typename A, typename B>
struct __TOSTR<std::pair<A, B>> {
    std::string apply(std::pair<A, B> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "()");
    }
};

//specialization for tuple
template <typename... Ts>
struct __TOSTR<std::tuple<Ts...>> {
    std::string apply(std::tuple<Ts...> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "()");
    }
};

//specialization for vector
template <typename T>
struct __TOSTR<std::vector<T>> {
    std::string apply(std::vector<T> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "[]");
    }
};

//specialization for list
template <typename T>
struct __TOSTR<std::list<T>> {
    std::string apply(std::list<T> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "[]");
    }
};

//specialization for array
template <typename T, std::size_t N>
struct __TOSTR<std::array<T, N>> {
    std::string apply(std::array<T, N> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "[]");
    }
};

//specialization for set
template <typename T>
struct __TOSTR<std::set<T>> {
    std::string apply(std::set<T> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "{}");
    }
};

//specialization for map
template <typename A, typename B>
struct __TOSTR<std::map<A, B>> {
    std::string apply(std::map<A, B> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "{}");
    }
};

//specialization for unordered_map
template <typename A, typename B>
struct __TOSTR<std::unordered_map<A, B>> {
    std::string apply(std::unordered_map<A, B> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "{}");
    }
};

//specialization for unordered_set
template <typename T>
struct __TOSTR<std::unordered_set<T>> {
    std::string apply(std::unordered_set<T> t, std::string fmt = "") {
        return __STRAUX::join(fmt, t, ", ", "{}");
    }
};

//specialization for shared_ptr
template <typename T>
struct __TOSTR<std::shared_ptr<T>> {
    std::string apply(std::shared_ptr<T> t, std::string fmt = "") {
        return t ? __TOSTR<T>().apply(*t, fmt) : "null";
    }
};

//specialization for optional
template <typename T>
struct __TOSTR<std::optional<T>> {
    std::string apply(std::optional<T> t, std::string fmt = "") {
        return t ? __TOSTR<T>().apply(*t, fmt) : "null";
    }
};

/**
 * Converte o (dado, vetor, par, lista, mapa) em string.
 * 
 * Se passado o parâmetro de formatação, o dado será formatado usando o modelo do printf.
 * Se o dado for um container, o formato será aplicado em todos os elementos
 * do container recursivamente.
 * Vectores, listas e mapas são impressos entre colchetes.
 * Pares são impressos entre parênteses.
 * Para ser impresso, o dado deve implementar o ostream operator<<
 * 
 * @param data Dado a ser convertido
 * @param cfmt Parâmetro de formatação no modo printf
 * @return String com o dado convertido
 * 
 * @note https://github.com/senapk/cppaux#tostr
 */
template<typename PRINTABLE>
std::string tostr(PRINTABLE data, std::string cfmt) {
    return __TOSTR<PRINTABLE>().apply(data, cfmt);
}

/**
 * Converte o (dado, vetor, par, lista, mapa) em string.
 * 
 * Se passado o parâmetro de formatação, o dado será formatado usando o modelo do printf.
 * Se o dado for um container, o formato será aplicado em todos os elementos
 * do container recursivamente.
 * Vectores, listas e mapas são impressos entre colchetes.
 * Pares são impressos entre parênteses.
 * Para ser impresso, o dado deve implementar o ostream operator<<
 * 
 * @param data Dado a ser convertido
 * @param cfmt Parâmetro de formatação no modo printf
 * @return String com o dado convertido
 * 
 * @note https://github.com/senapk/cppaux#tostr
 */
inline auto TOSTR(std::string cfmt = "") {
    return PIPE([cfmt](auto data) {
        return tostr(data, cfmt);
    });
};

//-------------------------------------------------

// Verifica se o dado recebido é igual ao esperado.
// Se não for, imprime o dado recebido e o esperado e encerra o programa.
// verify(4, 6, "testando se quatro é igual a seis");
template <typename PRINTABLE>
PRINTABLE verify(PRINTABLE received, PRINTABLE expected, std::string label = "") {
    if (received != expected) {
        std::cout << "\n----------label------------\n" << label 
                    << "\n---------received----------\n" << tostr(received) 
                    << "\n---------expected----------\n" << tostr(expected) 
                    << "\n---------------------------\n";
        exit(1);
    }
    return received;
};

// Verifica se o dado recebido é igual ao esperado.
// Se não for, imprime o dado recebido e o esperado e encerra o programa.
// 4 | VERIFY(6, "testando se quatro é igual a seis");
template <typename PRINTABLE> 
auto VERIFY(PRINTABLE expected, std::string label = "") {
    return PIPE([expected, label](PRINTABLE received) {
        return verify(received, expected, label);
    });
};


//-------------------------------------------------

// Encurtador de função lambda para um único parâmetro e uma única operação a ser retornada.
// O primeiro parâmetro é o nome da variável a ser utilizada, o segundo é a operação a ser realizada.
// Exemplo:
// auto f = FNT(x, x + 1);
// int x = f(5); // x = 6
#define FNT(x, fx)                  [] (auto x) { return fx; }

// Encurtador de função lambda para dois parâmetros e uma única operação a ser retornada.
#define FNT2(x, y, fxy)             [] (auto x, auto y) { return fxy; }

//-------------------------------------------------
class __SLICE {
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
    __SLICE(int begin = 0) {
        this->from_begin = begin == 0;
        this->begin = begin;
        this->to_end = true;
    }
    __SLICE(int begin, int end) {
        this->begin = begin;
        this->end = end;
        this->from_begin = false;
        this->to_end = false;
    }

    template<typename CONTAINER>
    auto operator()(CONTAINER container) {
        auto aux = __SLICE::new_vec_from(container);
        
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
};

// Fatia um container de begin até o fim retornando um vector com os elementos copiados.
// Se o valor de begin for negativo, ele será contado a partir do fim do container.
// O funcionamento é equivalente à função slice do Python ou do Javascript.
// Exemplos:
// slice(std::vector<int>{1, 2, 3, 4, 5}, 1)  | WRITE(); // [2, 3, 4, 5]
// slice(std::vector<int>{1, 2, 3, 4, 5}, -2) | WRITE(); // [4, 5]
template<typename CONTAINER>
auto slice(CONTAINER container, int begin = 0) {
    return __SLICE(begin)(container);
}

// Fatia um container de begin até o fim retornando um vector com os elementos copiados.
// Se o valor de begin for negativo, ele será contado a partir do fim do container.
// O funcionamento é equivalente à função slice do Python ou do Javascript.
// Exemplos:
// std::vector<int>{1, 2, 3, 4, 5} | SLICE(1)  | WRITE(); // [2, 3, 4, 5]
// std::vector<int>{1, 2, 3, 4, 5} | SLICE(-2) | WRITE(); // [4, 5]
inline auto SLICE(int begin = 0) {
    return PIPE([begin](auto container) {
        return __SLICE(begin)(container);
    });
};

// Fatia um container de begin até o fim retornando um vector com os elementos copiados.
// Se o valor de begin for negativo, ele será contado a partir do fim do container.
// O funcionamento é equivalente à função slice do Python ou do Javascript.
// Exemplos:
// slice(std::vector<int>{1, 2, 3, 4, 5}, 1, -1)  | WRITE(); // [2, 3, 4]
// slice(std::vector<int>{1, 2, 3, 4, 5}, -3, -1) | WRITE(); // [3, 4]
template<typename CONTAINER>
auto slice(CONTAINER container, int begin, int end) {
    return __SLICE(begin, end)(container);
}

// Fatia um container de begin até o fim retornando um vector com os elementos copiados.
// Se o valor de begin for negativo, ele será contado a partir do fim do container.
// O funcionamento é equivalente à função slice do Python ou do Javascript.
// Exemplos:
// std::vector<int>{1, 2, 3, 4, 5} | SLICE(1, -1)  | WRITE(); // [2, 3, 4]
// std::vector<int>{1, 2, 3, 4, 5} | SLICE(-3, -1) | WRITE(); // [3, 4]
inline auto SLICE(int begin, int end) {
    return PIPE([begin, end](auto container) {
        return __SLICE(begin, end)(container);
    });
};

// -------------------------------------------------

// Retorna um vetor com o resultado da aplicação da função fn para cada elemento do container
// Exemplo:
// map(std::vector<int>{1, 2, 3, 4, 5}, [](auto x) {return x * 2;}) | WRITE(); // [2, 4, 6, 8, 10]
// map(std::vector<int>{1, 2, 3, 4, 5}, FNT(x, x/2.0)) | WRITE(); // [0.5, 1, 1.5, 2, 2.5]
// map(range(26), FNT(x, (char) (x + 'a'))) | JOIN() | WRITE(); // abcdefghijklmnopqrstuvwxyz
template<typename CONTAINER, typename FUNCTION>
auto map(CONTAINER container, FUNCTION fn) {
    std::vector<decltype(fn(*container.begin()))> aux;
    std::transform(container.begin(), container.end(), std::back_inserter(aux), fn);
    return aux;
}

// Retorna um vetor com o resultado da aplicação da função fn para cada elemento do container
// Exemplo:
// std::vector<int>{1, 2, 3, 4, 5} | MAP([](auto x) {return x * 2;}) | WRITE(); // [2, 4, 6, 8, 10]
// std::vector<int>{1, 2, 3, 4, 5} | MAP(FNT(x, x/2.0)) | WRITE(); // [0.5, 1, 1.5, 2, 2.5]
// range(26) | MAP(FNT(x, (char) (x + 'a'))) | JOIN() | WRITE(); // abcdefghijklmnopqrstuvwxyz
template <typename FUNCTION>
auto MAP(FUNCTION fn) {
    return PIPE([fn](auto container) {
        return map(container, fn);
    });
};

//-------------------------------------------------

// Retorna um vetor com os elementos do container que satisfazem a função predicado fn
// Exemplo:
// filter(std::vector<int>{1, 2, 3, 4, 5}, [](auto x) {return x % 2 == 0;}) | WRITE(); // [2, 4]
// filter(std::vector<int>{1, 2, 3, 4, 5}, FNT(x, x % 2 == 1)) | WRITE(); // [1, 3, 5]
template<typename CONTAINER, typename FUNCTION>
auto filter(CONTAINER container, FUNCTION fn) {
    auto aux = slice(container, 0, 0);
    for(auto& x : container) {
        if(fn(x))
            aux.push_back(x);
    }
    return aux;
}

// Retorna um vetor com os elementos do container que satisfazem a função  predicado fn
// Exemplo:
// std::vector<int>{1, 2, 3, 4, 5} | FILTER([](auto x) {return x % 2 == 0;}) | WRITE(); // [2, 4]
// std::vector<int>{1, 2, 3, 4, 5} | FILTER(FNT(x, x % 2 == 1)) | WRITE(); // [1, 3, 5]
template <typename FUNCTION>
auto FILTER(FUNCTION fn) {
    return PIPE([fn](auto container) {
        return filter(container, fn);
    });
};

//-------------------------------------------------

// Transforma de string para o tipo solicitado utilizando o operador de extração do stream
// Dispara uma exceção caso a conversão não seja possível
// Exemplo:
// int x = strto<int>("123") | WRITE(); // 123
// double y = strto<double>("3.14") | WRITE(); // 3.14
template <typename READABLE>
READABLE strto(std::string value) {
    std::istringstream iss(value);
    READABLE aux;
    if (iss >> aux) {
        return aux;
    }
    throw std::runtime_error("strto: invalid conversion from " + value);
}

// Transforma de string para o tipo solicitado utilizando o operador de extração do stream
// Dispara uma exceção caso a conversão não seja possível
// Exemplo:
// int x = "123"s | STRTO<int>() | WRITE(); // 123
// double y = "3.14"s | STRTO<double>() | WRITE(); // 3.14
template <typename READABLE>
auto STRTO() {
    return PIPE([](std::string value) {
        return strto<READABLE>(value);
    });
};

//-------------------------------------------------

// Transforma de string para double utilizando a função strto
// double x {number("3.14")};
inline double number(std::string value) {
    return strto<double>(value);
}

// Transforma de string para double utilizando a função strto
// double x {"3.14"s | NUMBER()};
inline auto NUMBER() {
    return PIPE([](std::string value) {
        return number(value);
    });
};

//-------------------------------------------------
template <typename... Types>
struct __UNPACK {
    char delimiter;
    __UNPACK(char delimiter) : delimiter(delimiter) {}

    template<typename Head, typename... Tail>
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

};

// Transforma de string para tupla dados os tipos e o char separador
// unpack<int, double, std::string>("1:2.4:uva", ':') | WRITE(); // (1, 2.4, "uva")
template <typename... TS>
std::tuple<TS...> unpack(const std::string& line, char delimiter) {
    return __UNPACK<TS...>(delimiter)(line);
}

// Transforma de string para tupla dados os tipos e o char separador
// "1:2.4:uva"s | UNPACK<int, double, std::string>(':') | WRITE(); // (1, 2.4, "uva")
template <typename... TS>
auto UNPACK(char delimiter) {
    return PIPE([delimiter](std::string line) {
        return __UNPACK<TS...>(delimiter)(line);
    });
};

//-------------------------------------------------

// Une dois containers em um único container de pares limitado ao menor tamanho dos dois containers
// zip(vector<int>{1, 2, 3}, string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]
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
// Une dois containers em um único container de pares limitado ao menor tamanho dos dois containers
// vector<int>{1, 2, 3} | ZIP(string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]
template<typename CONTAINER_B>
auto ZIP(CONTAINER_B B) {
    return PIPE([B](auto A) {
        return zip(A, B);
    });
};

//-------------------------------------------------

// Une dois containers em um único container de pares limitado ao menor tamanho dos dois containers
// colocando o resultado da função fnjoin em cada par no container de saída
// Exemplo:
//zipwith(range(10), "pterodactilo"s, FNT2(x, y, tostr(x) + y)) | WRITE(); // ["0p", "1t", "2e", "3r", "4o", "5d", "6a", "7c", "8t", "9i"]
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

// Une dois containers em um único container de pares limitado ao menor tamanho dos dois containers
// colocando o resultado da função fnjoin em cada par no container de saída
// Exemplo:
// range(10) | ZIPWITH("pterodactilo"s, FNT2(x, y, tostr(x) + y)) | WRITE(); // ["0p", "1t", "2e", "3r", "4o", "5d", "6a", "7c", "8t", "9i"]
template<typename CONTAINER_B, typename FNJOIN>
auto ZIPWITH(CONTAINER_B B, FNJOIN fnjoin) {
    return PIPE([B, fnjoin](auto A) {
        return zipwith(A, B, fnjoin);
    });
};

//-------------------------------------------------

template<typename... Args>
class __FORMAT 
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

    __FORMAT(Args ...args): args(std::forward<Args>(args)...)
    {
    }

    std::string operator()(std::string fmt)
    {
        auto [texts, controls] = extract(fmt);
        auto vars = tuple_to_vector_str(controls);
        while(vars.size() < texts.size())
            vars.push_back("");
        return __STRAUX().join("", texts | ZIPWITH(vars, [](auto x, auto y) { return x + y;}), "", ""); 
    }
};

// Formata uma string com base nos argumentos passados utilizando um modelo de chaves para posicionar os argumentos.
// Se dentro da chave, houver um string de formatação, o dado será formatado com base nela.
// Containers são formatados de acordo com a função TOSTR
// Exemplo:
// format("O {} é {0.2f} e o {} é {0.2f}", "pi", 3.141592653, "e", 2.7182818) | WRITE(); // O pi é 3.14 e o e é 2.72
// format("Meu vetor é {}", range(10)) | WRITE(); // Meu vetor é [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
// format("Vetor com duas casas {%.2f}", std::vector<double>{1.1321, 2, 3.3}) | WRITE(); // Vetor com duas casas [1.13, 2.00, 3.30]
// format("Alinhado a esquerda 10 casas [{%-10s}]", "abacate") | WRITE(); // Alinhado a esquerda 10 casas [abacate   ]
// format("Alinhado a direita  10 casas [{%10s}]", "abacate") | WRITE(); // Alinhado a esquerda 10 casas [abacate   ]
template<typename... Args>
std::string format(std::string fmt, Args ...args) {
    return __FORMAT<Args...>(args...)(fmt);
}

// Formata uma string com base nos argumentos passados utilizando um modelo de chaves para posicionar os argumentos.
// Se dentro da chave, houver um string de formatação, o dado será formatado com base nela.
// Containers são formatados de acordo com a função TOSTR
// Exemplo:
// "O {} é {0.2f} e o {} é {0.2f}"s | FORMAT("pi", 3.141592653, "e", 2.7182818) | WRITE(); // O pi é 3.14 e o e é 2.72
// "Meu vetor é {}" | FORMAT(range(10)) | WRITE(); // Meu vetor é [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
// "Vetor com duas casas {%.2f}" | FORMAT(std::vector<double>{1.1321, 2, 3.3}) | WRITE(); // Vetor com duas casas [1.13, 2.00, 3.30]
// "Alinhado a esquerda 10 casas [{%-10s}]" | FORMAT("abacate") | WRITE(); // Alinhado a esquerda 10 casas [abacate   ]
// "Alinhado a direita  10 casas [{%10s}]" | FORMAT("abacate") | WRITE(); // Alinhado a esquerda 10 casas [abacate   ]
template<typename... Args>
auto FORMAT(Args ...args) {
    return PIPE([args...](std::string fmt) {
        return __FORMAT<Args...>(args...)(fmt);
    });
};

//-------------------------------------------------

// Gera um vetor de inteiros de init até end, mas não incluindo end, com passo step
// Exemplo:
// range(0, 10) | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
// range(0, 10, 2) | WRITE(); // [0, 2, 4, 6, 8]
// range(10, 0, -1) | WRITE(); // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
inline std::vector<int> range(int init, int end, int step = 1) {
    if (step == 0)
        throw std::runtime_error("step cannot be zero");
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

// Gera um vetor de inteiros de init até end, mas não incluindo end, com passo step
// Exemplo:
// 0 | RANGE(10) | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
// 0 | RANGE(10, 2) | WRITE(); // [0, 2, 4, 6, 8]
// 10 |RANGE(0, -1) | WRITE(); // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
inline auto RANGE(int end, int step = 1) {
    return PIPE([end, step](int init) {
        return range(init, end, step);
    });
};

// Gera um vetor de inteiros de 0 até end, mas não incluindo end, com passo 1
// Exemplo:
// range(10) | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
inline std::vector<int> range(int end) {
    return range(0, end, 1);
}

// Gera um vetor de inteiros de 0 até end, mas não incluindo end, com passo 1
// Exemplo:
// 10 | RANGE() | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
inline auto RANGE() {
    return PIPE([](int end) {
        return range(0, end, 1);
    });
};

//-------------------------------------------------

// Transforma em string utilizando a função TOSTR e envia para o std::cout
// Retorna o dado original
// O end padrão é o caractere de nova linha
// Exemplo:
// write(std::vector<int>{1, 2, 3}); // [1, 2, 3]
// int x = write(10); // 10

template <typename PRINTABLE>
PRINTABLE write(PRINTABLE data, std::string end = "\n") {
    std::cout << tostr(data) << end;
    return data;
}

// Transforma em string utilizando a função TOSTR e envia para o std::cout
// Retorna o dado original
// O end padrão é o caractere de nova linha
// Exemplo:
// std::vector<int>{1, 2, 3} | WRITE(); // [1, 2, 3]
// int x = 10 | WRITE(); // 10
inline auto WRITE(std::string end = "\n") {
    return PIPE([end](auto data) {
        return write(data, end);
    });
}

//-------------------------------------------------

// Transforma uma string em um vetor de strings, separando pelo delimitador
// Retorna o vetor de strings
// Exemplo:
// split("a,b,c", ',') | WRITE(); // [a, b, c]
// split("eu gosto de comer banana", ' ') | WRITE(); // [eu, gosto, de, comer, banana]")
// split("eu gosto de comer    banana") | WRITE(); // [eu, gosto, de, comer, , , , banana]")
inline std::vector<std::string> split(std::string content, char delimiter = ' ') {
    std::vector<std::string> aux;
    std::string token;
    std::istringstream tokenStream(content);
    while (std::getline(tokenStream, token, delimiter))
        aux.push_back(token);
    return aux;
}

// Transforma uma string em um vetor de strings, separando pelo delimitador
// Retorna o vetor de strings
// Exemplo:
// "a,b,c" | SPLIT(',') | WRITE(); // [a, b, c]
// "eu gosto de comer banana" | SPLIT(' ') | WRITE(); // [eu, gosto, de, comer, banana]")
// "eu gosto de comer    banana", ' ') | SPLIT() | WRITE(); // [eu, gosto, de, comer, , , , banana]")
inline auto SPLIT(char delimiter = ' ') {
    return PIPE([delimiter](std::string content) {
        return split(content, delimiter);
    });
};


//-------------------------------------------------
// Extrai uma linha e retorna como string
// O padrão é o std::cin, mas pode ser um fluxo ou arquivo
// Se não houver mais linhas, lança uma exceção
// Exemplo:
// auto line = input(); // lê uma linha do std::cin
inline std::string input(std::istream & is = std::cin) {
    std::string line;
    if (std::getline(is, line))
        return line;
    throw std::runtime_error("input empty");
}

// Extrai uma linha e retorna como string
// O padrão é o std::cin, mas pode ser um fluxo ou arquivo
// Se não houver mais linhas, lança uma exceção
// Exemplo:
// auto line = std::cin | INPUT(); // lê uma linha do std::cin
struct INPUT {
    friend std::string operator|(std::istream& is, INPUT) {
        return input(is);
    }
};

} // namespace fn

using namespace std::string_literals;

// Transforma uma string em um double utilizando a função STRTO
inline double operator+(std::string text) {
    return fn::strto<double>(text);
}
