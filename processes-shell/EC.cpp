#include <bits/stdc++.h> 
using namespace std;
#define M 168.0
#define h 600.0
#define as 40.0
#define asp 40.0
#define N 3000.0
#define alpha1 1.0
#define fc 14.3
#define b 300.0
#define l0 4800.0
#define xib 0.550
#define fyp 310.0
#define fy 310.0
#define rhominp 0.002 
#define ea 20
#define beta1 0.8
int main(){
    double h0 = h -as;
    double e0 = M*(1E+3)/N;
    double ei = e0 +ea;
    double zeta1 = 0.5*fc*b*h/(N*(1E+3));
    double zeta2=0;
    if (l0/h<15) zeta2=1;
    double eta = (1+(1/(1400*ei/h0))*(l0/h)*(l0/h)*zeta1*zeta2);
    if (eta*ei>0.3*h0){
        //alpha1=1.0,fc=14.3,b=400.0,fyp=310.0,Asp=240,h0=560.0,asp=40,fy=310.0,xib=0.550,beta1=0.8
        double e = eta*ei+h/2-as;

        //double Nu=alpha1*fc*b*x+fyp*Asp-sigmas*As;
        //Nu*ep = alpha1*fc*b*x*(x/2-asp)-sigmas*As*(h0-asp);
        //sigmas = ((xi-beta1)/(xib-beta1))*fy;
        //xi = x / h0;

        double Asp = (N*e - alpha1*fc*b*h0*h0*xib*(1-0.5*xib))/(fyp*(h0-asp)); 
        if (Asp<rhominp*b*h) {
            Asp = rhominp*b*h;
        }
        double As = (alpha1*fc*b*h0*xib-N)/fy+Asp*(fyp)/(fy);
        double x = (N-fyp*Asp+fy*As)/(alpha1*fc*b);
        double xi = x / h0;
        cout<<"Asp: "<<Asp<<'\n'<<"As: "<<As<<'\n'<<"x: "<<x<<'\n'<<"xi: "<<xi;
    }else{
        double e = eta*ei+h/2-as;
        double Asp = (N*e - alpha1*fc*b*h*(h0-0.5*h))/(fyp*(h0-asp)); 
        if (Asp<rhominp*b*h) {
            Asp = rhominp*b*h;
        }
        double As = (N*(1E+3)-alpha1*fc*b*h-fyp*Asp)/(fyp);

         cout<<"Asp: "<<Asp<<'\n'<<"As: "<<As<<'\n';
    }

    
}