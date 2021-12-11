#include <iostream>
#include <fstream>
#include <vector>
#include <x86intrin.h>

#define N 100

void mult_matrix(float* A, float* B, float* C){
    std::cout << "Preparing CPU ..." << std::endl;
    for (int i = 0; i < N; ++i) {
// инициализация нулями        
        for (int j = 0; j < N; ++j) 
            C[i * N + j] = 0;
        
        for (int k = 0; k < N; ++k) 
            for (int j = 0; j < N; ++j) 
                C[i * N + j] += A[i * N + k] * B[k * N + j];
        
    }
    if (C[N * N - 1] >= 0) 
        std::cout << "CPU ready!" << std::endl; 
}
// используется алгоритм умножения матриц
void cpuPrepare(){
    float* A = new float[N * N];
    float* B = new float[N * N];
    float* C = new float[N * N];
    for (int i = 0; i < N * N; ++i) {
        A[i] = i;
        B[i] = i;
    }
    mult_matrix(A, B, C);
    delete[] A;
    delete[] B;
    delete[] C;
}

double takeTime(uint32_t* array, uint32_t arrayLength) {
    uint32_t  k = 0;
// прогрев процессора
    cpuPrepare();
// "прогрев" кэша
    for (uint32_t i = 0; i < arrayLength; ++i)
        k = array[k];   
// для большей точности прогоняем несколько раз
    uint32_t K = 10;
    double time = UINT64_MAX, start, end;
    for(uint8_t i = 0; i < K; ++i){
        start = __rdtsc();
        for (uint32_t i = 0; i < arrayLength; ++i) {
            k = array[k];
    // некоторый мусор, чтобы процессором не был проигнорирован 
    // цикл, время работы которого измеряем
            if (k == 4) 
                std::cout << ".";   
        }
        end = __rdtsc();
        if(time > (end - start) / arrayLength)
            time = (end - start) / arrayLength;
    }
    return time;
}

void print(std::ofstream& output, uint32_t* array, uint32_t arrayLength){ 
    output << takeTime(array, arrayLength) << std::endl;
}

// прямой обход
void FillStrait(uint32_t* array, uint32_t arrayLength){
    for (uint32_t i = 0; i < arrayLength; ++i)
        array[i] = i + 1;
    
    array[arrayLength - 1] = 0;
    std::ofstream output;
    output.open("dstraight.txt", std::ios::out | std::ios::app);
    if(!output){
        std::cout << "File straight.txt can't be opened" << std::endl;
        return;
    }   

    print(output, array, arrayLength);

    output.close();
}

// обратный обход
void FillReverse(uint32_t* array, uint32_t arrayLength){
    for (uint32_t i = 1; i < arrayLength; ++i)
        array[i] = i - 1;
    
    array[0] = arrayLength - 1;
    std::ofstream output;
    output.open("dreverse.txt", std::ios::out | std::ios::app);
    if(!output){
        std::cout << "File reverse.txt can't be opened" << std::endl;
        return;
    }    
    
    print(output, array, arrayLength);

    output.close();
}

// случайный обход
void FillRand(uint32_t* array, uint32_t arrayLength){
    srand(time(NULL));
    uint32_t count = 0;
    uint32_t size = arrayLength;
    for (size_t i = 0; i < arrayLength; ++i)
        array[i] = i;
    while(count < arrayLength){
        std::swap(array[count], array[count + (uint32_t)rand()%size]);
        --size;
        ++count;
    }
    bool* used = new bool [arrayLength];
    for (size_t i = 0; i < arrayLength; i++)
        used[i] = false;
    bool isCycled = false;
    size_t iterator;
    size_t lastIdx = -1;
    while(!isCycled){
        iterator = 0;
        while (iterator < size && used[iterator])        
            iterator++;        
        if (iterator == size){
            array[lastIdx] = 0;
            isCycled = true;
        }
        else
        {
            if (lastIdx != -1)
                array[lastIdx] = iterator;
            
            while (!used[iterator]){
                used[iterator] = true;
                lastIdx = iterator;
                iterator = array[iterator];
            }
        }
    }
    delete[] used;
    std::ofstream output;
    output.open("drand.txt", std::ios::out | std::ios::app);
    if(!output){
        std::cout << "File rand.txt can't be opened" << std::endl;
        return;
    }   

    print(output, array, arrayLength);

    output.close();
}

int main(){
    uint32_t arrayLength = 256;    
// основной цикл
    while (arrayLength < 256 * 1024 * 32)
    {
        uint32_t* array = new uint32_t[arrayLength];
        FillStrait(array, arrayLength);
        FillReverse(array, arrayLength);
        FillRand(array, arrayLength);       
        std::cout << arrayLength << std::endl;
        arrayLength *= 1.2; 
        delete[] array;
    }    
    std::cout << "Data generation over" << std::endl;
    return 0;
}
