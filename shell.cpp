#include <iostream>
#include <aux.hpp>

using namespace aux;

int main(){
    Chain chain;
    Param ui;
    
    chain["soma"] = [&]() { 
        show << (to<int>(ui[1]) + to<int>(ui[2])); 
    };
    chain["sub"]  = [&]() {
        show << (to<int>(ui[1]) - to<int>(ui[2])); 
    };

    execute(chain, ui);
    return 0;
}