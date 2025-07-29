/*!
* @brief    DomainLock class is a mutex implementation the simple version where every thread
            just keep waiting till the variable is available.
            So If its locked the second thread won't get it!
* @details  - It uses an atomic flag for locking and memory_order_acquire/release
            - Also in the lock func there is a yield to tell the shchedueler,
              its free time u can check another task and return back to me later.
            - It logs the thread id when a thread trie to acquire/release the variable.
* @author   BenAli Samar
* @date     29/07/2025
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
private:
    // atomic bool flag (lowlevel)
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    bool try_lock()
    {
        return !flag.test_and_set(std::memory_order_acquire);
    }

    void lock()
    {
        // acquire --> load the atomic variable
        // checks the ready flag before doing anything (read) to the data
        // 'test_and_set' returns true if the flag was setted , means some other threads holds the lock
        while (flag.test_and_set(std::memory_order_acquire))
        {
            // spin --> busy wait if no sleep or yield is added !
            std::this_thread::yield();
            std::cout << "Tried to yield here " << std::endl;
        }
    }

    void unlock()
    {
        // release --> store the atomic variable
        // the ready flage to the variable to be close and no other part can be added to the variable
        flag.clear(std::memory_order_release);
        std::cout << " [Thread " << std::this_thread::get_id() << "] released the variable" << std::endl;
    }
};

DomainLock my_domainLock;
int counter = 0;

void worker()
{
    auto id = std::this_thread::get_id();
    for (int i = 0; i < 10; ++i)
    {
        if (my_domainLock.try_lock())
        {
            std::cout << "[Thread " << id << "] acquired lock." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++counter;
            my_domainLock.unlock();
        }
        else
        {
            std::cout << "[Thread " << id << " Couldn't acquire, maybe skip or try again later ..." << std::endl;
        }
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