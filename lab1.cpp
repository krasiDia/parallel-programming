import std;

int main()
{
    const int SIZE_MATRIX = 5;
    const int FILE_SIZE = 2000;
    std::ifstream file_1("matrix_1.txt");
    std::ifstream file_2("matrix_2.txt");
    std::ofstream file_3("matrix_result.txt");
    std::vector<std::vector<int>> arr_1(SIZE_MATRIX, std::vector<int>(SIZE_MATRIX));
    std::vector<std::vector<int>> arr_2(SIZE_MATRIX, std::vector<int>(SIZE_MATRIX));
    std::vector<std::vector<int>> arr_3(SIZE_MATRIX, std::vector<int>(SIZE_MATRIX));
    
    for (int i = 0; i < SIZE_MATRIX; ++i) {

        std::string line_1;
        std::string line_2;

        std::getline(file_1, line_1);
        std::getline(file_2, line_2);

        std::stringstream stream_1(line_1);
        std::stringstream stream_2(line_2);

        for (int j = 0; j < SIZE_MATRIX; ++j) {
            stream_1 >> arr_1[i][j];
            stream_2 >> arr_2[i][j];
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int k = 0; k < SIZE_MATRIX; ++k) {
        for (int i = 0; i < SIZE_MATRIX; ++i) {
            int result = 0;
            for (int j = 0; j < SIZE_MATRIX; ++j) {
                result += arr_1[k][j] * arr_2[j][i];
            }
            arr_3[k][i] = result;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end - start
    );


    for (int i = 0; i < SIZE_MATRIX; ++i) {
        for (int j = 0; j < SIZE_MATRIX; ++j) {
            file_3 << arr_3[i][j] << " ";
        }
        file_3 << "\n";
    }
    file_3 << "Time: " << time.count() << " nanoseconds\n";
    file_3 << "Volume of calculations: " << 2 * std::pow(SIZE_MATRIX, 3);
}