#pragma once

#include <cassert>

#include <util/system/compat.h>
#include <util/system/filemap.h>
#include <util/system/defaults.h>
#include <util/memory/alloc.h>
#include <util/generic/cast.h>
#include <util/generic/stroka.h>
#include <util/generic/yexception.h>

// Интерфейс чтения двумерного массива, отображённого на диск.
// Строки массива могут содержать различное количество элементов.
// Формат файла, содержащего двумерный массив:
//     4 байта - версия
//     4 байта - количество строк
//     sizeof(I) * <количество строк> - смещения начал строк в массиве данных
//     sizeof(T) * <общее количество элементов массива> - данные
// I - тип, задающий смещения начал строк в массиве данных
// T - тип данных, хранимый в массиве
template<class I, class T>
class FileMapped2DArray : private TMappedFile {
    I*     s_Ptr; // Массив смещений начал одномерных массивов, соответсвующих строкам, в массиве данных m_Ptr
    T*     m_Ptr; // Массив данных
    ui32   s_Size; // Размер первого измерения двумерного массива (количество строк, размер массива s_Ptr)
    T*     m_Finish; // Указатель на первый элемент за массивом данных m_Ptr
    ui32   ElemSize;
    static char m_Dummy[sizeof(T)];
public:
    FileMapped2DArray() : s_Ptr(0), m_Ptr(0), s_Size(0), m_Finish(0), ElemSize(0) {
        memset(m_Dummy, 0, sizeof m_Dummy);
    }
    explicit FileMapped2DArray(const Stroka & name)
        : TMappedFile(~name)
    {
        memset(m_Dummy, 0, sizeof m_Dummy);
        if (!InitSelf())
            ythrow yexception() << "cannot init mapped 2D array";
    }
    ~FileMapped2DArray() {
        /// don't call term() here as ~MappedFile also calls it's term()
        m_Ptr = 0;
    }
    bool init(const char *name) {
        TMappedFile::init(name);
        return InitSelf();
    }
    void term() {
        TMappedFile::term();
        s_Size = 0;
        s_Ptr = 0;
        m_Ptr = 0;
        m_Finish = 0;
    }
    const T *operator[](size_t pos1) const {
        assert(pos1 < size());
        // Для совместимости с файлами, созданными 32-битными программами:
        // Для некоторых файлов (например cmap-ов) смещения начал одномерных массивов в s_Ptr
        // зачем-то записывались декрементироваными на 1
        // Так вполне могло случиться, что там записано I(-1) (всегда было s_Ptr[0] == I(-1)),
        // а т.к I - обычно ui32, то
        // m_Ptr + s_Ptr[pos1] = m_Ptr + 0xFFFFFFFF в результате переполнений на 32-битной машине
        // давало m_Ptr - 1. На 64-битной машине этого не происходит и для смещения ui32(-1)
        // получается большой указатель, не имеющий к реальным данным никакого отношения.
        const I& offset = s_Ptr[pos1];
        if (offset == I(-1)) {
            return m_Ptr - 1;
        } else {
            return m_Ptr + offset;
        }
    }
    const T *very_end() const {
        return m_Finish;
    }
    size_t size() const {
        return s_Size;
    }
    const T &GetAt(size_t pos1, size_t pos2) const {
        if (pos1 >= s_Size)
            return *ReinterpretCast<T*>(m_Dummy);
        const T* const ptr = operator[](pos1) + pos2;
        if (ptr >= m_Finish)
            return *ReinterpretCast<T*>(m_Dummy);
        return *ptr;
    }
    const T * GetBegin(size_t pos1) const {
        if (pos1 >= s_Size)
            return NULL;
        return operator[](pos1);
    }
    const T * GetEnd(size_t pos1) const {
        if (pos1 >= s_Size)
            return NULL;
        return pos1 + 1 < size() ? operator[](pos1 + 1) : m_Finish;
    }
    size_t GetLength(size_t pos1) const {
        if (size())
        {
            const T* begin = operator[](pos1);
            const T* end;
            if (pos1 + 1 < size())
                end = operator[](pos1 + 1);
            else
                end = (s_Ptr[0] == I(-1)) ? m_Finish - 1 : m_Finish;
            return end - begin;
        } else
            return 0;
    }

private:
    bool InitSelf()
    {
        ElemSize = *(ui32*)getData();
        s_Size = 1[(ui32*)getData()];
        s_Ptr = (I*)((ui32*)getData() + 2);
        m_Ptr = (T*)(s_Ptr + s_Size);
        m_Finish = (T*)((char*)getData() + getSize());
        if (((char*)m_Finish - (char*)m_Ptr) % sizeof(T)) {
            term();
            return false;
        }
        return true;
    }
};

template<class I, class T> char FileMapped2DArray<I, T>::m_Dummy[sizeof(T)];
