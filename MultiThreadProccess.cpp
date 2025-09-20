#include "header.h"

std::mutex _mutex;
uint_fast64_t _total_sum= 0;

int ThreadProcess(uint_fast64_t left, uint_fast64_t right)
{
    uint_fast64_t sum = 0;
    for (uint_fast64_t i = left; i < right; i++)
    {
        sum += i;
    }
    _mutex.lock();
    _total_sum += sum;
    _mutex.unlock();
    return 0;
}

void MultiThreadingProccess(double end)
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
            threads[i] = std::thread(ThreadProcess, ranges[i], ranges[i + 1]);
        }

        for (int i = 0; i < numCores; i++)
        {
            threads[i].join();
        }


        for (uint_fast64_t num = ranges[numCores]; num <= end; num++)
        {
            _total_sum += num;
        }
        if (attempt == 9)
            std::cout << "Полученная сумма: " << _total_sum << '\n';
        _total_sum = 0;

        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "Время выполнения " << attempt << "-ой попытки: " << (float)duration.count() / (float)1000000 << " сек\n";

        avg_time += (float)duration.count() / (float)1000000;
    }
    std::cout << "Среднее время выполнения расчета  с помощью " << numCores << " потоков: " << avg_time / (float)10 << " сек\n";
}
