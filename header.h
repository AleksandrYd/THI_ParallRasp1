#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <conio.h>
#include <immintrin.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/number.hpp>

using namespace boost::multiprecision;

int main();

void MultiThreadingProccess(double end);

void MultiThreadingProccessWithVector(uint_fast64_t end);

uint_fast64_t serial_proccess();

void MatrixProccess();