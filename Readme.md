# FAKEFN for c++

[](toc)

- [Como utilizar as biblioteca](#como-utilizar-as-biblioteca)
- [Resumo](#resumo)
- [Documentação](#documentação)
  - [PRINT](#print)
  - [IOTA](#iota)
[](toc)

## Como utilizar

- TODAS as funções dessa biblioteca são **functors**, ou seja, objetos que podem ser chamados como funções.
- Os objetos são criados por meio de um construtor e podem ser chamados como funções ou via pipe |.
- Por exemplo, a função STR que é utilizada para gerar a string de um objeto, pode ser chamada como função ou via pipe.

```cpp
    //como função, esse primeiro par de parênteses é obrigatório
    STR()(5.7) // "5.7"
    STR()(std::vector<int>{1,2,3,4,5}) // "[1, 2, 3, 4, 5]"
    //ou
    5.7 | STR() // "5.7"
    std::vector<int>{1,2,3,4,5} | STR() // "[1, 2, 3, 4, 5]"

    //se for passado algum parâmetro para o STR(), 
    //ele utiliza para formatação usando as regras do printf
    5.7 | STR("%.2f") // "5.70"
    STR("%.3f")(5.7) // "5.700"

    //se aplicado a algum container, ele aplica a função para cada elemento
    std::vector<int>{1,2,3,4,5} | STR("%03d") // "[001, 002, 003, 004, 005]"
    STR("%03d")(std::vector<int>{1,2,3,4,5})  // "[001, 002, 003, 004, 005]"
```

## Resumo

```cpp

//----------------------------------------------------------
//                       BASE 
//----------------------------------------------------------

data: T   | PRINT(end="\n")               -> T              // imprime um conteúdo
data: T   | ASSERT(expected: T, label="") -> T              // verifica de dois valores são iguais
data: T   | PIPE(fn: T -> K)              -> K              // empacota uma função num PIPE
size: int | IOTA(init: T, inc = 1)        -> vector<T>      // gera um vetor sequencial

//----------------------------------------------------------
//                   PARA STRINGS
//----------------------------------------------------------

data: any     | STR(cfmt="")               -> str           // converte qualquer coisa para string e formata com printf
fmt : str     | FORMAT(args...)            -> str           // formata uma string com printf
text: str     | CAT(text: str)             -> str           // concatena duas strings
container<T>  | JOIN(sep = "", brakets="") -> str           // concatena os elementos de um container com um separador

//----------------------------------------------------------
//                  DE STRINGS
//----------------------------------------------------------

text: str | STR2<T>                -> T                     // transforma uma string em um tipo específico
text: str | SPLIT(del=' ')         -> vector<str>           // dado um delimitador, separa uma string em um vetor de strings
text: str | TUPLEFY<types...>(del) -> tuple<types...>       // dado um delimitador, separa uma string em uma tupla 
text: str | CSTR()                 -> const char *          // pega uma string gera um const char*

//----------------------------------------------------------
//             container<T> -> vector<T>  
//----------------------------------------------------------

container<T> | FILTER(fn: T -> bool) -> vector<T>           // filtra os elementos que satisfazem a função
container<T> | SLICE(init, end)      -> vector<T>           // copia uma parte do vetor
container<T> | DROP(qtd)             -> vector<T>           // remove os primeiros elementos
container<T> | TAKE(qtd)             -> vector<T>           // pega os primeiros elementos
container<T> | REVERSE()             -> vector<T>           // inverte a ordem dos elementos
container<T> | SHUFFLE()             -> vector<T>           // embaralha os elementos
container<T> | SORT()                -> vector<T>           // ordena os elementos
container<T> | SORTBY(fn)            -> vector<T>           // ordena os elementos com base em uma função
container<T> | UNIQUE()              -> vector<T>           // remove elementos duplicados

//----------------------------------------------------------
//             container<T> -> vector<K>
//----------------------------------------------------------

container<a>        | MAP(fn: a -> b) -> vector<b>          // aplica uma função em todos os elementos gerando um novo vetor
container<pair<a,b>>| KEYS()          -> vector<a>          // pega as chaves de um container de pares
container<pair<a,b>>| VALUES()        -> vector<b>          // pega os valores de um container de pares

//----------------------------------------------------------
//             container para valor
//----------------------------------------------------------
       
container<a> | INDEXOF(value: a)             -> int         // retorna o índice de um elemento ou -1
container<a> | FOLD(fn: (b, a) -> b, acc: b) -> b           // aplica uma função acumulativa em todos os elementos dado um valor inicial
container<a> | FOLD1(fn: (a, a) -> a)        -> a           // aplica uma função acumulativa em todos os elementos
container<a> | SUM()                         -> a           // soma todos os elementos
container<a> | FOREACH(fn: a -> void)        -> void        // aplica uma função em todos os elementos
container<a> | FINDIF(fn: a -> bool)         -> optional<a> // retorna o primeiro elemento que satisfaz a função ou nullopt

//----------------------------------------------------------
//             ZIP
//----------------------------------------------------------

container<a> | ZIP    (container<b>)               -> vector<pair<a,b>> // zipa dois containers em um container de pares
container<a> | ZIPWITH(container<b>, fn: (a,b)->c) -> vector<c>         // zipa dois containers usando uma função

```

## Documentação

### PRINT

- Ação: Imprime o valor do objeto.
- Pipe: qualquer variável ou container a ser impressa.
- Args: fim da linha(opcional).
- Retorna: a própria variável que foi passada pelo pipe.

```c++
data: T | PRINT(end="\n") -> T
```

```c++

// funciona com primitivos
5 | PRINT(); //5
4.5 | PRINT(); //4.5

// e quaisquer combinação de containers e tipos primitivos
std::vector<int>{1,2,3}         | PRINT(); //[1, 2, 3]
std::list<float>{1.5,2.5, 3.5}  | PRINT(); //[1.5, 2.5, 3.5]
std::make_pair("ovo", "queijo") | PRINT(); //(ovo, queijo)
std::make_tuple(1,2.3,"tres")   | PRINT(); //(1, 2.3, tres)

// após impressão, ele devolve o valor original
// dá pra mudar o fim de linha passando um argumento
5   | PRINT(" eh cinco\n")                 //5 eh cinco
    | PRINT(" nao eh seis\n");             //5 nao eh seis

// para imprimir classes, é necessário sobrecarregar a função
// std::ostream& operator<<(std::ostream& os, const T& t)
std::vector<Pessoa> {{"Joao", 5}, {"Maria", 6}} 
                    | PRINT(); //[Joao:5, Maria:6]
```

### IOTA

- Ação: Gera um vector com valores sequenciais.
- Pipe: Tamanho do vector.
- Args: Início da sequência e incremento(opcional).
- Retorna: O vetor gerado que tem o tipo do init.

```c++
- size: int | IOTA(init: T, inc = 1) -> vector<T>
```

```c++
4
```

