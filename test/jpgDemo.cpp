#include <iostream>
#include <fstream>
#include <stdio.h>

#define size 512

using namespace std;

//=============input images=============//
char image_buf_R[size][size]; int image_R[size][size];
char image_buf_G[size][size]; int image_G[size][size];
char image_buf_B[size][size]; int image_B[size][size];

//=============output random result======//
char result_buf_R[size][size]; int result_R[size][size];
char result_buf_G[size][size]; int result_G[size][size];
char result_buf_B[size][size]; int result_B[size][size];

//=============Input====================//
FILE *Fimage;
//=============Output====================//
FILE *Fresult;


int main(void)
{
	int n,i,j;



    //Load Image
    cout <<"Load image.\n";
	Fimage=fopen("test2.jpg","rb");
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			fread(&image_buf_R[i][j],1,1,Fimage);
			fread(&image_buf_G[i][j],1,1,Fimage);
			fread(&image_buf_B[i][j],1,1,Fimage);
			image_R[i][j] = (int)image_buf_R[i][j];
			image_G[i][j] = (int)image_buf_G[i][j];
			image_B[i][j] = (int)image_buf_B[i][j];
		}
	}
	fclose(Fimage);


    //Do some operations
    cout <<"Do some operations.\n";

	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			result_R[i][j] = image_R[i][j];
			result_G[i][j] = image_G[i][j];
			result_B[i][j] = image_B[i][j];
		}
	}




    //Write Image
    cout <<"Write Image.\n";
	Fresult=fopen("out.jpg","wb");
	for (i=0;i<size;i++)
    {
		for(j=0;j<size;j++)
		{
			result_buf_R[i][j]=(char)result_R[i][j];
            result_buf_G[i][j]=(char)result_G[i][j];
            result_buf_B[i][j]=(char)result_B[i][j];
		}
    }
	for (i=0;i<size;i++)
    {
		for(j=0;j<size;j++)
		{
            fwrite(&result_buf_R[i][j],1,1,Fresult);
            fwrite(&result_buf_G[i][j],1,1,Fresult);
            fwrite(&result_buf_B[i][j],1,1,Fresult);
		}
    }
    fclose(Fresult);



    cout <<"Finish.\n";

	return 0;
}
