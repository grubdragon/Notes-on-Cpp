#include <iostream>
using namespace std;
class Singleton
{

private:
    static Singleton* instance;
    // Constructor should be private
    Singleton() {
        cout << "Singleton constructor called" << endl;
    }

public:
    static Singleton* getInstance()
    {
        if (instance == 0)
        {
            instance = new Singleton();
        }
        return instance;
    }
};

Singleton* Singleton::instance = 0;

int main()
{
    Singleton* s = s->getInstance();
    Singleton* r = r->getInstance();
    // now s and r are same instance
    cout << s << " " << r << endl;
    return 0;
}
