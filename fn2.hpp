#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace fn {

using str_view = std::string_view;

//--------------------------------------------------------
//-------------------- ALIGN -------------- --------------
//--------------------------------------------------------

class Align {
    char align_mode { 0 }; //< > or ^, default is center
    int  align_size { 0 }; //size of the field
    char pad_char   { ' ' }; //char used to pad the field
    std::string format { "" }; //filtered format string

    bool extract_align() {
        std::string align_str_size;

        size_t i {0}, j {0};
        for (i = 0; i < format.size(); ++i) {
            if (format[i] == '<' || format[i] == '>' || format[i] == '^') { //achei o alinhamento
                this->align_mode = format[i];

                for (j = i + 1; j < format.size(); ++j) { //extraindo o tamanho do alinhamento
                    if (format[j] >= '0' && format[j] <= '9') {
                        align_str_size += format[j];
                    } else { // terminou o tamanho
                        break;
                    }
                }
                if (align_str_size.empty()) {
                    this->align_size = 0;
                    std::cout << "fail: format symbol `" << format[i] << "` must be followed by a size\n";
                    exit(1);
                }
                this->align_size = std::stoi(align_str_size);
                this->format.erase(i, j - i);
                return true;
            }
        }
        return false;
    }

    void extract_pad() {
        //search for : char in format, if exists and is followed by a char, then use that char as padding, and remove both from string
        auto pos = this->format.find(':');
        if (pos != std::string::npos) {
            if (pos + 1 < format.size()) {
                this->pad_char = format[pos + 1];
                this->format.erase(pos, 2);
            } else {
                std::cout << "fail: format symbol `:` must be followed by a padding char\n";
                exit(1);
            }
        }
    }
public:

    Align(str_view format) {
        this->format = format;
        this->extract_pad();
        this->extract_align();
    }

    std::string align_text(const std::string& str) {
        int len = str.length();
        if(this->align_mode == 0 || this->align_size < len) { 
            return str; 
        }
        int diff = this->align_size - len;
        
        //default is center
        int padleft = diff/2;
        int padright = diff - padleft;
        if(this->align_mode == '>') {
            padleft = diff;
            padright = 0;
        }
        else if(this->align_mode == '<') {
            padleft = 0;
            padright = diff;
        }
        return std::string(padleft, this->pad_char) + str + std::string(padright, this->pad_char);
    }

    const std::string& get_filtered_format() {
        return this->format;
    }
    int get_align_size() {
        return this->align_size;
    }
    char get_pad_char() {
        return this->pad_char;
    }
    char get_align_mode() {
        return this->align_mode;
    }
};

//--------------------------------------------------------
//-------------------- CFMT  -----------------------------
//--------------------------------------------------------

class CFMT {

    //transformation using sprintf
    template <typename T>
    static std::string c_transform(const T& data, const str_view& format) {
        std::string fmt(format);
        auto size = std::snprintf(nullptr, 0, fmt.c_str(), data);
        std::string output(size + 1, '\0');
        std::sprintf(&output[0], fmt.c_str(), data);
        if (output.back() == '\0') 
            output.pop_back();
        return output;
    }

    //conversion to string using stringstream
    template <typename T>
    static std::string sstream_transform(const T& data) {
        std::stringstream ss;
        ss << data;
        return ss.str();
    }


    template <typename T>
    static std::string process(const T& data, const str_view& format) 
    {
        if (format == "%s" || format == "") {
            return sstream_transform(data);
        }
        if (format.size() > 0 && format.find("%s") != std::string::npos) {//formatting a non string with %s
            return process(sstream_transform(data), format);
        }
        return c_transform(data, format);
    }

    //validate if the format is correct for a string
    static std::string process(const std::string& data, const str_view& format) 
    {
        return process(data.c_str(), format);
    }
    
    //validate if the format is correct for a const char *
    //write specialization for const char *
    static std::string process(const char* const& data, const str_view& format) 
    {
        if (format == "%s" || format == "") {
            return data;
        }
        return CFMT::c_transform(data, format);
    };

    
public:

    template <typename T>
    static std::string format(const T& data, const str_view& format) 
    {
        Align align(format);
        std::string filtered = align.get_filtered_format();
        return align.align_text(process(data, filtered));
    }
};

//--------------------------------------------------------
//-------------------- TOSTR PROTOTYPE -------------------
//--------------------------------------------------------

template <typename T> std::string tostr(const T& t     , const str_view& format = "");

//--------------------------------------------------------
//-------------------- JOIN  -----------------------------
//--------------------------------------------------------

namespace hide {
template <typename CONTAINER> 
std::string __join(const CONTAINER& container, const str_view& separator, const str_view& cfmt) 
{ 
    std::stringstream ss;
    for (auto it = container.begin(); it != container.end(); ++it) {
        ss << (it == container.begin() ? "" : separator);
        ss << tostr(*it, cfmt);
    }
    return ss.str();
}

template <typename... Ts>
std::string __join(std::tuple<Ts...> const &the_tuple, const str_view& separator, const str_view& cfmt)
{
    std::stringstream ss;
    std::apply( [&](Ts const &...tuple_args) {
            std::size_t n{0};
            ((ss << tostr(tuple_args, cfmt) << (++n != sizeof...(Ts) ? separator : "")), ...);
        }, the_tuple);
    return ss.str();
}

template <typename T1, typename T2>
std::string __join(const std::pair<T1, T2>& the_pair, const str_view& separator, const str_view& cfmt)
{
    std::stringstream ss;
    ss << tostr(the_pair.first, cfmt) << separator << tostr(the_pair.second, cfmt);
    return ss.str();
}
}
//__guide join
//[[join]]
template <typename T>
std::string join(const T& container, const str_view& separator = "", const str_view& cfmt = "") 
//[[join]]
{
    return hide::__join(container, separator, cfmt);
}

//class
struct JOIN {
    str_view delimiter;
    str_view cfmt;

//__guide join
    JOIN(const str_view& delimiter = "", const str_view& cfmt = "") : delimiter(delimiter), cfmt(cfmt) {}
    template <typename CONTAINER> std::string operator()(const CONTAINER& v) const { return join(v, delimiter, cfmt); }
    template <typename T> friend std::string operator|(const T& v, const JOIN& obj) { return obj(v); }
};

//--------------------------------------------------------
//-------------------- TOSTR -----------------------------
//--------------------------------------------------------

namespace hide {
template <typename T>             inline std::string __tostr(const T& t                      , const str_view& format) { return CFMT::format(t, format); }
                                  inline std::string __tostr(int i                           , const str_view& format) { return CFMT::format(i, format); }
                                  inline std::string __tostr(bool b                          , const str_view& format) { (void) format; return b ? "true" : "false"; }
                                  inline std::string __tostr(const char* s                   , const str_view& format) { return CFMT::format(s, format); }
                                  inline std::string __tostr(const std::string& s            , const str_view& format) { return CFMT::format(s, format); }
                                  inline std::string __tostr(const str_view& s               , const str_view& format) { return CFMT::format(s, format); }
template <typename A, typename B> inline std::string __tostr(const std::pair<A,B>& p         , const str_view& format) { return "(" + tostr(p.first, format) + ", " + tostr(p.second, format) + ")"; }
template <typename T>             inline std::string __tostr(const std::list<T>& t           , const str_view& format) { return "[" + join(t, ", ", format) + "]"; }
template <typename T>             inline std::string __tostr(const std::vector<T>& t         , const str_view& format) { return "[" + join(t, ", ", format) + "]"; }
template <typename ...Ts>         inline std::string __tostr(const std::tuple<Ts...>& t      , const str_view& format) { return "(" + join(t, ", ", format) + ")"; }
template <typename T, size_t N>   inline std::string __tostr(const std::array<T, N>& t       , const str_view& format) { return "[" + join(t, ", ", format) + "]"; }
template <typename T>             inline std::string __tostr(const std::set<T>& t            , const str_view& format) { return "{" + join(t, ", ", format) + "}"; }
template <typename K, typename T> inline std::string __tostr(const std::map<K,T>& t          , const str_view& format) { return "{" + join(t, ", ", format) + "}"; }
template <typename T>             inline std::string __tostr(const std::unordered_set<T>& t  , const str_view& format) { return "{" + join(t, ", ", format) + "}"; }
template <typename K, typename T> inline std::string __tostr(const std::unordered_map<K,T>& t, const str_view& format) { return "{" + join(t, ", ", format) + "}"; }
template <typename T>             inline std::string __tostr(const std::shared_ptr<T>& t     , const str_view& format) { return t == nullptr ? "null" : tostr(*t, format); }

}

//__guide tostr
//[[tostr]]
template <typename T> std::string tostr(const T& data     , const str_view& cfmt) 
//[[tostr]]
{ 
    return hide::__tostr(data, cfmt); 
}

//class
struct TOSTR {
    str_view cfmt;

//__guide tostr
    TOSTR(const str_view& cfmt = "") : cfmt(cfmt) {}

    template <typename T> std::string operator()(const T& t) const { return tostr(t, cfmt); }
    template <typename T> friend std::string operator|(const T& v, const TOSTR& obj) { return obj(v); }
};

//--------------------------------------------------------
//-------------------- FORMAT ----------------------------
//--------------------------------------------------------

//class
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
                ((result.push_back(tostr(tupleArgs, controls.at(++i)))), ...);
            }, this->args
        );

        return result;
    }

    //transforma {{ em \a e }} em \b
    std::string preprocess(std::string data) {
        std::string result1;
        data.push_back('_');
        for (int i = 0; i < (int) data.size() - 1; i++) {
            if (data[i] == '{' && data[i + 1] == '{') {
                result1.push_back('\a');
                i++;
            } else {
                result1.push_back(data[i]);
            }
        }
        std::string result2;
        for (int i = result1.size() - 1; i >= 0; i--) {
            if (result1[i] == '}' && result1[i - 1] == '}') {
                result2.push_back('\b');
                i--;
            } else {
                result2.push_back(result1[i]);
            }
        }
        std::string final_result;
        for (int i = result2.size() - 1; i >= 0; i--) {
            final_result.push_back(result2[i]);
        }
        return final_result;
    }

    std::pair<std::vector<std::string>, std::vector<std::string>> extract(std::string data)
    {
        data = preprocess(data);
        std::vector<std::string> texts = {""};
        std::vector<std::string> ctrls = {""};
        auto* destiny = &texts;

        for (char c : data) {
            if (c == '{') 
            {
                texts.push_back("");
                destiny = &ctrls;
            }
            else if (c == '}') 
            {
                ctrls.push_back("");
                destiny = &texts;
            }
            else
            {
                if (c == '\a') {
                    c = '{';
                } else if (c == '\b') {
                    c = '}';
                }
                destiny->back().append(std::string(1,c));
            }
        }
        while (texts.size() > ctrls.size())
            ctrls.push_back("");
        return {texts, ctrls};
    }

public:

//__guide format
    FORMAT(Args ...args): args(std::forward<Args>(args)...){}

    std::string operator()(std::string fmt)
    {
        auto [texts, controls] = extract(fmt);
        try {
            auto vars = tuple_to_vector_str(controls);
            
            if(vars.size() < texts.size() - 1) {
                throw std::out_of_range("");
            }
            std::stringstream ss;
            for (size_t i = 0; i < vars.size(); i++)
                ss << texts[i] << vars[i];
            ss << texts.back(); //ultimo texto
            return ss.str();
        } catch (std::out_of_range& e) {
            std::cout << "fail: verifique a quantidade de parâmetros passado para string: " << fmt << '\n';
            exit(1);
        }
    }

    friend std::string operator|(std::string fmt, FORMAT<Args...> obj) { return obj(fmt); }
};


//__guide format
//[[format]]
template<typename... Args> 
std::string format(std::string fmt, Args ...args) 
//[[format]]
{
    return FORMAT<Args...>(args...)(fmt); 
}


//--------------------------------------------------------
//-------------------- PRINT------------------------------
//--------------------------------------------------------

//__guide print
//[[print]]
template<typename... Args> std::string print(std::string fmt, Args ...args)
//[[print]]
{ 
    auto result = FORMAT<Args...>(args...)(fmt);
    std::cout << result;
    return result;
}
//class
template<typename... Args> 
struct PRINT {
    std::tuple<Args...> args;

//__guide print
    PRINT(Args ...args): args(std::forward<Args>(args)...) { }
    std::string operator()(std::string fmt) { return print(fmt, args); }
    friend std::string operator|(std::string fmt, PRINT<Args...> obj) { return obj(fmt); }
};

//--------------------------------------------------------
//-------------------- WRITE -----------------------------
//--------------------------------------------------------

//__guide write
//[[write]]
template <typename PRINTABLE> const PRINTABLE& write(const PRINTABLE& data, str_view end = "\n") 
//[[write]]
{
    std::cout << tostr(data) << end;
    return data;
}

//class
struct WRITE {
    str_view end;

//__guide write
    WRITE(str_view end = "\n"): end(end) { }

    template <typename PRINTABLE>        const PRINTABLE& operator()(const PRINTABLE& data           ) { return write(data, end); }
    template <typename PRINTABLE> friend const PRINTABLE& operator| (const PRINTABLE& data, WRITE obj) { return obj(data); }
};

//--------------------------------------------------------
//-------------------- SLICE -----------------------------
//--------------------------------------------------------

class SLICE {
public:
//__guide slice1
    SLICE(int begin = 0) {
        this->from_begin = begin == 0;
        this->begin = begin;
        this->to_end = true;
    }

//__guide slice2
    SLICE(int begin, int end) {
        this->begin = begin;
        this->end = end;
        this->from_begin = false;
        this->to_end = false;
    }
    template<typename CONTAINER>
    auto operator()(const CONTAINER& container) const {
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
    
    template<typename CONTAINER> friend auto operator|(const CONTAINER& container, const SLICE& obj) { return obj(container); }

private:
    int begin;
    int end;
    bool from_begin {false};
    bool to_end {false};

    template<typename CONTAINER>
    static auto new_vec_from(const CONTAINER& container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<decltype(fn(*container.begin()))> aux;
        return aux;
    }

    template <typename K, typename T>
    static auto new_vec_from(const std::map<K, T>& container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }

    template <typename K, typename T>
    static auto new_vec_from(const std::unordered_map<K, T>& container) {
        auto fn = [](auto x) {return x;}; 
        std::vector<std::pair<decltype(fn(container.begin()->first)), decltype(fn(container.begin()->second))>> aux;
        return aux;
    }
};

//__guide slice1
//[[slice]]
template<typename CONTAINER>
auto slice(const CONTAINER& container, int begin = 0)
//[[slice]]
{
    return SLICE(begin)(container);
}

//__guide slice2
template<typename CONTAINER>
auto slice(CONTAINER container, int begin, int end)
{
    return SLICE(begin, end)(container);
}

//--------------------------------------------------------
//-------------------- MAP   -----------------------------
//--------------------------------------------------------

//__guide map
//[[map]]
template<typename CONTAINER, typename FUNCTION>
auto map(const CONTAINER& container, FUNCTION fn)
//[[map]]
{
    std::vector<decltype(fn(*container.begin()))> aux;
    for (const auto& item : container)
        aux.push_back(fn(item));
    return aux;
}

template <typename FUNCTION>
struct MAP {
    FUNCTION fn;
//__guide map
    MAP(FUNCTION fn) : fn(fn) {};
    template<typename CONTAINER> auto operator()(const CONTAINER& container) const { return map(container, fn); }
    template<typename CONTAINER> friend auto operator|(const CONTAINER& container, const MAP& map) { return map(container); }
};

//--------------------------------------------------------
//-------------------- FILTER ----------------------------
//--------------------------------------------------------

//__guide filter
//[[filter]]
template<typename CONTAINER, typename FUNCTION>
auto filter(const CONTAINER& container, FUNCTION fn)
//[[filter]]
{
    auto aux = slice(container, 0, 0);
    for(const auto& x : container) {
        if(fn(x))
            aux.push_back(x);
    }
    return aux;
}

template <typename FUNCTION>
struct FILTER {
    FUNCTION fn;

//__guide filter
    FILTER(FUNCTION fn) : fn(fn) {};
    template<typename CONTAINER> auto operator()(const CONTAINER& container) const { return filter(container, fn); }
    template<typename CONTAINER> friend auto operator|(const CONTAINER& container, const FILTER& obj) { return obj(container); }
};

//--------------------------------------------------------
//-------------------- RANGE -----------------------------
//--------------------------------------------------------

//__guide range2
// [[range]]
inline std::vector<int> range(int init, int end, int step = 1)
//[[range]]
{
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

//__guide range1
inline std::vector<int> range(int end) {
    return range(0, end, 1);
}

struct RANGE {
    RANGE() : init(0), step(1) {};

    std::vector<int> operator()(int end) const {
        return range(init, end, step);
    }

    friend std::vector<int> operator|(int end, const RANGE& obj) {
        return obj(end);
    }

    int init {0};
    int end {0};
    int step {0};
};

//--------------------------------------------------------
//-------------------- ENUMERATE -------------------------
//--------------------------------------------------------

//__guide enumerate
//[[enumerate]]
template<typename CONTAINER>
auto enumerate(const CONTAINER& container)
//[[enumerate]]
{
    auto fn = [](auto x) {return x;}; 
    std::vector<std::pair<int, decltype(fn(*container.begin()))>> aux;
    int i = 0;
    for (const auto& item : container) {
        aux.push_back(std::make_pair(i, item));
        i++;
    }
    return aux;
}

struct ENUMERATE {
//__guide enumerate
    ENUMERATE() {};
    template<typename CONTAINER> auto operator()(const CONTAINER& container) const { return enumerate(container); }
    template<typename CONTAINER> friend auto operator|(const CONTAINER& container, const ENUMERATE& obj) { return obj(container); }
};



//--------------------------------------------------------
//-------------------- STRTO -----------------------------
//--------------------------------------------------------

//__guide strto
//[[strto]]
template <typename READABLE>
READABLE strto(std::string value)
//[[strto]]
{
    std::istringstream iss(value);
    READABLE aux;
    if (iss >> aux) {
        return aux;
    }
    throw std::runtime_error("strto: invalid conversion from " + value);
}

template <typename READABLE>
struct STRTO {
//__guide strto
    STRTO(){};
    READABLE operator()(std::string value) const { return strto<READABLE>(value); }
    friend READABLE operator|(std::string value, const STRTO& obj) { return obj(value); }
};

//--------------------------------------------------------
//-------------------- NUMBER ----------------------------
//--------------------------------------------------------

//__guide number
//[[number]]
inline double number(std::string value)
//[[number]]
{
    return strto<double>(value);
}

struct NUMBER {
//__guide number
    NUMBER(){}
    double operator()(std::string value) const { return number(value); }
    friend double operator|(std::string value, const NUMBER& obj) { return obj(value); }
};

//--------------------------------------------------------
//-------------------- UNPACK -----------------------------
//--------------------------------------------------------

template <typename... Types>
struct UNPACK {
    char delimiter;

//__guide unpack
    UNPACK(char delimiter) : delimiter(delimiter) {}

    template<typename Head, typename... Tail>
    std::tuple<Head, Tail...> tuple_read_impl(std::istream& is, char delimiter) const {
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

    std::tuple<Types...> operator()(std::string content) const {
        std::stringstream ss(content);
        return tuple_read_impl<Types...>(ss, this->delimiter);
    }
    
    friend std::tuple<Types...> operator|(std::string content, const UNPACK& obj) {
        return obj(content);
    }
};

//__guide unpack
//[[unpack]]
template <typename... TS>
std::tuple<TS...> unpack(const std::string& line, char delimiter)
//[[unpack]]
{
    return UNPACK<TS...>(delimiter)(line);
}


//--------------------------------------------------------
//-------------------- ZIP   -----------------------------
//--------------------------------------------------------

//__guide zip
//[[zip]]
template<typename CONTAINER_A, typename CONTAINER_B>
auto zip(const CONTAINER_A& A, const CONTAINER_B& B)
//[[zip]]
{
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

template <typename CONTAINER_B>
struct ZIP {
    CONTAINER_B container_b;

//__guide zip
    ZIP(const CONTAINER_B& container_b) : container_b(container_b) {}

    template<typename CONTAINER_A>
    auto operator()(const CONTAINER_A& container_a) const { return zip(container_a, container_b); }
    template<typename CONTAINER_A>
    friend auto operator|(const CONTAINER_A& container_a, const ZIP& obj) { return obj(container_a); }
};

//--------------------------------------------------------
//-------------------- ZIPWITH ---------------------------
//--------------------------------------------------------

//__guide zipwith
//[[zipwith]]
template<typename CONTAINER_A, typename CONTAINER_B, typename FNJOIN>
auto zipwith(const CONTAINER_A& A, const CONTAINER_B& B, FNJOIN fnjoin)
//[[zipwith]]
{
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


template<typename CONTAINER_B, typename FNJOIN>
struct ZIPWITH {
    CONTAINER_B container_b;
    FNJOIN fnjoin;

//__guide zipwith
    ZIPWITH(const CONTAINER_B& container_b, FNJOIN fnjoin) : container_b(container_b), fnjoin(fnjoin) {}

    template<typename CONTAINER_A>        auto operator()(const CONTAINER_A& container_a) const { return zipwith(container_a, container_b, fnjoin); }
    template<typename CONTAINER_A> friend auto operator| (const CONTAINER_A& container_a, const ZIPWITH& obj) { return obj(container_a); }
};

//--------------------------------------------------------
//-------------------- SPLIT -----------------------------
//--------------------------------------------------------

//__guide split
//[[split]]
inline std::vector<std::string> split(std::string content, char delimiter = ' ')
//[[split]]
{
    std::vector<std::string> aux;
    std::string token;
    std::istringstream tokenStream(content);
    while (std::getline(tokenStream, token, delimiter))
        aux.push_back(token);
    return aux;
}

struct SPLIT {
    char delimiter;

//__guide split
    SPLIT(char delimiter = ' ') : delimiter(delimiter) {}

    std::vector<std::string>        operator()(std::string content) const { return split(content, delimiter); }
    friend std::vector<std::string> operator| (std::string content, const SPLIT& obj) { return obj(content); }
};

//--------------------------------------------------------
//-------------------- INPUT -----------------------------
//--------------------------------------------------------

//__guide input
//[[input]]
inline std::string input(std::istream & is = std::cin)
//[[input]]
{
    std::string line;
    if (std::getline(is, line))
        return line;
    throw std::runtime_error("input empty");
}

struct INPUT {
//__guide input
    INPUT() {}

    std::string        operator()(std::istream& is = std::cin) const { return input(is); }
    friend std::string operator| (std::istream& is, const INPUT& obj) { return obj(is); }
};

} // namespace fn

using namespace std::string_literals;

//__guide operator+
inline double operator+(std::string text) {
    return fn::number(text);
}
