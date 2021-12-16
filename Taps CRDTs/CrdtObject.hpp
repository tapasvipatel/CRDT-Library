/*
    Copyright (c) 2021 [Tapasvi Patel, Rushab Roihan, Vishwa Gandhi, Winston Sun]

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __CRDTOBJECT__
#define __CRDTOBJECT__

#include <chrono>

template<typename T>
class CrdtObject
{
private:
    uint32_t id;
    auto date_last_modified;    //std::chrono::system_clock::now()
protected:
    virtual T query() = 0;
    virtual bool merge(const T& item) = 0;
    virtual bool serialize(std::string& buffer) = 0;
    virtual bool deserialize(std::string& buffer) = 0;
    virtual bool db_export() = 0;
    virtual bool db_import() = 0;
public:
    CrdtObject<T>();
    ~CrdtObject<T>();
};

#endif  // __CRDTOBJECT__
