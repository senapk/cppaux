# FAKEFN for c++

[](toc)
[](toc)

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

[](load)[](guides/duas.cpp)[](fenced=cpp)
[](load)[](guides/modopipe.cpp)[](fenced=cpp)

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
slice (container<a>, begin: int, end: int) -> [a]       // copia de begin até end

//----------------------------------------------------------
//             ZIP
//----------------------------------------------------------
enumerate(container<a>)                -> [(int,a)] // enumera os elementos
zip    (cont<a>, cont<b>)              -> [(a,b)] // zipa dois containers em um cont de pares
zipwith(cont<a>, cont<b>, fn: (a,b->c) -> [c]         // zipa dois containers usando uma função

//----------------------------------------------------------
//                       PIPE 
//----------------------------------------------------------
PIPE(FN)         Cria uma functor para ser executado em pipeline
FNT(x, fn)       Função de transformação simples de um parâmetro
FNT2(x, y, fxy)  Função de transformação simples de dois parâmetros e uma transformação

```

## Documentação

### String e Operador +

No C++11, foi introduzido o suporte a string literals, que permite criar strings de forma mais simples e legível.
Esse modo já está habilitado na biblioteca, mas pode ser inserido manualmente utilizando `using namespace std::string_literals;` no seu código.

Outra função útil para manipulação de strings foi inspirada no operator + do javascript, que permite converter uma string em um número.

[](load)[](guides/str.cpp)[](fenced=cpp)
[](load)

### PRINT

[](load)[](fn.hpp)[](fenced=cpp:extract=print)
[](load)

[](load)[](tests/print.cpp)[](fenced=cpp)
[](load)

### WRITE

[](load)[](fn.hpp)[](fenced=cpp:extract=write)
[](load)

[](load)[](tests/write.cpp)[](fenced=cpp)
[](load)

### TOSTR

[](load)[](fn.hpp)[](fenced=cpp:extract=tostr)
[](load)

[](load)[](tests/tostr.cpp)[](fenced=cpp)
[](load)

### FORMAT

[](load)[](fn.hpp)[](fenced=cpp:extract=format)
[](load)

[](load)[](tests/format.cpp)[](fenced=cpp)
[](load)

### INPUT

[](load)[](fn.hpp)[](fenced=cpp:extract=input)
[](load)

[](load)[](tests/input.cpp)[](fenced=cpp)
[](load)

### JOIN

[](load)[](fn.hpp)[](fenced=cpp:extract=join)
[](load)

[](load)[](tests/join.cpp)[](fenced=cpp)
[](load)

### RANGE

[](load)[](fn.hpp)[](fenced=cpp:extract=range)
[](load)

[](load)[](tests/range.cpp)[](fenced=cpp)
[](load)

### SLICE

[](load)[](fn.hpp)[](fenced=cpp:extract=slice)
[](load)

[](load)[](tests/slice.cpp)[](fenced=cpp)
[](load)

### FILTER

[](load)[](fn.hpp)[](fenced=cpp:extract=filter)
[](load)

[](load)[](tests/filter.cpp)[](fenced=cpp)
[](load)

### MAP

[](load)[](fn.hpp)[](fenced=cpp:extract=map)
[](load)

[](load)[](tests/map.cpp)[](fenced=cpp)
[](load)

### SPLIT

[](load)[](fn.hpp)[](fenced=cpp:extract=split)
[](load)

[](load)[](tests/split.cpp)[](fenced=cpp)
[](load)

### UNPACK

[](load)[](fn.hpp)[](fenced=cpp:extract=unpack)
[](load)

[](load)[](tests/unpack.cpp)[](fenced=cpp)
[](load)

### PIPE

[](load)[](fn.hpp)[](fenced=cpp:extract=pipe)
[](load)

[](load)[](tests/pipe.cpp)[](fenced=cpp)
[](load)

### FNT

[](load)[](fn.hpp)[](fenced=cpp:extract=fnt)
[](load)

[](load)[](tests/fnt.cpp)[](fenced=cpp)
[](load)

### ZIP

[](load)[](fn.hpp)[](fenced=cpp:extract=zip)
[](load)

[](load)[](tests/zip.cpp)[](fenced=cpp)
[](load)

### ZIPWITH

[](load)[](fn.hpp)[](fenced=cpp:extract=zipwith)
[](load)

[](load)[](tests/zipwith.cpp)[](fenced=cpp)
[](load)

### ENUMERATE

[](load)[](fn.hpp)[](fenced=cpp:extract=enumerate)
[](load)

[](load)[](tests/enumerate.cpp)[](fenced=cpp)
[](load)