/*!
 * @brief    A simple FIFO mutex using a ticket lock mechanism.
 *
 * @details  - This class implements a custom mutex where each thread is placed in a queue (FIFO),
 *             acquiring the lock when its ticket number matches the currently served one.
 *           - Internally, it uses atomic variables:
 *               • `ticket` for the next ticket to issue
 *               • `serving` for the ticket currently being served
 *             with proper memory ordering (acquire/release) to ensure synchronization.
 *           - The `lock()` method includes a call to `std::this_thread::yield()` to inform
 *             the scheduler that the thread is waiting, allowing other threads to run.
 *           - Thread IDs are logged when acquiring or releasing the lock for clarity and debugging.
 *
 * @note     This is **not production-ready** and is intended **only for educational purposes**.
 *
 * @author   BenAli Samar
 * @date     30/07/2025
 */

#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>

class DomainLock
{

    /*
            -- memory barriers/fences --
        -- sychnronization between acquire/release
        1- All memeory writes performed by the releasin thread before its release operation$
        2- becomes visible to the acquiring thread after its acquire operation successfully reads the value written by release

        ==> SO:
            ° acquire : ensures that no memory reads/writes from this thread are moved before lock()
            ° release: ensures that no memory operations from this thread are moved after the unlock()
    */

    /*
            -- Ticket lock --
        Idea : each thread gets a "ticket"
               Threads wait untill their number is called ( their ticket matches the serving number)
               Guarantees FIFO order : first in first out ;)
    */
private:
    std::atomic<size_t> ticket{0};  // next ticket to give out
    std::atomic<size_t> serving{0}; // ticket currently being served

public:
    // try_lock succeeds only if no other thread holds the lock.
    // It checks if ticket == serving, and if so, acquires the lock by incrementing the ticket.
    bool try_lock()
    {
        size_t expected = serving.load(std::memory_order_relaxed);
        return ticket.compare_exchange_strong(expected, expected + 1, std::memory_order_acquire);
    }

    void lock()
    {
        // Assign a unique ticket number to this thread.
        size_t my_ticket = ticket.fetch_add(1, std::memory_order_relaxed);

        // Wait until the thread's ticket is being served (FIFO fairness).
        while (serving.load(std::memory_order_acquire) != my_ticket)
        {
            // Yield CPU to avoid busy-waiting too aggressively.
            std::this_thread::yield();
            std::cout << "[Thread " << std::this_thread::get_id() << "] waiting for turn (ticket "
                      << my_ticket << ", serving " << serving.load() << ")\n";
        }
        std::cout << "[Thread " << std::this_thread::get_id() << "] acquired lock (ticket " << my_ticket << ")\n";
    }

    void unlock()
    {
        // Allow the next ticket-holder to proceed.
        serving.fetch_add(1, std::memory_order_release);
        std::cout << " [Thread " << std::this_thread::get_id() << "] released the variable" << std::endl;
    }
};

DomainLock my_domainLock;
int counter = 0;

void worker()
{
    for (int i = 0; i < 2; ++i)
    {
        my_domainLock.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++counter;
        my_domainLock.unlock();
    }
}

int main()
{
    std::thread t1(worker);
    std::thread t2(worker);

    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << std::endl;
}