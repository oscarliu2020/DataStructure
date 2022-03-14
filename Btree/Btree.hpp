#ifndef Btree_hpp
#define Btree_hpp
#include<algorithm>
template<class T,int t=2>
struct BNode{
    T key[2*t-1];
    int n=0;
    BNode<T,t>* ch[2*t]={0};
    bool leaf;
    ~BNode(){
        for(int i=0;i<=n;i++)delete ch[i];
    }
};
template<class T,int t=2,class Comp=std::less<T> >
class Btree{
public:
    Btree(const Comp &p=Comp()){
        root=new BNode<T,t>();
        root->leaf=true;
        root->n=0;
    }
    ~Btree(){
        delete root;
    }
    BNode<T,t>* search(T key);
    void insert(T key);
    void traverse();
    void remove(T key);
private:
    void remove(T key,BNode<T,t>* x);
    void removeLeaf(BNode<T,t>*,int );
    void removeNonLeaf(BNode<T,t>*,int);
    int findKey(BNode<T,t>* x,T key);
    void merge(BNode<T,t>* x,int i);
    void traverse(BNode<T,t>* p);
    void split(BNode<T,t>* x,int i);
    void insertNonFull(BNode<T,t>* x,T key);
    int prev(BNode<T,t>* x);
    int next(BNode<T,t>* x);
    void fill(BNode<T,t>* x,int idx);
    void fromLeft(BNode<T,t>* ,int);
    void fromRight(BNode<T,t>*,int);
    BNode<T,t>* root;
    Comp cmp;
};
template class BNode<int>;
template class Btree<int>;
#endif /* Btree_hpp */
