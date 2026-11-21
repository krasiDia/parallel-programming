#include <mpi.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
// Транспонирование матрицы
std::vector<int> transposition(const std::vector<int>& A, int N) {
    std::vector<int> tA(A.size());

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            tA[j * N + i] = A[i * N + j];
        }
    }

    return tA;
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Получаем размер матрицы из аргумента запуска
    int SIZE_MATRIX = 2000;

    if (argc > 1) {
        try {
            SIZE_MATRIX = atoi(argv[1]);
        }
        catch (...) {
            if (rank == 0) {
                std::cerr << "Invalid matrix size\n";
            }
            MPI_Finalize();
            return 1;
        }
    }

    if (SIZE_MATRIX <= 0 || SIZE_MATRIX % size != 0) {
        if (rank == 0) {
            std::cerr << "Invalid matrix size or number of processes\n";
        }

        MPI_Finalize();
        return 1;
    }

    const int rows_per_process = SIZE_MATRIX / size;
    const int elements_per_process = rows_per_process * SIZE_MATRIX;
    const int total_elements = SIZE_MATRIX * SIZE_MATRIX;

    std::vector<int> arr_1(total_elements);
    std::vector<int> arr_2(total_elements);

    // Только процесс 0 считывает исходные матрицы
    int read_success = 1;

    if (rank == 0) {

        std::ifstream file_1("matrix_1.txt");
        std::ifstream file_2("matrix_2.txt");

        if (!file_1.is_open() || !file_2.is_open()) {
            std::cerr << "Cannot open input files\n";
            read_success = 0;
        }
        else {
            for (int i = 0; i < total_elements; ++i) {
                if (!(file_1 >> arr_1[i]) ||
                    !(file_2 >> arr_2[i])) {

                    std::cerr << "Error reading matrices\n";
                    read_success = 0;
                    break;
                }
            }
        }

        if (read_success) {
            arr_2 = transposition(arr_2, SIZE_MATRIX);
        }
    }

    // Сообщаем всем процессам, успешно ли считаны матрицы
    MPI_Bcast(
        &read_success,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    if (!read_success) {
        MPI_Finalize();
        return 1;
    }

    std::vector<int> local_1(elements_per_process);
    std::vector<long long> local_3(elements_per_process);

    std::vector<long long> arr_3(total_elements);

    // Синхронизируем процессы перед измерением времени
    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    // Распределяем строки матрицы A
    MPI_Scatter(
        arr_1.data(),
        elements_per_process,
        MPI_INT,
        local_1.data(),
        elements_per_process,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    // Передаём транспонированную B всем процессам
    MPI_Bcast(
        arr_2.data(),
        total_elements,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    // Каждый процесс вычисляет свою часть результирующей матрицы
    for (int i = 0; i < rows_per_process; ++i) {

        for (int j = 0; j < SIZE_MATRIX; ++j) {

            long long result = 0;

            for (int k = 0; k < SIZE_MATRIX; ++k) {

                result += static_cast<long long>(
                    local_1[i * SIZE_MATRIX + k]
                    ) * arr_2[j * SIZE_MATRIX + k];

            }

            local_3[i * SIZE_MATRIX + j] = result;
        }
    }

    // Собираем результирующие строки на процессе 0
    MPI_Gather(
        local_3.data(),
        elements_per_process,
        MPI_LONG_LONG_INT,
        arr_3.data(),
        elements_per_process,
        MPI_LONG_LONG_INT,
        0,
        MPI_COMM_WORLD
    );

    double elapsed = MPI_Wtime() - start;

    // Получаем максимальное время среди всех процессов
    double max_elapsed = 0.0;

    MPI_Reduce(
        &elapsed,
        &max_elapsed,
        1,
        MPI_DOUBLE,
        MPI_MAX,
        0,
        MPI_COMM_WORLD
    );

    // Только процесс 0 записывает результат
    if (rank == 0) {

        std::ofstream file_3("matrix_result.txt");
        std::ofstream file_4("result.txt");

        if (!file_3.is_open() || !file_4.is_open()) {
            std::cerr << "Cannot open output files\n";
        }
        else {

            for (int i = 0; i < SIZE_MATRIX; ++i) {
                for (int j = 0; j < SIZE_MATRIX; ++j) {
                    file_3 << arr_3[i * SIZE_MATRIX + j] << " ";
                }

                file_3 << "\n";
            }

            file_4 << "Processes: " << size << "\n";
            file_4 << "Matrix size: " << SIZE_MATRIX << "\n";
            file_4 << "Time: " << max_elapsed << " seconds\n";
        }

        std::cout << "Processes: " << size << '\n';
        std::cout << "Matrix size: " << SIZE_MATRIX << '\n';
        std::cout << "Time: " << max_elapsed << " seconds\n";
    }

    MPI_Finalize();

    return 0;
}
