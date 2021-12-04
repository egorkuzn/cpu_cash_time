#include <iostream>
#include <fstream>

using namespace std;

int main(){
// обязательный "разогрев" процессора
    cpuPrepare();
    size_t arrayLength = 1;    
// основной цикл
    while (arrayLength < 900000)
    {
        uint32_t* array = new uint32_t[arrayLength];
        FillStrait(array, arrayLength);
        FillReverse(array, arrayLength);
        FillRand(array, arrayLength);        
        delete[] array;
    }
    
    return 0;
}