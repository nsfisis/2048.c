#include<stdio.h>
#include<stdlib.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>
#define f printf
#define n f("\r\n")
#define r(_,n)for(int _=0;_<n;++_)
#define t(_)(G+y*((x+S)%(S+1)+_*x)+(S+1-y)*i)

int S ,N,E,C ,*L ,*T  ,F[128] ,*G =F;struct  termios O   ;void x()  {tcsetattr(0,TCSAFLU\
SH,&O);f("\e[?25h\e"  "[?1049l\e[2J\e[H");}  void _(){   f("+");r(  i,8*S-1)f("-");f("+")
;n;}void l(){f("|");  r(i,S){r(j,7)f(" ");f  ("|" ); }   n;}int m(  int x,int y,int d){C=
0;r(i,S){E=0;L=0;r(j  ,S)if(*(T=t(j)))if(L   &&*T==*L)   {if(d){*L  *=2;T=0;}L=0;C=1;}else
          {L=t(E);if  (d)*L=*T;   ++E;}for(  ;E<S;++E)   {T=t(E);C  |=*T;if(    d)*T=0;}}
          return! C;  }void p()   {E=0;r(i,  S *S)if(!   G[i])++E;  if(E==0)    return;N=
          rand()%E;r  (i,S*S)if   (!G[i]&&   --E==N)G[   i]=rand()  %8?2:4;}    int main(
          int A,char  **a){srand  (time(0))  ;S=A> 1?(   *a[1]-48)  :4;if(S<    2 ||S>8){
fputs("invalid board size" "\n"   ,stderr);  return 1;} atexit(x);  f ("\033[?1049h");tc\
getattr(0,&O);struct  termios o   =O;o.      c_iflag &=~ ( BRKINT|  ICRNL| INPCK |ISTRIP|
IXON);o.c_oflag&=~    OPOST;o.    c_cflag|=  CS8;o.c_lflag&=~(ECHO  |ICANON|IEXTEN|ISIG);
o.c_cc[VMIN]=0;o.c_cc [VTIME]=1   ;tcsetattr (0,2,&o);f("\e[?25l")  ;p();p();while(32){f(
"\e[2J\e[H");_();r(i  ,S){l();f   ("| ");r(  j,S){if(!*G)f("     "  " | "); else if (*G>=
1024)f(" "            "%2dk  |"   " ", * G/              1024);else f("%4d  "   "| ",*G);
++G;}n;l()            ;if(i<S-1   ){f("|");              r(j,S){r(  j,7)f("-"   );if(j<S-
1) f("+");            }f( "|");   n;}}_();G              -= S*S;f(  "  h"",j"   ",k,l: "
"move") ;r            (i,9 )f(    " " ); f(              "q: quit"  );fflush(   stdout );
char k,e=0;while(     read(0,&k, 1)!=1);if(              k=='q')    goto q;if(k=='h')m(1,
1,e=1);if(k=='j')m(-  1,S,e=1);if(k=='k')m(              1,S,e=1);  if(k  =='l')m(-1 ,1,e
=1);if(e ){if(C)p();  if(m(1,1,0)&m (-1,1,0              )&m( 1,S,  0)&m(-1,S ,0))goto q;
usleep(2048*8);}}q:;  } /*nsfisis*/
