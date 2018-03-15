#include <stdio.h>
#include <windows.h>
#include "emmintrin.h"

#pragma comment(linker, "/STACK:10000000000")

#define OUTHER_SIZE 100
#define INNER_SIZE 10

int main()
{
	LARGE_INTEGER Frequency, StartTime, EndTime, ElapsedTime;

	_declspec(align(16)) double m1[OUTHER_SIZE * OUTHER_SIZE][INNER_SIZE * INNER_SIZE] = { 0 };
	_declspec(align(16)) double m2[OUTHER_SIZE * OUTHER_SIZE][INNER_SIZE * INNER_SIZE] = { 0 };
	_declspec(align(16)) double res1[OUTHER_SIZE * OUTHER_SIZE][INNER_SIZE * INNER_SIZE] = { 0 };
	_declspec(align(16)) double res2[OUTHER_SIZE * OUTHER_SIZE][INNER_SIZE * INNER_SIZE] = { 0 };
	_declspec(align(16)) double res3[OUTHER_SIZE * OUTHER_SIZE][INNER_SIZE * INNER_SIZE] = { 0 };

	// init
	for (int i = 0; i < OUTHER_SIZE; i++) {
		for (int j = 0; j < OUTHER_SIZE; j++) {
			for (int i1 = 0; i1 < INNER_SIZE; i1++) {
				for (int j1 = 0; j1 < INNER_SIZE; j1++) {
					m1[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] = (double)i1 + (double)j / 20;
					m2[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] = (double)j1 * (double)i / 13;
				}
			}
		}
	}

	// auto vector
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartTime);

	for (int i = 0; i < OUTHER_SIZE; i++) {
		for (int j = 0; j < OUTHER_SIZE; j++) {
			for (int k = 0; k < OUTHER_SIZE; k++) {
				for (int i1 = 0; i1 < INNER_SIZE; i1++) {
					for (int k1 = 0; k1 < INNER_SIZE; k1++) {
						for (int j1 = 0; j1 < INNER_SIZE; j1++) {
							res1[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] += m1[i * OUTHER_SIZE + k][i1 * INNER_SIZE + k1] * m2[k * OUTHER_SIZE + j][k1 * INNER_SIZE + j1];
						}
					}
				}
			}
		}
	}

	QueryPerformanceCounter(&EndTime);
	ElapsedTime.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	printf("auto vect: %lld\n", ElapsedTime.QuadPart);

	// disabled vector
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartTime);

	for (int i = 0; i < OUTHER_SIZE; i++) {
		for (int j = 0; j < OUTHER_SIZE; j++) {
			for (int k = 0; k < OUTHER_SIZE; k++) {
				for (int i1 = 0; i1 < INNER_SIZE; i1++) {
					for (int k1 = 0; k1 < INNER_SIZE; k1++) {
#pragma loop(no_vector)
						for (int j1 = 0; j1 < INNER_SIZE; j1++) {
							res2[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] += m1[i * OUTHER_SIZE + k][i1 * INNER_SIZE + k1] * m2[k * OUTHER_SIZE + j][k1 * INNER_SIZE + j1];
						}
					}
				}
			}
		}
	}

	QueryPerformanceCounter(&EndTime);
	ElapsedTime.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	printf("disa vect: %lld\n", ElapsedTime.QuadPart);

	// sse2
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartTime);

	for (int i = 0; i < OUTHER_SIZE; i++) {
		for (int j = 0; j < OUTHER_SIZE; j++) {
			for (int k = 0; k < OUTHER_SIZE; k++) {

				double *m1_ptr = m1[i * OUTHER_SIZE + k];
				double *m2_ptr = m2[k * OUTHER_SIZE + j];
				double *res_ptr = res3[i * OUTHER_SIZE + j];

				for (int i1 = 0; i1 < INNER_SIZE; i1++) {
					for (int k1 = 0; k1 < INNER_SIZE; k1++) {
						const __m128d m1_ = _mm_load1_pd(&(m1_ptr[i1 * INNER_SIZE + k1]));
						for (int j1 = 0; j1 < INNER_SIZE; j1 += 2) {
							__m128d m2_ = _mm_loadu_pd(&(m2_ptr[k1 * INNER_SIZE + j1]));
							__m128d res_ = _mm_loadu_pd(&(res_ptr[i1 * INNER_SIZE + j1]));
							_mm_storeu_pd(
								&(res_ptr[i1 * INNER_SIZE + j1]),
								_mm_add_pd(res_, _mm_mul_pd(m1_, m2_))
							);
						}
					}
				}
			}
		}
	}

	QueryPerformanceCounter(&EndTime);
	ElapsedTime.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	printf("sse2 vect: %lld\n", ElapsedTime.QuadPart);


	// check
	bool isOK = true;
	for (int i = 0; (i < OUTHER_SIZE) && isOK; i++) {
		for (int j = 0; (j < OUTHER_SIZE) && isOK; j++) {
			for (int i1 = 0; (i1 < INNER_SIZE) && isOK; i1++) {
				for (int j1 = 0; j1 < INNER_SIZE; j1++) {
					if (res1[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] != res2[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] || res1[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1] != res3[i * OUTHER_SIZE + j][i1 * INNER_SIZE + j1]) {
						isOK = false;
						break;
					}
				}
			}
		}
	}

	if (isOK) {
		puts("All answers are similar!");
	}
	else {
		puts("Answers are not similar!");
	}

	return 0;
}