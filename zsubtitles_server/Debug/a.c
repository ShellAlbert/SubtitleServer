    #include <stdio.h>  
    #include <stdlib.h>  
    void main()  
    {  
         int nrows=10;
	 int ncolumns=100;
         int **Array;  
         int i,j;  
         Array=(int **)malloc(nrows*sizeof(int *));  
         for(i=0;i<nrows;i++)  
             Array[i]=(int *)malloc(ncolumns*sizeof(int));  
         for(i=0;i<nrows;i++)  
         {  
             for(j=0;j<ncolumns;j++)  
             {  
                 Array[i][j]=1;  
                 printf("%d ",Array[i][j]);   //动态数组的成员都可以用正常的数组下标 Array[i][j]  
             }  
             printf("/n");  
         }  
         free(Array);  
    }  
