#include <bits/stdc++.h> 
#define alpha1 1

#define fc 14.3
#define fy 300
#define fyp 300

#define bf 450
#define hf 180
#define h 550
#define b 250
#define as 40

#define M 500
#define xib 0.518 
using namespace std;
int main(){
    double h0=h-as;
    double Mt=alpha1*fc*bf*hf*(h0-hf/2)*(1E-6);
    if (Mt>M){
        double alphas=M/(alpha1*fc*bf*h0*h0);
        double xi=1-sqrt(1-2*alphas);
        if (xi<xib){
            double gammas=0.5*(1+sqrt(1-2*alphas));
            double As=M*(1E6)/(fy*gammas*h0);
            cout<<"type 1: "<<As<<'\n';
        }else{
            printf("超筋");
        }
    }else{
        double Mu1=alpha1*fc*(bf-b)*hf*(h0-hf/2)*(1E-6);
        double Mu2=M-Mu1;
        double alphas=Mu2*(1E6)/(alpha1*fc*b*h0*h0);
        double xi=1-sqrt(1-2*alphas);
        if (xi<xib){
            double gammas=0.5*(1+sqrt(1-2*alphas));
            double As2=Mu2*(1E6)/(fy*gammas*h0);
            double As1=(alpha1*fc*(bf-b)*hf)/fy;
            double As=As1+As2;
            cout<<"type 2: "<<As<<'\n';
        }else{
            printf("超筋");
        }
    }


}
