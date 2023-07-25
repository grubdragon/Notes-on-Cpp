// // Try_lock_for
// #include <iostream>
// #include <thread>
// #include <mutex>

// using namespace std;

// int x = 0;
// timed_mutex m;

// void increment(int t)
// {
//     if (m.try_lock_for(std::chrono::seconds(1)))
//     {
//         x++;
//         this_thread::sleep_for(std::chrono::seconds(2));
//         cout << "Thread " << t << " entered" << endl;
//         m.unlock();
//     }
//     else
//     {
//         cout << "Thread " << t << " not entered" << endl;
//     }
// }

// int main()
// {
//     std::thread t1(increment, 1);
//     std::thread t2(increment, 2);

//     cout << "Pre" << endl;

//     t1.join();
//     t2.join();

//     cout << "Post" << endl;

//     cout << "Final: "
//          << "X = " << x << endl;

//     return 0;
// }

// *****************************************************************

// Try lock unitl
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int x = 0;
timed_mutex m;

void increment(int t)
{
    auto timestamp = std::chrono::steady_clock::now();
    if (m.try_lock_until(timestamp + std::chrono::seconds(1)))
    {
        x++;
        this_thread::sleep_for(std::chrono::seconds(2));
        cout << "Thread " << t << " entered" << endl;
        m.unlock();
    }
    else
    {
        cout << "Thread " << t << " not entered" << endl;
    }
}

int main()
{
    std::thread t1(increment, 1);
    std::thread t2(increment, 2);

    cout << "Pre" << endl;

    t1.join();
    t2.join();

    cout << "Post" << endl;

    cout << "Final: "
         << "X = " << x << endl;

    return 0;
}