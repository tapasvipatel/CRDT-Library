namespace operation
{

template<typename T>
counter<T>::counter()
{
    this->payload = T();
}

template<typename T>
counter<T>::counter(T payload)
{
    this->payload = payload;
}

template<typename T>
counter<T>::counter(const counter<T>& rhs)
{
    this->payload = rhs.payload;
}

template<typename T>
counter<T>::~counter()
{
    ;
}

template<typename T>
T counter<T>::query()
{
    return this->payload;
}

template<typename T>
void counter<T>::increment()
{
    this->payload++;
}

template<typename T>
void counter<T>::decrement()
{
    this->payload--;
}

template<typename T>
counter<T> counter<T>::operator+(const counter<T>& rhs)
{
    return counter<T>(this->payload += rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator-(const counter<T>& rhs)
{
    return counter<T>(this->payload -= rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator*(const counter<T>& rhs)
{
    return counter<T>(this->payload *= rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator/(const counter<T>& rhs)
{
    return counter<T>(this->payload /= rhs.payload);
}

/*
* modulo operator is specialized for int32_t type only
*/
template<>
counter<int32_t> counter<int32_t>::operator%(const counter<T>& rhs)
{
    return counter<int32_t>(this->payload % rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator^(const counter<T>& rhs)
{
    return counter<T>(this->payload /= rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator^(const counter<T>& rhs)
{
    return counter<T>(this->payload ^ rhs.payload);
}

template<typename T>
bool counter<T>::operator^(const counter<T>& rhs)
{
    return this->payload & rhs.payload;
}

template<typename T>
bool counter<T>::operator^(const counter<T>& rhs)
{
    return this->payload & rhs.payload;
}

template<typename T>
bool counter<T>::operator|(const counter<T>& rhs)
{
    return this->payload | rhs.payload;
}

template<typename T>
bool counter<T>::operator~(const counter<T>& rhs)
{
    return this->payload ~ rhs.payload;
}
    
template<typename T>
void counter<T>::operator=(const counter<T>& rhs)
{
    return this->payload = rhs.payload;
}

template<typename T>
bool counter<T>::operator<(const counter<T>& rhs)
{
    return this->payload < rhs.payload;
}

template<typename T>
bool counter<T>::operator>(const counter<T>& rhs)
{
    return this->payload > rhs.payload;
}

template<typename T>
bool counter<T>::operator<=(const counter<T>& rhs)
{
    return this->payload <= rhs.payload;
}

template<typename T>
bool counter<T>::operator>=(const counter<T>& rhs)
{
    return this->payload >= rhs.payload;
}

template<typename T>
void counter<T>::operator++()
{
    this->payload++;
}

template<typename T>
void counter<T>::operator--()
{
    this->payload--;
}

template<typename T>
bool counter<T>::operator==(const counter<T>& rhs)
{
    return this->payload == rhs.payload;
}

template<typename T>
bool counter<T>::operator!=(const counter<T>& rhs)
{
    return this->payload != rhs.payload;
}

template<typename T>
void counter<T>::operator+=(const counter<T>& rhs)
{
    this->payload += rhs.payload;
}

template<typename T>
void counter<T>::operator-=(const counter<T>& rhs)
{
    this->payload -= rhs.payload;
}

template<typename T>
void counter<T>::operator/=(const counter<T>& rhs)
{
    this->payload /= rhs.payload;
}

template<typename T>
void counter<T>::operator*=(const counter<T>& rhs)
{
    this->payload *= rhs.payload;
}

}
