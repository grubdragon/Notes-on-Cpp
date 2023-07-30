# Design Patterns in Cpp [Work In Progress]
Well known patterns (and anti-patterns) that are well established to solve classical problems. Useful to know when to apply them.

## Singleton Pattern

### What?
This design pattern addresses the issue of ensuring that there is only a single instance of a class. 

### Why?
This is useful when exactly one object is needed to coordinate actions across the system. The concept is sometimes generalized to systems that operate more efficiently when only one object exists, or that restrict the instantiation to a certain number of objects.


### Types
- Eager initialization
- Lazy initialization
- Thread Safe Initialization (Eager/Lazy)

### Lazy and Eager Initialization
Lazy initialization is a technique used to delay the creation of an object or calculation of a value until it is actually needed. This can help improve performance and reduce memory usage. 

Eager initialization is the opposite of lazy initialization. In eager initialization, the instance of the class is created at the time of class loading, this is the easiest method to create a singleton class but it has a drawback that instance is created even though client application might not be using it.

```cpp
// Singleton class Example
class Singleton
{

private:
    static Singleton* instance;
    // Constructor should be private
    Singleton() {}
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

// Lazy initialization
int main()
{
    Singleton* s = s->getInstance();
    Singleton* r = r->getInstance();
    // now s and r are same instance
    return 0;
}

/*
// Eager initialization
int main()
{
    Singleton* s = s->getInstance();
    
    // Lots and lots of code

    Singleton* r = r->getInstance();
    
    return 0;
}
*/
```

### Thread Safety
The above implementation works fine in case of single threaded environment but when it comes to multithreaded systems, it can cause issues if multiple threads are inside the `if` condition at the same time. We have some methods to deal with this.

#### Pre-Thread Eager Initialization
We create the instance of the class before children threads are created.

Pros:
1. Very simple to implement.

Cons:
1. May lead to resource wastage. Because instance of class is created always, whether it is required or not.
2. Wastes pre-thread stage CPU time.


#### Plain Mutex
This is the simplest way to make the `getInstance()` method thread safe. Just put the method body inside the `lock()` and `unlock()`.
Pros:
1. Simple to implement
2. Lazy initialization
   
Cons:
1. Slow performance because of locking overhead.
2. Unnecessary synchronization that is not required once the instance variable is initialized.
   
```cpp
// class definition ...

private:
    mutex m;
public:
    static Singleton* getInstance()
    {
        // Thread Safe Initialization
        m.lock();
        if (instance == 0)
        {
            instance = new Singleton();
        }
        m.unlock();

        return instance;
    }
// ... rest of the code
```

#### Double Checked Locking
Double checked locking principle is a software design pattern used to reduce the overhead of acquiring a lock by first testing the locking criterion (the "lock hint") without actually acquiring the lock. Only if the locking criterion check indicates that locking is required does the actual locking logic proceed.

Pros:
1. Needs only one check of the instance variable, instead of two, when the instance is already created.
2. Performance almost comparable to the plain single threaded lazy initialization method.

```cpp
// class definition ...

private:
    mutex m;
public:
    static Singleton* getInstance()
    {
        // Thread Safe Initialization
        if (instance == 0)
        {
            m.lock();
            if (instance == 0)
            {
                instance = new Singleton();
            }
            m.unlock();
        }
        return instance;
    }
// ... rest of the code
```

