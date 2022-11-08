#include<dlfcn.h>

int sum(int *a,int n){
    int i,s=0;
    for (i=0;i<n;i++){
        s+=a[i];
    }
    return s;
}
int array[2]={1,2};
int main(){
    int val = sum(array,2);
    return val;
}