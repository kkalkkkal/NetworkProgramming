/*
 * udp_client.c
 *
 *  Created on: 2020. 5. 18.
 *      Author: minho
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUFF_SIZE 4096
void error_handling(char *message);

static int client_socket;
struct sockaddr_in server_addr, client_addr;



struct packet{
	char data[4096]; // 데이터
	int data_size; // 데이터 크기
	int data_seq; // 데이터 시퀀스 번호
};


int main(void)
{
	FILE *myfile; // 전송할 파일.
	char filename[100]; //파일 이름 변수
	struct packet test_packet;
	int count = 1; // 데이터 시퀀스 카운트
	int packet_len = 0; // 구조체 크기
	int buf[1];
	int fileSize = 0; // 파일 크기


	client_socket = socket(PF_INET, SOCK_DGRAM, 0);

	if(client_socket == -1)
	{
		error_handling("socket() error");
	}

	memset(&server_addr, 0 , sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	server_addr.sin_port = htons(4001);  // 인자를 받을 때는 atoi(argv[1])로 대체

	if(-1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		error_handling("connect() error"); // connect() 실패
	}

	// connect하여 커널에 소켓이 연결되었을 경우, 파일 전송 시작.
	//127.0.0.1로 데이터 전송
	while(1)
	{
		//-> 함수 내에서 파일명을 입력 받아서 전송할 경우, 주석 해제.
		//fputs("전송할 파일명 입력 : ", stdout);
		//fgets(filename, sizeof(filename), stdin);
		//char *ptr= strtok(filename, "\n");


		memset(&test_packet, 0, sizeof(test_packet)); // packet 초기화

		// myfile 파일 읽기
		if((myfile = fopen("Katuri.ttf", "r")) == NULL) // filename
		{
			error_handling("fopen() error");
		}

		if(count == 1) // 처음 파일을 읽어들일 때
		{
			fseek(myfile, 0 , SEEK_END); // 파일 포인터를 파일 끝으로 이동
			fileSize = ftell(myfile); // 파일 총 크기 저장.
			// 파일 포인터의 현재 위치를 얻음 = 파일의 크기
		}// 파일 포인터 반환.

		//패킷에 데이터 저장

		fseek(myfile, 0 , SEEK_SET); // 파일 포인터를 파일 처음으로 이동
		while(fileSize > 0)
		{
			fgets(test_packet.data, BUFF_SIZE, myfile);
			//printf("%s", test_packet.data);

			if(fileSize > BUFF_SIZE)
			{
				test_packet.data_size = BUFF_SIZE;// 각 패킷마다 기본 4096 바이트.
			}
			else
			{
				test_packet.data_size = fileSize;// 마지막에는 잉여 바이트 출력.
			}

			// 현 데이터 시퀀스 번호를 패킷에 저장.
			test_packet.data_seq = count;


			// 파일 데이터 패킷 전송 ( 구조체 크기 )
			packet_len = sizeof(test_packet);

			if(-1 == sendto(client_socket, (char*)&packet_len,
					sizeof(int), 0,(struct sockaddr*)&server_addr, sizeof(server_addr)))
			{
				error_handling("sendto() error");
			}


			// 파일 데이터 패킷 전송 ( 구조체 데이터 )
			if(-1 == sendto(client_socket, (char*)&test_packet,
					packet_len, 0,(struct sockaddr*)&server_addr, sizeof(server_addr)))
			{
				error_handling("sendto() error");
			}

			printf("%d byte data (seq %d) send.\n", test_packet.data_size, test_packet.data_seq);

			count++; // 데이터 시퀀스 번호 증가
			fileSize = fileSize - BUFF_SIZE; // 읽어들인 바이트 만큼 감소.
		}

		// 파일이 끝났음을 알림.
		buf[0] = 0x1A;
		if(-1 == sendto(client_socket, buf,
							BUFF_SIZE, 0,(struct sockaddr*)&server_addr, sizeof(server_addr)))
		{
			error_handling("sendto() error");
		}

		break;
	}


	close(client_socket);
	fclose(myfile);
	printf("end\n");
	return 0;
}


void error_handling(char *message)
{
	fputs(message, stderr); // message를 stderr에 저장
	fputc('\n', stderr); // stderr출력
	exit(1); // 오류 메시지 로그 출력 후 종료
}
