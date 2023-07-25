#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int total = 0;
mutex m;

void adder()
{
    if (m.try_lock())
    {
        total += 100;
        m.unlock();
    }
}

int main()
{
    std::thread t1(adder);
    std::thread t2(adder);

    cout << "Pre" << endl;

    t1.join();
    t2.join();

    cout << "Post" << endl;

    cout << total << endl;

    return 0;
}