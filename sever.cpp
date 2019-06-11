/* server */
#include <iostream>
#include <winsock.h>
#include <stdio.h>
#include <conio.h>


using namespace std;
#define size 250
#define SIZE 1024
#define MULTI_IP_A "230.0.0.5"
#define MULTI_PORT 9999
char EXIT_CODE[34] = "qiowsdfklmvasdf%*asdfasdf82&s4$sa";
char JPEG_CODE[34] = "qiowsdwerrvasdf%*asd&a-df82&s4$!a";

void *ThreadMain(void *arg);             /* Main program of a thread */
void help();
void printClntTable();
void jpgSending(int serv_sd,sockaddr_in serv);

string clntTable[SIZE][2];
int tableSize = 0;

struct ThreadArgs /* Structure of arguments to pass to client thread */
{
    int clntIndex;
    int clntSock;                      /* Socket descriptor for client */
    sockaddr_in clntAddr;
};
int main()
{
    help();
    struct ThreadArgs *threadArgs;   /* Pointer to argument structure for thread */
    DWORD  threadID;                 /* Thread ID from CreateThread() */

    int clntIndex = -1;               /* Index for client table */
	int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
	struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
 	int clntLen;            /* Length of client address data structure */

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        printf("WSAStartup() failed");
        exit(1);
    }

	/* Create socket for incoming connections */
    if ((servSock = socket(AF_INET, SOCK_STREAM, 0) ) < 0)
        printf("socket() failed");

    /* Construct local address structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = 0;                 /* Any incoming interface */
    echoServAddr.sin_port = htons(7788);              /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (LPSOCKADDR)&echoServAddr, sizeof(echoServAddr)) < 0)
        printf("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 10) < 0)
        printf("listen() failed");

    for (;;) /* Run forever */
    {
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (LPSOCKADDR) &echoClntAddr, &clntLen)) < 0)
            printf("accept() failed");

            clntIndex ++;
            tableSize ++;
            /* Create separate memory for client argument */
            threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
            threadArgs -> clntSock = clntSock;
            threadArgs -> clntAddr = echoClntAddr;
            threadArgs -> clntIndex = clntIndex;

            if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadMain, threadArgs,
                  0, (LPDWORD) &threadID) == NULL)
                printf("thread_create() failed");

            //printf("with thread %ld\n", threadID);
    }
}
void *ThreadMain(void *threadArgs)
{
    char str[2]="",rstr[SIZE];
    int n;
    bool flag = false;  // flag for get Client name
    sockaddr_in clntAddr  = ((struct ThreadArgs *) threadArgs) -> clntAddr;  /** Get client info **/
    int         clntSock  = ((struct ThreadArgs *) threadArgs) -> clntSock; /* Socket descriptor for client connection */
    int         clntIndex = ((struct ThreadArgs *) threadArgs) -> clntIndex;

    free(threadArgs);              /* Deallocate memory for argument */
    int len_clntAddr = sizeof(clntAddr);

    /**  let socket-client become non-blocking  **/
    u_long iMode=1;
    ioctlsocket(clntSock,FIONBIO, &iMode);

    /**    Create a multi-socket     **/
    SOCKET serv_sd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serv;
    int multicastTTL = 1;
    if( setsockopt( serv_sd,IPPROTO_IP, IP_MULTICAST_TTL,
                    (char *) &multicastTTL, sizeof(multicastTTL)) == SOCKET_ERROR )
        cout << "setsockopt() failed" <<endl;
    cout << "server will multicast." <<endl;

    int serv_len = sizeof(serv);
    serv.sin_family      = AF_INET;
    serv.sin_port        = htons(MULTI_PORT);
    serv.sin_addr.s_addr = inet_addr(MULTI_IP_A);
/** ------------------------------------------------------------- */

    /**  Build Client Table  **/
    getpeername(clntSock, (LPSOCKADDR)&clntAddr, &len_clntAddr );
    printf("client IP: %s\n", inet_ntoa(clntAddr.sin_addr));
    clntTable[clntIndex][1] = inet_ntoa(clntAddr.sin_addr); // Setting client IP in Client Table
    /**  Setting client name in Client Table **/
    while(1){
        n = recv(clntSock, rstr, SIZE, 0);  rstr[n] = '\0';
        if( n > 0 ){
            clntTable[clntIndex][0] = rstr;
            break;
        }
    }

    for (;;)
    {
        n = recv(clntSock, rstr, SIZE, 0);  // mono-cast getting message from client
        //rstr[n] = '\0';
        if(n > 0){           /**  have get message from client    **/
            if(strcmp(rstr,EXIT_CODE) == 0 ){
                cout << "in!" << endl;
                swap(clntTable[clntIndex][0],clntTable[tableSize-1][0]);
                swap(clntTable[clntIndex][1],clntTable[tableSize-1][1]);
                if(tableSize >0) tableSize -= 1;
            }
            else
                sendto(serv_sd,rstr,strlen(rstr)+1,0,(LPSOCKADDR)&serv,serv_len);   // forwarding the message to multi-IP
        }
        if(_kbhit()){
            char selection;
            selection = _getch();
            switch( selection ){
                case'p':    //傳送圖片
                    sendto(serv_sd,JPEG_CODE,strlen(JPEG_CODE)+1,0,(LPSOCKADDR)&serv,serv_len);
                    jpgSending(serv_sd,serv);
                    break;

                case't':    // 印出目前連線列表及線上人數
                    printClntTable();
                    break;

                case'h':     // 印出小幫手
                    help();
                    break;

                case'C':     //清除螢幕
                    system("cls");
                    help();
                    break;

                case'Q':    //關閉程式

                    WSACleanup();
                    exit(1);
                    break;
            }
        }
    }

}
void help()
{
    cout << "-------------------------" << endl;
    printf("按下\"h\"開啟幫助\n");
    printf("按下\"t\"顯示列表\n");
    printf("按下\"p\"傳送圖片\n");
    printf("按下\"C\"清除螢幕\n");
    printf("按下\"Q\"關閉程式\n");
    cout << "-------------------------" << endl;
}
void printClntTable()
{
    cout <<"-------------------------" << endl;
    for(int i=0; i<tableSize; i++){
        for(int j=0;j<2;j++){
            cout << clntTable[i][j] << "\t| ";
        }
        cout << endl << "-------------------------" << endl;
    }
    cout << "目前線上人數： " << tableSize << endl;
}
void jpgSending(int serv_sd,sockaddr_in serv)
{
    int serv_len = sizeof(serv);

    char image_buf[size*size];

    FILE *Fimage;
    string Filename;

    cout << "請輸入欲傳送的圖片檔名 (e.g: test.jpg) ";
    cin >> Filename;
    Fimage=fopen(Filename.c_str(),"rb");
    if(!Fimage){
        cout << "FILE Error! " << endl;
    }
    else{
        fread(&image_buf,1,size*size,Fimage);
    }
	fclose(Fimage);


	sendto(serv_sd,image_buf,size*size,0,(LPSOCKADDR)&serv,serv_len);
	cout << strlen(image_buf) +1;
}

