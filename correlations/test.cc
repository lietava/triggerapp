#include <iostream>
using namespace std;
int main()
{
 int ninputs=4;
 int ncorrelations=ninputs*(ninputs+1)/2;
 int l = 1;
 for(int i=1;i<ninputs;i++){
        int m = l/ninputs;
        int n = (l % ninputs);
        l += 1;
        l += ((l%ninputs)==0) * (l/ninputs);
        cout << "i,n " << i << " " << m << " "<<  n  << endl;
 }
 int dim=ninputs-1;
 int ll=0;;
 for(int i=ninputs;i<ncorrelations;i++){
        int m = l/ninputs;
        int n = l % ninputs;
	int j = ll/dim;
        int k = ll % dim;
        //cout << "m,n,k,i " << m << " " << n << " " << l  << " " << i << endl;
        l += 1;
        l += ((l%ninputs)==0) * (l/ninputs);
        ll += 1;
        ll += ((ll%dim)==0) * (ll/dim);
        //cout << "m,n,k " << inputNames[m] << " " << inputNames[n] << " " << i  << endl;
        //int del=2*delta+delay[m]-delay[n];
	cout << "m= " << m << " n=" << n  << " j= " << j << " k= " << k << endl;
 }

 return 0;
}

