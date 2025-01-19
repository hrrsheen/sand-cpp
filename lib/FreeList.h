#ifndef FREE_LIST_HPP
#define FREE_LIST_HPP

#include <variant>
#include <vector>

#define _ELEMENT(x) std::get<0>(x);
#define _NEXT(x) std::get<1>(x);

template <class T>
class FreeList {
    static_assert(!std::is_same_v<T, int>);
private:
    std::vector<T>      data;
    std::vector<int>    next;
    int firstFree;

public:
    /// Creates a new free list.
    FreeList();

    /// Inserts an element to the free list and returns an index to it.
    int Insert(T& element);
    int Insert(T element);

    // Removes the nth element from the free list.
    void Erase(int n);

    // Removes all elements from the free list.
    void Clear();

    // Returns the range of valid indices.
    int Range() const;

    // Returns the nth element.
    T& operator[](int n);

    // Returns the nth element.
    const T& operator[](int n) const;
};

template <class T>
FreeList<T>::FreeList(): firstFree(-1) {}

template <class T>
int FreeList<T>::Insert(T& element) {
    if (firstFree != -1) {
        const int index = firstFree;
        firstFree = next[firstFree];
        data[index] = std::move(element);
        next[index] = -1;

        return index;
    } else {
        data.push_back(std::move(element));
        next.push_back(-1);

        return static_cast<int>(data.size() - 1);
    }
}

template <class T>
int FreeList<T>::Insert(T element) {
    if (firstFree != -1) {
        const int index = firstFree;
        firstFree = next[firstFree];
        data[index] = std::move(element);
        next[index] = -1;

        return index;
    } else {
        data.push_back(std::move(element));
        next.push_back(-1);

        return static_cast<int>(data.size() - 1);
    }
}

template <class T>
void FreeList<T>::Erase(int n) {
    next[n] = firstFree;
    data.erase(data.begin() + n);
    firstFree = n;
}

template <class T>
void FreeList<T>::Clear() {
    next.clear();
    data.clear();
    firstFree = -1;
}

template <class T>
int FreeList<T>::Range() const {
    return static_cast<int>(data.size());
}

template <class T>
T& FreeList<T>::operator[](int n) {
    return data[n];
}

template <class T>
const T& FreeList<T>::operator[](int n) const {
    return data[n];
}

#endif