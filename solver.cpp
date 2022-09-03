#include <iostream>
#include <sstream>
// #include "auxiliar.hpp"
#include <vector>

#include "aux.hpp"

using namespace aux;

struct Pessoa {
    std::string nome;
    int idade;

    //só funciona se o tipo tiver :
    //  1) construtor padrão
    //  2) sobrescrita de entrada 
    //  3) sobrescrita de de saída

    Pessoa(std::string nome = "", int idade = 0): nome{nome}, idade{idade} {}

    
    friend std::ostream& operator<<(std::ostream &os, Pessoa p) {
        return os << p.nome << ":" << p.idade;
    }

    friend std::istream& operator>>(std::istream &is, Pessoa &p) {
        std::getline(is, p.nome, ':');
        is >> p.idade;
        return is;
    }
};

void test_fmt() {
    show << (fmt(true) == "true");
    show << (fmt(false) == "false");
    show << (fmt(3.4123) == "3.41");
    show << (fmt(Pessoa("ana", 8)) == "ana:8");
    show << (fmt(1) == "1");
    // com vetores é possivel definir o separador
    show << (fmt(std::vector<int>{1,2,3,4}) == "[1,2,3,4]");
    //
    show << (fmt(std::vector<int>{1,2,3,4}, ", ") == "[1, 2, 3, 4]"); //imprimindo com separador ", "
    show << (fmt(std::vector<Pessoa>{{"ana", 1}, {"julia", 2}}) == "[ana:1,julia:2]");
}

void test_to() {
    show << (to<int>("1") == 1);
    show << (to<double>("1.2432423") == 1.2432423);
    show << (to<bool>("true") == true);
    show << (to<bool>("false") == false); //qualquer coisa diferente de "true" é false
}

void test_to_vet() {
    //o to_vet usa o separador default do ',' 
    auto vet = to_vet<int>("[1,2,3,4]");
    show << vet; 

    //passando o separador como -
    show << fmt(to_vet<Pessoa>("[ana:1-julia:2]", '-'), ", "); //[ana:1, julia:2]
}

void test_join() {
    auto vet = std::vector<Pessoa>{{"ana", 1}, {"julia", 2}, {"fca", 4}};
    std::string nomes = join(FX(x.nome), vet);
    show << nomes;
}

void test_slice() {
    auto vet = std::vector<int> {1,2,3,4,5,6};
    show << slice(vet, 1, -1);
}

void test_clist() {
    std::vector<std::pair<std::string, int>> vet {{"ana", 1}, {"julia", 2}, {"fca", 4}};
    //pegar os nomes dobrados de quem tem menos de 3 anos
    auto fn = [](auto x) -> std::string {return x.first;};
    auto pred = [](auto x) -> bool {return x.second < 5;};
    auto nomes = clist(fn, vet, pred);
    show << fmt(nomes);
}

//os nomes de quem tem mais de 2 anos
void test_clist2() {
    std::vector<Pessoa> vet {{"bel", 7}, {"ana", 1}, {"julia", 2}, {"fca", 4}};
    //pegar os nomes dobrados de quem tem menos de 3 anos
    show << clist(FX(x.nome + " silva"), vet, FX(x.idade > 2)); //["bel silva", "fca silva"]

}

//as pessoas que tem letra inicial menor que 'c'
void test_clist3() {
    std::vector<Pessoa> vet {{"bel", 7}, {"ana", 1}, {"julia", 2}, {"fca", 4}};
    auto pessoas = clist(IDX, vet, FX(x.nome[0] < 'c'));
    show << fmt(pessoas);
}

//os nomes a partir de um vetor de ponteiros
void test_clist4() {
    std::vector<Pessoa*> vet {new Pessoa{"bel", 7}, new Pessoa{"ana", 1}, new Pessoa{"julia", 2}, new Pessoa{"fca", 4}};
    auto nomes = clist(FX(x->nome), vet);
    show << fmt(nomes);
}

void test_to_vet2() {
    auto vint = FX(to_vet<int>(x));
    show << vint("[1,2,3,4]");
}

int main() {
    test_fmt();
    test_to();
    test_to_vet();
    test_slice();
    test_clist();
    test_clist2();
    test_clist3();
    test_clist4();
    test_to_vet2();

    show << std::vector<int>{1, 2, 3, 4};
}