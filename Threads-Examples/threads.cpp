#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void printer()
{
    std::this_thread::sleep_for(chrono::seconds(5));
    for (int i = 0; i < 10; i++)
    {
        cout << "P = " << i << endl;
    }
}

int main()
{
    std::thread t1(printer);
    cout << "Pre" << endl;
    t1.detach();
    t1.detach();
    cout << "Post" << endl;
    return 0;
}