#include <iostream>
#include <fstream>

#define N 1000

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

unsigned long long takeTime(uint32_t* array, uint32_t arrayLength) {
	uint32_t  k = 0;
// "прогрев" кэша
	for (uint32_t i = 0; i < arrayLength; ++i)
		k = array[k];	
// примитивный таймер
	union ticks {
		unsigned long long t64;
		struct s32 {
			long th, tl;
		} t32;
	} start, end;
// для большей точности прогоняем несколько раз
	uint32_t K = 50;
	asm("rdtsc\n" : "=a"(start.t32.th), "=d"(start.t32.tl));
//\/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/
	for (uint32_t i = 0; i < arrayLength * K; ++i) {
		k = array[k];
	}
//\/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/
	asm("rdtsc\n" : "=a"(end.t32.th), "=d"(end.t32.tl));
// некоторый мусор, чтобы процессором не был проигнорирован 
// цикл, время работы которого измеряем
	if (k == 4) 
		std::cout << "chetuiriy";	
	return (end.t64 - start.t64) / (arrayLength * K);
}

void print(std::ofstream& output, uint32_t* array, uint32_t arrayLength){    
    if(arrayLength / 256 / 1024) {
        output << arrayLength / 256 / 1024 << " Mbyte: " << takeTime(array, arrayLength) << std::endl;
    } else if(arrayLength / 256) {
        output << arrayLength / 256 << " Kbyte: " << takeTime(array, arrayLength) << std::endl;
    } else {
        output << arrayLength * 4 << " byte: " << takeTime(array, arrayLength) << std::endl;
    }
}


// прямой обход
void FillStrait(uint32_t* array, uint32_t arrayLength){
	for (uint32_t i = 0; i < arrayLength - 1; ++i)
		array[i] = i + 1;
	
	array[arrayLength - 1] = 0;
    std::ofstream output;
    output.open("straight.txt", std::ios::out | std::ios::app);
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
    output.open("reverse.txt", std::ios::out | std::ios::app);
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
    bool* bitMap = new bool[arrayLength];
    for (uint32_t i = 0; i < arrayLength; ++i)
        bitMap[i] = false;
    uint32_t next_index = rand() % arrayLength;
    for(uint32_t i = 0; i < arrayLength; ++i){
        while (bitMap[next_index])
            next_index = rand() % arrayLength;
        bitMap[next_index] = true;
        array[i] = next_index;
    }
    std::ofstream output;
    output.open("rand.txt", std::ios::out | std::ios::app);
    if(!output){
        std::cout << "File rand.txt can't be opened" << std::endl;
        return;
    }   

    print(output, array, arrayLength);

    output.close();
    delete[] bitMap;
}

int main(){
// обязательный "разогрев" процессора
    cpuPrepare();
    uint32_t arrayLength = 32;    
// основной цикл
    while (arrayLength < 900000)
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