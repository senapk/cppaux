#include <iostream>
#include <sstream>
#include "auxiliar.hpp"
#include <vector>
#include <list>
#include <memory>

struct Pessoa {
    std::string nome;
    int idade;
    Pessoa(std::string nome, int idade): nome{nome}, idade{idade} {}
    friend std::ostream& operator<<(std::ostream &os, Pessoa p) {
        return os << p.nome << ":" << p.idade;
    }
};

int main() {
    // {
    //     std::vector<Pessoa> vet {Pessoa("ana", 13), Pessoa("eva", 15), Pessoa("rui", 56)};
    //     auto menores = aux::filter(vet, [](auto& p){return p.idade < 18;});
    //     std::cout << aux::format(menores) << '\n';
    // }
    // {
    //     std::vector<int*> vet {new int(1), new int(5), new int(8)};
    //     std::cout << aux::format(vet) << '\n';
    // }

    {
        std::vector<int> vet {1, 5, 8};
        auto size = aux::foldl(vet, 0, [](auto& acc, auto){ return acc + 1; });
        std::cout << size << '\n';
    }

}