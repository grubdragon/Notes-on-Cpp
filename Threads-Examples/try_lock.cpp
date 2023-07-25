#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int x = 0;
int y = 0;
mutex m1, m2;

void increment_one(int &var, mutex &m)
{
    cout << &m << endl;
    for (int i = 0; i < 20; i++)
    {
        m.lock();
        var++;
        m.unlock();
    }
}

void increment_both(int &var1, int &var2, mutex &m1, mutex &m2)
{
    for (int i = 0; i < 20; i++)
    {
        int lock_res = std::try_lock(m1, m2);
        if (lock_res == -1)
        {
            cout << "X = " << var1 << "; Y = " << var2 << endl;
            m1.unlock();
            m2.unlock();
        }
        else
        {
            cout << "Locking failed due to " << (lock_res == 0 ? "X" : "Y") << endl;
        }
    }
}

int main()
{
    std::thread t1(increment_one, std::ref(x), std::ref(m1));
    std::thread t2(increment_one, std::ref(y), std::ref(m2));
    std::thread t3(increment_both, std::ref(x), std::ref(y), std::ref(m1), std::ref(m2));

    cout << "Pre" << endl;

    t1.join();
    t2.join();
    t3.join();

    cout << "Post" << endl;

    cout << "Final: "
         << "X = " << x << "; Y = " << y << endl;

    return 0;
}