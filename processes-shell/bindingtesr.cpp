#include<iostream>
using namespace std;
class A
{
public:
    void func(){ std::cout << "A::func()\n"; }
};

class B : public A
{
public:
    void func(){ std::cout << "B::func()\n"; }
};

class C : public A
{
public:
    void func(){ std::cout << "C::func()\n"; }
};

class E
{
public:
    virtual void func()
    {
        std::cout << "E::func()\t"<<"\n";
    }
};

class F : public E
{
public:
    void func()
    {
        std::cout << "F::func()\t" <<"\n";
    }
};

int main(int argc ,char* argv[],char* env[]){
    C* pc = new C(); //pc的静态类型是它声明的类型C*，动态类型也是C*；
    B* pb = new B(); //pb的静态类型和动态类型也都是B*；
    A* pa = pc;      //pa的静态类型是它声明的类型A*，动态类型是pa所指向的对象pc的类型C*；
    pa = pb;         //pa的动态类型可以更改，现在它的动态类型是B*，但其静态类型仍是声明时候的A*；
    C *pnull = NULL; //pnull的静态类型是它声明的类型C*,没有动态类型，因为它指向了NULL；
    
    F* pf = new F();
    E* pe  =pf;
    E *penull = NULL; 

    cout<<typeid(pa).name()<<'\n';
    pa->func();
    pb->func();
    pc->func();
    pnull->func();

    cout<<typeid(pe).name()<<'\n';
    pe->func();
    pf->func();
    //segmentation fault
    penull->func();//空指针异常，因为是func是virtual函数，因此对func的调用只能等到运行期才能确定，然后才发现pnull是空指针；
}