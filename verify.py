import numpy as np

SIZE_MATRIX = 3

A = np.loadtxt("matrix_1.txt", dtype=int)
B = np.loadtxt("matrix_2.txt", dtype=int)

with open("matrix_result.txt", "r") as file:
    C_cpp = np.array([
        list(map(int, file.readline().split()))
        for _ in range(SIZE_MATRIX)
    ])

C_python = A @ B

if np.array_equal(C_cpp, C_python):
    print("Результаты совпадают!")
else:
    print("ОШИБКА: результаты не совпадают!")

    print("\nРезультат C++:")
    print(C_cpp)

    print("\nРезультат Python:")
    print(C_python)