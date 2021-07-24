#pragma once

#include <string.h>

#define FIFO_BUFFER_MAX_ELEMENTS 50

template <class tEType> class TFIFOBuffer
{
  public:
    TFIFOBuffer() : firstElement(0), elementsCount(0)
    {
    }

    ~TFIFOBuffer()
    {
    }

    bool Push(tEType &_element)
    {
        if ((firstElement + elementsCount) < (FIFO_BUFFER_MAX_ELEMENTS - 1))
            elements[firstElement + (elementsCount++)] = _element;
        else // [---*******]
        {
            if (!firstElement)
                return false;

            memmove(&elements[0], &elements[firstElement], elementsCount * sizeof(tEType));
            firstElement = 0;
            elements[firstElement + (elementsCount++)] = _element;
        }

        return true;
    }

    bool Pop(tEType *_op = nullptr)
    {
        if (Free())
            return false;

        --elementsCount;
        if (_op)
            *_op = elements[firstElement];

        ++firstElement;
        return true;
    }

    bool Free()
    {
        return (!elementsCount);
    }

    void PopAll()
    {
        firstElement = 0;
        elementsCount = 0;
    }

    tEType *TopElement()
    {
        if (Free())
            return nullptr;

        return &elements[firstElement];
    }

  private:
    int firstElement, elementsCount;
    tEType elements[FIFO_BUFFER_MAX_ELEMENTS];
};
