template <typename T>
int binary_search(const T &elem, const T *va, int length);

template <typename T>
void insert_into_nonstrictly_increasing_ordered_list(const T &elem, T *va, int &length) {
    int pos = binary_search(elem, va, length);

    for (int i = length; i > pos; --i)
        // move move move!
        va[i] = va[i-1];
    va[pos] = elem;

    ++length;
}


template <typename T>
int binary_search(const T &elem, const T *va, const int length) {
    // @return:
    //   the appropriate position to put the element in
    // IMPORTANT:
    //   this function ISN'T meant to find EXACTLY where `elem` is

    // exit the recursions when R == L+1 (that is, M == L)

    // special case: the array is empty
    if (length == 0)
        return 0;

    int L = 0, R = length - 1, M = (L+R)/2;

    // special case: out of the range of `va`
    // can also deal with the case that (length == 1)
    if (elem < va[L])
        return L;
    else if (elem > va[R])
        return R+1;

    if (va[M] > elem) {
        if (M == L)
            return -1;
        else
            return L+binary_search(elem, va+L, M-L+1);  // [L, M]

    } else if (va[M] < elem) {
        if (M == L)
            return M+1;
        else
            return M+binary_search(elem, va+M, R-M+1);  // [M, R]

    } else {  // va[M] == elem
        return M;
    }
}
