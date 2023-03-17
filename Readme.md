# FAKEFN for c++

[](toc)

- [Duas opções de funções](#duas-opções-de-funções)
- [Resumo das funções](#resumo-das-funções)
  - [String](#string)
  - [PIPE](#pipe)
  - [TOSTR](#tostr)
  - [JOIN](#join)
  - [WRITE](#write)
  - [RANGE](#range)
  - [VERIFY](#verify)
  - [FNT](#fnt)
  - [SLICE](#slice)
  - [FILTER](#filter)
[](toc)

## Duas opções de funções

- Todas as funções estão no namespace fn. Para utilizar, basta incluir o arquivo fn.hpp e usar o namespace fn.
- Todas as funções da biblioteca estão disponibilizadas em dois formatos.
  - Formato de função normal, escrita em minúsculo, com parênteses e argumentos.
  - Formato de função em pipeline, escrita em maiúsculo, onde o primeiro argumento da função é passado pelo pipe.
- Exemplo:
  - A função write imprime o valor passado por parâmetro e retorna o valor.
  - Ela tem um parâmetro opcional, que é o fim da linha, que por default é "\n".
  - Modo função: `write(data, end)`.
  - Modo pipeline: `data | WRITE(end)`.

[](load)[](tests/duas.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;
int main() {
    write("Hello World");
    "Hello World" | WRITE();

    int a = write(5, " :a\n");  //imprime o "5:a" e retorna o 5
    int b = 5 | WRITE(" :b\n"); //imprime o "5:b" e retorna o 5
    (void) a; //só pra não reclamar que não estamos utilizando o a
    (void) b; //só pra não reclamar que não estamos utilizando o b
}
```

[](load)[](tests/modopipe.cpp)[](fenced=cpp)

## Resumo das funções

```py

//----------------------------------------------------------
//                       PIPE 
//----------------------------------------------------------
PIPE(FN)         Cria uma functor para ser executado em pipeline
FNT(x, fn)       Função de transformação simples de um parâmetro
FNT2(x, y, fxy)  Função de transformação simples de dois parâmetros e uma transformação

//----------------------------------------------------------
//                       BASE 
//----------------------------------------------------------

write(data: DATA, end = "\n")        -> DATA   // imprime um conteúdo convertido pelo tostr
input(fluxo = cin)                   -> str    // lê linha inteira
range(end: int)                      -> [int]  // vetor de [0, end[, não inclui o end
range(init: int, end: int, step = 1) -> [int]  // vetor de [init, end[, não inclui o end
operator+(token: str)                -> double // transforma string em double
"nome"s                              -> str    // transforma raw string em string

//----------------------------------------------------------
//                   PARA STRINGS
//----------------------------------------------------------

tostr (data: DATA, cfmt = "")                   -> str   // converte qualquer coisa para string e formata
format(fmt: str, Args ...args)                  -> str   // formata uma string usando {} e printf
join  (container, separator = "", brakets = "") -> str   // concatena os elementos de um container 

//----------------------------------------------------------
//                  DE STRINGS
//----------------------------------------------------------

strto <TYPE> (value: str)                    -> TYPE                // dado tipo, converte string para esse tipo
unpack<TS...>(line: str, delimiter: char)    -> std::tuple<TS...>;  // dado tipos e delimitador, separa em uma tupla 
split        (content: str, delimiter = ' ') -> [str]               // dado um delimitador, separa em vetor de strings

//----------------------------------------------------------
//             filter, map, slice  
//----------------------------------------------------------

filter(container<a>, fn: (a -> bool))      -> [a]       // filtra os elementos que satisfazem a função
map   (container<a>, fn: (a -> b   ))      -> [b]       // aplica uma função em todos os elementos
slice (container<a>, begin = 0)            -> [a]       // copia de begin até o final
slice (container<a>, begin: int, end: int) -> [a];      // copia de begin até end

//----------------------------------------------------------
//             ZIP
//----------------------------------------------------------

zip    (cont<a>, cont<b>)              -> [(a,b)] // zipa dois containers em um cont de pares
zipwith(cont<a>, cont<b>, fn: (a,b->c) -> [c]         // zipa dois containers usando uma função

```

### String

No C++11, foi introduzido o suporte a string literals, que permite criar strings de forma mais simples e legível.
Esse modo já está habilitado na biblioteca, mas pode ser inserido manualmente utilizando `using namespace std::string_literals;` no seu código.

Outra função útil para manipulação de strings foi inspirada no operator + do javascript, que permite converter uma string em um número.

[](load)[](tests/str.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;
int main() {
    auto a = "aa";              //const char *
    auto b = std::string("bb"); //std::string
    auto c = "cc"s;             //std::string

    auto d = +"23.556"s;        //double
    auto e = (int) +"23.556"s;  //int (cast)

    std::cout << a <<  " " << b << " " << c << " " << d << " " << e << std::endl;
    //aa bb cc 23.556 23
}
```

[](load)

### PIPE

[](load)[](fn.hpp)[](fenced=cpp:extract=pipe)

```cpp
/**
 * @brief Functor para criação de funções Pipeline.
 * 
 * PIPE é um functor, ou seja, uma struct que após instanciada, funciona como uma função.
 * Ela é construída passando uma função que recebe um único parâmetro qualquer.
 * O PIPE então guarda essa função para que possa ser executada em pipeline ou invocada diretamente.
 * 
 * @param fn função a ser guardada
 * 
 * @warning 5 | PIPE([](int x){return x * 2;}) | WRITE(); // 10
 * @note https://github.com/senapk/cppaux/#pipe
 */
template<typename FUNCTION> 
struct PIPE
```

[](load)

[](load)[](tests/pipe.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;
int main() {
    auto fn = PIPE([](int x) { return 2 * x; });
    fn(1) | WRITE(); // 2
    fn(2) | WRITE(); // 4
    fn(fn(3)) | WRITE(); // 12
    5 | fn | WRITE(); // 10
    5 | fn | fn | fn | WRITE(); // 40
}
```

[](load)

### TOSTR

[](load)[](fn.hpp)[](fenced=cpp:extract=tostr)

```cpp
/**
 * Converte o dado passado em string.
 * 
 * Se passado o parâmetro de formatação cfmt, o dado será formatado usando o modelo do printf.
 * Se o dado for um container, o formato será aplicado em todos os elementos
 * do container recursivamente.
 * 
 * vectors, lists e arrays são impressos entre colchetes.
 * maps e sets são impressos entre chaves.
 * pairs e tuples são impressos entre parênteses.
 * shared_ptr e optional são impressos como o valor contido ou null.
 * 
 * Para classe do usuário ser impressa, ela deve sobrecarregar o ostream& operador <<.
 * 
 * @param data Dado a ser convertido
 * @param cfmt Parâmetro de formatação no modo printf
 * @return String com o dado convertido
 * @warning tostr(std::list<int>{1,2,3}, "%02d") | WRITE();
 * @note https://github.com/senapk/cppaux#tostr
 */
template<typename PRINTABLE>
std::string tostr(PRINTABLE data, std::string cfmt)
```

[](load)

[](load)[](tests/tostr.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include <memory>
#include <optional>
#include "fn.hpp"
using namespace fn;
int main() {

    // modo função
    tostr(5.6123, "%.2f")                     | VERIFY("5.61"s);
    tostr(std::vector<int> {1, 2, 3})         | VERIFY("[1, 2, 3]"s);
    tostr(std::pair<int, int> {1, 2}, "%03d") | VERIFY("(001, 002)"s);
    tostr("banana", "<%-8s>")                 | VERIFY("<banana  >"s);

    // números
    5.6123 | TOSTR("%.2f")     | VERIFY("5.61"s);
    5      | TOSTR("%02d")     | VERIFY("05"s);

    // alinhamento de string
    "banana"  | TOSTR("%-8s")  | VERIFY("banana  "s);
    "banana"  | TOSTR("%8s")   | VERIFY("  banana"s);

    // containers
    std::vector<int> {1, 2, 3}         | TOSTR() | VERIFY("[1, 2, 3]"s);
    std::list<int> {1, 2, 3}           | TOSTR() | VERIFY("[1, 2, 3]"s);
    std::array<int, 3> {1, 2, 3}       | TOSTR() | VERIFY("[1, 2, 3]"s);
    

    // pair
    std::pair<int, int> {1, 2}         | TOSTR() | VERIFY("(1, 2)"s);

    // tuples
    std::make_tuple(1, 5.42, "banana") | TOSTR() | VERIFY("(1, 5.42, banana)"s);

    // estruturas aninhadas
    std::make_pair(std::make_pair(4, "ovo"), "chiclete") | TOSTR() | VERIFY("((4, ovo), chiclete)"s);

    // formatação recursiva
    std::make_tuple(1, 2, 3) 
        | TOSTR("%03d") 
        | VERIFY("(001, 002, 003)"s, "str");

    std::make_pair(std::vector<int>{1,2,3}, std::vector<int>{4,5,6}) 
        | TOSTR("%03d") 
        | VERIFY("([001, 002, 003], [004, 005, 006])"s, "str");


    // mapas
    std::map<int, int>{{1, 2}, {3, 4}} | TOSTR() | VERIFY("[(1, 2), (3, 4)]"s);
    std::map<std::string, int>{{"c", 1}, {"a", 2}} | TOSTR() | VERIFY("[(a, 2), (c, 1)]"s);
    std::unordered_map<int, int>{{1, 2}, {3, 4}}             | TOSTR() | VERIFY("[(3, 4), (1, 2)]"s);
    std::unordered_map<std::string, int>{{"c", 1}, {"a", 2}} | TOSTR() | VERIFY("[(a, 2), (c, 1)]"s);

    // sets
    std::set<int> {1, 2, 3}            | TOSTR() | VERIFY("[1, 2, 3]"s);
    std::unordered_set<int>{1, 2, 3}          | TOSTR() | VERIFY("[3, 2, 1]"s);
    std::unordered_set<std::string>{"c", "a"} | TOSTR() | VERIFY("[a, c]"s);

    // bool não são alterados
    std::vector<bool> {true, false, true} | TOSTR() | VERIFY("[1, 0, 1]"s);

    // optional são impressos como o valor contido ou null
    std::vector<std::optional<int>> {1, 2, std::nullopt} 
        | TOSTR() 
        | VERIFY("[1, 2, null]"s);

    // shared_ptr são impressos como o valor contido ou null
    std::vector<std::shared_ptr<int>>{std::make_shared<int>(1), std::make_shared<int>(2), nullptr}
        | TOSTR()
        | VERIFY("[1, 2, null]"s);
}
```

[](load)

### JOIN

[](load)[](fn.hpp)[](fenced=cpp:extract=join)

```cpp
/**
 * Transforma um container, par ou tupla em string, separando os elementos
 * pelo separador e envolvendo com os brakets.
 *
 * Se os elementos não forem strings, eles serão transformados em string utilizando
 * a função tostr.
 * 
 * @param container Container a ser transformado em string
 * @param separator Separador entre os elementos
 * @param brakets   Brakets que envolvem a string
 * @return string com os elementos concatenados
 * 
 * @warning join(std::vector<int>{1,2,3}, " ", "{}") | WRITE(); // "{1 2 3}"
 * @note https://github.com/senapk/cppaux#join
 */
template <typename CONTAINER>
std::string join(CONTAINER container, std::string separator = "", std::string brakets = "")
```

[](load)

[](load)[](tests/join.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    v | JOIN(", ") | VERIFY("1, 2, 3, 4, 5"s);
    v | JOIN()     | VERIFY("12345"s);

    std::list<int> {1, 2, 3, 4, 5}
        | JOIN(", ", "{]") | VERIFY("{1, 2, 3, 4, 5]"s);

    "abced"s | JOIN(", ") | VERIFY("a, b, c, e, d"s);

    std::make_tuple("ovo", 1, 1.3) | JOIN(", ", "<>")          | VERIFY("<ovo, 1, 1.3>"s);
    std::make_pair("ovo", 1)       | JOIN(":") | TOSTR("<%s>") | VERIFY("ovo:1"s);
}
```

[](load)

### WRITE

[](load)[](fn.hpp)[](fenced=cpp:extract=write)

```cpp
/**
 * Tranforma um dado em string utilizando a função tostr e envia para o std::cout quebrando a linha.
 * 
 * @param data Dado a ser transformado em string
 * @param end String de quebra de linha
 * @return Dado original
 * 
 * @note https://github.com/senapk/cppaux#write
 */
template <typename PRINTABLE>
PRINTABLE write(PRINTABLE data, std::string end = "\n") 
```

[](load)

[](load)[](tests/write.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include <memory>
#include "fn.hpp"
using namespace fn;

struct Pessoa {
    std::string nome;
    int idade;
public:
    Pessoa(std::string nome, int idade) : nome(nome), idade(idade) {}
};
std::ostream& operator<<(std::ostream& os, const Pessoa& p) {
    return os << p.nome << ":" << p.idade;
}

int main() {
    // funciona com primitivos
    5 | WRITE(); //5
    4.5 | WRITE(); //4.5

    // e quaisquer combinação de containers e tipos primitivos
    std::vector<int>{1,2,3}         | WRITE(); //[1, 2, 3]
    std::list<float>{1.5,2.5, 3.5}  | WRITE(); //[1.5, 2.5, 3.5]
    std::make_pair("ovo", "queijo") | WRITE(); //(ovo, queijo)
    std::make_tuple(1,2.3,"tres")   | WRITE(); //(1, 2.3, tres)

    // após impressão, ele devolve o valor original
    // dá pra mudar o fim de linha passando um argumento
    5   | WRITE(" eh cinco\n")                 //5 eh cinco
        | WRITE(" nao eh seis\n");             //5 nao eh seis

    // para imprimir classes, é necessário sobrecarregar a função operator <<
    std::vector<Pessoa> {{"Joao", 5}, {"Maria", 6}} | WRITE(); //[Joao:5, Maria:6]

    // tudo que pode ser transformado pelo tostr, pode ser enviado diretamente para o WRITE
    std::make_shared<Pessoa>("Joao", 5) | WRITE(); //Joao:5

}
```

[](load)

### RANGE

[](load)[](fn.hpp)[](fenced=cpp:extract=range)

```cpp
/**
 * @brief Gera um vetor de inteiros de init até end, mas não incluindo end, com passo step.
 * 
 * @param init início
 * @param end limite superior
 * @param step passo do incremento
 * @return vetor de inteiros
 * 
 * @note https://github.com/senapk/cppaux#range
*/
inline std::vector<int> range(int init, int end, int step = 1)
```

[](load)

[](load)[](tests/range.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;

int main() {

    range(0, 10)     | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    range(0, 10, 2)  | WRITE(); // [0, 2, 4, 6, 8]
    range(10, 0, -1) | WRITE(); // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]

    range(1, 5) | JOIN() | VERIFY("1234"s, "range 1");
    range(5)    | JOIN() | VERIFY("01234"s, "range 2");
    range(10, -1, -2) | JOIN(", ") | VERIFY("10, 8, 6, 4, 2, 0"s, "range 3");

    range(26) | MAP([](int x) -> char {return 'a' + x;}) | JOIN() | VERIFY("abcdefghijklmnopqrstuvwxyz"s, "range");

    range(10, -1, -2) | JOIN(", ") | VERIFY("10, 8, 6, 4, 2, 0"s, "iota");
    0 | RANGE(5, 2) | JOIN() | VERIFY("024"s, "iota");
    range(-10, 10, 2) | JOIN(",") | VERIFY("-10,-8,-6,-4,-2,0,2,4,6,8"s, "iota");
    range(10, 0, -1) | JOIN() | VERIFY("10987654321"s, "iota");
    
    auto tochar = [](int x) -> char {return 'a' + x;};
    range(26) | MAP(tochar)  | JOIN() | VERIFY("abcdefghijklmnopqrstuvwxyz"s, "iota");
    range(25, -1,  -1) | MAP(tochar)  | JOIN() | VERIFY("zyxwvutsrqponmlkjihgfedcba"s, "iota");
}
```

[](load)

### VERIFY

[](load)[](fn.hpp)[](fenced=cpp:extract=verify)

```cpp
/**
 * Verifica se o dado recebido é igual ao esperado.
 * Se não for, imprime o dado recebido e o esperado e encerra o programa.
 * Ambos os dados devem ser do mesmo tipo e são convertidos para string usando o tostr.
 * 
 * @param received Dado recebido
 * @param expected Dado esperado
 * @return Dado recebido
 * 
 * @note https://github.com/senapk/cppaux#verify
 * 
*/
template <typename PRINTABLE>
PRINTABLE verify(PRINTABLE received, PRINTABLE expected, std::string label = "")
```

[](load)

### FNT

[](load)[](fn.hpp)[](fenced=cpp:extract=fnt)

```cpp
/**
 * Encurtador de função lambda para um único parâmetro e uma única operação a ser retornada.
 * O primeiro parâmetro é o nome da variável a ser utilizada, o segundo é a operação a ser realizada.
 * 
 * @param x Nome da variável
 * @param fx Operação a ser realizada
 * @return Função lambda
 * 
 * @note https://github.com/senapk/cppaux#fnt
 */
#define FNT(x, fx)                  [] (auto x) { return fx; }
```

[](load)

[](load)[](tests/fnt.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;

int main() {
    auto fn = FNT(x, x + 1);
    fn(4) | WRITE();// 5

    4   | PIPE(FNT(x, x + 1)) // 5
        | PIPE(FNT(x, x * 2)) // 10
        | WRITE(); // 10

    5        | PIPE(FNT(x, x + 1)) | VERIFY(6, "FNT");
    "banana" | PIPE(FNT(x, std::string(x) + "s")) | VERIFY("bananas"s, "FNT");
    
    int x = 8;
    1 | PIPE([&x](auto z) {return x + z;}) | VERIFY(9, "FNT");

    //dupla ação, chama função e retorna o proprio objeto
    std::vector<int> {1, 2}
        | PIPE(FNT(v, (v.push_back(3), v)))
        | TOSTR() | VERIFY("[1, 2, 3]"s, "FNT");
    
    5 | PIPE([](auto x){int y = x + 1; return y;}) | VERIFY(6, "FNT");
}
```

[](load)

### SLICE

[](load)[](fn.hpp)[](fenced=cpp:extract=slice)

```cpp
/**
 * Fatia um container de begin até o fim retornando um vector com os elementos copiados.
 * O funcionamento é equivalente à função slice do Python ou do Javascript.
 * 
 * @param container Container a ser fatiado
 * @param begin Índice inicial
 * @return Vector com os elementos copiados
 * 
 * @warning std::vector<int>{1, 2, 3, 4, 5} | SLICE(1)  | WRITE(); // [2, 3, 4, 5]
 * 
 * @note https://github.com/senapk/cppaux#slice
*/
template<typename CONTAINER>
auto slice(CONTAINER container, int begin = 0)
```

[](load)

[](load)[](tests/slice.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;

int main() {
    std::list<int> {1, 2, 3, 4} | SLICE() | PIPE(FNT(v, v[1])) | VERIFY(2, "vectorize");
    "abcdef" | PIPE(FNT(v, v[1])) | VERIFY('b', "vectorize");

    std::map<std::string, int> {{"c", 1}, {"a", 2}}
        | SLICE()
        | PIPE([](auto vet) { 
            vet.push_back({"t", 8}); 
            return vet;
        })
        | PIPE(FNT(vet, vet.back()))
        | TOSTR()
        | VERIFY("(t, 8)"s, "slice");


    range(1, 5)  | SLICE(0, 10)  | JOIN() | VERIFY("1234"s, "slice");

    range(1, 10) | SLICE(2, 5)   | JOIN() | VERIFY("345"s, "slice");
    range(10)    | SLICE(2, 5)   | JOIN() | VERIFY("234"s, "slice");
    range(10)    | SLICE(1)      | JOIN() | VERIFY("123456789"s, "slice");
    range(10)    | SLICE(-1)     | JOIN() | VERIFY("9"s, "slice");
    range(10)    | SLICE(-2)     | JOIN() | VERIFY("89"s, "slice");
    range(10)    | SLICE(-3, -1) | JOIN() | VERIFY("78"s, "slice");
    range(10)    | SLICE()       | JOIN() | VERIFY("0123456789"s, "slice");
    range(10)    | SLICE(0, 0)   | JOIN() | VERIFY(""s, "slice");
    range(10)    | SLICE(0, 1)   | JOIN() | VERIFY("0"s, "slice");
    range(10)    | SLICE(5, 2)   | JOIN() | VERIFY(""s, "slice");
    range(10)    | SLICE(5, -2)  | JOIN() | VERIFY("567"s, "slice");

    std::string("banana") | SLICE(2, 5) | JOIN() | VERIFY("nan"s, "slice");

    "abcdef" | TOSTR() | SLICE(2)     | JOIN() | VERIFY("cdef"s, "slice texto");
    "abcdef" | TOSTR() | SLICE(2, -1) | JOIN() | VERIFY("cde"s, "slice texto");
    "abcdef" | TOSTR() | SLICE(3, -4) | JOIN() | VERIFY(""s, "slice texto");

    std::list<int> {1, 2, 3, 4} | SLICE(2, 5) | JOIN() | VERIFY("34"s, "slice");

    std::map<std::string, int> m = {{"jose", 5}, {"bianca", 6}, {"maria"s, 7}};
    m | SLICE(0, 2) | MAP(FNT(x, x.first)) | JOIN(",") | VERIFY("bianca,jose"s, "slice");
}
```

[](load)

### FILTER

[](load)[](fn.hpp)[](fenced=cpp:extract=filter)

```cpp
/**
 * Retorna um vetor com os elementos do container que satisfazem a função predicado fn
 * @param container Container a ser filtrado
 * @param fn Função predicado
 * @return Vector com os elementos que satisfazem a função predicado
 * 
 * @note https://github.com/senapk/cppaux#filter
 */
template<typename CONTAINER, typename FUNCTION>
auto filter(CONTAINER container, FUNCTION fn)
```

[](load)

[](load)[](tests/filter.cpp)[](fenced=cpp)

```cpp
#include <iostream>
#include "fn.hpp"
using namespace fn;
struct Pessoa {
    std::string nome;
    int idade;
    Pessoa(std::string nome, int idade) : nome(nome), idade(idade) {}
};
std::ostream& operator<<(std::ostream& os, const Pessoa& p) {
    return os << p.nome << ":" << p.idade;
}

int main() {
    range(0, 10) | FILTER(FNT(x, x % 2 == 0)) | JOIN() | VERIFY("02468"s);
    "banana"s    | FILTER(FNT(x, x == 'a'))   | JOIN() | VERIFY("aaa"s);
    "banana"s    | FILTER(FNT(x, x != 'a'))   | JOIN() | VERIFY("bnn"s);

    std::vector<int> {1, 2, 3, 4, 5}
        | FILTER(FNT(x, x % 2 == 0)) 
        | JOIN(", ") 
        | VERIFY("2, 4"s);

    std::vector<Pessoa>{{"Joao", 20}, {"Maria", 30}}
        | FILTER(FNT(p, p.idade > 20)) 
        | JOIN(", ") 
        | VERIFY("Maria:30"s);

}
```

[](load)
