#include "header.h"

const int n = 4096;
std::mutex _mutex_matrix;
float* C_serial;
float* C_multithreads;

void thread_proccess(float* A, float* B, int block, int thread_num)
{
	float summa;
	int first_row = block * thread_num;
	int last_row = block * (thread_num + 1);

	for (int i = first_row; i < last_row; i++)
	{
		for (int j = 0; j < n; j++)
		{
			summa = 0;
			for (int k = 0; k < n; k++)
			{
				
				summa += A[i * n + k] * B[k * n + j];
			}
			_mutex_matrix.lock();
			C_multithreads[i * n + j] = summa;
			_mutex_matrix.unlock();
		}
	}
}

bool check_functions()
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (C_multithreads[i * n + j] != C_serial[i * n + j])
				return false;
		}
	}
	return true;
}

void multithreads_mul(float *A, float *B)
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	const int numCores = sysinfo.dwNumberOfProcessors;

	int block = n / numCores;
	std::vector<std::thread>  threads(numCores);
	
	for (int i = 0; i < numCores; i++)
	{
		threads[i] = std::thread(thread_proccess, A, B, block, i);
	}

	for (int i = 0; i < numCores; i++)
	{
		threads[i].join();
	}
}

void serial_mul(float* A, float* B)
{
	float summa;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			summa = 0;
			for (int k = 0; k < n; k++)
			{
				summa += A[i * n + k] * B[k * n + j];
			}

			
			C_serial[i * n + j] = summa;
		}
	}
}

void MatrixProccess()
{
	float* A = new float[n * n];
	float* B = new float[n * n];
	C_serial = new float[n * n];
	C_multithreads = new float[n * n];

	float average = 0;

	srand(time(NULL) % 100);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A[i * n + j] = rand() % 10;
			B[i * n + j] = rand() % 10;
			C_serial[i * n + j] = 0;
			C_multithreads[i * n + j] = 0;
		}
	}

	for (int i = 0; i < 10; i++)
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		serial_mul(A, B);
		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

		std::cout << "Время выполнения " << i << "-ой попытки: " << (float)duration.count() / (float)1000000 << " сек\n";

		average += (float)duration.count()/1000000;
	}
	
	std::cout << "Среднее время однопоточного перемножения матриц: " << average / (float)10 << '\n';

	average = 0;

	for (int i = 0; i < 10; i++)
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		multithreads_mul(A, B);
		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

		std::cout << "Время выполнения " << i << "-ой попытки: " << (float)duration.count() / (float)1000000 << " сек\n";

		average += (float)duration.count() / 1000000;
	}

	std::cout << "Среднее время многопоточного перемножения матриц: " << average / (float)10 << '\n';

	if (check_functions())
		std::cout << "Матрицы совпали)\n";
	else
		std::cout << "Матрицы не совпали(\n";
}