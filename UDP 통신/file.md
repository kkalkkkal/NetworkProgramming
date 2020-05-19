
UDP 통신을 통해 파일 업로드하는 예제.

udp_client.c : udp 클라이언트 파일, 파일을 업로드하는 소스 udp_server.c : udp 서버 파일, 파일을 읽어들이는 소스

중요 포인트 : 읽어들이는 패킷의 크기만큼 서버에서 받을 수 있도록 클라이언트에서 그 패킷의 크기를 전송하는 것이 선행되어야함.


-소켓 포트 할당 때문에 실행파일이 bind 안 될 때 해결법 :
<리눅스 특정 port 죽이는 방법>

1. 특정 포트 확인.

netstat -nap|grep port  (ex: netstat -nap|grep 1099)


2. 특정 포트에서 사용하는 프로그램 확인.

lsof -i TCP:port  (ex: lsof -i TCP:1099)


3. 특정 포트를 사용하는 프로그램 죽이기.

fuser -k -n tcp port  (ex:fuser -k -n tcp 1099)
[출처] 리눅스 포트 죽이기(lsof -i TCP:port)|작성자 와스고수
