# FAKEFN for c++

<!-- toc -->
- [Requisitos](#requisitos)
- [Instalação no sistema](#instalação-no-sistema)
- [Usando como arquivo local do projeto](#usando-como-arquivo-local-do-projeto)
- [Duas opções de funções](#duas-opções-de-funções)
- [Resumo das funções](#resumo-das-funções)
- [Documentação](#documentação)
  - [String e Operador +](#string-e-operador-)
  - [PRINT](#print)
  - [WRITE](#write)
  - [TOSTR](#tostr)
  - [FORMAT](#format)
  - [INPUT](#input)
  - [JOIN](#join)
  - [RANGE](#range)
  - [SLICE](#slice)
  - [FILTER](#filter)
  - [MAP](#map)
  - [SPLIT](#split)
  - [UNPACK](#unpack)
  - [ZIP](#zip)
  - [ZIPWITH](#zipwith)
  - [ENUMERATE](#enumerate)
<!-- toc -->

## Requisitos

- Você precisa do GCC 7.3 ou superior para compilar o código.
- Quando for compilar, utilize a flag `-std=c++17`.

## Instalação no sistema

- Copie o arquivo para uma pasta de includes do sistema e use no seu código como `#include <fn.hpp>`.
  - Windows: Copie a biblioteca para dentro da pasta MinGW\include.
  - Linux: Copie a biblioteca para dentro da pasta /usr/local/include.

    ```bash
    sudo curl https://raw.githubusercontent.com/senapk/cppaux/master/fn.hpp -o /usr/local/include/fn.hpp
    ```

```cpp
//main.cpp
#include <fn.hpp>

int main() {
    fn::write("ola mundo");
}

```

## Usando como arquivo local do projeto

- Copie o arquivo para a pasta do seu projeto e inclua no seu código como `#include "fn.hpp"`.

```bash
curl https://raw.githubusercontent.com/senapk/cppaux/master/fn.hpp -o fn.hpp
```

```cpp
//main.cpp
#include "fn.hpp"

int main() {
    fn::write("ola mundo");
}

```

## Duas opções de funções

- As funções estão no namespace fn.
  - Para utilizar, basta incluir o arquivo fn.hpp e usar o namespace fn.
- As funções estão disponibilizadas em dois formatos:
  - Formato de função normal, escrita em minúsculo, com parênteses e argumentos.
  - Formato de função em pipeline, escrita em maiúsculo, onde o primeiro argumento da função é passado pelo pipe.
- Exemplo:
  - A função `tostr()` transforma um dado passado em uma string.
    - Modo função: `tostr(data, fmt)`.
    - Modo pipeline: `data | TOSTR(fmt))`.
    - Um parâmetro opcional é a formatação a ser aplicada utilizando o padrão printf.
  - A função `write()` imprime um dado passado no fluxo de saída padrão.
    - Modo função: `write(data)`.
    - Modo pipeline: `data | WRITE())`.

<!-- load guides/duas.cpp fenced=cpp -->

```cpp
#include <iostream>
#include <vector>
#include "fn.hpp"
using namespace fn;
int main() {
    std::vector<int> vet {1, 2, 3, 4};
    
    //modo função
    write(tostr(vet, "%02d")); //imprime [01, 02, 03, 04]
    
    //modo pipeline
    vet | TOSTR("%02d") | WRITE(); //imprime [01, 02, 03, 04]
}
```

<!-- load -->

## Resumo das funções

```js



//----------------------------------------------------------
//                       IMPRIMINDO 
//----------------------------------------------------------

write(data: DATA, end = "\n")        -> DATA   // imprime um conteúdo convertido pelo tostr
print(fmt: str, Args ...args)        -> DATA   // imprime utilizando o format

//----------------------------------------------------------
//                       AÇUCAR SINTÁTICO
//----------------------------------------------------------

input(fluxo = cin)                   -> str    // lê linha inteira
operator+(token: str)                -> double // transforma string em double
"nome"s                              -> str    // transforma raw string em string
range(end: int)                      -> [int]  // vetor de [0, end[, não inclui o end
range(init: int, end: int, step = 1) -> [int]  // vetor de [init, end[, não inclui o end

//----------------------------------------------------------
//                   PARA STRINGS
//----------------------------------------------------------

tostr (data: DATA, cfmt = "")                   -> str   // converte qualquer coisa para string e formata
format(fmt: str, Args ...args)                  -> str   // formata uma string usando {} e printf
join  (container, separator = "", cfmt = "") -> str   // concatena os elementos de um container 

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
slice (container<a>, begin: int, end: int) -> [a]       // copia de [begin, end[

//----------------------------------------------------------
//             ZIP
//----------------------------------------------------------
enumerate(container<a>)                -> [(int,a)] // enumera os elementos
zip    (cont<a>, cont<b>)              -> [(a,b)] // zipa dois containers em um cont de pares
zipwith(cont<a>, cont<b>, fn: (a,b->c) -> [c]         // zipa dois containers usando uma função


```

## Documentação

### String e Operador +

No C++11, foi introduzido o suporte a string literals, que permite criar strings de forma mais simples e legível.
Esse modo já está habilitado na biblioteca, mas pode ser inserido manualmente utilizando `using namespace std::string_literals;` no seu código.

Outra função útil para manipulação de strings foi inspirada no operator + do javascript, que permite converter uma string em um número.

<!-- load guides/str.cpp fenced=cpp -->

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

<!-- load -->

### PRINT

<!-- load fn.hpp fenced=cpp:extract=print -->

```cpp
/**
 * @note #### `print(str fmt, Args ...args) -> str`
 * 
 * @note - Invoca a função `format` e imprime o resultado na tela.
 * @note - As quebras de linha precisam ser inseridas explicitamente.
 * 
 * @note #### Parâmetros
 * @note `param: fmt` O texto com os {} para substituir pelos argumentos.
 * @note `param: args` Os argumentos a serem substituídos.
 * @note `return` O texto que foi impresso.
 * 
 * @note #### Exemplos
 * @note `print("O {} é {%0.2f}.\n", "pi", 3.141592653); //o pi e 3.14.`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#print
 * 
 */
template<typename... Args> std::string print(std::string fmt, Args ...args)
```

<!-- load -->

<!-- load tests/print.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;
int main() {

    // Valores entram nas chaves
    print("O valores sao {} e {}.\n", 5, 7); 
    // O valores sao 5 e 7.

    std::vector<int> vet {1, 2, 3};
    print("O primeiro elemento de {} eh {}.\n", vet, vet[0]); 
    // O primeiro elemento de [1, 2, 3] eh 1.

    print("{}{}\n", "bana", "nada");
    // bananada

    print("{{ Dobre para poder inserir chaves. }}\n"); 
    // { Dobre para poder inserir chaves. }

    // números
    print("Formate com printf assim {%.2f}.\n", 5.6123);
    // Formate com printf assim 5.61 ou assim 6.32.
    print("Ou completando com zeros {%02d}\n", 5); 
    // Ou completando com zeros 05
    print("Hora {%02d}:{%02d}:{%02d}\n", 5, 6, 7);
    // Hora 05:06:07

    // alinhamento de string definindo o tamanho
    print("Usando <10 para alinhar [{<10}].\n", "banana");
    // Usando <10 para alinhar [banana    ].
    print("Usando >10 para alinhar [{>10}]\n", "banana");
    // Usando >10 para alinhar [    banana].
    print("Usando ^10 para alinhar [{^10}]\n", "banana");
    // Usando ^10 para alinhar [  banana  ].  

    // alinhamento de string mudando o pad
    print(":[char] muda o char de pad {:_<10}.\n", "banana");
    // :[char] muda o char de pad banana____.
    print(":[char] muda o char de pad {:=>10}.\n", "banana");
    // :[char] muda o char de pad =====banana.
    print(":[char] muda o char de pad {:*^10}.\n", "banana");
    // :[char] muda o char de pad **banana**.  

    // containers
    // vetores, listas ou arrays são impressos por default com [] e separados por ,
    print("vector {}\n", std::vector<int> {1, 2, 3});  // [1, 2, 3]
    print("list {}\n", std::list<int> {1, 2, 3});      // [1, 2, 3]
    print("array {}\n", std::array<int, 3> {1, 2, 3}); // [1, 2, 3]
    
    // pair e tuplas
    // pair são impressos por default com () e separados por ,
    print("pair {}\n", std::make_pair(1, 2));      // (1, 2)
    print("tuple {}\n", std::make_tuple(1, 2, 3)); // (1, 2, 3)


    // quaisquer estruturas aninhadas funcionam como esperado
    print("pair {}\n", std::make_pair(1.7, std::make_pair("ovo", 3))); 
    // (1.7, (2, 3))

    // se uma formatação for aplicada a um container, ela será aplicada a todos os elementos
    // formatação recursiva
    print("{%.2f}\n", std::vector<double> {1.7, 2.3, 3.4}); 
    // [1.70, 2.30, 3.40]

    // 4 espaços, completa com 0 pra ter 2 digitos, faz o pad usando _
    print("{:_^4%02d}\n", std::make_pair(std::vector<int>{1,2,3}, std::vector<int>{4,5,6}));
    // ([_01_, _02_, _03_], [_04_, _05_, _06_])
     

    // map e unordered_maps são impressos entre {} e separados por , 
    // cada elemento eh um par
    print("map {}\n", std::map<int, int>{{1, 2}, {3, 4}}); 
    // {(1, 2), (3, 4)}
    print("map {}\n", std::map<std::string, int>{{"casa", 1}, {"ave", 2}}); 
    // {(ave, 2), (casa, 1)}

    // sets e unordered_sets são impressos entre {} e separados por ,
    print("set {}\n", std::set<int>{1, 2, 3}); // {1, 2, 3}

    //boleanos são impressos como true ou false
    print("Verdade eh {} e mentira eh {}\n", true, false);
    print("lista de bools {}\n", std::list<bool> {true, false, true});

    // vector são otimizados automaticamente como um array de bits
    print("vector de bools {}\n", std::vector<bool> {true, false, true}); 
    // [0, 1, 0]
}
```

<!-- load -->

### WRITE

<!-- load fn.hpp fenced=cpp:extract=write -->

```cpp
/**
 * @note #### `write(T data, str end = "\n") -> T`
 * @note #### `T data| WRITE(str end = "\n") -> T`
 * 
 * @note - Envia o dado para o `std::cout` quebrando a linha.
 * @note - Se o dado não for uma `string`, será convertido pela função `tostr`.
 * @note - O modo pipe é `| WRITE()`.
 * 
 * @note #### Parâmetros
 * @note `param: data` Dado a ser transformado e impresso.
 * @note `param: end` (opcional) String a ser inserida no final da linha
 * @note `return` O dado original recebido.
 * 
 * @note #### Exemplos
 * @note `write(vector<int> {1, 2, 3}); // [1, 2, 3]`
 * @note `vector<int> {1, 2, 3} | WRITE(); // [1, 2, 3]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#write
 */
template <typename PRINTABLE> const PRINTABLE& write(const PRINTABLE& data, str_view end = "\n") 
```

<!-- load -->

<!-- load tests/write.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

struct Pessoa {
    std::string nome;
    int idade;
public:
    Pessoa(std::string nome, int idade) : nome(nome), idade(idade) {}
};

//obrigatório para imprimir objetos diretamente via write, tostr ou print
std::ostream& operator<<(std::ostream& os, Pessoa p) {
    return os << p.nome << ":" << p.idade;
}

int main() {
    // funciona com primitivos
    5   | WRITE(); //5
    4.5 | WRITE(); //4.5

    // e quaisquer combinação de containers e tipos primitivos
    std::vector<int>{1,2,3}         | WRITE(); //[1, 2, 3]
    std::list<float>{1.5,2.5, 3.5}  | WRITE(); //[1.5, 2.5, 3.5]
    std::make_pair("ovo", "queijo") | WRITE(); //(ovo, queijo)
    std::make_tuple(1,2.3,"tres")   | WRITE(); //(1, 2.3, tres)

    // após impressão, ele devolve o valor original
    // dá pra mudar o fim de linha passando um argumento
    write("sem quebrar linha", "");
    write(" ou mudando a quebra de linha", ".\n");
    // sem quebrar linha ou mudando a quebra de linha.

    // para imprimir classes, é necessário sobrecarregar a função operator <<
    std::vector<Pessoa> pessoas {{"Joao", 5}, {"Maria", 6}};
    
    pessoas | WRITE(); //[Joao:5, Maria:6]

    // a mesma coias para o print
    print("A pessoa eh {}.\n", pessoas[0]);
}
```

<!-- load -->

### TOSTR

<!-- load fn.hpp fenced=cpp:extract=tostr -->

```cpp
/**
 * @note #### `tostr(T data, str cfmt = "") -> str`
 * @note #### `T data| TOSTR(str cfmt = "") -> str`
 * @note - Converte o dado passado em string.
 * 
 * @note #### Parâmetros
 * @note `param: data` Dado a ser convertido.
 * @note `param: cfmt` Parâmetro de formatação no modo printf.
 * @note `return` String com o dado convertido.
 * 
 * @note #### Funcionamento para tipos primitivos
 * @note - Converte o dado em string.
 * @note - Se `cfmt` for passado, o dado será formatado usando o modelo do `printf`.
 * @note `tostr(1.2) | WRITE(); // "1.2"`
 * @note `tostr(1.2, "%.4f") | WRITE(); // "1.2000"`
 * 
 * @note #### Funcionamento para containers
 * @note - Elementos são impressos usando vírgulas como separador.
 * @note `vector`, `list` e `array` são impressos entre colchetes. 
 * @note `map` e `set` são impressos entre chaves. 
 * @note `pair` e `tuple` são impressos entre parênteses.
 * @note - O parâmetro de formatação é aplicado em todos os elementos do container recursivamente.
 * @note `tostr(list<int>{1,2,3}, "%02d") | WRITE(); //[01, 02, 03]`
 * 
 * @note #### Formatação personalizada
 * @note - Veja a função `join`.
 * @note `std::vector<int> vet {1, 2, 3};`
 * @note `join(vet, " - ", "%02d") | WRITE(); // "01 - 02 - 03"`
 * 
 * @note #### Funcionamento para tipos do usuário
 * @note - Basta implementar a função de saída
 * @note `ostream& operador <<(ostream& os, TIPO tipo)`.
 * 
 * @note  #### Exemplos
 * @note `tostr(std::list<int>{1,2,3}, "%02d") | WRITE(); //[01, 02, 03]`
 * @note `std::pair<int, int>(2, 1) | TOSTR() | WRITE(); //(2, 1)`
 * 
 * @note  #### Mais exemplos em https://github.com/senapk/cppaux#tostr
 * 
 */
template <typename T> std::string tostr(const T& data     , const str_view& cfmt) 
```

<!-- load -->

<!-- load tests/tostr.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;
int main() {

    // modo função
    tostr(5.6123, "%.2f")                     | WRITE(); // 5.61
    tostr(std::vector<int> {1, 2, 3})         | WRITE(); // [1, 2, 3]
    tostr(std::pair<int, int> {1, 2}, "%03d") | WRITE(); // (001, 002)
    tostr("banana", "%-8s")                   | WRITE(); // banana  

    // números
    5.6123 | TOSTR("%.2f")     | WRITE(); // 5.61
    5      | TOSTR("%02d")     | WRITE(); // 05

    // alinhamento de string
    "banana"  | TOSTR("<8")   | WRITE(); // banana  
    "banana"  | TOSTR(">8")   | WRITE(); //   banana

    // containers
    // a função write já chama a função tostr para não primitivos
    std::vector<int> {1, 2, 3}         | WRITE(); // [1, 2, 3]
    std::list<int> {1, 2, 3}           | WRITE(); // [1, 2, 3]
    std::array<int, 3> {1, 2, 3}       | WRITE(); // [1, 2, 3]
    
    // pair
    std::pair<int, int> {1, 2}         | WRITE(); // (1, 2)

    // tuples
    // std::make_tuple(1, 5.42, "banana") | WRITE(); // (1, 5.42, banana)

    // estruturas aninhadas
    std::make_pair(std::make_pair(4, "ovo"), "chiclete") | WRITE(); // ((4, ovo), chiclete)

    //formatação recursiva
    std::make_tuple(1, 2, 3) 
        | TOSTR("%03d") 
        | WRITE(); // (001, 002, 003)

    std::make_pair(std::vector<int>{1,2,3}, std::vector<int>{4,5,6}) 
        | TOSTR("%03d")
        | WRITE(); // ([001, 002, 003], [004, 005, 006])

    // mapas
    std::map<int, int>{{1, 2}, {3, 4}} | WRITE(); // {(1, 2), (3, 4)}
    std::map<std::string, int>{{"c", 1}, {"a", 2}} | WRITE(); // {(a, 2), (c, 1)}
    std::unordered_map<int, int>{{1, 2}, {3, 4}}   | WRITE(); // {(3, 4), (1, 2)}
    std::unordered_map<std::string, int>{{"c", 1}, {"a", 2}} | WRITE(); // {(c, 1), (a, 2)}

    // sets
    std::set<int> {1, 2, 3}                   | WRITE(); // {1, 2, 3}
    std::unordered_set<int>{1, 2, 3}          | WRITE(); // {3, 2, 1}
    std::unordered_set<std::string>{"c", "a"} | WRITE(); // {a, c}

    true | WRITE(); // true
    false | WRITE(); // false
    std::make_pair(true, 5.43) | WRITE(); // (true, 5.43)
    std::list<bool> {true, false, true} | WRITE(); // [true, false, true]
    std::vector<bool> {true, false, true} | WRITE(); // [true, false, true]
}
```

<!-- load -->

### FORMAT

<!-- load fn.hpp fenced=cpp:extract=format -->

```cpp
/**
 * @note #### `format(str fmt, Args ...args ) -> str`
 * @note #### `str fmt| FORMAT(Args ...args ) -> str`
 * 
 * @note - Formata uma string com base nos argumentos passados 
 * @note - Utilizando um modelo de chaves para posicionar os argumentos.
 * @note - Se dentro da chave, houver um string de formatação, o dado será formatado com base nela.
 * @note - Não primitivos são formatados de acordo com a função `tostr`
 * 
 * @note #### Parâmetros
 * @note `param: fmt` O texto com os {} para substituir pelos argumentos.
 * @note `param: args` Os argumentos a serem substituídos.
 * @note `return` O texto formatado.
 * 
 * @note #### Exemplos
 * @note `format("O {} é {%0.2f}.", "pi", 3.141592653) | WRITE(); //o pi e 3.14.`
 * @note `"O {} é {%0.2f}." | FORMAT("pi", 3.141592653) | WRITE(); //o pi e 3.14.`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#format
 * 
 */
std::string format(std::string fmt, Args ...args) 
```

<!-- load -->

<!-- load tests/format.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {

    format("O {} é {%0.2f} e o {} é {%0.4f}", "pi", 3.141592653, "e", 2.7182818) | WRITE(); 
    // O pi é 3.14 e o e é 2.7218

    format("Meu vetor é {}", std::vector<int>{0,1,2,3,4,5,6,7,8,9}) | WRITE();
    // Meu vetor é [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    format("Vetor com duas casas {%.2f}", std::vector<double>{1.1321, 2, 3.3}) | WRITE(); 
    // Vetor com duas casas [1.13, 2.00, 3.30]

    format("Alinhado a esquerda 10 casas [{<10}]", "abacate") | WRITE();
    // Alinhado a esquerda 10 casas [abacate   ]

    format("Alinhado a direita 10 casas [{>10}]", "abacate") | WRITE(); 
    // Alinhado a direita 10 casas [   abacate]

    "Contando de 1 a 10 {}"s | FORMAT(std::vector<int>{1,2,3,4,5,6,7,8,9,10}) | WRITE(); 
    // Contando de 1 a 10 [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    // Se faltar parametros ele coloca vazio
    "{} {} tem {} anos" | FORMAT("jose"s, "luiz", 5) | WRITE(); 
    // jose 5 tem  anos

    "{}, eu {} {%02d} bananas {}." | FORMAT("Hoje", "comi", 5, "maduras") | WRITE(); 
    // Hoje, eu comi 05 bananas maduras.

    "Eu ({<10}) {%02d} bananas{}" | FORMAT("comi"s, 5, '.') | WRITE(); 
    // Eu (comi      ) 05 bananas.

    "Hoje eh dia {{{}}}" | FORMAT(5) | WRITE();
    // Hoje eh dia {5}
}
```

<!-- load -->

### INPUT

<!-- load fn.hpp fenced=cpp:extract=input -->

```cpp
/**
 * @note #### `input(istream source = std::cin) -> str`
 * 
 * @note - Extrai uma linha inteira e retorna como string.
 * @note - O padrão é o std::cin, mas pode ser um fluxo ou arquivo.
 * @note - Se não houver mais linhas, lança uma exceção.
 * 
 * @note #### Parâmetros
 * @note `param: source` (opcional) de onde ler a linha.
 * @note `return` linha lida.
 * @note `throws: std::runtime_error` Caso não haja mais linhas.
 * 
 * @note #### Exemplos
 * @note `auto line = input();`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#input
 */
inline std::string input(std::istream & is = std::cin)
```

<!-- load -->

<!-- load tests/input.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {
    auto line = input();
    line | WRITE();
}
```

<!-- load -->

### JOIN

<!-- load fn.hpp fenced=cpp:extract=join -->

```cpp
/**
 * @note #### `join(T container, str separator = "", str cfmt = "") -> str`
 * @note #### `T container| JOIN(str separator = "", str cfmt = "") -> str`
 * 
 * @note - Transforma um container, par ou tupla em string.
 * @note - Se os elementos não forem strings, eles serão transformados pela função `tostr`.
 * 
 * @note #### Parâmetros
 * @note `param: container` Container a ser transformado em string.
 * @note `param: separator` Separador entre os elementos.
 * @note `param: cfmt` String de formatação.
 * @note `return` String com os elementos concatenados.
 * 
 * @note  #### Exemplos
 * @note `join(vector<int>{1, 2, 3}, " ") | WRITE(); // "1 2 3"`
 * @note `vector<int>{1, 2, 3} | JOIN("=") | WRITE(); // "1=2=3"`
 * @note `vector<int>{1, 2, 3} | JOIN("=", "%02d") | WRITE(); // "01=02=03"`
 * 
 * @note  #### Mais exemplos em https://github.com/senapk/cppaux#join
 */
template <typename T>
std::string join(const T& container, const str_view& separator = "", const str_view& cfmt = "") 
```

<!-- load -->

<!-- load tests/join.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    v | JOIN(", ") | WRITE(); // 1, 2, 3, 4, 5
    v | JOIN()     | WRITE(); // 12345

    std::list<int> {1, 2, 3, 4, 5}
        | JOIN(", ") 
        | WRITE(); // 1, 2, 3, 4, 5

    "abced"s | JOIN(", ") | WRITE(); // a, b, c, e, d

    std::make_tuple("ovo", 1, 1.3) 
        | JOIN(", ")
        | WRITE(); // ovo, 1, 1.3

     std::make_pair("ovo", 1)
         | JOIN(":")
         | WRITE(); // ovo:1
}
```

<!-- load -->

### RANGE

<!-- load fn.hpp fenced=cpp:extract=range -->

```cpp
/**
 * @note #### `range(int init, int end, int step = 1) -> vector<int>`
 * 
 * @note - Gera um vetor de inteiros de init até end, mas não incluindo end, com passo step.
 * 
 * @note #### Parâmetros
 * @note `param: init` início.
 * @note `param: end` limite superior.
 * @note `param: step` passo do incremento.
 * @note `return` vetor de inteiros.
 * 
 * @note #### Exemplos
 * @note `range(0, 10) | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]`
 * @note `range(0, 10, 2) | WRITE(); // [0, 2, 4, 6, 8]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#range
*/
inline std::vector<int> range(int init, int end, int step = 1)
```

<!-- load -->

<!-- load tests/range.cpp fenced=cpp -->

```cpp
#include "fn.hpp"

using namespace fn;

int main() {

    range(0, 10)     | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    range(0, 10, 2)  | WRITE(); // [0, 2, 4, 6, 8]
    range(10, 0, -1) | WRITE(); // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]

    range(1, 5) | WRITE(); // [1, 2, 3, 4]
    range(5)    | WRITE(); // [0, 1, 2, 3, 4]
    range(10, -1, -2) | WRITE(); // [10, 8, 6, 4, 2, 0]

    range(26) 
        | MAP([](int x) -> char {return 'a' + x;}) 
        | WRITE() // [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z]
        | JOIN()
        | WRITE(); // abcdefghijklmnopqrstuvwxyz

    range(10, -1, -2)
        | WRITE(); // [10, 8, 6, 4, 2, 0]

    5 | RANGE() | WRITE(); // [0, 1, 2, 3, 4]

    range(-10, 10, 2) | WRITE(); // [-10, -8, -6, -4, -2, 0, 2, 4, 6, 8]

    range(10, 0, -1) | WRITE(); // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
    
    auto tochar = [](int x) -> char {return 'a' + x;};
    range(26) 
        | MAP(tochar)  
        | JOIN() 
        | WRITE(); // abcdefghijklmnopqrstuvwxyz

    range(25, -1,  -1)
        | MAP(tochar)
        | JOIN()
        | WRITE(); // zyxwvutsrqponmlkjihgfedcba
}
```

<!-- load -->

### SLICE

<!-- load fn.hpp fenced=cpp:extract=slice -->

```cpp
/**
 * @note #### `slice(CONTAINER<T> container, int begin = 0) -> vector<T>`
 * @note #### `CONTAINER<T> container| SLICE(int begin = 0) -> vector<T>`
 * 
 * @note - Fatia um container retornando um `vector` com os elementos copiados.
 * @note - Funciona como a função `slice` do Python ou do Javascript. 
 * @note - Se não passado nenhum parâmetro, copia todos os elementos. 
 * @note - Os índices podem ser negativos para contar a partir final.
 * 
 * @note #### Parâmetros
 * @note `param: container` container a ser fatiado.
 * @note `param: begin` (opcional) índice inicial.
 * @note `return` vector com os elementos copiados.
 * 
 * @note #### Exemplos
 * @note `vector<int> vet {1, 2, 3, 4, 5};`
 * @note `slice(vet, 1) | WRITE(); // [2, 3, 4, 5]`
 * @note `slice(vet, -2) | WRITE(); // [4, 5]`
 * @note `vet | SLICE(1) | WRITE(); // [2, 3, 4, 5]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#slice
*/
template<typename CONTAINER>
auto slice(const CONTAINER& container, int begin = 0)
```

<!-- load -->

<!-- load tests/slice.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {
    range(1, 10) | SLICE(2, 5)   | WRITE(); // [3, 4, 5]
    range(10)    | SLICE(2, 5)   | WRITE(); // [2, 3, 4]
    range(10)    | SLICE(1)      | WRITE(); // [1, 2, 3, 4, 5, 6, 7, 8, 9]
    range(10)    | SLICE(-1)     | WRITE(); // [9]
    range(10)    | SLICE(-2)     | WRITE(); // [8, 9]
    range(10)    | SLICE(-3, -1) | WRITE(); // [7, 8]
    range(10)    | SLICE()       | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    range(10)    | SLICE(0, 0)   | WRITE(); // []
    range(10)    | SLICE(0, 1)   | WRITE(); // [0]
    range(10)    | SLICE(5, 2)   | WRITE(); // []
    range(10)    | SLICE(5, -2)  | WRITE(); // [5, 6, 7]

    std::string("banana") | SLICE(2, 5) | WRITE(); // nan

    "abcdef" | TOSTR() | SLICE(2)     | WRITE(); // [c, d, e, f]
    "abcdef" | TOSTR() | SLICE(2, -1) | WRITE(); // [c, d, e]
    "abcdef" | TOSTR() | SLICE(3, -4) | WRITE(); // []

    std::list<int> {1, 2, 3, 4} | SLICE(2, 5) | WRITE(); // [3, 4]

    std::map<std::string, int> m = {{"jose", 5}, {"bianca", 6}, {"maria"s, 7}};
    m   | SLICE(0, 2)
        | MAP([](auto x) { return x.first; })
        | WRITE(); // [jose, bianca]

    std::vector<int> v = std::list<int> {1, 2, 3, 4} | SLICE(); // transforma em vetor


    std::map<std::string, int> {{"c", 1}, {"a", 2}}
        | SLICE() // transforma em vetor de pares
        | WRITE(); // [(a, 2), (c, 1)]


    range(1, 5) 
        | WRITE() // [1, 2, 3, 4]
        | SLICE(0, 10) 
        | WRITE(); // [1, 2, 3, 4]
}
```

<!-- load -->

### FILTER

<!-- load fn.hpp fenced=cpp:extract=filter -->

```cpp
/**
 * @note #### `filter(CONTAINER<T> container, FUNCTION fn) -> vector<T>`
 * @note #### `CONTAINER<T> container| FILTER(FUNCTION fn) -> vector<T>`
 * 
 * @note - Retorna um vetor com os elementos do container que satisfazem a função predicado fn.
 * 
 * @note #### Parâmetros
 * @note `param: container` Container a ser filtrado.
 * @note `param: fn` Função predicado.
 * @note `return` Vector com os elementos que satisfazem a função predicado.
 * 
 * @note #### Exemplos
 * @note `vector<int> vet {1, 2, 3};`
 * @note `filter(vet, [](int x) {return x % 2 == 0;}) | WRITE(); // [2]`
 * @note `vet| FILTER([](int x) {return x % 2 == 0;}) | WRITE(); // [2]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#filter
 * 
 */
template<typename CONTAINER, typename FUNCTION>
auto filter(const CONTAINER& container, FUNCTION fn)
```

<!-- load -->

<!-- load tests/filter.cpp fenced=cpp -->

```cpp
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
    range(0, 10) | FILTER([](auto x) { return x % 2 == 0;}) | WRITE(); // [0, 2, 4, 6, 8, 10]
    "banana"s    | FILTER([](auto x) { return x == 'a';})   | WRITE(); // [a, a, a]
    "banana"s    | FILTER([](auto x) { return x != 'a';})   | WRITE(); // [b, n, n]

    std::vector<int> {1, 2, 3, 4, 5}
        | FILTER([](auto x) { return x % 2 == 0;})
        | WRITE(); // [2, 4]

    std::vector<Pessoa>{{"Joao", 20}, {"Maria", 30}}
        | FILTER([](auto p) {return p.idade > 20;}) 
        | WRITE(); // [Maria:30]

}
```

<!-- load -->

### MAP

<!-- load fn.hpp fenced=cpp:extract=map -->

```cpp
/**
 * @note #### `map(CONTAINER<T> container, FUNCTION fn) -> vector<fn(T)>`
 * @note #### `CONTAINER<T> container| MAP(FUNCTION fn) -> vector<fn(T)>`
 * 
 * @note - Retorna um vetor com o resultado da aplicação da função fn para cada elemento do container.
 * 
 * @note #### Parâmetros
 * @note `param: container` Container a ser mapeado.
 * @note `param: fn` Função a ser aplicada em cada elemento do container.
 * @note `return` Vector com os elementos resultantes da aplicação da função.
 * 
 * @note #### Exemplos
 * @note `vector<int> vet {1, 2, 3};`
 * @note `map(vet, [](int x) {return x * x;}) | WRITE(); // [1, 4, 9]`
 * @note `vet| MAP([](int x) {return x * x;}) | WRITE(); // [1, 4, 9]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#map
 */
template<typename CONTAINER, typename FUNCTION>
auto map(const CONTAINER& container, FUNCTION fn)
```

<!-- load -->

<!-- load tests/map.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

struct Pessoa {
    std::string nome;
    int idade;

    Pessoa(std::string nome = "", int idade = 0): nome{nome}, idade{idade} {}
};

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
        v   | MAP([](auto x) {return x + 1;}) 
            | WRITE(); // [2, 3, 4, 5, 6]

        std::vector<Pessoa> vp = {{"Joao", 20}, {"Maria", 30}};
        vp  | MAP([](auto p) {return p.idade + 1;}) 
            | WRITE(); // [21, 31]

        "abcde"s
            | MAP([](char c) -> char {return c + 1;}) 
            | WRITE(); // [b, c, d, e, f]
}
```

<!-- load -->

### SPLIT

<!-- load fn.hpp fenced=cpp:extract=split -->

```cpp
/**
 * @note #### `split(str content, char delimiter = ' ') -> vector<str>`
 * @note #### `str content| SPLIT(char delimiter = ' ') -> vector<str>`
 * 
 * @note - Transforma uma string em um vetor de strings, separando pelo delimitador.
 * 
 * @note #### Parâmetros
 * @note `param: content` String a ser separada.
 * @note `param: delimiter` (opcional) Caractere delimitador.
 * @note `return` Vetor de strings.
 *
 * @note #### Exemplos
 * @note `split("a,b,c", ',') | WRITE(); // [a, b, c]`
 * @note `"a,b,c"| SPLIT(',') | WRITE(); // [a, b, c]`
 * @note `split("a b c") | WRITE(); // [a, b, c]`
 * @note `"a b c"|SPLIT()| WRITE(); // [a, b, c]`
 *  
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#split
 */
inline std::vector<std::string> split(std::string content, char delimiter = ' ')
```

<!-- load -->

<!-- load tests/split.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {

    split("eu gosto de comer banana", ' ') | WRITE(); // [eu, gosto, de, comer, banana]")
    split("a,b,c", ',')                    | WRITE(); // [a, b, c]
    split("eu gosto de comer banana", ' ') | WRITE(); // [eu, gosto, de, comer, banana]")
    split("eu gosto de comer    banana")   | WRITE(); // [eu, gosto, de, comer, , , , banana]")
}
```

<!-- load -->

### UNPACK

<!-- load fn.hpp fenced=cpp:extract=unpack -->

```cpp
/**
 * @note #### `unpack<...TS>(str value, char delimiter) -> tuple<TS...>`
 * @note #### `str value| UNPACK<...TS>(char delimiter) -> tuple<TS...>`
 * 
 * @note - Transforma de string para tupla dados os tipos de cada elemento e o char separador.
 * 
 * @note #### Parâmetros
 * @note `param: TS...` Tipos a serem extraídos.
 * @note `param: value` String a ser convertida.
 * @note `param: delimiter` Caractere separador entre os elementos.
 * @note `return` Tupla com os elementos convertidos.
 * 
 * @note #### Exemplos
 * @note unpack<int, double, std::string>("1:2.4:uva", ':') | WRITE(); // (1, 2.4, "uva") 
 * @note "1:2.4:uva"| UNPACK<int, double, std::string>(':') | WRITE(); // (1, 2.4, "uva")
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#unpack
 * 
 */
template <typename... TS>
std::tuple<TS...> unpack(const std::string& line, char delimiter)
```

<!-- load -->

<!-- load tests/unpack.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

int main() {
    "5:6:7"
        | UNPACK<int, int, int>(':') 
        | WRITE(); // (5, 6, 7)

    "banana-5.68-8-c" 
        | UNPACK<std::string, double, int, char>('-')
        | WRITE(); // (banana, 5.68, 8, c)
}
```

<!-- load -->

### ZIP

<!-- load fn.hpp fenced=cpp:extract=zip -->

```cpp
/**
 * @note #### `zip(CONTAINER_A<T> container_a, CONTAINER_B<K> container_b) -> vector<pair<T, K>>`
 * @note #### `CONTAINER_A<T> container_a| ZIP(CONTAINER_B<K> container_b) -> vector<pair<T, K>>`
 * 
 * @note - Une dois containers em um vetor de pares limitado ao menor tamanho dos dois containers.
 * 
 * @note #### Parâmetros
 * @note `param: container_a` primeiro container
 * @note `param: container_b` segundo container
 * @note `return` Vetor de pares
 * 
 * @note #### Exemplos
 * @note `zip(vector<int>{1, 2, 3}, string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]`
 * @note `vector<int>{1, 2, 3}| ZIP(string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#zip
 */
template<typename CONTAINER_A, typename CONTAINER_B>
auto zip(const CONTAINER_A& A, const CONTAINER_B& B)
```

<!-- load -->

<!-- load tests/zip.cpp fenced=cpp -->

```cpp
#include "fn.hpp"

using namespace fn;

int main() {
        range(1, 5) | ZIP(range(10, 20)) 
            | WRITE(); // [(1, 10), (2, 11), (3, 12), (4, 13)]

        zip(range(10, 20), range(1, 5))
            | WRITE(); // [(10, 1), (11, 2), (12, 3), (13, 4)]
            
        "banana"s | ZIP(range(1, 10)) 
            | WRITE(); // [(b, 1), (a, 2), (n, 3), (a, 4), (n, 5), (a, 6)]
}
```

<!-- load -->

### ZIPWITH

<!-- load fn.hpp fenced=cpp:extract=zipwith -->

```cpp
/**
 * @note #### `zipwith(CONTAINER_A<T> container_a, CONTAINER_B<K> container_b, FUNCTION fnjoin) -> vector<fnjoin(T, K)>`
 * @note #### `CONTAINER_A<T> container_a| ZIPWITH(CONTAINER_B<K> container_b, FUNCTION fnjoin) -> vector<fnjoin(T, K)>`
 * 
 * @note - Une dois containers através da aplicação da função fnjoin em cada par limitado ao menor tamanho dos dois containers.
 * 
 * @note #### Parâmetros
 * @note `param: container_a` primeiro container
 * @note `param: container_b` segundo container
 * @note `param: fnjoin` função a ser aplicada em cada par
 * @note `return` Vetor com os resultados
 * 
 * @note #### Exemplos
 * @note `auto concat = [](auto x, auto y) { return tostr(x) + y; };`
 * @note `zipwith(vector<int>{1, 2, 3}, string("pterodactilo"), concat) | WRITE(); // ["1p", "2t", "3e"]`
 * @note `zipwith(range(10), "pterodactilo"s, concat) | WRITE(); // ["0p", "1t", "2e", "3r", "4o", "5d", "6a", "7c", "8t", "9i"]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#zipwith
 * 
 */
template<typename CONTAINER_A, typename CONTAINER_B, typename FNJOIN>
auto zipwith(const CONTAINER_A& A, const CONTAINER_B& B, FNJOIN fnjoin)
```

<!-- load -->

<!-- load tests/zipwith.cpp fenced=cpp -->

```cpp
#include "fn.hpp"
using namespace fn;

#define FNT2(x, y, fn) [](auto x, auto y) { return fn; }

int main() {
    range(1, 20) | ZIPWITH(range(11, 15), FNT2(x, y, x + y)) 
        | WRITE(); // [12, 14, 16, 18]

    zipwith(range(1, 5), range(11, 20), FNT2(x, y, tostr(x) + tostr(y)))
        | WRITE(); // [111, 212, 313, 414]

    //junta dois char em uma string
    auto fnjoin = FNT2(x, y, (std::string{x, y}));
    "ABCDEF"s | ZIPWITH("abcdef"s, fnjoin)
        | WRITE(); // [Aa, Bb, Cc, Dd, Ee, Ff]

    zipwith(range(10), "pterodactilo"s, FNT2(x, y, tostr(x) + y))
        | WRITE(); // [0p, 1t, 2e, 3r, 4o, 5d, 6a, 7c, 8t, 9i]

    range(10) | ZIPWITH(range(10), FNT2(x, y, x + y))
        | WRITE(); // [0, 2, 4, 6, 8, 10, 12, 14, 16, 18]
}
```

<!-- load -->

### ENUMERATE

<!-- load fn.hpp fenced=cpp:extract=enumerate -->

```cpp
/**
 * @note #### `enumerate(CONTAINER<T> container) -> vector<pair<int, T>>`
 * @note #### `CONTAINER<T> container|ENUMERATE()-> vector<pair<int, T>>`
 * 
 * @note - Retorna um vetor de pares com os indices seguidos dos elementos originais do vetor.
 * 
 * @note #### Parâmetros
 * @note `param: container` Container a ser enumerado.
 * @note `return` Vector com os pares.
 * 
 * @note #### Exemplos
 * @note `vector<int> vet {1, 2, 3};`
 * @note `enumerate(vet) | WRITE(); // [(0, 1), (1, 2), (2, 3)]`
 * @note `vet|ENUMERATE()| WRITE(); // [(0, 1), (1, 2), (2, 3)]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#enumerate
 */
template<typename CONTAINER>
auto enumerate(const CONTAINER& container)
```

<!-- load -->

<!-- load tests/enumerate.cpp fenced=cpp -->

```cpp
#include "fn.hpp"


int main() {

    std::vector<int> a {1, 3, 2, 5};

    for (auto [i, x] : a | fn::ENUMERATE()) {
        fn::print("{} {}\n", i, x);
    }

    fn::write(fn::enumerate(a));  // [(0, 1), (1, 3), (2, 2), (3, 5)]

    fn::write(fn::enumerate("banana"s)); // [(0, b), (1, a), (2, n), (3, a), (4, n), (5, a)]

    std::vector<double> d = {1.2, 2.1, 5.3, 6.7, 9.34};
    d   | fn::ENUMERATE() 
        | fn::MAP(
            [](auto p){ return fn::format("{:_^5}:{%.2f}", std::get<0>(p), std::get<1>(p)); })
        | fn::JOIN("\n") 
        | fn::WRITE();

    /*
__0__:1.20
__1__:2.10
__2__:5.30
__3__:6.70
__4__:9.34
    */
}
```

<!-- load -->
