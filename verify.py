import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('SIZE_MATRIX', type=int, help='matrix size')  
arg = parser.parse_args()

with open("matrix_1.txt", "r") as file:
    A = np.array([
        list(map(int, file.readline().split()[:arg.SIZE_MATRIX]))
        for _ in range(arg.SIZE_MATRIX)
    ])

with open("matrix_2.txt", "r") as file:
    B = np.array([
        list(map(int, file.readline().split()[:arg.SIZE_MATRIX]))
        for _ in range(arg.SIZE_MATRIX)
    ])

C_cpp = np.loadtxt("matrix_result.txt", dtype=int)

C_python = A @ B

if np.array_equal(C_cpp, C_python):
    print("Результаты совпадают!")
else:
    print("ОШИБКА: результаты не совпадают!")

    print("\nРезультат C++:")
    print(C_cpp)

    print("\nРезультат Python:")
    print(C_python)