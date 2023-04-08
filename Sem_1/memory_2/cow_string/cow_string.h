
#include<cstddef>

class CowString {
private:
    struct State{
        size_t Size_;
        size_t Capacity_;
        int ref_count;
        char* data;
        bool uncopyable;

        State() : Size_(0), Capacity_(2), ref_count(1), uncopyable(false){
            data = new char[2];
        }

        State(size_t size_, size_t capacity_, char* data_) : uncopyable(false){
            ref_count = 1;
            Size_ = size_;
            Capacity_ = capacity_;
            data = new char[capacity_];
            for(size_t i = 0; i < size_; ++i){
                data[i] = data_[i];
            }
        }
    
        ~State(){
            delete[] data;
        }
        

    };
    
    State* cur_state;
public:
    // constrcutors
    // copy operator=
    CowString operator=(CowString& other){
        if(cur_state->ref_count > 1){
            cur_state->ref_count--;
        }
        cur_state = other.cur_state;
        cur_state->ref_count++;
        return *this;
    }

    CowString() : cur_state (new State()) {}
    /*String a( "some text" );
    String b( a );
    a[4] = '-';*/
    CowString(const CowString& other){
        cur_state = other.cur_state;
        cur_state->ref_count++;

    }


    ~CowString(){
        cur_state->ref_count--;
        if(cur_state->ref_count == 0){
            delete cur_state;
            
        }
    }

    const char& At(size_t index) const{
        return (cur_state -> data[index]);
    }

    char& operator[](size_t index){
        if(cur_state -> ref_count > 1){
            --cur_state->ref_count;
            cur_state = new State(cur_state->Size_, cur_state->Capacity_, cur_state->data);
        }
        //cur_state->uncopyable = true;
        return cur_state->data[index];
    }

    const char& Back() const{
        return cur_state->data[Size() - 1];
    }

    void PushBack(char c){
        if(cur_state ->ref_count > 1){
           --cur_state->ref_count;
            cur_state = new State(cur_state->Size_, cur_state->Capacity_, cur_state->data);
           
        }
        //cur_state->uncopyable = true;
        if(Size() == Capacity()){
            Reserve(Capacity() * 2);
        }
        cur_state->data[Size()] = c;
        cur_state->Size_++;
    }

    size_t Size() const{
        return cur_state->Size_;
    }
    size_t Capacity() const{
        return cur_state->Capacity_;
    }
    
    void Reserve(size_t newCapacity){
        if (newCapacity > Capacity()){
            char* newData = new char[newCapacity];
            for (size_t i = 0; i < Size(); ++i){
                newData[i] = cur_state->data[i];
            }
            delete[] cur_state->data;
            cur_state->data = newData;
            cur_state->Capacity_ = newCapacity;
        }
    }
    void Resize(size_t newSize){
        if (newSize > Capacity()){
            Reserve(newSize);
        }
        cur_state->Size_ = newSize;
    }
};
