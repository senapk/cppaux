# FAKEFN for c++

[](toc)

- [Como utilizar as biblioteca](#como-utilizar-as-biblioteca)
- [Resumo](#resumo)
- [Documentação](#documentação)
  - [WRITE](#print)
  - [RANGE](#iota)
[](toc)

## Como utilizar

## Resumo

```py

//----------------------------------------------------------
//                       BASE 
//----------------------------------------------------------

write(data: DATA, end = "\n")        -> DATA   // imprime um conteúdo
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

## Documentação

Os exemplos de uso utilizam o modelo de formatação de string definido em:

```cpp

//por default, um literal texto é criado como um const char *
//para criar uma string, basta adicionar um s no final do texto.

auto a = "texto";              //const char *
auto b = std::string("texto"); //std::string
auto c = "texto"s;             //std::string

```

### WRITE

- Ação: Imprime o valor do objeto.
- Pipe: qualquer variável ou container a ser impressa.
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

### ASSERTEQ

- Ação: Dispara um warning caso o valor passado e esperado sejam diferentes.
- Pipe: Valor a ser comparado.
- Args: Valor esperado e mensagem de erro(opcional).
- Retorna: O valor próprio valor passado pelo pipe.
- Detalhes: Ambos os valores devem ser do mesmo tipo.

```c++

5        | TOSTR() | ASSERTEQ("5"s, "assert1"); 
5.0f     | TOSTR() | ASSERTEQ("5"s, "assert2");
5.123f   | TOSTR() | ASSERTEQ("5.123"s, "assert3");
"banana" | ASSERTEQ("banana", "assert4");
```