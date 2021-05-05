#ifndef SINGLETONMIXIN_H
#define SINGLETONMIXIN_H

template<typename T>
class Singleton : public T {
private:
    Singleton()
        :T() {}
    ~Singleton(){}

public:
    static Singleton& getInstance(){
        static Singleton instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

#endif