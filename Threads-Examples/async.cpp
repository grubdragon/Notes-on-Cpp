#include <iostream>
#include <thread>
#include <future>

using namespace std;

#define ll long long

ll adder(ll x)
{
    // prints same id as main for case I(deferred), prints different id from main in case II(async)
    cout << "Thread ID (adder): " << this_thread::get_id() << endl;
    ll sum = 0;
    for (int i = 0; i < x; i++)
    {
        sum += i * (i - 1) / 2;
    }
    return sum;
}

int main()
{
    ll x = 23;

    future<ll> f = async(launch::deferred, adder, x);

    cout << "Pre_get" << endl;
    cout << "Thread ID (main): " << this_thread::get_id() << endl;
    cout << f.get() << endl; // waits for the promised value

    f = async(launch::async, adder, x);

    cout << "Pre_get" << endl;
    cout << "Thread ID (main): " << this_thread::get_id() << endl;
    cout << f.get() << endl; // waits for the promised value

    return 0;
}