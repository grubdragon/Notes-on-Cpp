#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int x = 0;
recursive_mutex m;

void increment(int t, int recursions)
{
    if (recursions < 0)
    {
        return;
    }
    m.lock();
    x++;
    cout << "Thread = " << t << ";  Recursion = " << recursions << endl;
    increment(t, --recursions);
    m.unlock();
}

int main()
{
    std::thread t1(increment, 1, 10);
    std::thread t2(increment, 2, 10);

    cout << "Pre" << endl;

    t1.join();
    t2.join();

    cout << "Post" << endl;

    cout << "Final: "
         << "X = " << x << endl;

    return 0;
}