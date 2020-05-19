/*
 * udp_server.c
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

#define BUFF_SIZE 4096
void error_handling(char *message);

static struct sockaddr_in server_addr, client_addr;
static int server_socket, client_socket, n;
static char buff_rcv[BUFF_SIZE];

struct packet{
	char data[4096]; // 데이터
	int data_size; // 데이터 크기
	int data_seq; // 데이터 시퀀스 번호
};

int main(void)
{
	int str_len; // 받은 데이터 길이
	struct packet test_packet;
	socklen_t client_len;
	int packet_len = -1, n = 0; // 받은 구조체 크기

	server_socket = socket(PF_INET, SOCK_DGRAM, 0); // udp 소캣 할당

	if(server_socket == -1) // 소캣 할당 실패시
	{
		error_handling("socket() error");
	}

	memset(&server_addr, 0 , sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port = htons(4001);  // 인자를 받을 때는 atoi(argv[1])로 대체

	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) // bind 수행
	{
		error_handling("bind() error"); // 실패 시 오류 출력
	}

	while(1)
	{
		memset(&test_packet, 0, sizeof(test_packet)); // packet 초기화

		client_len = sizeof(client_addr);


		n = recvfrom(server_socket, (void *)&packet_len, sizeof(packet_len), 0 ,
						(struct sockaddr*)&client_addr, &client_len);

		if( n != sizeof(packet_len))
		{
			error_handling("packet_len error");
		}

		if( packet_len == 0x1A) // 0x1A를 받으면
		{
			printf("recv complete.\n");
			continue; // 루프 초기화
		}

		str_len = recvfrom(server_socket, &test_packet, packet_len, 0 ,
				(struct sockaddr*)&client_addr, &client_len);

		//printf("%d byte data (seq %d) received\n", test_packet.data_size, test_packet.data_seq);

		if(str_len == -1)
		{
			error_handling("recvfrom() error");
		}


		printf("%d byte data (seq %d) received\n", test_packet.data_size, test_packet.data_seq);


	}

	close(server_socket);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr); // message를 stderr에 저장
	fputc('\n', stderr); // stderr출력
	exit(1); // 오류 메시지 로그 출력 후 종료
}
