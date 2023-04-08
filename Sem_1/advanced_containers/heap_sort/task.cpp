#include "task.h"

using namespace std;

class PriorityQueue
{
    int Left(int i) {
        return (2 * i + 1);
    }
 
    int Right(int i) {
        return (2 * i + 2);
    }
 

    void heapify(vector<int> &arr, int i, int size)
    {
        int left = Left(i);
        int right = Right(i);
        int largest = i;

        if (left < size && arr[left] > arr[i]) {
            largest = left;
        }
 
        if (right < size && arr[right] > arr[largest]) {
            largest = right;
        }

        if (largest != i)
        {
            swap(arr[i], arr[largest]);
            heapify(arr, largest, size);
        }
    }
 
public:
 
    PriorityQueue(vector<int> &arr, int n)
    {
        int i = (n - 2) / 2;
        while (i >= 0) {
            heapify(arr, i--, n);
        }
    }
 
    int pop(vector<int> &arr, int size)
    {
        if (size <= 0) {
            return -1;
        }
 
        int top = arr[0];
        arr[0] = arr[size-1];
        heapify(arr, 0, size - 1);
 
        return top;
    }
};

void heapsort(vector<int> &arr, int n)
{
    PriorityQueue pq(arr, n);
 
    while (n > 0)
    {
        arr[n - 1] = pq.pop(arr, n);
        n--;
    }
}

vector<int> HeapSort(const vector<int>& v) {
    vector<int> vec = v;
    heapsort(vec, v.size());
    return vec;

}
