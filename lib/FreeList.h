#include <vector>

template <class T>
class FreeList
{
public:
    /// Creates a new free list.
    FreeList();

    /// Inserts an element to the free list and returns an index to it.
    size_t Insert(const T& element);

    // Removes the nth element from the free list.
    void Erase(size_t n);

    // Removes all elements from the free list.
    void Clear();

    // Returns the range of valid indices.
    size_t Range() const;

    // Returns the nth element.
    T& operator[](size_t n);

    // Returns the nth element.
    const T& operator[](size_t n) const;

private:
    union FreeElement
    {
        T       element;
        size_t  next;
    };
    std::vector<FreeElement> data;
    size_t first_free;
};

template <class T>
FreeList<T>::FreeList(): first_free(-1) {}

template <class T>
size_t FreeList<T>::Insert(const T& element) {
    if (first_free != -1) {
        const size_t index = first_free;
        first_free = data[first_free].next;
        data[index].element = element;

        return index;
    } else {
        FreeElement fe;
        fe.element = element;
        data.push_back(fe);

        return static_cast<size_t>(data.size() - 1);
    }
}

template <class T>
void FreeList<T>::Erase(size_t n) {
    data[n].next = first_free;
    first_free = n;
}

template <class T>
void FreeList<T>::Clear() {
    data.clear();
    first_free = -1;
}

template <class T>
size_t FreeList<T>::Range() const {
    return static_cast<size_t>(data.size());
}

template <class T>
T& FreeList<T>::operator[](size_t n) {
    return data[n].element;
}

template <class T>
const T& FreeList<T>::operator[](size_t n) const {
    return data[n].element;
}