#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

typedef unsigned int size_t;

template <typename Tp, size_t h, size_t w> class Matrix;
template <typename Tp, size_t h, size_t w> std::istream &operator>>(std::istream &is, Matrix<Tp, h, w> &mt);
template <typename Tp, size_t h, size_t w> std::ostream &operator<<(std::ostream &os, const Matrix<Tp, h, w> &mt);
template <typename Tp, size_t h1, size_t h2, size_t w1, size_t w2>
    Matrix<Tp, h1, w2> operator*(const Matrix<Tp, h1, w1> &mt1, const Matrix<Tp, h2, w2> &mt2);
    // let the compile deduce the h1, h2, w1 and w2 automatically

template <typename Tp, size_t h = 0, size_t w = 0>
class Matrix {
public:
    Matrix();
    Matrix(const Matrix &);
    Matrix(const Tp array[h][w]);
    ~Matrix();
    Matrix operator+(const Matrix &mt) const;
    Matrix operator-(const Matrix &mt) const;
    template <typename T, size_t h1, size_t h2, size_t w1, size_t w2>
        friend Matrix<T, h1, w2> operator*(const Matrix<T, h1, w1> &mt1, const Matrix<T, h2, w2> &mt2);
    inline Tp *operator[](const size_t at) const;
    Matrix operator-() const;
    friend std::istream &operator>> <Tp>(std::istream &is, Matrix &mt);
    friend std::ostream &operator<< <Tp>(std::ostream &os, const Matrix &mt);

private:
    Tp *head = 0;
};

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w>::Matrix() {
    assert((h == 0 && w == 0) || (h != 0 && w != 0));
    head = new Tp[h*w];
}

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w>::Matrix(const Matrix<Tp, h, w> &mt) {
    if (head && head != mt.head) delete[] head;
    // prevents `mt = mt;`

    head = new Tp[h*w];
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            *(head + i*w + j) = mt[i][j];
        }
    }
}

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w>::Matrix(const Tp array[h][w]) {
    if (head) delete[] head;

    head = new Tp[w*h];
    for (size_t i = 0; i<h; ++i) {
        for (size_t j = 0; j<w; ++j) {
            *(head + i*w + j) = array[i][j];
        }
    }
};

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w>::~Matrix() {
    delete[] head;
}

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w> Matrix<Tp, h, w>::operator+(const Matrix<Tp, h, w> &mt) const {
    Matrix temp;
    temp.head = new Tp[h*w];
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            *(temp.head + i*w + j) = (*this)[i][j] + mt[i][j];
        }
    }

    return temp;
}

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w> Matrix<Tp, h, w>::operator-(const Matrix<Tp, h, w> &mt) const {
    return *this + (-mt);
}

template <typename Tp, size_t h1, size_t h2, size_t w1, size_t w2>
Matrix<Tp, h1, w2> operator*(const Matrix<Tp, h1, w1> &mt1, const Matrix<Tp, h2, w2> &mt2) {
    assert(w1 == h2);

    if (!(h1 && h2 && w1 && w2)) {
        Matrix<Tp, h1, w2> temp;
        for (size_t i = 0; i < h1; ++i) {
            for (size_t j = 0; i < w2; ++j) {
                temp[i][j] = (Tp) 0;
            }
        }
        return temp;

    } else {
        Matrix<Tp, h1, w2> temp;
        temp.head = new Tp[h1*w2];
        memset(temp.head, 0, sizeof(Tp[h1*w2]));
        for (size_t i=0; i<h1; ++i)
            for (size_t j=0; j<w2; ++j)
                for (size_t k=0; k<w1; ++k)
                    temp.head[i*w2 + j] += mt1[i][k] * mt2[k][j];
        return temp;
    }
};

template <typename Tp, size_t h, size_t w>
Tp *Matrix<Tp, h, w>::operator[](const size_t at) const {
    assert(at < h);
    return head + at * w;
}

template <typename Tp, size_t h, size_t w>
Matrix<Tp, h, w> Matrix<Tp, h, w>::operator-() const {
    Matrix temp;
    temp.head = new Tp[w*h];
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            temp[i][j] = -(*this)[i][j];
        }
    }
    return temp;
}

template <typename Tp, size_t h, size_t w>
std::istream &operator>>(std::istream &is, Matrix<Tp, h, w> &mt) {
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            is >> *(mt.head + i*w + j);
        }
    }
    return is;
}

template <typename Tp, size_t h, size_t w>
std::ostream &operator<<(std::ostream &os, const Matrix<Tp, h, w> &mt) {
    os << "[ ";
    for (size_t i = 0; i < h; ++i) {
        if (i) os << "  ";

        for (size_t j = 0; j < w; ++j) {
            os << mt[i][j] << " ";
        }

        if (i != h - 1) os << std::endl;
    }
    os << "]";
    return os;
}

int main() {
    using namespace std;
    const int a[2][3] = {
        {1, 2, 3},
        {2, 3, 4},
    };
    const int b[2][3] = {
        {3, 2, 1},
        {6, 5, 4},
    };

    Matrix<int, 2, 3> mt_a(a), mt_b(b);
    Matrix<int, 3, 1> mt_c;
    cout << "please input num1 num2 num3: ";
    cin >> mt_c;

    cout << mt_a << endl << mt_b << endl << mt_c << endl;

    cout << endl << "mt_a + mt_b =" << endl;
    cout << mt_a + mt_b << endl;
    cout << endl << "mt_a - mt_b =" << endl;
    cout << mt_a - mt_b << endl;
    cout << endl << "mt_a * mt_c =" << endl;
    cout << mt_a * mt_c << endl;
}
