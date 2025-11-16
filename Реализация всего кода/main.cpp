#include <iostream>
#include <vector>
#include<random>
#include<algorithm>
#include <fstream>
#include <numeric>
#include <chrono>

class ArrayGenerator {
public:
    static std::vector<int> generateRandom(size_t n, int minValue, int maxValue) {
        std::vector<int> arr(n);
        std::mt19937 gen(std::random_device{}());
        // Означает: "давай случайные целые числа от minValue до maxValue включительно".
        std::uniform_int_distribution<int> dist(minValue, maxValue);

        for (auto& x : arr) {
            x = dist(gen);
        }
        return arr;
    }

    static std::vector<int> generateReverse(size_t n, int minValue, int maxValue) {
        std::vector<int> arr = generateRandom(n, minValue, maxValue);
        std::sort(arr.begin(), arr.end(), std::greater<int>());
        return arr;
    }
    static std::vector<int> generateAlmostSorted(size_t n, int minValue, int maxValue, double disorderRatio) {
        std::vector<int> arr = generateRandom(n, minValue, maxValue);
        std::sort(arr.begin(), arr.end());

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, n -1);

        size_t swaps = static_cast<size_t>(n * disorderRatio);
        for (size_t i = 0; i < swaps; i++) {
            size_t idx1 = dist(gen);
            size_t idx2 = dist(gen);
            std::swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }
};

// --- гибридный MERGE + INSERTION SORT ---
class SortAlgorithms {
public:
    static void insertionSort(std::vector<int>& arr, int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    // Обычный рекурсивный MergeSort
    static void mergeSort(std::vector<int>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    // Гибридный MergeSort + InsertionSort
    static void mergeSortHybrid(std::vector<int>& arr, int left, int right, int threshold = 15) {
        if (right - left + 1 <= threshold) {
            insertionSort(arr, left, right);
            return;
        }
        int mid = left + (right - left) / 2;
        mergeSortHybrid(arr, left, mid, threshold);
        mergeSortHybrid(arr, mid + 1, right, threshold);
        merge(arr, left, mid, right);
    }

private:
    static void merge(std::vector<int>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<int> L(n1);
        std::vector<int> R(n2);

        for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
        for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }
};


class SortTester {
    public:
    // Тип функции сортировки
    using SortFunction  = std::function<void(std::vector<int>&, int, int)>;

    // Тип функции для гибридного варианта (с threshold)
    using HybridFunction = std::function<void(std::vector<int>&, int, int, int)>;

    // измеряет время выполнения одной сортировки
    static long long measureTime(SortFunction sortFn, std::vector<int> arr) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFn(arr, 0, static_cast<int>(arr.size()) - 1);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    // то же самое для гибридной версии
    static long long measureTimeHybrid(HybridFunction sortFn, std::vector<int> arr, int threshold) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFn(arr, 0, static_cast<int>(arr.size()) - 1, threshold);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    // усреднённое время из нескольких прогонов
    static double averageTime(SortFunction sortFn, const std::vector<int>& arr, int repeats = 5) {
        std::vector<long long> times;
        for (int i = 0; i < repeats; ++i)
            times.push_back(measureTime(sortFn, arr));
        return std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    }

    static double averageTimeHybrid(HybridFunction sortFn, const std::vector<int>& arr, int threshold, int repeats = 5) {
        std::vector<long long> times;
        for (int i = 0; i < repeats; ++i)
            times.push_back(measureTimeHybrid(sortFn, arr, threshold));
        return std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    }
};

int main() {
    const int MIN_SIZE = 500;
    const int MAX_SIZE = 20000;   // ограничиваем, чтобы не ждать вечность
    const int STEP = 500;         // шаг увеличиваем, чтобы меньше итераций
    const int MIN_VAL = 0;
    const int MAX_VAL = 6000;

    std::vector<int> thresholds = {5, 10, 20, 30, 50};

    std::ofstream out("results.csv");
    out << "Type,Size,Algorithm,Threshold,Time(ms)\n";

    // генерируем массивы один раз
    auto randomFull  = ArrayGenerator::generateRandom(MAX_SIZE, MIN_VAL, MAX_VAL);
    auto reverseFull = ArrayGenerator::generateReverse(MAX_SIZE, MIN_VAL, MAX_VAL);
    auto almostFull  = ArrayGenerator::generateAlmostSorted(MAX_SIZE, MIN_VAL, MAX_VAL, 0.02);

    std::vector<std::pair<std::string, std::vector<int>>> datasets = {
        {"Random", randomFull},
        {"Reverse", reverseFull},
        {"AlmostSorted", almostFull}
    };

    // основной цикл
    for (auto& [typeName, fullArray] : datasets) {
        for (int n = MIN_SIZE; n <= MAX_SIZE; n += STEP) {
            std::vector<int> arr(fullArray.begin(), fullArray.begin() + n);

            // стандартный MergeSort
            double timeMerge = SortTester::averageTime(SortAlgorithms::mergeSort, arr, 2);  // меньше повторов
            out << typeName << "," << n << ",MergeSort,0," << timeMerge << "\n";
            std::cout << typeName << " size " << n << " → MergeSort: " << timeMerge << " ms\n";

            // гибридные версии
            for (int t : thresholds) {
                double timeHybrid = SortTester::averageTimeHybrid(SortAlgorithms::mergeSortHybrid, arr, t, 2);
                out << typeName << "," << n << ",MergeHybrid," << t << "," << timeHybrid << "\n";
                std::cout << "   threshold " << t << " → " << timeHybrid << " ms\n";
            }
        }
    }

    out.close();
    std::cout << "\nРезультаты сохранены в results.csv (упрощённый замер)\n";
    return 0;
}
