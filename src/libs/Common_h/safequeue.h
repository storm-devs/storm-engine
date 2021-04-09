#pragma once

#include "windows.h"

template <class Type> class SafeQueue; // forward declaration
template <class Type> class _safe_queue_item
{
    Type value;
    _safe_queue_item *next;
    _safe_queue_item()
    {
        next = nullptr;
    };
    _safe_queue_item(const Type &val)
    {
        value = val;
        next = nullptr;
    };
    ~_safe_queue_item(){};
    void DeleteQueue()
    { // delete all items
        if (next != nullptr)
            next->DeleteQueue();
        delete this;
    };
    friend class SafeQueue<Type>; // the one and the only one friend
                                  // who can use this class
};

template <class Type> class SafeQueue
{
    CRITICAL_SECTION lock;
    _safe_queue_item<Type> *first; // head pointer
    _safe_queue_item<Type> *last;  // Tail pointer

  public:
    bool Push(const Type &item) // Add element at the end of queue
    {
        EnterCriticalSection(&lock);
        _safe_queue_item<Type> *next = new _safe_queue_item<Type>(item);
        if (first == nullptr)
            last = first = next;
        else
        {
            last->next = next;
            last = next;
        }
        LeaveCriticalSection(&lock);
        return true;
    };
    Type Pop()
    { // Retrieve element from the head of queue, and remove it from queue
        EnterCriticalSection(&lock);
        Type val;
        _safe_queue_item<Type> *oldFirst = first;
        if (first == nullptr)
        {
            LeaveCriticalSection(&lock);
            throw "Queue underflow !";
        }
        else
        {
            val = first->value;
            first = first->next;
            delete oldFirst;
            LeaveCriticalSection(&lock);
        }
        return val;
    };
    bool IsEmpty()
    { // check if queue is empty
        EnterCriticalSection(&lock);
        bool empty = (first == nullptr);
        LeaveCriticalSection(&lock);
        return empty;
    };
    void Reset()
    { // reset contents of the queue
        EnterCriticalSection(&lock);
        if (first != nullptr)
        {
            first->DeleteQueue();
            first = nullptr;
        }
        LeaveCriticalSection(&lock);
    };
    SafeQueue()
    {
        InitializeCriticalSection(&lock);
        first = last = nullptr;
    };
    ~SafeQueue()
    { // destructor - deletes all
        EnterCriticalSection(&lock);
        if (first != nullptr)
            first->DeleteQueue();
        first = nullptr;
        LeaveCriticalSection(&lock);
        DeleteCriticalSection(&lock);
    };
};
