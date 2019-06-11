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
   IP_A : 負責訊息最後的傳送
   IP_B : 負責持續保持 Client 的 key down
   IP_C : 負責訊息的傳傳
*/
#define MULTI_PORT 9999
using namespace std;

void help();
void delay(int sec);
void jpgGenerating();

int main(int argc, char** argv)
{
    char  str[MAX],rstr[MAX],S_str[MAX],get_str[MAX],str2[MAX]=" 已加入聊天室",str3[MAX]=" 已離開聊天室";
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

    SOCKET        	sd;     // 負責接受訊息來自 IP_A
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
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);  // 選擇要使用的網卡
// -------------------------------------------------------/
     /**  Start using non-blocking   **/
    //enable non-blcking mode when iMode=1
    u_long iMode=1;
    ioctlsocket(sd,FIONBIO, &iMode);        /**  let socket-sd become non-blocking  **/

    u_long iMode2=1;
    ioctlsocket(serv_sd,FIONBIO, &iMode2);


    bool connect=false;     // 判斷是否已經連線
    char name[MAX];         // 儲存Client 的暱稱
    int Nlength = 0;        // Length of Client's name
	while(1)
	{
        if( connect ){
            n=recvfrom(sd, rstr, MAX, 0,(LPSOCKADDR) &multi_serv,&multi_serv_len ); // receive message from multi-IP
            rstr[n]='\0';

            if( n>= 0){
                if( strcmp(rstr,JPEG_CODE) == 0){
                    cout << ">> Server 即將傳送圖片" << endl;
                    Sleep(5000);
                    cout << ">> 圖片產生中...." << endl;
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
                case'a':     // 開始連線
                    if( setsockopt(sd,IPPROTO_IP, IP_ADD_MEMBERSHIP,
                                   (char *) &multicastRequest, sizeof(multicastRequest)) < 0){
                        cout << "重複連線! " <<endl;
                        break;}
                    cout << ">> 請輸入你的暱稱: ";
                    cin >> name;    cin.get();
                    send(serv_sd, name,strlen(name)+1,0);   //傳送姓名資訊給server

                    Nlength = strlen(name);
                    name[Nlength] = ':';   name[Nlength+1] = ' '; name[Nlength+2] = '\0';
                    connect = true;
                    cout << ">> 已連線至 " << MULTI_IP_A << " Port: " << MULTI_PORT << endl;

                    strcpy(S_str, name);
                    strcat(S_str, str2);
                    send(serv_sd, S_str,strlen(S_str)+1,0);
                    break;

                case'm':    // 讓client輸入訊息
                    if( connect ){
                        cout << ">> 請輸入訊息: ";
                        cin.getline(get_str,MAX,'\n');
                        strcpy(S_str, name);
                        strcat(S_str, get_str);
                        send(serv_sd, S_str,strlen(S_str)+1,0);     // mono-send to the server
                    }
                    else    cout << ">> 連線遺失，請按'a'來重新建立連線 " << endl;
                    break;

                case'd':    // 刪除連線
                    multicastDelete.imr_multiaddr.s_addr = inet_addr(MULTI_IP_A);
                    multicastDelete.imr_interface.s_addr = htonl(INADDR_ANY);   // INADDR_ANY 為預設網卡(大部分電腦只有一張網卡)
                    if( setsockopt(sd,IPPROTO_IP, IP_DROP_MEMBERSHIP,
                                   (char *) &multicastDelete, sizeof(multicastDelete)) < 0){
                        cout << ">> 已經沒有連線可以刪除了耶..." <<endl;
                        break;}
                    cout << ">> 正在刪除連線..."; delay(3); cout << "OK!"<<endl;
                    connect = false;

                    strcpy(S_str, name);
                    strcat(S_str, str3);
                    send(serv_sd, S_str,strlen(S_str)+1,0);        // 離線訊息
                    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   離線代碼
                    name[0] = '\0';
                    system("pause");
                    exit(1);
                    break;


                case'h':     // 印出小幫手
                    help();
                    break;

                case'C':     //清除螢幕
                    system("cls");
                    help();
                    break;

                case'Q':    //關閉程式
                    if(name[0] == '\0') name[0] = '?';
                    strcpy(S_str, name);
                    strcat(S_str, str3);
                    send(serv_sd, S_str,strlen(S_str)+1,0);     // 離線訊息
                    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   離線代碼
                    delay(2);

                    closesocket(sd);
                    WSACleanup();
                    exit(1);
                    break;
            }
        }
	}
    send(serv_sd, EXIT_CODE,strlen(EXIT_CODE)+1,0);//   離線代碼
  	closesocket(sd);
    closesocket(serv_sd);
   	WSACleanup();
   	return 0;
}
void help()
{
    cout << "-------------------------" << endl;
    printf("按下\"a\"建立連線\n");
    printf("按下\"h\"開啟幫助\n");
    printf("按下\"m\"輸入訊息\n");
    printf("按下\"d\"關閉連線\n");
    printf("按下\"C\"清除螢幕\n");
    printf("按下\"Q\"關閉程式\n");
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



