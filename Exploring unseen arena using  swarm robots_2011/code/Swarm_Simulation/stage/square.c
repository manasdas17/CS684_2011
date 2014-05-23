#include<stdio.h>
#include<math.h>
#include<stdlib.h>
int main(int argc, char * argv[] )
{
FILE * fp, *fs;
char x[10], y[10];
char line_input[80];
int i=0,k=0,l,j;
float tmp1=0,tmp2=0;
int flag=0, ls=0;
fp = fopen("sample.txt", "r");
fs = fopen("ls2438.txt", "w");
while(!feof(fp)) {
	i=0;k=0;
       fgets(line_input,80,fp);
         if (line_input[k]== '-'){
		flag=1; k++;
	 }
	while ( i<4 ){
		x[i]=line_input[k++];
		i++;
	}
	for (j =0,l=0; j<4;j++){ 
          if (j==1)
		continue ;
	   tmp1+= (x[j]-48)*pow(10,l);
	   l--;
	}
	if (flag==1){
	   tmp1*=-1;
	   flag=0;
	}
	i=0;
	k++;
         if (line_input[k]== '-'){
		flag=1; k++;
	 }
	while ( i<4){
		y[i]=line_input[k++];
		i++;
	}
        
	for (j =0,l=0; j< 4;j++){ 
          if ( j==1)
		continue ;
	   tmp2+= (y[j]-48)*pow(10,l);
		l--;
     	}
	if (flag==1){
	   tmp2*=-1;
	   flag=0;
	}
	for ( int ls=0;ls<6;ls++){
 		for (int sl=0;sl<6;sl++){
			fprintf(fs,"\n%f %f",tmp1+((ls-3)/6.0), tmp2+((sl-3)/6.0));
 		}
	}
	tmp1=0;
	tmp2=0;

    }
fclose(fp);
fclose(fs);
return 0;
}
