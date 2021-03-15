#include <vector>
#include <functional>


// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
template<typename T, typename Z>
void heapify(T& arr, int n, int i, std::function<bool(Z&, Z&)> greater)
{
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (l < n && greater(*std::next(arr.begin(), l), *std::next(arr.begin(), largest)))
        largest = l;

    // If right child is larger than largest so far
    if (r < n && greater(*std::next(arr.begin(), r), *std::next(arr.begin(), largest)))
        largest = r;

    // If largest is not root
    if (largest != i) {
        std::swap(*std::next(arr.begin(), i), *std::next(arr.begin(), largest));

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest, greater);
    }
}

// main function to do heap sort
template<typename T>
void heapSort(T& arr, int n, std::function<bool(T&, T&)> greater)
{
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i, greater);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        std::swap(*arr.begin(), *std::next(arr.begin(), i));

        // call max heapify on the reduced heap
        heapify(arr, i, 0, greater);
    }
}

template<typename T>
bool greater(T& a, T& b) {
    return a > b;
}

template <typename T>
void heapSort(T& arr, std::function<bool(T&, T&)> greater=greater) {
    heapSort(arr, arr.size(), greater);
}
