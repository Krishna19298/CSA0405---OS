#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define M 3
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static int safe(int n,int (*mx)[M],int (*al)[M],int *av)
{ int work[M],*fin=calloc(n,sizeof(int)),cnt=0; memcpy(work,av,sizeof work);
  while(cnt<n){int found=0;
   for(int p=0;p<n;p++){ if(fin[p])continue; int ok=1;
     for(int r=0;r<M;r++) if(mx[p][r]-al[p][r]>work[r]){ok=0;break;}
     if(ok){for(int r=0;r<M;r++)work[r]+=al[p][r];fin[p]=1;cnt++;found=1;} }
   if(!found){free(fin);return 0;} }
  free(fin);return 1;}
int main(void){ int ns[]={10,100,1000,2000,4000};
 printf("%6s %12s %12s\n","n","time (ms)","passes");
 for(int k=0;k<5;k++){int n=ns[k]; int (*mx)[M]=malloc(n*sizeof *mx),(*al)[M]=malloc(n*sizeof *al); int av[M]={0,0,0};
  /* worst case: process p can only finish after process p+1 has finished -> n passes */
  for(int p=0;p<n;p++){ al[p][0]=1; al[p][1]=1; al[p][2]=1; mx[p][0]=1+(n-1-p); mx[p][1]=1; mx[p][2]=1; }
  int reps = n<=100 ? 2000 : (n==1000 ? 20 : 3);
  double t=now(); int ok=0; for(int i=0;i<reps;i++) ok=safe(n,mx,al,av); double d=(now()-t)/reps*1000;
  printf("%6d %12.4f %12s\n",n,d,ok?"safe":"UNSAFE"); free(mx);free(al);}
 return 0;}
