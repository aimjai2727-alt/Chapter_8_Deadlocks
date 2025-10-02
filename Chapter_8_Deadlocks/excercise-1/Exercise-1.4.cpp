#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex resourceA, resourceB, resourceC;

void process1() {
    // Lock both resourceA and resourceB atomically
    std::lock(resourceA, resourceB);
    std::lock_guard<std::mutex> lockA(resourceA, std::adopt_lock);
    std::lock_guard<std::mutex> lockB(resourceB, std::adopt_lock);

    std::cout << "Process 1: Locked A and B\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Process 1: Done\n";
}

void process2() {
    // Lock both resourceB and resourceC atomically
    std::lock(resourceB, resourceC);
    std::lock_guard<std::mutex> lockB(resourceB, std::adopt_lock);
    std::lock_guard<std::mutex> lockC(resourceC, std::adopt_lock);

    std::cout << "Process 2: Locked B and C\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Process 2: Done\n";
}

void process3() {
    // Lock both resourceA and resourceC atomically
    std::lock(resourceA, resourceC);
    std::lock_guard<std::mutex> lockA(resourceA, std::adopt_lock);
    std::lock_guard<std::mutex> lockC(resourceC, std::adopt_lock);

    std::cout << "Process 3: Locked A and C\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Process 3: Done\n";
}

int main() {
    std::thread t1(process1);
    std::thread t2(process2);
    std::thread t3(process3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "All processes completed without deadlock.\n";
    return 0;
}
