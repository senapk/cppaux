# FAKEFN for c++

[](toc)

- [Duas opções de funções](#duas-opções-de-funções)
- [Resumo das funções](#resumo-das-funções)
  - [String](#string)
  - [PIPE](#pipe)
  - [TOSTR](#tostr)
  - [WRITE](#write)
  - [RANGE](#range)
  - [VERIFY](#verify)
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
 * 
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

### WRITE

- Ação: Imprime o valor do objeto.
- Pipe: qualquer variável ou container a serem impressos.
- Args: fim da linha(opcional).
- Retorna: a própria variável que foi passada pelo pipe.

```c++
data: T | WRITE(end="\n") -> T
```

```c++

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

// para imprimir classes, é necessário sobrecarregar a função
// std::ostream& operator<<(std::ostream& os, const T& t)
std::vector<Pessoa> {{"Joao", 5}, {"Maria", 6}} 
                    | WRITE(); //[Joao:5, Maria:6]
```

### RANGE

- Ação: Gera um vector com valores sequenciais.
- Pipe: Tamanho do vector.
- Args: Início da sequência e incremento(opcional).
- Retorna: O vetor gerado.
- Detalhes: O tipo do vetor é deduzido pelo tipo do argumento inicial e o incremento é sempre um valor inteiro.

```c++

```c++
- size: int | RANGE(init: T, inc = 1) -> vector<T>
```

```c++
4
```

### VERIFY

- Ação: Dispara um warning caso o valor passado e esperado sejam diferentes.
- Pipe: Valor a ser comparado.
- Args: Valor esperado e mensagem de erro(opcional).
- Retorna: O valor próprio valor passado pelo pipe.
- Detalhes: Ambos os valores devem ser do mesmo tipo.

```c++

5        | TOSTR() | VERIFY("5"s, "assert1"); 
5.0f     | TOSTR() | VERIFY("5"s, "assert2");
5.123f   | TOSTR() | VERIFY("5.123"s, "assert3");
"banana" | VERIFY("banana", "assert4");
```
