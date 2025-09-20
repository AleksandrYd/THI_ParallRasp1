#include "header.h"

const uint_fast64_t end = 10000000000;

uint_fast64_t serial_proccess()
{
    uint_fast64_t sum = 0;

    for (uint_fast64_t i = 1; i <= end; i++)
    {
        sum += i;
    }

    return sum;
}

int main()
{
    char mode;
    

    setlocale(LC_ALL, "Russian");
    while (1)
    {
        std::cout << "Что проверяем? (а бэ цэ дэ)\n";
        std::cin >> mode;
        switch (mode)
        {
            case 'a':
            {
                float avg_time = 0;
                for (int attempt = 0; attempt < 10; attempt++)
                {
                    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

                    if (attempt == 9)
                    {
                        std::cout << "Полученная сумма : " << serial_proccess() << '\n';
                    }
                    else
                        serial_proccess();

                    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
                    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                    avg_time += duration.count() / (float)1000000;
                    std::cout << "Время выполнения " << attempt << "-ой попытки: " << (float)duration.count() / (float)1000000 << " сек\n";
                }

                std::cout << "Среднее время выполнения с использованием 1 потока: " << avg_time / (float)10 << '\n';
                break;
            }
            case 'b':
            {
                MultiThreadingProccess(end);
                break;
            }
            case 'c':
            {
                MultiThreadingProccessWithVector(end);
                break;
            }
            case 'd':
                MatrixProccess();
                break;
            
        }
    }
}
