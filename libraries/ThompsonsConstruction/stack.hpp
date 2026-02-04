#ifndef stack_hpp
#define stack_hpp

template <class T>
class Stack {
    private:
        T* info;
        int n;
        int maxN;
        void grow() {
            T* old = info;
            info = new T[2*maxN];
            for (int i = 0; i < maxN; i++) {
                info[i] = old[i];
            }
            delete [] old;
            maxN *= 2;
        }
        void init(int size) {
            maxN = size;
            n = 0;
            info = new T[maxN];
        }
    public:
        Stack(int size = 255) {
            init(size);
        }
        Stack(const Stack& st) {
            init(st.maxN);
            for (int i = 0; i < st.n; i++) {
                info[i] = st.info[i];
            }
            n = st.n;
        }
        ~Stack() {
            delete [] info;
        }
        bool empty() const {
            return n == 0;
        }
        int size() const {
            return n;
        }
        void push(T item) {
            if (n+1 == maxN) grow();
            info[n++] = item;
        }
        T& top() {
            return info[n-1];
        }
        T pop() {
            return info[--n];
        }
        void clear() {
            n = 0;
        }
        Stack& operator=(const Stack& st) {
            if (this != &st) {
                init(st.maxN);
                for (int i = 0; i < st.n; i++) {
                    info[i] = st.info[i];
                }
                n = st.n;
            }
            return *this;
        }
};

#endif