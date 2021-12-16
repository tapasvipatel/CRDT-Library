#ifndef __COUNTER_H__
#define __COUNTER_H__

template<typename T=int32_t>
class counter
{
private:
    T payload;
protected:
    T query();
    void increment();
    void decrement();
public:
    counter<T>();
    counter<T>(T payload);
    counter<T>(const counter<T>& rhs);
    ~counter();

    counter<T> operator+(const counter<T>& rhs);
    counter<T> operator-(const counter<T>& rhs);
    counter<T> operator*(const counter<T>& rhs);
    counter<T> operator/(const counter<T>& rhs);
    counter<T> operator%(const counter<T>& rhs);
    counter<T> operator^(const counter<T>& rhs);
    bool operator&(const counter<T>& rhs);
    bool operator|(const counter<T>& rhs);
    bool operator~(const counter<T>& rhs);
    void operator=(const counter<T>& rhs);
    bool operator<(const counter<T>& rhs);
    bool operator>(const counter<T>& rhs);
    bool operator<=(const counter<T>& rhs);
    bool operator>=(const counter<T>& rhs);
    void operator++();
    void operator--();
    bool operator==(const counter<T>& rhs);
    bool operator!=(const counter<T>& rhs);
    void operator+=(const counter<T>& rhs);
    void operator-=(const counter<T>& rhs);
    void operator/=(const counter<T>& rhs);
    void operator*=(const counter<T>& rhs);
};

#endif  // __COUNTER_H__
