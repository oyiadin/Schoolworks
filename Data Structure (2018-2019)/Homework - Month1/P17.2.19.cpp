template <typename T>
struct Node {
    Node *next;
    T value;
};


template <typename T>
void remove_values_between(Node<T> *list, T mink, T maxk) {
    Node<T> *at = list, *next = at->next;
    // `next` is the very one to be compared with `mink` and `maxk`

    while (next) {
        if (next->value > mink) {
            Node<T> *nextnext = at->next;
            // if there aren't any elements to be deleted
            // this default value of `nextnext` would ensure that
            // everything is the same as its original state

            while (next && next->value < maxk) {
                // found a element in the range of (mink, maxk), deleting it

                nextnext = next->next;
                // the reason why `nextnext` should exist is that,
                // after i removed the compared one, `next`,
                // i can't find the bond towards the next element without a `nextnext`
                delete next;
                next = nextnext;
            }
            at->next = nextnext;
            return;
        }
        at = at->next;
        next = at->next;
    }
}

// 我的算法的时间复杂度：O(N)
// 分析：
// 算法分为两个阶段：
// * 阶段1：遍历此线性表，直到找到比 mink 大的元素
// * 阶段2：从阶段1找到的元素起，继续遍历该表，直到找到比 maxk 小的元素
//     在遍历的过程中，顺带将遍历到的每个元素进行删除
// 算法的时间耗费主要发生在两阶段的遍历操作上，从整体的角度看来，
// 该算法相当于从头开始遍历，直至找到 >=maxk 的元素为止
// 遍历的频度与问题规模呈线性关系，因此有时间复杂度为 O(N)
