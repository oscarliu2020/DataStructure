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
    for(int j=x->n;j>=i+1;j--)x->ch[j+1]=x->ch[j];
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
//    cout<<k<<": "<<x->key[0]<<'\n';
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
    y->key[t-1]=x->key[i];
    for(int j=0;j<z->n;j++)
        y->key[j+t]=z->key[j];
    if(!y->leaf)
        for(int j=0;j<=z->n;j++)
            y->ch[j+t]=z->ch[j];
    for(int j=i+1;j<x->n;j++)x->key[j-1]=x->key[j];
    for(int j=i+2;j<=x->n;j++)x->ch[j-1]=x->ch[j];
    y->n+=z->n+1;
    --x->n;
    delete z;
}

template<class T,int t,class Comp>
int Btree<T,t,Comp>::prev(BNode<T,t>* x)
{
    while(!x->leaf)
    {
        x=x->ch[x->n];
    }
    return x->key[x->n-1];
}
template<class T,int t,class Comp>
int Btree<T,t,Comp>::next(BNode<T,t>* x)
{
    while(!x->leaf)
    {
        x=x->ch[0];
    }
    return x->key[0];
}
template<class T,int t,class Comp>
int Btree<T,t,Comp>::findKey(BNode<T,t>* x,T k)
{
    int i=0;
    while(i<x->n&&cmp(x->key[i],k))++i;
    return i;
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::remove(T k,BNode<T,t>* x){
    int idx=findKey(x,k);
    if(idx<x->n&&x->key[idx]==k){
        if(x->leaf)
            removeLeaf(x,idx);
        else
            removeNonLeaf(x,idx);
    }else{
        if(x->leaf){
            cout<<"not exist.\n";
            return;
        }
        bool flag=(idx==x->n);
        if(x->ch[idx]->n<t)
            fill(x,idx);
        if(flag&&idx>x->n)
            remove(k,x->ch[idx-1]);
        else
            remove(k,x->ch[idx]);
    }
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::fromLeft(BNode<T,t>* x,int idx){
    int k=x->key[idx-1];
    BNode<T,t>* l=x->ch[idx-1];
    BNode<T,t>* cur=x->ch[idx];
    x->key[idx-1]=l->key[l->n-1];
    for(int i=cur->n;i>=1;i--){
        cur->key[i]=cur->key[i-1];
    }
    cur->key[0]=k;
    if(!cur->leaf){
        for(int i=cur->n;i>=0;i--)
                cur->ch[i+1]=cur->ch[i];
    }
    if(!cur->leaf)
        cur->ch[0]=l->ch[l->n];
    --l->n;
    ++cur->n;
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::fromRight(BNode<T,t>* x,int idx){
    int k=x->key[idx];
    BNode<T,t>* r=x->ch[idx+1];
    BNode<T,t>* cur=x->ch[idx];
    x->key[idx]=r->key[0];
    cur->key[cur->n]=k;
    if(!cur->leaf)
        cur->ch[cur->n+1]=r->ch[0];
    for(int i=1;i<r->n;i++)
        r->key[i-1]=r->key[i];
    if(!r->leaf)
        for(int i=1;i<=r->n;i++)
            r->ch[i-1]=r->ch[i];
    --r->n;
    ++cur->n;

}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::fill(BNode<T,t>* x,int idx){
    if(idx&&x->ch[idx-1]->n>=t)
        fromLeft(x,idx);
    else if(idx!=x->n&&x->ch[idx+1]->n>=t)
        fromRight(x,idx);
    else{
        if(idx!=x->n)
            merge(x,idx);
        else
            merge(x,idx-1);
    }
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::removeLeaf(BNode<T,t>* x,int i){
    for(int j=i+1;j<x->n;j++)
        x->key[j-1]=x->key[j];
    --x->n;
}
template<class T, int t,class Comp>
void Btree<T,t,Comp>::removeNonLeaf(BNode<T,t>* x,int id){
    
    if(x->ch[id]->n>=t){
        int p=prev(x->ch[id]);
        x->key[id]=p;
        remove(p,x->ch[id]);
    }else if(x->ch[id+1]->n>=t){
        int s=next(x->ch[id+1]);
        x->key[id]=s;
        remove(s,x->ch[id+1]);
    }else{
        merge(x,id);
        remove(x->key[id],x->ch[id]);
    }
}
template<class T,int t,class Comp>
void Btree<T,t,Comp>::remove(T k){
    if(!root)cout<<"The tree is empty\n";
    remove(k,root);
    if(root->n==0){
        BNode<T,t>* tmp=root;
        if(root->leaf)
            root=NULL;
        else
            root=root->ch[0];
        delete tmp;
    }
}
