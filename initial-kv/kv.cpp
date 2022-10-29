#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<assert.h>
#define LISTSIZE 4
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

    void openFile(){
        ifstream infile; 
        infile.open("database.txt",ios::in);  
        assert(infile.is_open());   
        string s;
        char* ptr;
        int i=0;
        while(getline(infile,s))
        {
            int j=0;
            char * strs = new char[s.length() + 1] ;
	        strcpy(strs, s.c_str()); 
            char* lineTemp=strdup(strs);
            for (char* kv=strsep(&lineTemp,">");kv!=NULL;kv=strsep(&lineTemp,">"),++j){
                if (j==0){
                    continue;
                }else{
                    int k=0;
                    string args[2];
                    char* kvTemp=kv;
                    for (char* arg=strsep(&kvTemp,",");arg!=NULL;arg=strsep(&kvTemp,","),++k){
                        args[k]=arg;
                    }
                    //TODO:instead of add(),create hashTable directly
                    add(args[0],args[1]);
                }
            };
        }
        infile.close();             
    }

    void saveFile(){
        ofstream outfile; 
        outfile.open("database.txt");  
        assert(outfile.is_open());   
        for (int i=0;i<LISTSIZE;++i){
            outfile<<bptr[i].ptr;
            Node* currentNode=bptr[i].ptr;
            for (;currentNode!=NULL;currentNode=currentNode->ptr){
                outfile<<">"<<currentNode->key<<','<<currentNode->value;
            };
            outfile<<'\n';
        };
        outfile.close();             
    }

public:

    HashTable(int size){
        bptr = new HeadNode[size];
        for (int i=0;i<LISTSIZE;i++){
            bptr[i].ptr = NULL;
        };
        openFile();
    };

    ~HashTable(){
        saveFile();
    };

    void add(string key,string value){
        if (get(key)==value) return;
        int position = hash(key);
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
        };
    };

    string get(string key){
        int position = hash(key);
        if (bptr[position].ptr!=NULL){
            Node* currentNode=bptr[position].ptr;
            for (;currentNode!=NULL;currentNode=currentNode->ptr){
                if (currentNode->key==key){
                    return currentNode->value;
                };
            };
        }
        return key+" not found";
    };

    void del(string key){
        int position = hash(key);
        if (bptr[position].ptr!=NULL){
            Node* currentNode=bptr[position].ptr;
            if (currentNode->key==key){
            bptr[position].ptr=currentNode->ptr;
            free(currentNode);
            return;
            }else{
                for (;currentNode->ptr!=NULL;currentNode=currentNode->ptr){
                    //if next node's key match the target ,delete next node 
                    //and change currentNode ptr
                    if (currentNode->ptr->key==key){
                        currentNode->ptr=currentNode->ptr->ptr;
                        free(currentNode->ptr);
                        return;
                    };
                };
            }
        }
        cout<<key<<" not found"<<'\n';
    };

    void clear(){
        for (int i=0;i<LISTSIZE;++i){
            if (bptr[i].ptr!=NULL){
                Node* currentNode=bptr[i].ptr;
                for (;currentNode!=NULL;currentNode=currentNode->ptr){
                    del(currentNode->key);
                };
            }
        }
    }

    void all(){
        for (int i=0;i<LISTSIZE;++i){
            if (bptr[i].ptr!=NULL){
                Node* currentNode=bptr[i].ptr;
                for (;currentNode!=NULL;currentNode=currentNode->ptr){
                    cout<<currentNode->key<<","<<currentNode->value<<'\n';
                };
            }
        }
    }

    void show(){
        for (int i=0;i<LISTSIZE;++i){
            cout<<bptr[i].ptr;
            Node* currentNode=bptr[i].ptr;
            for (;currentNode!=NULL;currentNode=currentNode->ptr){
                cout<<" -> "<<" key : "<<currentNode->key<<" , value: "<<currentNode->value;
            };
            cout<<'\n';
        }
    }
};


int main(int argc,char *argv[]){

    HashTable hashTable(LISTSIZE);
    if (argc==1){
        return 0;
    };
    
    for (int i=1;i<argc;i++){
        string args[3];
        //strcpy strdup
        char* temp=strdup(argv[i]);
        int j=0;
        for (char* arg=strsep(&temp,",");arg!=NULL;arg=strsep(&temp,","),++j){
            args[j]=arg;
        }
        if (args[0]=="p"){
            //printf("begin process put...\n");
            hashTable.add(args[1],args[2]);
        }
        if (args[0]=="g"){
            //printf("begin process get...\n");
            string result=hashTable.get(args[1]);
            if (result==(args[1]+" not found")){
                cout<<args[1]<<" not found"<<'\n';
            }else{
                cout<<args[1]<<","<<hashTable.get(args[1])<<'\n';
            }
        }
        if (args[0]=="d"){
            //printf("begin process del...\n");
            hashTable.del(args[1]);
        }
        if (args[0]=="a"){
            //printf("begin process all...\n");
            hashTable.all();
        }
        if (args[0]=="c"){
            //printf("begin process clear...\n");
            hashTable.clear();
        }
    };
    return 0;
}