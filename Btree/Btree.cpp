#include "Btree.hpp"
#include<iostream>
#include<cassert>
using std::cout;
template<class T,int t,class Comp>
void Btree< T, t, Comp>::split(BNode<T,t>* x,int i){
    BNode<T,t>* z=new BNode<T,t>();
    BNode<T,t>* y=x->ch[i];
    z->leaf=y->leaf;
    for(int j=0;j<t-1;j++)
        z->key[j]=y->key[j+t];
    if(!y->leaf)
        for(int j=0;j<t;j++)
            z->ch[j]=y->ch[j+t];
    y->n=z->n=t-1;
    for(int j=x->n;i=j>=i+1;j--)x->ch[j+1]=x->ch[j];
    x->ch[i+1]=z;
    for(int j=x->n-1;j>=i;j--)x->key[j+1]=x->key[j];
    x->key[i]=y->key[t-1];
    ++x->n;
    assert(x->ch[i]==y&&x->ch[i+1]==z);
   // cout<<"split: "<<x->n<<' '<<y->key[0]<<' '<<z->key[0]<<'\n';
}
template<class T,int t,class Comp>
void Btree< T, t, Comp>::insert(T k){
    if(root->n==2*t-1)
    {
       // cout<<"full ";
        BNode<T,t>* s=new BNode<T,t>();
        s->n=0;
        s->ch[0]=root;
        s->leaf=false;
        root=s;
        split(s,0);
        insertNonFull(s,k);
        
    }else{
        //cout<<"nonfull ";
        insertNonFull(root,k);
        
    }
    //cout<<"insert "<<root->n<<'\n';
}
template<class T,int t,class Comp>
void Btree< T, t, Comp>::insertNonFull(BNode<T,t>* x, T k){
    cout<<k<<": "<<x->key[0]<<'\n';
    if(x->leaf){
        int i=x->n-1;
        //cout<<x->n<<" *\n";

        while(i>=0&&cmp(k,x->key[i]))x->key[i+1]=x->key[i],--i;
        //out<<x->n<<" *\n";
        x->key[i+1]=k;
        //cout<<x->n<<" *\n";
        ++x->n;
        //cout<<x->n<<" *\n";
    }else{
        //cout<<".\n";
        int i=x->n-1;
        while(i>=0&&cmp(k,x->key[i]))--i;
        ++i;
        if(x->ch[i]->n==2*t-1){
            split(x,i);
            if(i<cmp(x->key[i],k))++i;
        }
        insertNonFull(x->ch[i],k);
        //cout<<".\n";
    }
}
template<class T,int t,class Comp>
BNode<T,t>* Btree< T, t, Comp>::search(T k){
    BNode<T,t>* cur=root;
    while(cur){
        int i=0;
        while(i<cur->n&&cmp(cur->key[i],k))i++;
        if(i<cur->n&&!cmp(k,cur->key[i]))return cur;
        cur=cur->ch[i];
    }
    return NULL;
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::traverse()
{
    
    for(int i=0;i<root->n;i++){
        if(!root->leaf)traverse(root->ch[i]);
        cout<<" "<<root->key[i];
    }
    traverse(root->ch[root->n]);
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::traverse(BNode<T,t>* p)
{
    
    for(int i=0;i<p->n;i++){
        if(!p->leaf)traverse(p->ch[i]);
        cout<<" "<<p->key[i];
    }
    if(!p->leaf)traverse(p->ch[p->n]);
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::merge(BNode<T,t>* x,int i){
    BNode<T,t>* y=x->ch[i];
    BNode<T,t>* z=x->ch[i+1];

    for(int j=0;j<z->n;j++)
        y->key[j+t]=z->key[j];
    if(!y->leaf)
        for(int j=0;j<=z->n;j++)
            y->ch[j]=z->ch[j+t];
    for(int j=i+1;j<x->n;j++)x->key[j-1]=x->key[j];
    
    for(int j=i+2;j<=n;j++)x->ch[j-1]=x->ch[j];
    y->n+=z->n+1;
    --x->n;
    delete z;
}

template<class T,int t,class Comp>
int Btree<T,t,Comp>::prev(T k)
{
    
}
template<class T,int t,class Comp>
int Btree<T,t,Comp>::next(T k)
{
    
}
template<class T,int t,class Comp>
int Btree<T,t,Comp>::findKey(BNode<T,t>* x,T k)
{
    int i=0;
    while(i<x->n&&cmp(x->key[i],k))++i;
    if(!cmp(x,x->key[i]))return i;
    return -1;
}
