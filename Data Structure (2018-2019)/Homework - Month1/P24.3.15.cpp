// 我对"过程"与"函数"两种写法的讨论：
// 函数式编程倾向于将实现过程分解为互相独立的"小模块"，也就是函数
// 在分解出来的函数基础上，额外包装新的功能，从而形成更"高级"的函数
// 我的写法是比较类似面向过程的，如果用函数式的写法，可以将两个栈之间类似的操作独立成一个函数
// 通过 f = (x) -> ++x, sub_func(stack, f) 的形式来编写 push/pop 函数
// 其中 f 根据操作的是哪个栈来进行变换

// 综上，面向过程的方法更符合直觉，代码读起来更直观，但是复用性较差
// 函数式编程可以提高代码的复用率，但是代码可能会比较零散，对编写者要求较高


#define abs(x) ((x)>0 ? (x) : -(x))
template <typename T>
struct Stack {
    T *base, *top;
};


template <typename T>
class TwoWayStack {
public:
    enum WhichStack {
        Stack_Left = 0,
        Stack_Right = 1
    };


public:
    // public for testing convenience
    Stack<T> stack[2];
    int max_size, used_size;
    T * &memory = stack[0].base;

public:
    explicit TwoWayStack(int initial_size = 4)
            : max_size(initial_size), used_size(0) {
        init(initial_size);
    }
    ~TwoWayStack() {
        clear();
    }
    bool init(int initial_size = 4);
    bool clear();
    bool push(WhichStack ws, const T &elem);
    T pop(WhichStack ws);
};


template<typename T>
bool TwoWayStack<T>::push(TwoWayStack::WhichStack ws, const T &elem) {
    if (used_size >= max_size) {
        // to enlarge the allocated memory size
        // i googled for how to do something like realloc() did in C,
        // but i can't find a satisfying solution,
        // so i would just simply new[] a new object
        // and delete[] the old one manually

        int new_size = (2 * max_size) ?: 4;
        max_size = new_size;
        T *mem = new T[new_size];

        if (memory == nullptr) {
            stack[0].base = stack[0].top = mem;
            stack[1].base = stack[1].top = mem+(new_size-1);

        } else {
            // move the old datas to the new one
            T *old_mem = memory;
            int i;

            i = 0;
            while (stack[0].base != stack[0].top) {
                mem[i] = *stack[0].base++;
                ++i;
            }
            stack[0].base = mem;
            stack[0].top = mem+i;

            i = new_size-1;
            while (stack[1].base != stack[1].top) {
                mem[i] = *stack[1].base--;
                --i;
            }
            stack[1].base = mem+new_size-1;
            stack[1].top = mem+(new_size-(max_size-1-i));

            delete[] old_mem;
        }
    }

    ++used_size;
    switch (ws) {
        case Stack_Left:
            *stack[Stack_Left].top++ = elem;
            break;
        case Stack_Right:
            *stack[Stack_Right].top-- = elem;
            break;
        default:
            return false;
    }

    return true;
}

template<typename T>
T TwoWayStack<T>::pop(TwoWayStack::WhichStack ws) {
    if (used_size <= 0 || stack[ws].base == stack[ws].top)
        throw "do pop() on an empty stack";

    --used_size;
    switch (ws) {
        case Stack_Left:
            return *--stack[Stack_Left].top;
        case Stack_Right:
            return *++stack[Stack_Right].top;
        default:
            throw "illegal value";
    }
}

template<typename T>
bool TwoWayStack<T>::init(int initial_size) {
    if (initial_size == 0) {
        // special case: size == 0
        max_size = 0;
        used_size = 0;
        stack[0].base = stack[0].top = nullptr;
        stack[1].base = stack[1].top = nullptr;
        return true;
    }

    T *mem = new T[initial_size];
    // if (!mem) return false;
    // if failed, an exception would be automatically thrown in C++

    max_size = initial_size;
    used_size = 0;
    stack[0].base = stack[0].top = mem;
    stack[1].base = stack[1].top = mem+max_size-1;

    return true;
}

template<typename T>
bool TwoWayStack<T>::clear() {
    delete[] memory;
    return init(0);
}