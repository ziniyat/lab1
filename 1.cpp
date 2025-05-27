#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

// Очередь задач без синхронизации (для демонстрации проблем)
std::queue<int> noSinh;

// Функция для добавления задач в очередь (без синхронизации)
void Dobavl_noSinh(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        noSinh.push(id * 1000 + i); // Добавляем задачу в очередь
    }
}

// Функция для извлечения задач из очереди (без синхронизации)
void Izvlech_noSinh(int id) {
    while (!noSinh.empty()) {
        int task = noSinh.front(); // Получаем задачу из очереди
        noSinh.pop(); // Удаляем задачу из очереди
    }
}

// Очередь задач с использованием std::atomic для синхронизации
std::queue<int> atomic_queue;
std::atomic<bool> done(false); // Флаг завершения работы

// Функция для добавления задач в очередь (с использованием std::atomic)
void Dobavl_atomic(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        atomic_queue.push(id * 1000 + i); // Добавляем задачу в очередь
    }
}

// Функция для извлечения задач из очереди (с использованием std::atomic)
void Izvlech_atomic(int id) {
    while (!done || !atomic_queue.empty()) { // Пока есть задачи или работа не завершена
        if (!atomic_queue.empty()) {
            int task = atomic_queue.front(); // Получаем задачу из очереди
            atomic_queue.pop(); // Удаляем задачу из очереди
        }
    }
}

// Очередь задач с использованием std::mutex для синхронизации
std::queue<int> mutex_queue;
std::mutex mtx; // Мьютекс для блокировки доступа к очереди

// Функция для добавления задач в очередь (с использованием std::mutex)
void Dobavl_mutex(int id, int tasks) {
    for (int i = 0; i < tasks; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // Блокируем доступ к очереди
        mutex_queue.push(id * 1000 + i); // Добавляем задачу в очередь
    }
}

// Функция для извлечения задач из очереди (с использованием std::mutex)
void Izvlech_mutex(int id) {
    while (true) {
        std::lock_guard<std::mutex> lock(mtx); // Блокируем доступ к очереди
        if (mutex_queue.empty()) break; // Если очередь пуста, завершаем работу
        int task = mutex_queue.front(); // Получаем задачу из очереди
        mutex_queue.pop(); // Удаляем задачу из очереди
    }
}

// Функция для тестирования каждого подхода
void test_queue(int CountDobavlS, int CountIzvlechS, int zadania_Dobavl) {
    // Тестирование без синхронизации (демонстрация проблем)
    {
        std::vector<std::thread> Dobavls, Izvlechs;
        auto start = std::chrono::steady_clock::now();

        // Запуск потоков-производителей
        for (int i = 0; i < CountDobavlS; ++i) {
            Dobavls.emplace_back(Dobavl_noSinh, i, zadania_Dobavl);
        }

        // Запуск потоков-потребителей
        for (int i = 0; i < CountIzvlechS; ++i) {
            Izvlechs.emplace_back(Izvlech_noSinh, i);
        }

        // Ожидание завершения потоков
        for (auto& t : Dobavls) t.join();
        for (auto& t : Izvlechs) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время работы без синхронизации: " << duration.count() << "ms" << std::endl;
    }

    // Тестирование с использованием std::atomic
    {
        std::vector<std::thread> Dobavls, Izvlechs;
        auto start = std::chrono::steady_clock::now();

        // Запуск потоков-производителей
        for (int i = 0; i < CountDobavlS; ++i) {
            Dobavls.emplace_back(Dobavl_atomic, i, zadania_Dobavl);
        }

        // Запуск потоков-потребителей
        for (int i = 0; i < CountIzvlechS; ++i) {
            Izvlechs.emplace_back(Izvlech_atomic, i);
        }

        // Ожидание завершения потоков-производителей
        for (auto& t : Dobavls) t.join();

        // Устанавливаем флаг завершения работы
        done = true;

        // Ожидание завершения потоков-потребителей
        for (auto& t : Izvlechs) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время Atomic-а: " << duration.count() << "ms" << std::endl;

        // Сбрасываем флаг для следующих тестов
        done = false;
    }

    // Тестирование с использованием std::mutex
    {
        std::vector<std::thread> Dobavls, Izvlechs;
        auto start = std::chrono::steady_clock::now();

        // Запуск потоков-производителей
        for (int i = 0; i < CountDobavlS; ++i) {
            Dobavls.emplace_back(Dobavl_mutex, i, zadania_Dobavl);
        }

        // Запуск потоков-потребителей
        for (int i = 0; i < CountIzvlechS; ++i) {
            Izvlechs.emplace_back(Izvlech_mutex, i);
        }

        // Ожидание завершения потоков
        for (auto& t : Dobavls) t.join();
        for (auto& t : Izvlechs) t.join();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "время Mutex-a: " << duration.count() << "ms" << std::endl;
    }
}

int main() {
    const int CountDobavlS = 2; // Количество потоков-производителей
    const int CountIzvlechS = 2; // Количество потоков-потребителей
    const int zadania_Dobavl = 1000; // Количество задач на каждого производителя

    // Запуск тестирования
    test_queue(CountDobavlS, CountIzvlechS, zadania_Dobavl);

    return 0;
}
