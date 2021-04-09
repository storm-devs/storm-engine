//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*  description: Template Quick Sort class
//*
//****************************************************************
#pragma once

template <class TYPE> class GMXQSort
{
  public:
    void QSort(int (*compare_f)(TYPE, TYPE), TYPE *ArrayToSort, int uCount)
    {
        if (uCount <= 0)
            return;
        QSortRecursive(compare_f, ArrayToSort, 0, (uCount - 1));
    }

  protected:
    void QSortRecursive(int (*compare_func)(TYPE, TYPE), TYPE *pArr, int d, int h)
    {
        int i, j;
        TYPE str;

    begin:

        i = h;
        j = d;

        str = pArr[(d + h) >> 1];

        do
        {
            while (compare_func(pArr[j], str) && (j < h))
                j++;
            while (compare_func(str, pArr[i]) && (i > d))
                i--;

            if (i >= j)
            {
                if (i != j)
                {
                    TYPE zal;
                    zal = pArr[i];
                    pArr[i] = pArr[j];
                    pArr[j] = zal;
                }
                i--;
                j++;
            }
        } while (j <= i);

        if (d < i)
        {
            QSortRecursive(compare_func, pArr, d, i);
        }

        if (j < h)
        {
            d = j;
            goto begin;
        }
    }
};
