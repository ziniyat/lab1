#include <iostream>       
#include <vector>        
#include <algorithm>      
#include <chrono>         
#include <cstdlib>        
#include <boost/thread.hpp> 

// Функция для однопоточной сортировки
void OdnopotochSort(std::vector<int>& mas) {
    std::sort(mas.begin(), mas.end()); 
}

// Функция для сортировки части массива (выполняется в потоке)
void SortChast(std::vector<int>& part) {
    std::sort(part.begin(), part.end()); // Сортируем часть массива
}

// Функция для многопоточной сортировки
void MnogopotSort(std::vector<int>& Mas, int CountPotokov) {
    const int MasSize = Mas.size(); // Размер массива
    const int PartSize = MasSize / CountPotokov; // Размер части массива для каждого потока

    // Вектор для хранения частей массива
    std::vector<std::vector<int>> parts(CountPotokov);

    // Разделяем массив на части
    for (int i = 0; i < CountPotokov; ++i) {
        parts[i].resize(PartSize); //устанавливаем размер каждой части, resize - изменяет размер вектора  parts[i] так, чтобы он мог хранить PartSize элементов.
        std::copy(Mas.begin() + i * PartSize, // начальный индекс части
                  Mas.begin() + (i + 1) * PartSize, //конечный индекс части
                  parts[i].begin());
    }

    // Вектор для хранения потоков
    std::vector<boost::thread> threads;

    // Создаем потоки для сортировки каждой части
    for (int i = 0; i < CountPotokov; ++i) {
        threads.emplace_back(SortChast, std::ref(parts[i])); // Запускаем поток
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Объединяем отсортированные части в один массив
    std::vector<int> SortedMas;
    for (const auto& part : parts) {
        SortedMas.insert(SortedMas.end(), part.begin(), part.end());
    }

    // Сортируем объединенный массив (если части пересекаются)
    std::sort(SortedMas.begin(), SortedMas.end());

    // Возвращаем отсортированный массив
    Mas = SortedMas;
}

int main() {
    const int MasSize = 1000000; // Размер массива
    std::vector<int> Mas(MasSize); // Основной массив

    // Заполняем массив случайными числами
    std::srand(std::time(0)); // Инициализация генератора случайных чисел
    for (int i = 0; i < MasSize; ++i) {
        Mas[i] = std::rand() % 100000; // Случайное число от 0 до 99999
    }

    // Однопоточная сортировка
    std::vector<int> mas1 = Mas; // Копируем массив
    auto start1 = std::chrono::steady_clock::now(); // Засекаем время начала
    OdnopotochSort(mas1); // Сортируем
    auto end1 = std::chrono::steady_clock::now(); // Засекаем время окончания
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1); // Вычисляем время
    std::cout << "Однопоточная сортировка заняла: " << duration1.count() << "ms" << std::endl; 

    // Многопоточная сортировка с Boost.Thread
    for (int CountPotokov : {2, 4, 8}) { // Тестируем на 2, 4 и 8 потоках
        std::vector<int> mas2 = Mas; // Копируем массив
        auto start2 = std::chrono::steady_clock::now(); // Засекаем время начала
        MnogopotSort(mas2, CountPotokov); // Сортируем в нескольких потоках
        auto end2 = std::chrono::steady_clock::now(); // Засекаем время окончания
        auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2); // Вычисляем время
        std::cout << "Многопоточная сортировка с  " << CountPotokov << " потоками заняла: " << duration2.count() << "ms" << std::endl; 
    }

    return 0;
}
