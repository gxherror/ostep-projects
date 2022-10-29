#include<stdio.h>
#include<string.h>
#include<iostream>
#define LISTSIZE 2
using namespace std;

struct Node 
{
    string key;
    string value;
    Node* ptr;
};

struct HeadNode
{
    Node* ptr;
};


class HashTable
{
private:
    HeadNode* bptr;

    //get key and return value's address
    //add type hash
    int hash(string key){
        int hash, i;
        for (hash = key.length(), i = 0; i < key.length(); i++){
             hash += key[i];
        };
        return (hash % LISTSIZE);
    };

public:

    HashTable(int size){
        bptr = new HeadNode[size];
        for (int i=0;i<LISTSIZE;i++){
            bptr[i].ptr = NULL;
        }
    };

    ~HashTable(){

    };

    void add(string key,string value){
        int position = hash(key);
        cout<<"position is : "<<position<<'\n';
        HeadNode* headNode=&bptr[position];
        //first node is empty
        if (headNode->ptr==NULL){
            Node* newNode = new Node;
            newNode->key=key;
            newNode->value=value;
            newNode->ptr=NULL;
            headNode->ptr=newNode;
        }else{
            Node* currentNode=headNode->ptr;
            for (;currentNode->ptr!=NULL;currentNode=currentNode->ptr){};
            //first node have been occupied , find the end of linked list
            //and add a new node to the end of linked list,  
            Node* newNode = new Node;
            newNode->key=key;
            newNode->value=value;
            newNode->ptr=NULL;
            currentNode->ptr=newNode;
        }
    };

    string get(string key){
        int position = hash(key);
        Node* currentNode=bptr[position].ptr;
        for (;currentNode!=NULL;currentNode=currentNode->ptr){
            if (currentNode->key==key){
                return currentNode->value;
            };
        };
        return "";
    };

    void del(string key){
        int position = hash(key);
        Node* currentNode=bptr[position].ptr;
        if (currentNode->key==key){
            bptr[position].ptr=currentNode->ptr;
            free(currentNode);
        }else{
            for (;currentNode->ptr!=NULL;currentNode=currentNode->ptr){
                //if next node's key match the target ,delete next node 
                //and change currentNode ptr
                if (currentNode->ptr->key==key){
                    currentNode->ptr=currentNode->ptr->ptr;
                    free(currentNode->ptr);
                };
            };
        }
        

    };


    void show(){
        for (int i=0;i<LISTSIZE;++i){
            cout<<bptr[i].ptr;
            Node* currentNode=bptr[i].ptr;
            for (;currentNode!=NULL;currentNode=currentNode->ptr){
                cout<<" -> "<<" key : "<<currentNode->key<<" , value: "<<currentNode->value;
            };
            cout<<'\n';
        }
    };
};


int main(int argc,char *argv[]){

    HashTable hashTable(LISTSIZE);

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
            hashTable.add(args[1],args[2]);
        }
        if (args[0]=="g"){
            printf("begin process get...\n");
            cout<<"value is : "<<hashTable.get(args[1])<<'\n';
        }
        if (args[0]=="d"){
            printf("begin process del...\n");
            hashTable.del(args[1]);
        }
    }
    hashTable.show();
    return 0;
}