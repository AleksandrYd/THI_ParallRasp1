#include "header.h"

std::mutex _vector_mutex;
uint_fast64_t _vector_total_sum = 0;

uint_fast64_t sse2_add(double* A, double* B)
{
    uint_fast64_t C = 0;
    double buffer[4] = { 0,0,0,0 };

    __m256d c_line = _mm256_setzero_pd(); // заполн€ем результирующий SSE-регистр нул€ми
    __m256d a_line = _mm256_load_pd(A); // загружаем 4 элемента double из массива A в SSE-регистр
    __m256d b_line = _mm256_load_pd(B); // загружаем 4 элемента double из массива B в SSE-регистр
    c_line = _mm256_add_pd(a_line, b_line); // сложение двух SSE-регистров

    _mm256_store_pd(buffer, c_line); // перемещаем числа из sse регистра в массив из озу

    for (int i = 0; i < 4; i++)
    {
        C += buffer[i];
    }
    return C;
}

int ThreadProcessWithVector(uint_fast64_t left, uint_fast64_t right)
{
    uint_fast64_t sum = 0;
    uint_fast64_t i = left;
    while (i + 7 < right)
    {
        double A[4] = { i, i + 1, i + 2, i + 3 };
        double B[4] = { i + 4, i + 5, i + 6, i + 7 };

        sum += sse2_add(A, B);

        i += 8;
    }

    for (uint_fast64_t num = i; num < right; num++)
    {
        sum += num;
    }

    _vector_mutex.lock();
    _vector_total_sum += sum;
    _vector_mutex.unlock();
    return 0;
}

void MultiThreadingProccessWithVector(uint_fast64_t end)
{
    float avg_time = 0;
    double begin = 1;

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    const int numCores = sysinfo.dwNumberOfProcessors;

    uint_fast64_t block = (end + begin) / numCores;
    std::vector<uint_fast64_t> ranges(numCores + 1);
    ranges[0] = begin;
    for (int i = 1; i < numCores + 1; i++)
    {
        ranges[i] = ranges[i - 1] + block;
    }

    std::vector<std::thread>  threads(numCores);

    for (int attempt = 0; attempt < 10; attempt++)
    {
        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numCores; i++)
        {
            threads[i] = std::thread(ThreadProcessWithVector, ranges[i], ranges[i + 1]);
        }
        for (int i = 0; i < numCores; i++)
        {
            threads[i].join();
        }

        for (uint_fast64_t num = ranges[numCores]; num <= end; num++)
        {
            _vector_total_sum += num;
        }
        if (attempt == 9)
            std::cout << "ѕолученна€ сумма: " << _vector_total_sum << '\n';
        _vector_total_sum = 0;
        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "¬рем€ выполнени€ " << attempt << "-ой попытки: " << (float)duration.count() / (float)1000000 << " сек\n";

        avg_time += (float)duration.count() / (float)1000000;
    }
    std::cout << "—реднее врем€ выполнени€ расчета  с помощью " << numCores << " потоков и применением векторизации: " << avg_time / (float)10 << " сек\n";
}
