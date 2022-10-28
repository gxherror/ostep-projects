#include<stdio.h>
#include<string.h>
#include<iostream>
#define LISTSIZE 100
using namespace std;

struct node 
{
    string value;
    node* ptr;
};

class HashTable
{
private:
    node* bptr;

    //get key and return value's address
    int hash(string key){
        int hash, i;
        for (hash = key.length(), i = 0; i < key.length(); i++){
             hash += key[i];
        };
        return (hash % LISTSIZE);
    }
public:

    HashTable(int size){
        bptr = new node[size];
        for (int i=0;i<LISTSIZE;i++){
        bptr[i].value = "";
        bptr[i].ptr = NULL;
        }
    };

    ~HashTable(){

    };

    void add(string key,string value){
        int position = hash(key);
        cout<<"position is : "<<position<<'\n';
        if (bptr[position].value=="" && bptr[position].ptr==NULL){
            bptr[position].value=value;
        }else if (bptr[position].ptr== NULL){
            node* next = new node;
            next->value="";
        }else{

        }
        
        
    };

    string get(string key){

    };

    void del(string key){

    };

};


int main(int argc,char *argv[]){

    HashTable hashtable(LISTSIZE);

    if (argc==1){
        cout<<"not enough args";
    }
    
    for (int i=1;i<argc;i++){
        string args[3];
        //strcpy strdup
        char* temp=strdup(argv[i]);
        int j=0;
        for (char* arg=strsep(&temp,",");arg!=NULL;arg=strsep(&temp,","),++j){
            args[j]=arg;
        }
        if (args[0]=="p"){
            printf("begin process put...\n");
            hashtable.add(args[1],args[2]);
        }
        if (args[0]=="g"){

        }
        if (args[0]=="d"){

        }
    }
    return 0;
}