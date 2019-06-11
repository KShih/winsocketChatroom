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
    char image_buf[512*512];
    char result_buf[512*512];


    //Load Image
    cout <<"Load image.\n";
	Fimage=fopen("test2.jpg","rb");

        fread(&image_buf,1,512*512,Fimage);


    //Write Image
    cout <<"Write Image.\n";
	Fresult=fopen("out2.jpg","wb");


        fwrite(&image_buf,1,512*512,Fresult);


    fclose(Fresult);



    cout <<"Finish.\n";

	return 0;
}
