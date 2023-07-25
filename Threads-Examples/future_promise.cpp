#include <iostream>
#include <thread>
#include <future>

using namespace std;

#define ll long long

void adder(promise<ll> &&p, int x)
{
    ll sum = 0;
    for (int i = 0; i < x; i++)
    {
        sum += i * (i - 1) / 2;
    }
    this_thread::sleep_for(chrono::seconds(5));
    p.set_value(sum);
}

int main()
{
    ll x = 23;
    promise<ll> p;
    future<ll> f = p.get_future();
    std::thread t(adder, move(p), x);

    cout << "Pre_get" << endl;
    cout << f.get() << endl; // waits for the promised value

    t.join();

    return 0;
}