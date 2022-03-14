#include<bits/stdc++.h>
#include"Btree.hpp"
using namespace std;
int main(){
	Btree<int,2> T;
	T.insert(1);
	T.insert(2);
	T.insert(3);
	T.insert(4);
	T.insert(6);
	T.insert(0);
	T.remove(6);
	T.remove(0);
	T.remove(7);
	auto res= T.search(6);
	if(res==NULL)cout<<"not found\n";
    else	for(auto k:res->key){
		cout<<k<<endl;
	}
	T.traverse();
	return 0;
}
