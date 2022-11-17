#include <bits/stdc++.h> 
using namespace std;
#define M 900.0
#define N 2386.1

#define h 500.0
#define b 500.0
#define l0 3500.0
#define as 45.0
#define asp 45.0

/*混凝土受压区等效矩形应力图系数，当<=C50 alpha1= 1.0 , beta1 = 0.8 */
#define alpha1 1.0
#define beta1 0.8
/*对<=C50 ,HPB235 = 0.614 ,HPB335 = 0.550 ,HPB400 = 0.518*/
#define xib 0.550

#define fc 16.7
#define fyp 360.0
#define fy 360.0


#define rhominp 0.002 
#define ea 20

double h0=0.0,e=0.0;

void little_eccentric();

int main(){
    h0= h -as;
    //double e0 = M*(1E+3)/N;
    double e0 = 200.0;
    double ei = e0 +ea;
    //double zeta1 = 0.5*fc*b*h/(N*(1E+3));
    double zeta1 = 1.0;
    //if (zeta1>= 1) zeta1 =1.0;
    double zeta2=0;
    if (l0/h<15) zeta2=1;
    double eta = (1+(1/(1400*ei/h0))*(l0/h)*(l0/h)*zeta1*zeta2);
    if (eta*ei>0.3*h0){
        e = eta*ei+h/2-as;
        double x = N*(1E+3) / (alpha1*fc*b);
        if (x > xib*h0) {
            little_eccentric();
        }
        //alpha1=1.0,fc=14.3,b=400.0,fyp=310.0,Asp=240,h0=560.0,asp=40,fy=310.0,xib=0.550,beta1=0.8

        //double Nu=alpha1*fc*b*x+fyp*Asp-sigmas*As;
        //Nu*ep = alpha1*fc*b*x*(x/2-asp)-sigmas*As*(h0-asp);
        //sigmas = ((xi-beta1)/(xib-beta1))*fy;
        //xi = x / h0;
        double Asp = (N*(1E+3)*e - alpha1*fc*b*x*(h0-0.5*x))/(fyp*(h0-asp)); 
        //double Asp = (N*(1E+3)*e - alpha1*fc*b*h0*h0*xib*(1-0.5*xib))/(fyp*(h0-asp)); 
        if (Asp<rhominp*b*h) {
            Asp = rhominp*b*h;
        }
        double As = (alpha1*fc*b*h0*xib-N)/fy+Asp*(fyp)/(fy);
        //double x = (N-fyp*Asp+fy*As)/(alpha1*fc*b);
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

void little_eccentric(){
    double xi = (N-xib*alpha1*fc*b*h0)/(((N*e-0.43*alpha1*fc*b*h0*h0)/((beta1 - xib )*(h0 - asp)))+alpha1*fc*b*h0) +xib;
    double x = xi * h0;
    double Asp = (N*(1E+3)*e - alpha1*fc*b*x*(h0-0.5*x))/(fyp*(h0-asp)); 
}