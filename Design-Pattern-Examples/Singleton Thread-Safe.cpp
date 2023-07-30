// Thread safe example for singleton design pattern
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;
class Singleton
{

private:
    // static member variables
    static Singleton* instance;
    static mutex m;
        
    // Constructor should be private
    Singleton() {
        cout << "Singleton constructor called" << endl;
    }

public:
    static Singleton* getInstance()
    {
        if (instance == 0)
        {
            lock_guard<mutex> lock(m);
            if (instance == 0){
                instance = new Singleton();
            }
        }
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
mutex Singleton::m;

void singletonGet()
{
    Singleton* s = Singleton::getInstance();
    cout << s << endl;
}

int main()
{
    thread t1(singletonGet);
    thread t2(singletonGet);
    thread t3(singletonGet);
    thread t4(singletonGet);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}
