#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int total = 0;
mutex m;

void adder(int t)
{
    unique_lock<mutex> lock(m, std::defer_lock);
    lock.lock();
    for (int i = 0; i < 10; i++)
    {
        cout << "T" << t << ": " << i << endl;
    }
    // no unlock but still will be fine, due to auto unlock in destructor.
}

int main()
{
    std::thread t1(adder, 1);
    std::thread t2(adder, 2);

    cout << "Pre" << endl;

    t1.join();
    t2.join();

    cout << "Post" << endl;

    cout << total << endl;

    return 0;
}