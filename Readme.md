# FAKEFN for c++

[](toc)

- [Como utilizar as biblioteca](#como-utilizar-as-biblioteca)
- [Resumo](#resumo)
- [Documentação](#documentação)
  - [WRITE](#print)
  - [RANGE](#iota)
[](toc)

## Como utilizar

- TODAS as funções dessa biblioteca são **functors**, ou seja, objetos que podem ser chamados como funções.
- Os objetos são criados por meio de um construtor e podem ser chamados como funções ou via pipe |.
- Por exemplo, a função TOSTR que é utilizada para gerar a string de um objeto, pode ser chamada como função ou via pipe.

```cpp
//como função, esse primeiro par de parênteses é obrigatório
TOSTR()(5.7) // "5.7"
TOSTR()(std::vector<int>{1,2,3,4,5}) // "[1, 2, 3, 4, 5]"
//ou
5.7 | TOSTR() // "5.7"
std::vector<int>{1,2,3,4,5} | TOSTR() // "[1, 2, 3, 4, 5]"

//se for passado algum parâmetro para o TOSTR(), 
//ele utiliza para formatação usando as regras do printf
5.7 | TOSTR("%.2f") // "5.70"
TOSTR("%.3f")(5.7) // "5.700"

//se aplicado a algum container, ele aplica a função para cada elemento
std::vector<int>{1,2,3,4,5} | TOSTR("%03d") // "[001, 002, 003, 004, 005]"
TOSTR("%03d")(std::vector<int>{1,2,3,4,5})  // "[001, 002, 003, 004, 005]"
```

## Resumo

```py

//----------------------------------------------------------
//                       MAIN 
//----------------------------------------------------------
TOSTR
FORMAT
JOIN
MAP
FILTER
SPLIT
WRITE
STRTO
SLICE
FMAP
PIPE

//----------------------------------------------------------
//                       BASE 
//----------------------------------------------------------

data: T   | WRITE(end="\n")               -> T        // imprime um conteúdo
data: T   | ASSERTEQ(expected: T, label="") -> T        // verifica de dois valores são iguais
data: T   | PIPE(fn: T -> K)              -> K        // empacota uma função num PIPE
size: int | RANGE(init: T, inc = 1)        -> vector<T>// gera um vetor sequencial
//----------------------------------------------------------
//                   PARA STRINGS
//----------------------------------------------------------

data: any    | TOSTR(cfmt="")               -> str      // converte qualquer coisa para string e formata
fmt : str    | FORMAT(args...)            -> str      // formata uma string com printf
text: str    | CAT(text: str)             -> str      // concatena duas strings
container<T> | JOIN(sep = "", brakets="") -> str      // concatena os elementos de um container 

//----------------------------------------------------------
//                  DE STRINGS
//----------------------------------------------------------

text: str | STRTO<T>()                -> T             // transforma uma string em um tipo específico
text: str | SPLIT(del=' ')           -> vector<str>   // dado um delimitador, separa em vetor de strings
text: str | UNPACK<T...>(del)       -> tuple<T...>   // dado um delimitador, separa em uma tupla 
text: str | CSTR()                   -> const char *  // pega uma string gera um const char*

//----------------------------------------------------------
//             container<T> -> vector<T>  
//----------------------------------------------------------

cont<T> | FILTER(fn: T -> bool)      -> vector<T>     // filtra os elementos que satisfazem a função
cont<T> | SLICE(init, end)           -> vector<T>     // copia uma parte do vetor
cont<T> | DROP(qtd)                  -> vector<T>     // remove os primeiros elementos
cont<T> | TAKE(qtd)                  -> vector<T>     // pega os primeiros elementos
cont<T> | REVERSE()                  -> vector<T>     // inverte a ordem dos elementos
cont<T> | SHUFFLE()                  -> vector<T>     // embaralha os elementos
cont<T> | SORT()                     -> vector<T>     // ordena os elementos
cont<T> | SORTBY(fn)                 -> vector<T>     // ordena os elementos com base em uma função
cont<T> | UNIQUE()                   -> vector<T>     // remove elementos duplicados

//----------------------------------------------------------
//             cont<T> -> vector<K>
//----------------------------------------------------------

cont<a>        | MAP(fn: a -> b)     -> vector<b>     // aplica uma função em todos os elementos
cont<pair<a,b>>| KEYS()              -> vector<a>     // pega as chaves de um cont de pares
cont<pair<a,b>>| VALUES()            -> vector<b>     // pega os valores de um cont de pares

//----------------------------------------------------------
//             cont para valor
//----------------------------------------------------------
       
cont<a> | INDEXOF(value: a)            -> int         // retorna o índice de um elemento ou -1
cont<a> | FOLD(fn: (b,a) -> b, acc: b) -> b           // aplica uma função acumulativa dado valor inicial
cont<a> | FOLD1(fn: (a,a) -> a)        -> a           // aplica uma função acumulativa
cont<a> | SUM()                        -> a           // soma todos os elementos
cont<a> | FOREACH(fn: a -> void)       -> void        // aplica uma função em todos os elementos
cont<a> | FINDIF(fn: a -> bool)        -> optional<a> // retorna o primeiro elemento que satisfaz a função

//----------------------------------------------------------
//             ZIP
//----------------------------------------------------------

cont<a> | ZIP    (cont<b>)               -> vector<pair<a,b>> // zipa dois containers em um cont de pares
cont<a> | ZIPWITH(cont<b>, fn: (a,b)->c) -> vector<c>         // zipa dois containers usando uma função

```

## Documentação

Os exemplos de uso utilizam o modelo de formatação de string definido em:

```cpp

//esse include é obrigatório para rodar os exemplos para habilitar a criação
//de strings utilizando o 
using namespace std::string_literals;

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