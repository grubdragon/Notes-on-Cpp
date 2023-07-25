# Threads in Cpp

## Threading Basics (with join, detach and joinable)
### How to create a thread?
Creating a thread is simple. Use the thread API with a function to be called. Syntax:
```cpp
#include <iostream>
#include <thread>
using namespace std;
void thread_function(){
    cout << "Simple function" << endl;
}
int main(){
    thread t1(thread_function);
    t1.join();

    return 0;
}
```

### How to wait for a thread?
Using the `join` does exactly that, waits for the thread to complete.

### What happens if you join twice?
Error like below.
```
terminate called after throwing an instance of 'std::system_error'
  what():  Invalid argument
```

### How to avoid a double join?
Check if you can call `join` using the `joinable` API.
```cpp
#include <iostream>
#include <thread>
using namespace std;
void thread_function(){
    cout << "Simple function" << endl;
}
int main(){
    thread t1(thread_function);
    if(t1.joinable())
        t1.join(); // this statement runs
    if(t1.joinable())
        t1.join(); // this statement doesn't run
    
    return 0;
}
```

### How to not wait for a thread?
Using the `detach` API.

```cpp
#include <iostream>
#include <thread>
#include <chrono>

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
    cout << "Post" << endl;
    return 0;
}
```

This will let main go on without caring about `printer` function (and if you run it, most likely won't print `printer` statements).

### Detach twice is good though, right?
No. Similar error.
```
terminate called after throwing an instance of 'std::system_error'
  what():  Invalid argument
```

### How to avoid double detach?
Same way, using `joinable`.

## Mutexes
Mutex = mutual exclusion, for accessing same data for each thread one at a time(avoiding race condition). Section between lock and unlock is called a critical section.

### How to use a mutex?
Define and call `lock` and `unlock` APIs.
```cpp
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int total = 0;
mutex m;

void adder()
{
    m.lock();
    total += 100;
    m.unlock();
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
```

### Avoid blocking when mutex can't be locked immediately?
Use `try_lock`. It tries locking and returns true if mutex locks, false otherwise. Returns immediately.
```cpp
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
```

### A general try Lock for multiple mutexes
Global function that takes multiple mutexes and tries to lock all of them. Success = -1, Failure = index of mutex not locked(releases all the previously locked).
Note: Using std::ref is necessary for passing params by reference to thread API.
```cpp
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
```

### A general Lock for multiple mutexes
Just use `std::lock(m_1, m_2, ..., m_n)`, works pretty much the same, prevents deadlock, yada yada.

### Timed Mutexes
Quite different from regular mutex. Has member functions:
1. lock
2. try_lock
3. try_lock_for // NEW!
4. try_lock_until // NEW!
5. unlock
   
3, 4 make it different from a regular mutex

#### Try_lock_for
`try_lock_for(timeout)`: Wait for upto `timeout` time for a mutex. 

In the code below, one of the threads won't be entered (generally, unless theres some really late scheduling for the second thread being run).

```cpp
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int x = 0;
timed_mutex m;

void increment(int t)
{
    if (m.try_lock_for(std::chrono::seconds(1)))
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
```

#### Try_lock_until
`try_lock_until(timestamp)`: Wait till `timestamp` time for a mutex. 

Same code but with try_lock_until.

```cpp
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
```

### Recursive mutexes
Similar to mutex, but can be locked multiple times by the same thread. Has overhead over mutex, avoid if possible. Must be unlocked = times locked, so that other threads can lock too.

```cpp
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
```

Can also be done in loop form.
```cpp
for(int i = 0; i < 5; i++){
    m.lock();
    x++;
}
for(int i = 0; i < 5; i++){
    m.unlock();
    x++;
}
```

## Mutex Wrappers

### Lock Guard - simplified mutex usage
A lock guard is a simple, lightweight mutex wrapper. Acquires lock during creation, unlocks during destruction. Explicit unlock not allowed. Copying not allowed.
```cpp
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int total = 0;
mutex m;

void adder()
{
    lock_guard<mutex> lock(m);
    total += 100;
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
```

### Unique Lock
* Ownership wrapper of a mutex. 
* Very similar to lock guard, but with slightly more features.
  * Destructor auto-unlocks if needed.
  * Has flexibility to lock later (not available in lock_guard).
* Syntax: 
  * Own a mutex m with `std::unique_lock<mutex> lock(m)`.
  * Ownership strategy can be used with: `std::unique_lock<mutex> lock(m, [locking strategy])`
* Features:
  * Allows multiple strategies for locking
    * No strategy: Calls regular `.lock()` on it. 
    * defer_lock: Doesn't acquire ownership of mutex
    * try_to_lock: Proceed without blocking (try_lock)
    * adopt_lock: Assumes thread has ownership already.
  * Time Constrained locking (try_lock_for/until)
  * Recursive locking
  * Transferring lock ownership
  * Condition variables
* Code:
```cpp
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
``` 



## Condition Variables
A condition variable is used for making threads wait on a condition. Needs a mutex to work.
 
A thread first must acquire a mutex. Then it waits on a CV(with a specific condition to wait for) and unlocks the mutex while it waits. Then it can re-check the condition on receiving a notification from either of (`notify_one`/`notify_all`). If the condition it is waiting on is fulfilled, it will reacquire the lock and continue, else it keeps waiting.


```cpp
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

#define ll long long

mutex m;
condition_variable cv;

ll sum = 0;

void producer(int x)
{
    lock_guard<mutex> lg(m);
    cout << "Producer" << endl;
    for (int i = 0; i < x; i++)
    {
        sum += i * (i - 1) / 2;
    }

    cv.notify_one();
}

void consumer(int x)
{
    unique_lock<mutex> ul(m);
    cout << "Consumer pre-wait" << endl;
    cv.wait(ul, []
            { return sum > 0; });
    cout << "Consumer post-wait" << endl;
    cout << sum * 100 << endl;
}

int main()
{
    std::thread c(consumer, 10);
    std::thread p(producer, 10);

    p.join();
    c.join();

    return 0;
}
```

## Future and Promise
* Promise: Acts like a holder for a value.
* Future: A future is a child of a Promise object. When the 'promise'd value is set, you can call get from its future object.
* Promise can be moved after extracting its future. 
* Syntax:
  * `promise<T> p;` creates a promise. 
  * `future<T> f = p.get_future();` gets its future.
  * `p.set_value(val)` sets promised value.
  * `f.get()` waits till it gets the promised value and continues from there.
* Possible to move promise to child thread after extracting future in parent thread. `set_value` called in child thread and `get` called in parent thread. 

Code:
```cpp
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
```

## Async task and launch policies
Use almost exactly like threads API, just starts with launch policy as first param (rest are the same).
Launch policy may be `std::launch::async` or `std::launch::deferred` or `std::launch::async | std::launch::deferred`. 

Policies
* `std::launch::async`: Eager execution, requests thread from thread pool to run task immediately.
* `std::launch::deferred`: Lazy execution, requests thread from thread pool as need arises.
* both: Does whichever it 

```cpp
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
```