#include<functional>
template<class T>
struct RBNode{
    T key;
    char color:1;//0:black 1:red
    RBNode<T> *ch[2],pa;
    RBNode(const T& d,)
    ~RBNode(){
        if(ch[0])delete ch[0];
        if(ch[1])delete ch[1];
    }
};
template<class T,class comp=std::less<T>>
class RBTree{
    public:

    private:
    RBNode<T>* root
};