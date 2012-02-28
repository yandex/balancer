#pragma once

#include <util/system/atomic.h>

//////////////////////////////
// lock free lifo stack
template <class T>
class TLockFreeStack
{
    struct TNode
    {
        T Value;
        TNode *Next;

        TNode() {
        }
        TNode(const T &val)
            : Value(val)
            , Next(0)
        {
        }
    };

    TNode *volatile Head;
    TNode *volatile FreePtr;
    TAtomic DequeueCount;

    void TryToFreeMemory()
    {
        TNode *current = FreePtr;
        if (!current)
            return;
        if (AtomicAdd(DequeueCount, 0) == 1) {
            // node current is in free list, we are the last thread so try to cleanup
            if (AtomicCas(&FreePtr, (TNode*)0, current))
                EraseList(current);
        }
    }
    void EraseList(TNode *volatile p)
    {
        while (p) {
            TNode *next = p->Next;
            delete p;
            p = next;
        }
    }
    TLockFreeStack(const TLockFreeStack&) {
    }
    void operator=(const TLockFreeStack&) {
    }
public:
    TLockFreeStack()
        : Head(0)
        , FreePtr(0)
        , DequeueCount(0)
    {
    }
    ~TLockFreeStack()
    {
        EraseList(Head);
        EraseList(FreePtr);
    }
    void Enqueue(const T& t)
    {
        TNode *volatile node = new TNode(t);
        for (;;) {
            node->Next = Head;
            if (AtomicCas(&Head, node, node->Next))
                break;
        }
    }
    bool Dequeue(T *res)
    {
        AtomicAdd(DequeueCount, 1);
        for (TNode *current = Head; current; current = Head) {
            if (AtomicCas(&Head, current->Next, current)) {
                *res = current->Value;
                // delete current; // ABA problem
                // even more complex node deletion
                TryToFreeMemory();
                if (AtomicAdd(DequeueCount, -1) == 0) {
                    // no other Dequeue()s, can safely reclaim memory
                    delete current;
                } else {
                    // Dequeue()s in progress, put node to free list
                    for (;;) {
                        current->Next = FreePtr;
                        if (AtomicCas(&FreePtr, current, current->Next))
                            break;
                    }
                }
                return true;
            }
        }
        TryToFreeMemory();
        AtomicAdd(DequeueCount, -1);
        return false;
    }
    bool IsEmpty()
    {
        AtomicAdd(DequeueCount, 0); // mem barrier
        return Head == 0; // access is not atomic, so result is approximate
    }
};
