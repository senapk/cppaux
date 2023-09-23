#!/bin/python3

guides = {}

guides["join"] = r"""
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
"""

guides["tostr"] = r"""
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
 */
"""

guides["format"] = r"""
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
"""

guides["print"] = r"""
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
"""

guides["write"] = r"""
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
 * @note `param: end` (opcional) String a ser inserida no final da linha.
 * @note `return` O dado original recebido.
 * 
 * @note #### Exemplos
 * @note `write(vector<int> {1, 2, 3}); // [1, 2, 3]`
 * @note `vector<int> {1, 2, 3} | WRITE(); // [1, 2, 3]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#write
 */
"""

guides["slice1"] = r"""
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
"""

guides["slice2"] = r"""
/**
 * @note #### `slice(CONTAINER<T> container, int begin, int end) -> vector<T>`
 * @note #### `CONTAINER<T> container| SLICE(int begin, int end) -> vector<T>`
 * 
 * @note - Fatia um container retornando um `vector` com os elementos copiados.
 * @note - Funciona como a função `slice` do Python ou do Javascript. 
 * @note - Os índices podem ser negativos para indicar que a contagem deve ser feita a partir do final.
 * 
 * @note #### Parâmetros
 * @note `param: container` container a ser fatiado.
 * @note `param: begin` índice inicial.
 * @note `param: end` índice final.
 * @note `return` vector com os elementos copiados.
 * 
 * @note #### Exemplos
 * @note `vector<int> vet {1, 2, 3, 4, 5};`
 * @note `slice(vet, 1, -1) | WRITE(); // [2, 3, 4]`
 * @note `vet| SLICE(1, -1) | WRITE(); // [2, 3, 4]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#slice
 */
"""

guides["map"] = r"""
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
"""

guides["filter"] = r"""
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
"""

guides["range2"] = r"""
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
"""

guides["range1"] = r"""
/**
 * @note #### `range(int end) -> vector<int>`
 * 
 * @note - Gera um vetor de inteiros de 0 até end, mas não incluindo end, com passo 1.
 * 
 * @note #### Parâmetros
 * @note `param: end` limite superior.
 * @note `return` vetor de inteiros.
 * 
 * @note #### Exemplos
 * @note `range(10) | WRITE(); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]`
 * 
 * @note #### Veja  mais exemplos em https://github.com/senapk/cppaux#range
 */
"""

guides["enumerate"] = r"""
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
"""

guides["strto"] = r"""
/**
 * @note #### `strto<READABLE>(str value) -> READABLE`
 * @note #### `str value|STRTO<READABLE>()-> READABLE`
 * 
 * @note - Transforma de string para o tipo solicitado utilizando o operador de extração de stream.
 * @note - Dispara uma exceção caso a conversão não seja possível.
 * 
 * @note #### Parâmetros
 * @note `param: READABLE` Tipo a ser convertido.
 * @note `param: value` String a ser convertida.
 * @note `return` Valor convertido.
 * @note `throws: std::runtime_error` Caso a conversão não seja possível.
 * 
 * @note #### Exemplos
 * @note `strto<int>("1") | WRITE(); // 1`
 * @note `strto<double>("1.2") | WRITE(); // 1.2`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#strto
 * 
*/
"""

guides["number"] = r"""
/**
 * @note #### `number(str value) -> double`
 * 
 * @note - Transforma de string para double utilizando a função strto.
 * 
 * @note #### Parâmetros
 * @note `param: value` String a ser convertida.
 * @note `return` Valor convertido para double.
 * @note `throws: std::runtime_error` Caso a conversão não seja possível.
 * 
 * @note #### Exemplos
 * @note `number("1.23") | WRITE(); // 1.23`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#number
 * 
*/
"""

guides["unpack"] = r"""
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
"""

guides["zip"] = r"""
/**
 * @note #### `zip(CONTAINER_A<T> container_a, CONTAINER_B<K> container_b) -> vector<pair<T, K>>`
 * @note #### `CONTAINER_A<T> container_a| ZIP(CONTAINER_B<K> container_b) -> vector<pair<T, K>>`
 * 
 * @note - Une dois containers em um vetor de pares limitado ao menor tamanho dos dois containers.
 * 
 * @note #### Parâmetros
 * @note `param: container_a` Primeiro container.
 * @note `param: container_b` Segundo container.
 * @note `return` Vetor de pares.
 * 
 * @note #### Exemplos
 * @note `zip(vector<int>{1, 2, 3}, string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]`
 * @note `vector<int>{1, 2, 3}| ZIP(string("pterodactilo")) | WRITE(); //[(1, p), (2, t), (3, e)]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#zip
 */
"""

guides["zipwith"] = r"""
/**
 * @note #### `zipwith(CONTAINER<T> container_a, CONTAINER<K> container_b, FUNCTION fnjoin) -> vector<fnjoin(T, K)>`
 * @note #### `CONTAINER<T> container_a| ZIPWITH(CONTAINER<K> container_b, FUNCTION fnjoin) -> vector<fnjoin(T, K)>`
 * 
 * @note - Une dois containers através da aplicação da função fnjoin em cada par limitado ao menor tamanho dos dois containers.
 * 
 * @note #### Parâmetros
 * @note `param: container_a` Primeiro container.
 * @note `param: container_b` Segundo container.
 * @note `param: fnjoin` Função a ser aplicada em cada par.
 * @note `return` Vetor com os resultados.
 * 
 * @note #### Exemplos
 * @note `auto concat = [](auto x, auto y) { return tostr(x) + y; };`
 * @note `zipwith(vector<int>{1, 2, 3}, string("pterodactilo"), concat) | WRITE(); // ["1p", "2t", "3e"]`
 * @note `zipwith(range(10), "pterodactilo"s, concat) | WRITE(); // ["0p", "1t", "2e", "3r", "4o", "5d", "6a", "7c", "8t", "9i"]`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#zipwith
 * 
 */
"""

guides["split"] = r"""
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
"""

guides["input"] = r"""
/**
 * @note #### `input(istream source = std::cin) -> str`
 * 
 * @note - Extrai uma linha inteira e retorna como string.
 * @note - O padrão é o std::cin, mas pode ser um fluxo ou arquivo.
 * @note - Se não houver mais linhas, lança uma exceção.
 * 
 * @note #### Parâmetros
 * @note `param: source` Origem da linha.
 * @note `return` Linha lida.
 * @note `throws: std::runtime_error` Caso não haja mais linhas.
 * 
 * @note #### Exemplos
 * @note `auto line = input();`
 * 
 * @note #### Veja mais exemplos em https://github.com/senapk/cppaux#input
 */
"""

guides["operator+"] = r"""
/**
 * @note #### `+str -> double`
 * 
 * @note - Transforma de string para double utilizando a função number.
 * 
 * @note #### Parâmetros
 * @note `param: str` String a ser convertida.
 * @note `return` Valor convertido para double.
 * 
 * @note #### Exemplos
 * @note `+"1.23"s | WRITE(); // 1.23`
 * 
*/
"""

lines = open("fn.hpp").read().split("\n")

output = []
size = len(lines)
i = 0
while (i < size):
    line = lines[i]
    if line.startswith("//__guide"):
        output.append(line)
        label = line.split(" ")[1]
        i += 1
        if i < size and lines[i].startswith("//"):
            output.append(lines[i])
            i += 1
        data = guides[label].split("\n")[1:-1]
        output += data
    else:
        output.append(line)
        i += 1

with open("fn.hpp", "w") as f:
    f.write("\n".join(output))


