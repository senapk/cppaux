---
marp: true
---
<!-- markdownlint-disable MD024 -->

# FN.HPP

## Açucar sintático para funções de formatação de strings e operações básicas de programação funcional em C++

### O objetivo dessa biblioteca é didático e não teve ser utilizado em sistemas que requerem alta performance

---

# std::string_literals estão habilitados por padrão

## C++ padrão

```cpp
#include <iostream>
int main() {
    auto texto1 = "hoje é dia de festa"; // const char*
    auto texto2 = std::string("hoje é dia de festa"); // std::string
}
```

## fn.hpp

```cpp
#include "fn.hpp" // using namespace std::string_literals;
int main() {
    auto texto1 = "hoje é dia de festa"; // const char*
    auto texto2 = "hoje é dia de festa"s; // std::string
}
```

---

## Conversão de string para inteiros como no JavaScript

## C++ padrão

```cpp
#include <iostream>
int main() {
    auto numero1 = std::stoi("42"); // int
    auto numero2 = std::stoll("42"); // long long
}
```

## fn.hpp

```cpp
#include "fn.hpp"
int main() {
    auto numero1 = +"42"; // double
    auto numero2 = (int)+"42"; // int
    auto numero3 = fn::strto<int>("42"); // int
    auto numero4 = fn::strto<double>("42"); // double
}
```

---

# Conversão de um tipo qualquer para string

## C++ padrão

```cpp
#include <iostream>
int main() {
    auto texto = std::to_string(42); // std::string
    auto vetor = std::to_string(std::vector<int>{1, 2, 3}); // ERRO
}
```

## fn.hpp

```cpp
#include "fn.hpp"
int main() {
    auto texto = fn::tostr(42); // std::string
    auto vetor = fn::tostr(std::vector<int>{1, 2, 3}); // [1, 2, 3]
}
```

---

# Gerando Strings Formatadas C++ padrão

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
int main() {
    int dia = 7; double peso = 65.5;

    // enviando para um fluxo
    std::stringstream ss;
    ss << "Dia " << std::setw(2) << std::setfill('0') << dia 
       << " - " << std::fixed << std::setprecision(2) << peso << " kg\n";
    auto texto = ss.str(); // coletando o texto gerado
    // enviando para a saída
    std::cout << texto; // Dia 07 - 65.50 kg
}
```

---

# Gerando Strings Formatadas FN.HPP

```cpp
#include "fn.hpp"
int main() {
    int dia = 7; double peso = 65.5;

    // enviando diretamente para a saída
    fn::print("Dia {%02d} - {%.2f} kg\n", dia, peso); // Dia 07 - 65.50 kg
    
    // gerando primeiro uma string
    auto texto = fn::format("Dia {%02d} - {%.2f} kg\n", dia, peso);
    std::cout << texto; // Dia 07 - 65.50 kg
}
```

---

# Como funcionam os placeholders no FN.HPP

```cpp
#include "fn.hpp"
int main() {
    // fn::print envia diretamente para a saída
    // fn::format gera uma string, os parâmetros são iguais em ambos

    // o {} chama a função fn::tostr
    // se houver um parâmetro, ele utiliza a mesma regra do prinf

    fn::print("Hoje é {}, dia {%02d}\n", "sábado", 7); // Hoje é sábado, dia 07
    
    // Funciona para array, vector, set, map, pair, tuple, etc
    std::vector<int> vetor{1, 2, 3};
    std::map<int, std::string> mapa{{1, "um"}, {2, "dois"}};
    fn::print("vetor: {}\n", vetor); // vetor: [1, 2, 3]
    fn::print("mapa: {}\n", mapa); // mapa: [(1, um), (2, dois)]
}
```

---

# split e join

```cpp
#include "fn.hpp"

int main() {
    auto texto = "um,dois,tres";
    auto vetor = fn::split(texto, ','); // std::vector<std::string>
    fn::print("vetor: {}\n", vetor); // vetor: [um, dois, tres]

    auto texto2 = fn::join(vetor, "-");
    fn::print("texto2: {}\n", texto2); // texto2: um-dois-tres
}
```

---

# Formatando containers

```cpp
#include "fn.hpp"
int main() {
    std::vector<double> vet = {1.1, 2.2, 3.3};
    auto out = fn::tostr(vet); // [1.1, 2.2, 3.3]

    // fn::print utiliza a função fn::tostr para formatar os containers
    fn::print("vetor: {}\n", vet); // vetor: [1.1, 2.2, 3.3]

    // parâmetros de formatação são aplicados a cada elemento
    fn::print("vetor: {%.2f}\n", vet); // vetor: [1.10, 2.20, 3.30]

    // utilizando o  join para formatação personalizada
    fn::print("vetor: [ {} ]\n", fn::join(vet, " - ")); // vetor: [ 1.1 - 2.2 - 3.3 ]
}
```

---

# Simplificando entrada e saída com input e write

```cpp
#include "fn.hpp"
int main() {
    // ler dois números, um por linha e escrever a soma
    // input retorna a linha inteira como string
    auto a = +fn::input(); // double
    auto b = +fn::input(); // double

    // o write chama a função fn::tostr para o parâmetro
    fn::write(a + b); // escreve a soma com uma quebra de linha
    fn::write(a + b, ""); //escreve sem quebra de linha
}
```

---

# filter, map, slice

```cpp
#include "fn.hpp"
int main() {
    std::vector<int> vet = {1, 2, 3, 4, 5};

    auto pares = fn::filter(vet, [](int x) { return x % 2 == 0; }); // [2, 4]
    auto dobro = fn::map(vet, [](int x) { return x * 2; }); // [2, 4, 6, 8, 10]
    auto middle = fn::slice(vet, 1, -1); // [2, 3, 4]  como em Python
}
```

---

# Escrevendo em PIPELINE - Exemplo 1

Todas as funções da biblioteca tem uma variante em maiúsculo que pega a primeira entrada da função a partir da saída da função anterior.

```cpp
#include "fn.hpp"
using namespace fn;
int main() {
    std::vector<int> vet = {1, 2, 3, 4, 5};
    write(vet);    // [1, 2, 3, 4, 5]
    vet | WRITE(); // [1, 2, 3, 4, 5]

    auto data1 = format("{%02d}:{%02d}:{%02d}", 1, 2, 3);  // 01:02:03
    auto data2 = "{%02d}:{%02d}:{%02d}" | FORMAT(1, 2, 3); // 01:02:03
    write(data1);    // 01:02:03
    data2 | WRITE(); // 01:02:03
}
```

---

# Escrevendo em PIPELINE - Exemplo 2

```cpp
#include "fn.hpp"
using namespace fn;

int main() {
    // ler uma linha com números de números
    // quebrar em palavras, descartar o primeiro, 
    // transformar em números, pegar apenas os ímpares e imprimir

    input() // "1 2 3 4 5"
        | SPLIT(' ') // retorna um vetor de strings ["1", "2", "3", "4", "5"]
        | SLICE(1)   // descarta o primeiro elemento ["2", "3", "4", "5" ]
        | MAP(strto<int>) // retorna um vetor de inteiros [2, 3, 4, 5]
        | FILTER([](int x) { return x % 2 == 1; }) // pega apenas os ímpares [3, 5]
        | WRITE(); // [3, 5]
}
```
