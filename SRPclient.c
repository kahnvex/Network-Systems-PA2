/* client.c */
/* This is a sample UDP client/sender.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>   
#include <sys/time.h> 
#include <signal.h>
#include <unistd.h>
#include "sendto_.h"
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "types.h"


#define REMOTE_SERVER_PORT 50005
#define SWS 4
#define MAXSEQNUM 8
#define FILESIZE (24*1024)
#define BUFSIZE 100
//#define DELM "/"

struct sockaddr_in cliAddr, remoteServAddr, recAddr;


int main(int argc, char *argv[]) {

  int sock, lSize, readResult, selret;
  FILE* fp;
  unsigned int recLen;
  int i = 0;
  frame framearray[11];
  char* sendFrame;
  char* p;
  char filebuffer[FILESIZE];
  char recvmsg[BUFSIZE];
  char directemp[BUFSIZE];
  char filename[BUFSIZE];
  char msgbuffer[512];
  struct timeval time;
  fd_set readFDS;
  double timearray[SWS];
  frame frameArray[MAXSEQNUM];
  int LB = 0, RB = 4;

  char msg0[] = "This is Frame 0";
  char msg1[] = "This is Frame 1";
  char msg2[] = "This is Frame 2";
  char msg3[] = "This is Frame 3";
  char msg4[] = "This is Frame 4";
  char msg5[] = "This is Frame 5";
  char msg6[] = "This is Frame 6";
  char msg7[] = "This is Frame 7";
  char msg8[] = "This is Frame 8";
  char msg9[] = "This is Frame 9";
  char msg10[] = "This is Frame 10";

  setFrame(&framearray[0],0,0,sizeof(msg0),0,msg0);
  setFrame(&framearray[1],1,0,sizeof(msg1),0,msg1);
  setFrame(&framearray[2],2,0,sizeof(msg2),0,msg2);
  setFrame(&framearray[3],3,0,sizeof(msg3),0,msg3);
  setFrame(&framearray[4],4,0,sizeof(msg4),0,msg4);
  setFrame(&framearray[5],5,0,sizeof(msg5),0,msg5);
  setFrame(&framearray[6],6,0,sizeof(msg6),0,msg6);
  setFrame(&framearray[7],7,0,sizeof(msg7),0,msg7);
  setFrame(&framearray[8],8,0,sizeof(msg8),0,msg8);
  setFrame(&framearray[9],9,0,sizeof(msg9),0,msg9);
  setFrame(&framearray[10],10,1,sizeof(msg10 ),0,msg10);

  //check command line args.
  if(argc<6) {
    printf("usage : %s <server> <error rate> <random seed> <send_file> <send_log> \n", argv[0]);
    exit(1);
  }

  printf("argv4: %s\n",argv[4]);

  if(strcmp(argv[4],"0")){

    strcpy(directemp,argv[4]);
    p = strtok(directemp,DELM);
      
    while(p != NULL){
      strcpy(filename,p);
      p= strtok(NULL,DELM);
    }
      
    printf("Filename: %s\n",filename);
    fp = fopen(argv[4],"r");
    if(fp == NULL){
      printf("Error opening file: %s\n",strerror(errno));
    }
     
    //strcpy(msgbuffer,filebuffer);
  }

  
  
  printf("error rate : %f\n",atof(argv[2]));
  printf("%s: sending data to '%s' \n", argv[0], argv[1]);
    

  /* Note: you must initialize the network library first before calling
     sendto_().  The arguments are the <errorrate> and <random seed> */
  init_net_lib(atof(argv[2]), atoi(argv[3]));

  /* get server IP address (input must be IP address, not DNS name) */


  //setup socket structs
  /***************************************************/
  remoteServAddr.sin_family = AF_INET;
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);
  inet_pton(AF_INET, argv[1], &remoteServAddr.sin_addr);
  
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);
  /*************************************************/



  /* socket creation */
  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("Unable to connect socket!\n");
    exit(1);
  }

  char array1[513];
  array1[512] ='\0';
  char array2[513];
  array2[512] = '\0';
  char array3[513];
  array3[512] = '\0';

  readtoframe(array1, &fp);
  readtoframe(array2, &fp);
  readtoframe(array3, &fp);
 
  printf("Array 1:\n %s\n\n\n\n", array1);
  printf("Array 2:\n %s\n\n\n\n", array2);
  printf("Array 3:\n %s\n\n\n\n", array3);

  //printFrame(framearray[0]);
    
  //sendFrame = makedatapacket(framearray[0]);

  while(1){
    int oldLB = LB;
    
    int moveCount = MoveForward(&LB, &RB, frameArray, MAXSEQNUM);

    SendNextFrames(moveCount, frameArray, MAXSEQNUM, RB, fp);

    /*gettimeofday(&time,NULL);
    double t1 = time.tv_sec + (time.tv_usec/1000000.0);
    printf("Sent time: %f\n", t1);*/
    /*if((sendto_(sock,sendFrame, strlen(sendFrame),0, (struct sockaddr *) &remoteServAddr,
		sizeof(remoteServAddr)))< 0 ){
      printf("Error Sending\n");
      perror("sendto()");
      exit(1);
      }*/
      
    selret = ballinselect(sock,&readFDS,10,0);

    
    if ((selret != -1) && (selret != 0)){
      
      if(recvfrom(sock, &recvmsg, sizeof (recvmsg), 0,(struct sockaddr *) &recAddr, &recLen) < 0){
	perror("recvfrom()");
	exit(1);
      }

      printf("Received Ack!\n");
      
      printf("Ack Buffer: %s\n", recvmsg);

    }
    else if (selret == 0) {
      printf("timeout\n");
      gettimeofday(&time,NULL);
      double t2= time.tv_sec + (time.tv_usec/1000000.0);
      printf("Time Out Time time: %f\n",t2);
    } else if (selret < 0) {printf("select() failed\n");}
  }

  free(sendFrame);
  fclose(fp);
    
}
