# FAKEFN for c++

[](toc)
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
[](load)

### PIPE

[](load)[](fn.hpp)[](fenced=cpp:extract=pipe)
[](load)

[](load)[](tests/pipe.cpp)[](fenced=cpp)
[](load)

### TOSTR

[](load)[](fn.hpp)[](fenced=cpp:extract=tostr)
[](load)

[](load)[](tests/tostr.cpp)[](fenced=cpp)
[](load)

### JOIN

[](load)[](fn.hpp)[](fenced=cpp:extract=join)
[](load)

[](load)[](tests/join.cpp)[](fenced=cpp)
[](load)

### WRITE

[](load)[](fn.hpp)[](fenced=cpp:extract=write)
[](load)

[](load)[](tests/write.cpp)[](fenced=cpp)
[](load)

### RANGE

[](load)[](fn.hpp)[](fenced=cpp:extract=range)
[](load)

[](load)[](tests/range.cpp)[](fenced=cpp)
[](load)

### VERIFY

[](load)[](fn.hpp)[](fenced=cpp:extract=verify)
[](load)

### FNT

[](load)[](fn.hpp)[](fenced=cpp:extract=fnt)
[](load)

[](load)[](tests/fnt.cpp)[](fenced=cpp)
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
