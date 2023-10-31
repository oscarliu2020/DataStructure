#include<iostream>
#include<memory>
#include<vector>
#include<assert.h>
using namespace std;
struct Node{
    //static int count;
    using Ptr=shared_ptr<Node>;
    using Ptrw=weak_ptr<Node>;
    int key;
    size_t sz;
    size_t count;
    //1: red, 0: black
    unsigned int color:1;
    Ptr left,right;
    Ptrw pa;
    Node(int key){
        //++count;
        this->key = key;
        left = right = nullptr;
        this->color = 1;
        sz=1;
        count=1;
    }
    void update_size(){
        sz=count;
        if(left!=nullptr)sz+=left->sz;
        if(right!=nullptr)sz+=right->sz;
    }
    inline Ptr parent(){
        return this->pa.lock();
    }
    inline Ptr parent(const shared_ptr<Node>& x){
        this->pa=x;
        return this->pa.lock();
    }
    inline bool isblack(){
        return this->color==0;
    }
    inline bool isred(){
        return this->color==1;
    }
    inline bool has_sibling(){
        return this->parent()!=nullptr&&this->parent()->left!=nullptr&&this->parent()->right!=nullptr;
    }
    inline Ptr sibling(){
        if(this->parent()==nullptr)
            return nullptr;
        if(this==this->parent()->left.get())
            return this->parent()->right;
        else
            return this->parent()->left;
    }
    inline bool is_leaf(){
        return (this->left==nullptr)&&(this->right==nullptr);
    }
    inline bool is_root(){
        return this->parent()==nullptr;
    }
    inline Ptr uncle(){
        if(this->parent()==nullptr)
            return nullptr;
        return this->parent()->sibling();
    
    }
    inline int direction(){

        if(this==this->parent()->left.get())
            return 0;
        else
            return 1;
    
    }
};
class RBtree {
    using Ptr=typename Node::Ptr;
    Ptr root;
    //size_t sz;
    inline void left_rotate(const Ptr& x){
        Ptr y=x->right;
        Ptr z=y->left;
        x->right=z;
        if(z!=nullptr)
            z->parent(x);
        y->left=x;
        y->parent(x->parent());
        x->parent(y);
        if(y->parent()==nullptr)root=y;
        else{
            if(y->parent()->left==x)
                y->parent()->left=y;
            else
                y->parent()->right=y;
        }
        x->update_size();
        y->update_size();
    }
    inline void right_rotate(const Ptr& x){
        Ptr y=x->left;
        Ptr z=y->right;
        y->right=x;
        x->left=z;
        if(z!=nullptr)
            z->parent(x);
        y->parent(x->parent());
        x->parent(y);
        if(y->parent()==nullptr)root=y;
        else {
            if(y->parent()->left==x)
                y->parent()->left=y;
            else
                y->parent()->right=y;
        }
        x->update_size();
        y->update_size();
    }
    void maintain_insert(const Ptr  cur)
    {
        //case 1: cur is root
        if(cur->parent()==nullptr){
        //cout<<"case 1\n";
            cur->color=0;
            return;
        }
        if(cur->parent()->isblack())return;
        if(cur->uncle()!=nullptr&&cur->uncle()->isred()){
            //case 2: uncle is red
            //cout<<"case 2\n";
            cur->parent()->color=0;
            cur->uncle()->color=0;
            cur->parent()->parent()->color=1;
            maintain_insert(cur->parent()->parent());
            return;
        }else{
            //case 3: uncle is black
            //cout<<"case 3\n";
            int d2=cur->direction();
            int d1=cur->parent()->direction();
            Ptr t=cur->parent()->parent();
            if(d1!=d2){
                //case 3.1: cur and parent() are not in the same direction
                //cout<<"case 3.1\n";
                if(d2==0)
                    right_rotate(cur->parent());
                else left_rotate(cur->parent());
            }

            if(d1==0)
                right_rotate(t);
            else left_rotate(t);
            t->color=1;
            t->parent()->color=0;
        }
        
    }
    Ptr prev(const Ptr& cur)const{
        if(cur->left==nullptr)return nullptr;
        Ptr t=cur->left;
        while(t->right!=nullptr)t=t->right;
        return t;
    }
    Ptr next(const Ptr& cur) const{
        if(cur->right==nullptr)return nullptr;
        Ptr t=cur->right;
        while(t->left!=nullptr)t=t->left;
        return t;
    }
    void insert(Ptr cur, int key){
        Ptr prev=cur;
        while(cur!=nullptr){
            //cout<<cur->key<<" ";
            if(cur->key<key)
                prev=cur,cur=cur->right;
            else if(cur->key>key)
                prev=cur,cur=cur->left;
            else{
                cur->sz++;
                cur->count++;
                while(cur!=nullptr){
                    cur->update_size();
                    cur=cur->parent();
                }
                return;
            }
        }
        //cout<<'\n';
        cur=make_shared<Node>(key);
        cur->parent(prev); 
        assert(prev!=nullptr);
        if(prev->key<key)
            prev->right=cur;
        else
            prev->left=cur;
        assert(!cur->pa.expired());
        if(cur->parent()->isred())maintain_insert(cur);
        //update size
        while(cur!=nullptr){
            cur->update_size();
            cur=cur->parent();
        }
    }
    void printTreeHelper(const Ptr& root, int space=0,int dep=0) const{
        if (root != nullptr) {
            space += 10;
            printTreeHelper(root->right, space,dep+1);
            std::cout << std::endl;
            for (int i = 10; i < space; i++)
                std::cout << " ";
            //output red if node is red
            cout<<dep<<".";
            if(root->color==1)
                std::cout<<"\033[1;31m("<<root->key<<", "<<root->sz<<")\033[0m\n";
            else
                std::cout<<"("<<root->key<<", "<<root->sz<<")\n";
            printTreeHelper(root->left, space,dep+1);
        }
    }
    void maintain_remove(const Ptr cur){
        if(cur->is_root()){
            //node is root
            cur->color=0;
            return;
        }
        //cout<<cur->key<<cur->parent()->key<<endl;
        assert(cur->isblack());
        assert(cur->sibling());
        Ptr sib=cur->sibling();
        if(sib!=nullptr&&sib->isred()){
            //sibling is red,swap the colors and rotate
            assert(sib->parent()!=nullptr);
            assert(sib->parent()->isblack());
            cur->parent()->color=1;
            sib->color=0;
            if(cur->direction()==0)
                left_rotate(cur->parent());
            else
                right_rotate(cur->parent());
            sib=cur->sibling();
            // maintain_remove(cur);
            // return ;
        }
        assert(sib!=nullptr);
        if((sib->left==nullptr||sib->left->isblack())&&(sib->right==nullptr||sib->right->isblack())){
            if(cur->parent()->isblack()){
                //sibling is black and both children are black
                sib->color=1;
                //size_t sz=cur->parent()->sz;
                maintain_remove(cur->parent());
                //assert(sz==cur->parent()->sz+1);
            }else{
                //sibling is black and both children are black ,parent is red
                sib->color=1;
                cur->parent()->color=0;
            }
            return;
        }
       //size_t sz=cur->parent()->sz;
        if(sib->direction()==0){
            Ptr t=sib->parent();
            if(sib->right!=nullptr&&sib->right->isred()){
                sib->right->color=0;
                sib->color=1;
                left_rotate(sib);
            }
            right_rotate(t);
            if(!t->is_root())t->parent()->color=t->color;
            else root=t->parent(),root->color=0;
            t->color=t->sibling()->color=0;
            //assert(t->parent()->sz==sz);
        }else{
            Ptr t=sib->parent();
            if(sib->left!=nullptr&&sib->left->isred()){
                sib->left->color=0;
                sib->color=1;
                right_rotate(sib);
            }
            left_rotate(t);
            if(!t->is_root())t->parent()->color=t->color;
            else root=t->parent(),root->color=0;
            t->color=t->sibling()->color=0;
            //assert(t->parent()->sz==sz);
        }
    }
    bool remove(Ptr node,int k){
        if(node==nullptr)return 0;
        // if(node->key<k){
        //     if(node->right!=nullptr&&remove(node->right,k))return node->update_size(),1;
        //     else return 0;
        // }else if(node->key>k){
        //     if(node->left!=nullptr&&remove(node->left,k))return node->update_size(),1;
        //     else return 0;
            
        // }
        while(node!=nullptr&&node->key!=k){
            if(node->key<k)node=node->right;
            else node=node->left;
        }
        if(node==nullptr)return 0;
        assert(node!=nullptr);
        assert(k==node->key);
        if(node->count>1){
            node->count--;
            Ptr c=node;
            while(c!=nullptr){
                c->update_size();
                c=c->parent();
            }
            return 1;
        }
        assert(node->count==1);
        if(node->left!=nullptr&&node->right!=nullptr){
            //cout<<"case 1\n";
            Ptr p=next(node);
            assert(p->left==nullptr||p->right==nullptr);
            node->key=p->key;
            swap(node->count,p->count);
            p->update_size();
            node->update_size();
            if(node->parent())node->parent()->update_size();
            if(p->parent())p->parent()->update_size();
            remove(node->right,p->key);
            node->update_size();
            if(node->parent())node->parent()->update_size();
            return 1;
        }
        if(node->is_leaf()){
            //cout<<"case 2\n";
            if(node->is_root()){
                root=nullptr;
                return 1;
            }
            if(node->isblack())maintain_remove(node);
            // size_t sz=node->parent()->sz;
            // size_t l=(node->parent()->left!=nullptr?node->parent()->left->sz:0);
            // size_t r=(node->parent()->right!=nullptr?node->parent()->right->sz:0);
            if(node->direction()==0)node->parent()->left=nullptr;
            else node->parent()->right=nullptr;
            Ptr c=node;
            while(c!=nullptr){
                c->update_size();
                c=c->parent();
            }
            return 1;
        }else{
            //cout<<"case 3\n";
            Ptr replacement;
            assert(node->left||node->right);
            if(node->left!=nullptr)replacement=node->left;
            else replacement=node->right;
            if(node->parent()==nullptr){
                root=replacement;
                root->parent(nullptr);
                root->color=0;
                root->update_size();
            }else{
                //cout<<node->parent()->key<<endl;
                //cout<<node->color<<" "<<node->key<<endl;
                if(node->direction()==0)node->parent()->left=replacement;
                else node->parent()->right=replacement;
                //cout<<node->color<<" "<<node->key<<endl;
                replacement->parent(node->parent());
                assert(replacement->isred());
                assert(node->isblack());
                if(node->isblack()){
                    if(replacement->isred())replacement->color=0;
                    else maintain_remove(replacement);
                }
                Ptr c=replacement;
                while(c!=nullptr){
                    c->update_size();
                    c=c->parent();
                }
            }
            return 1;
        }
        return 0;
    }
    public:
    RBtree():root(nullptr){}
    size_t size() const {
        if(root==nullptr)return 0;
        return root->sz;
    }
    void insert(int key){
        if(root==nullptr){
            root=make_shared<Node>(key);
            root->color=0;
            return;
        }
        insert(root,key);
    }
    void printTree() const{
        printTreeHelper(root);
    }
    bool find(int x) const{
        Ptr cur=root;
        while(cur!=nullptr){
            if(cur->key==x){
                //std::cout<<"Found "<<x<<std::endl;
                return 1;
            }else if(cur->key<x){
                cur=cur->right;
            }else{
                cur=cur->left;
            }
        }
        //std::cout<<"Not found "<<x<<std::endl;
        return  0;
    }
    size_t count(int x){
        Ptr cur=root;
        while(cur!=nullptr){
            if(cur->key==x){
                //std::cout<<"Found "<<x<<std::endl;
                return cur->count;
            }else if(cur->key<x){
                cur=cur->right;
            }else{
                cur=cur->left;
            }
        }
        //std::cout<<"Not found "<<x<<std::endl;
        return  0;
    }
    std::vector<int> inorder(const Ptr& cur) const{
        std::vector<int> ans;
        if(cur==nullptr)
            return ans;
        std::vector<int> left=inorder(cur->left);
        std::vector<int> right=inorder(cur->right);
        ans.insert(ans.end(),left.begin(),left.end());
        ans.push_back(cur->key);
        ans.insert(ans.end(),right.begin(),right.end());
        return ans;
    }
    std::vector<int> inorder() const{
        return inorder(root);
    }
    bool remove(int k){
        if(root==nullptr)return 0;
        return remove(root,k);
    }
    int prev(int x){
        Ptr cur=root;
        Ptr ans=nullptr;
        while(cur!=nullptr){
            if(cur->key<x){
                ans=cur;
                cur=cur->right;
            }else{
                cur=cur->left;
            }
        }
        if(ans==nullptr)return -1;
        return ans->key;
    
    }
    int next(int x){
        Ptr cur=root;
        Ptr ans=nullptr;
        while(cur!=nullptr){
            if(cur->key>x){
                ans=cur;
                cur=cur->left;
            }else{
                cur=cur->right;
            }
        }
        if(ans==nullptr)return -1;
        return ans->key;
    }
    int rank(int x){
        Ptr cur=root;
        int ans=0;
        while(cur!=nullptr){
            if(cur->key<x){
                ans+=cur->count;
                if(cur->left!=nullptr)ans+=cur->left->sz;
                cur=cur->right;
            }else if(cur->key==x){
                if(cur->left!=nullptr)ans=ans+cur->left->sz+1;
                else ans++;
                return ans;
            }
            else{
                cur=cur->left;
            }
        }
        return ans;
    }
    int kth(int k){
        Ptr cur=root;
        while(cur!=nullptr){
            if(cur->left&&cur->left->sz>=k){
                cur=cur->left;
            }else if(cur->left&&cur->left->sz+cur->count>=k){
                return cur->key;
            }else if(cur->left==nullptr&&cur->count>=k){
                return cur->key;
            }
            else{
                k-=cur->count;
                if(cur->left)k-=cur->left->sz;
                //if(cur->right)k-=cur->right->sz;
                cur=cur->right;
            }
        }
        return -1;
    }
};
RBtree r;
int sz=0;
void solve(){
    int opt,x;
    cin>>opt>>x;
    switch(opt){
        case 1:
            r.insert(x);
            sz++;
            assert(sz==r.size());
            assert(r.find(x));
            break;
        case 2:
            {
                bool f=r.remove(x);
                if(f)sz--;
                if(sz!=r.size()){cout<<sz<<' '<<r.size()<<'\n';throw 1;}
            }
            break;
        case 3:
            cout<<r.rank(x)<<"\n";
            break;
        case 4:
            cout<<r.kth(x)<<'\n';
            break;
        case 5:
            cout<<r.prev(x)<<'\n';
            break;
        case 6:
            cout<<r.next(x)<<'\n';
            break;
    }
}
int main(){
    cin.tie(0)->sync_with_stdio(0);
    int t;
    cin>>t;
    int line=1;
    try
    {
        while(t--)solve(),++line;
    }catch(int e){
        cout<<"error at line "<<line+1<<endl;
    }

    
    // cout<<"=============="<<endl;
    // r.printTree();
    // r.insert(1);
    // r.insert(2);
    // r.insert(2);
    // r.insert(3);
    // r.insert(3);
    // r.insert(1);
    // r.remove(1);
    // r.printTree();
    // cout<<endl;
    // cout<<r.rank(2);
    return 0;
}