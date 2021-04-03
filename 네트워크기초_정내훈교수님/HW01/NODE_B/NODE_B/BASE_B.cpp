#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h>
#include "conn.h"
#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_PORT        3500

void do_node_b();

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
};


SOCKETINFO node_A;
CONN g_conn;

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"  ����: " << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	while (true);
}

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	if (0 != Error) {
		wcout << L"������ ���������ϴ�. �����մϴ�.";
		exit(0);
	}

	SOCKET client_s = node_A.socket;

	if (dataBytes == 0)
	{
		closesocket(node_A.socket);
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���
	// cout << "From client received " << dataBytes << " bytes\n";
	node_A.dataBuffer.len = dataBytes;
	memset(&(node_A.overlapped), 0, sizeof(WSAOVERLAPPED));
	for (DWORD i = 0; i < dataBytes; ++i)
		g_conn.set_value(node_A.messageBuffer[i] == 1);


	node_A.dataBuffer.len = MAX_BUFFER;
	node_A.dataBuffer.buf = node_A.messageBuffer;
	memset(&node_A.overlapped, 0, sizeof(WSAOVERLAPPED));
	DWORD flags = 0;
	WSARecv(node_A.socket, &node_A.dataBuffer, 1, NULL, &flags, &(node_A.overlapped), recv_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	if (0 != Error) {
		wcout << L"������ ���������ϴ�. �����մϴ�.";
		exit(0);
	}

	SOCKET client_s = node_A.socket;

	if (dataBytes == 0) {
		closesocket(node_A.socket);
		wcout << L"Node A�� ��������ϴ�.\n";
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���

	delete overlapped;
}
void send_value(bool value)
{
	SOCKETINFO* over = new SOCKETINFO;
	over->dataBuffer.len = 1;
	over->dataBuffer.buf = over->messageBuffer;
	memset(&(over->overlapped), 0, sizeof(WSAOVERLAPPED));
	if (true == value) over->messageBuffer[0] = 1;
	else over->messageBuffer[0] = 0;
	int res = WSASend(node_A.socket, &(over->dataBuffer), 1, NULL, 0, &(over->overlapped), send_callback);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Send Error! ", err_no);
	}
}


CONN::CONN() : stat(false)
{
}

void CONN::set(bool value)
{
	stat = value;
	SleepEx(0, TRUE);
	send_value(value);
}

void CONN::set_value(bool value)
{
	stat = value;
}

bool CONN::get()
{
	SleepEx(0, TRUE);
	return stat;
}


int main()
{
	wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	wcout << L"��� B�� �����մϴ�.\n";



	wcout << L"��� A�� ������ �õ��մϴ�.\n";
	node_A.socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);


	while (true) {
		int res = connect(node_A.socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
		if (0 != res) {
			wcout << L"��� A�� ������ �����ϴ�. ��õ� �մϴ�." << endl;
		}
		else break;
	}
	wcout << L"��� A�� ����Ǿ����ϴ�.\n";

	//unsigned long noblock = 1;
	//ioctlsocket(node_A.socket, FIONBIO, &noblock);

	node_A.dataBuffer.len = MAX_BUFFER;
	node_A.dataBuffer.buf = node_A.messageBuffer;
	memset(&node_A.overlapped, 0, sizeof(WSAOVERLAPPED));
	DWORD flags = 0;
	WSARecv(node_A.socket, &node_A.dataBuffer, 1, NULL, &flags, &(node_A.overlapped), recv_callback);

	do_node_b();

	closesocket(node_A.socket);

	WSACleanup();
}

