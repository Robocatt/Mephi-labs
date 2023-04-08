#include <cstdlib>
#include <iterator>

/*
 * Нужно написать функцию, которая принимает на вход диапазон, применяет к каждому элементу данную операцию и затем складывает результат применения операции в новый диапазон
 * Входной диапазон задан итераторами [firstIn; lastIn)
 * Выходной диапазон начинается с firstOut и имеет такую же длину как входной диапазон
 * Операция является функцией с одним аргументом (унарная функция), возвращающая результат такого типа, который можно положить в OutputIt
 */

template<class InputIt, class OutputIt, class UnaryOperation>
void Transform(InputIt firstIn, InputIt lastIn, OutputIt firstOut, UnaryOperation op) {
    while (firstIn != lastIn)
    {
        *firstOut++ = op(*firstIn++);
    }
}

/*
 * Нужно написать функцию, которая принимает на вход диапазон и переставляет элементы в нем таким образом, чтобы элементы,
 * которые удовлетворяют условию p, находились ближе к началу диапазона, чем остальные элементы.
 * Входной диапазон задан итераторами [first; last)
 * p является функцией с одним аргументом (унарная функция), возвращающая результат типа bool
 */

template<class BidirIt, class UnaryPredicate>
void Partition(BidirIt first, BidirIt last, UnaryPredicate p) {
    BidirIt pos = first;
    while (first != last){
        if(p(*first)){
            std::iter_swap(first, pos);
            pos++;
        }
        first++;
    }
}

/*
 * Нужно написать функцию, которая принимает на вход два отстотированных диапазона и объединяет их в новый отсортированный диапазон, содержащий все элементы обоих входных диапазонов.
 */
template<class InputIt1, class InputIt2, class OutputIt>
void Merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt firstOut) {
    while (first1 != last1 && first2 != last2){
        if(*first1 < *first2){
            *firstOut++ = *first1++;
        }else{
            *firstOut++ = *first2++;
        }
    }

    while (first1 != last1){
        *firstOut++ = *first1++;
    }

    while (first2 != last2){
        *firstOut++ = *first2++;
    }

}


/*
 * Напишите класс "диапазон чисел Фибоначчи"
 * Экземпляр класса представляет из себя диапазон от первого до N-го числа Фибоначчи (1, 2, 3, 5, 8, 13 и т.д.)
 * С помощью функций begin и end можно получить итераторы и пробежать по диапазону или передать их в STL-алгоритмы
 */

class FibonacciRange {
public:

    class Iterator {
        friend class FibonacciRange;
    public:
        using value_type = uint64_t;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::input_iterator_tag;

        Iterator(value_type prev_, value_type cur_, int num_) : prev(prev_), cur(cur_), num(num_) {}

        value_type operator *() const {
            return cur;
        }

        Iterator& operator ++() {
            //prefix increment
            value_type next = cur + prev;
            prev = cur;
            cur = next;
            num++;
            return (*this);
        }

        Iterator operator ++(int) {
            // postfix increment
            Iterator copy = (*this);
            ++(*this);
            return copy;
        }

        bool operator ==(const Iterator& rhs) const {
            return num == rhs.num;
        }

        bool operator !=(const Iterator& rhs) const {
            return !(this->num == rhs.num);
        }

        bool operator <(const Iterator& rhs) const {
            return cur < rhs.cur;
        }
        value_type prev;
        value_type cur;
        int num;
    };

    FibonacciRange(size_t amount_) : amount(amount_)  {}

    Iterator begin() const {
        return Iterator(1,1,1);
    }

    Iterator end() const {
        return Iterator(0, 0, amount + 1);
    }

    size_t size() const {
        return amount;
    }
private:
    size_t amount;
    
};
