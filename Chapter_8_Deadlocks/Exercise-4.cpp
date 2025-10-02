#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <memory>
#include <chrono>

class BankAccount {
private:
    double balance;
    std::mutex mtx;
    int accountId;

public:
    BankAccount(int id, double initial)
        : accountId(id), balance(initial) {}

    static bool transfer(BankAccount& from, BankAccount& to, double amount) {
        if (&from == &to) return false;

        // Lock both mutexes in consistent order to avoid deadlock
        BankAccount* first = (&from < &to) ? &from : &to;
        BankAccount* second = (&from < &to) ? &to : &from;

        std::lock(first->mtx, second->mtx);
        std::lock_guard<std::mutex> lock1(first->mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(second->mtx, std::adopt_lock);

        if (from.balance >= amount) {
            from.balance -= amount;
            to.balance += amount;
            std::cout << "Transfer: $" << amount
                      << " from Account " << from.accountId
                      << " to Account " << to.accountId << "\n";
            return true;
        }
        return false;
    }

    double getBalance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }

    int getId() const { return accountId; }
};

int main() {
    // Create random generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create accounts using unique_ptr
    std::vector<std::unique_ptr<BankAccount>> accounts;
    for (int i = 0; i < 5; ++i) {
        accounts.emplace_back(std::make_unique<BankAccount>(i, 1000.0));
    }

    // Create multiple threads to perform random transfers
    std::vector<std::thread> threads;
    const int numTransfersPerThread = 20;

    for (int t = 0; t < 10; ++t) {
        threads.emplace_back([&accounts, &gen]() {
            std::uniform_int_distribution<> accDist(0, accounts.size() - 1);
            std::uniform_real_distribution<> amountDist(1.0, 100.0);

            for (int i = 0; i < numTransfersPerThread; ++i) {
                int fromIdx = accDist(gen);
                int toIdx = accDist(gen);
                double amount = amountDist(gen);

                BankAccount::transfer(*accounts[fromIdx], *accounts[toIdx], amount);

                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // optional delay
            }
        });
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    // Print final balances
    std::cout << "\nFinal balances:\n";
    double total = 0;
    for (const auto& acc : accounts) {
        double bal = acc->getBalance();
        std::cout << "Account " << acc->getId() << ": $" << bal << "\n";
        total += bal;
    }

    std::cout << "Total money in system: $" << total << "\n";

    return 0;
}
