/* client.cpp */
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <string.h>

#define MAX 1024
#define size 250
#define MULTI_IP_A "230.0.0.5"
#define SERVER_IP "192.168.0.7"
char EXIT_CODE[34]= "qiowsdfklmvasdf%*asdfasdf82&s4$sa";
char JPEG_CODE[34]= "qiowsdwerrvasdf%*asd&a-df82&s4$!a";

/*
   IP_A : �t�d�T���̫᪺�ǰe
   IP_B : �t�d����O�� Client �� key down
   IP_C : �t�d�T�����Ƕ�
*/
#define MULTI_PORT 9999
using namespace std;

void help();
void delay(int sec);
void jpgGenerating();

int main(int argc, char** argv)
{
    char  str[MAX],rstr[MAX],S_str[MAX],get_str[MAX],str2[MAX]=" �w�[�J��ѫ�",str3[MAX]=" �w���}��ѫ�";
   	int n;
    help();
    WSADATA 		wsadata;
   	if(WSAStartup(0x101,(LPWSADATA) &wsadata)!=0){
		fprintf(stderr,"echo_srv: WSAStartup() fails\n");
		exit(1);
	}
// --------------------------------------------------------//
	/**  Create socket to mono-send message to server   **/

	SOCKET serv_sd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv;
	int serv_len = sizeof(serv);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv.sin_port = htons(7788);

	connect(serv_sd, (SOCKADDR*)&serv, sizeof(serv));
//--------------------------------------------------------//

// --------------------------------------------------------//
	/**  Create socket receive from multi group  **/

    SOCKET        	sd;     // �t�d�����T���Ӧ� IP_A
   	struct sockaddr_in multi_serv;

   	sd=socket(AF_INET, SOCK_DGRAM, 0);

   	int multi_serv_len = sizeof(multi_serv);
    multi_serv.sin_family       = AF_INET;
   	multi_serv.sin_addr.s_addr  = 0;
   	multi_serv.sin_port         = htons(MULTI_PORT);
   	if(bind(sd,(LPSOCKADDR)&multi_serv,sizeof(multi_serv))<0){
		printf("bind error!\n");
	}

    struct ip_mreq multicastRequest;    // join multi group
    struct ip_mreq multicastDelete;     // leave multi group

    multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTI_IP_A);    // specify the multi group
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);  // ��ܭn�ϥΪ����d
// -------------------------------------------------------/
     /**  Start using non-blocking   **/
    //enable non-blcking mode when iMode=1
    u_long iMode=1;
    ioctlsocket(sd,FIONBIO, &iMode);        /**  let socket-sd become non-blocking  **/

    u_long iMode2=1;
    ioctlsocket(serv_sd,FIONBIO, &iMode2);


    bool connect=false;     // �P�_�O�_�w�g�s�u
    char name[MAX];         // �x�sClient ���ʺ�
    int Nlength = 0;        // Length of Client's name
	while(1)
	{
        if( connect ){
            n=recvfrom(sd, rstr, MAX, 0,(LPSOCKADDR) &multi_serv,&multi_serv_len ); // receive message from multi-IP
            rstr[n]='\0';

            if( n>= 0){
                if( strcmp(rstr,JPEG_CODE) == 0){
                    cout << ">> Server �Y�N�ǰe�Ϥ�" << endl;
                    Sleep(5000);
                    cout << ">> �Ϥ����ͤ�...." << endl;
                    char image_buf[size*size];
                    while(1){
                        int k = recvfrom(sd, image_buf, size*size, 0,(LPSOCKADDR) &multi_serv,&multi_serv_len );
                        if( k > 0){
                            cout << k<< endl;
                            FILE *Fresult;
                            Fresult=fopen("output.jpg","wb");
                            if(!Fresult)    cout << "file wrong!" <<endl;
                            else{
                                fwrite(&image_buf,1,size*size,Fresult);
                                fclose(Fresult);
                                break;
                            }
                        }
                    }
                }
                else
                    cout << rstr << endl;
            }

        }
        if(_kbhit()){
            char selection;
            selection = _getch();
            switch( selection ){
                case'a':     // �}�l�s�u
                    if( setsockopt(sd,IPPROTO_IP, IP_ADD_MEMBERSHIP,
                                   (char *) &multicastRequest, sizeof(multicastRequest)) < 0){
                        cout << "���Ƴs�u! " <<endl;
                        break;}
                    cout << ">> �п�J�A���ʺ�: ";
                    cin >> name;    cin.get();
                    send(serv_sd, name,strlen(name)+1,0);   //�ǰe�m�W��T��server

                    Nlength = strlen(name);
                    name[Nlength] = ':';   name[Nlength+1] = ' '; name[Nlength+2] = '\0';
                    connect = true;
                    cout << ">> �w�s�u�� " << MULTI_IP_A << " Port: " << MULTI_PORT << endl;

                    strcpy(S_str, name);
                    strcat(S_str, str2);
                    send(serv_sd, S_str,strlen(S_str)+1,0);
                    break;

                case'm':    // ��client��J�T��
                    if( connect ){
                        cout << ">> �п�J�T��: ";
                        cin.getline(get_str,MAX,'\n');
                        strcpy(S_str, name);
                        strcat(S_str, get_str);
                        send(serv_sd, S_str,strlen(S_str)+1,0);     // mono-send to the server
                    }
                    else    cout << ">> �s�u�򥢡A�Ы�'a'�ӭ��s�إ߳s�u " << endl;
                    break;

                case'd':    // �R���s�u
                    multicastDelete.imr_multiaddr.s_addr = inet_addr(MULTI_IP_A);
                    multicastDelete.imr_interface.s_addr = htonl(INADDR_ANY);   // INADDR_ANY ���w�]���d(�j�����q���u���@�i���d)
                    if( setsockopt(sd,IPPROTO_IP, IP_DROP_MEMBERSHIP,
                                   (char *) &multicastDelete, sizeof(multicastDelete)) < 0){
                        cout << ">> �w�g�S���s�u�i�H�R���F�C..." <<endl;
                        break;}
                    cout << ">> ���b�R���s�u..."; delay(3); cout << "OK!"<<endl;
                    connect = false;

                    strcpy(S_str, name);
                    strcat(S_str, str3);
                    send(serv_sd, S_str,strlen(S_str)+1,0);        // ���u�T��
                    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   ���u�N�X
                    name[0] = '\0';
                    system("pause");
                    exit(1);
                    break;


                case'h':     // �L�X�p����
                    help();
                    break;

                case'C':     //�M���ù�
                    system("cls");
                    help();
                    break;

                case'Q':    //�����{��
                    if(name[0] == '\0') name[0] = '?';
                    strcpy(S_str, name);
                    strcat(S_str, str3);
                    send(serv_sd, S_str,strlen(S_str)+1,0);     // ���u�T��
                    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   ���u�N�X
                    delay(2);

                    closesocket(sd);
                    WSACleanup();
                    exit(1);
                    break;
            }
        }
	}
    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   ���u�N�X
  	closesocket(sd);
    closesocket(serv_sd);
   	WSACleanup();
   	return 0;
}
void help()
{
    cout << "-------------------------" << endl;
    printf("���U\"a\"�إ߳s�u\n");
    printf("���U\"h\"�}�����U\n");
    printf("���U\"m\"��J�T��\n");
    printf("���U\"d\"�����s�u\n");
    printf("���U\"C\"�M���ù�\n");
    printf("���U\"Q\"�����{��\n");
    cout << "-------------------------" << endl;
}
void delay(int sec)
{
	time_t _t;
	_t = time(NULL);
	while( time(NULL) <= _t + sec-1 ){};
}
void jpgGenerating()
{

}



