#pragma once
#include "mainh.h"

// ��� ��� �������������� Dll ���������� ����� ���������� �� ������.
int init_WSAlib(int a, int b) {
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(a, b);
	if (FAILED(WSAStartup(DllVersion, &wsaData))) { // ���� ��� ����. WSAStartup ������ 0 ���� ������ !=0.
		cout << "Error init winsock." << endl;
		return WSAGetLastError();
	}
	return 0;
}

int get_data(int argc, char **argv, vector<char*> &buffer) {

	// ��� ��� �������������� Dll ���������� ����� ���������� �� ������.
	if (FAILED(init_WSAlib(2, 1)))
		return WSAGetLastError();

	// ��������� ��������� �� ��������� ����� �� ������ segfault'�.
	char *host = "localhost", *port = "80", *URI = "/", *debug = "no_debug";
	// ��� ������ ����������� ��������� �� ��������, ���� ��� ����.
	for (int i = 1; i < argc; i++)
		switch (i)
		{
		case 1:
			host = argv[1];
			break;
		case 2:
			port = argv[2];
			break;
		case 3:
			URI = argv[3];
			break;
		case 4:
			debug = argv[4];
			break;
		default:
			break;
		}

	//�������� ����.
	for (;;) {

		extern int exit_flag;

		// �������� ������� �������.
		if (exit_flag)
			break;

		// ������� �����.
		SOCKET mysocket;
		// MSDN: ���� �� ����, socket() ������ ���������� ������, ���� ������, �������� INVALID_SOCKET. 
		if (INVALID_SOCKET == (mysocket = socket(AF_INET, SOCK_STREAM, 0))) {
			int error = WSAGetLastError();
			cout << "Error create socket. Error code : %d" << error << endl;
			exit_flag = 1;
			return 1;
		}

		// �������� hostname.
		struct addrinfo hints, *servinfo;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		// MSDN: Success returns zero. Failure returns a nonzero Windows Sockets error code...
		if (getaddrinfo(host, port, &hints, &servinfo)) {
			cout << "Get IP-address failed." << endl;
			exit_flag = 1;
			return 1;
		}

		/* Remember!
		� ��������� addrinfo ���� ���� ai_addr, ��������, ���������� �� ��������� ���� sockaddr
		�������, ����� ���� ��������� � ��������� ���� sockaddr_in �������, ������������ ������� connect.
		��������� ����� ������� ��� ���� ��������� sockaddr_in � ����������� ����, �� ��� ����� ������ ����� ������.
		*/

		// ����������� � �������� ��������� ��������� �����.
		sockaddr_in *s_maddr = (sockaddr_in*)servinfo->ai_addr; DEBUG_1
			// MSDN: If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR...
			if (SOCKET_ERROR == (connect(mysocket, (sockaddr*)s_maddr, sizeof(*s_maddr)))) {
				int error = WSAGetLastError();
				cout << "Error connection. Error code : %d" << error << endl;
				exit_flag = 1;
				return 1;
			}

		// ��������� HTTP-������.
		stringstream raw_req;
		string final_req;
		raw_req << "GET " << URI << " HTTP/1.1\r\n";
		raw_req << "Host: " << host << "\r\n\r\n";
		final_req = raw_req.str();

		DEBUG_2;

		// ����������...
		/*
		MSDN: If no error occurs, send returns the total number of bytes sent,
		which can be less than the number requested to be sent in the len parameter.
		Otherwise, a value of SOCKET_ERROR is returned...
		*/
		if (SOCKET_ERROR == (send(mysocket, final_req.c_str(), final_req.length(), 0))) {
			int error = WSAGetLastError();
			cout << "Send message error. Error code : %d" << error << endl;
			exit_flag = 1;
			return 1;
		}

		extern mutex mainMut;
		mainMut.lock();
		// ��������� �����.
		vector<char*>::iterator new_item(buffer.end()); // ����� ������ "buffer.insert(buffer.begin(), "")" , �� ��� ����������. ��������...
		buffer.insert(new_item, ""); // ����������� � ������� ���� ������� ���� char*.
									 // ����������� 1 Mb ������ � ������� � ��������� �� ������ ���������� 
									 // ������ ����������� 0-�� �������� �������.
		buffer.back() = (char*)calloc(ONE_MB, sizeof(char));
		unsigned long int final_message_len = 0; // ��������� ������ ������.
		int message_len = 0; DEBUG_4; // ������ ������. (���������� ��������� ����.)

		do {
			// �������� � ������� ��������� �� ������ ������� ������(�������), ������������ ����� ������� ONE_MB ������. 
			/*
			MSDN: If no error occurs, recv returns the number of bytes received and the buffer pointed to by the buf parameter
			will contain this data received. If the connection has been gracefully closed, the return value is zero.
			Otherwise, a value of SOCKET_ERROR is returned
			*/
			if (SOCKET_ERROR == (message_len = recv(mysocket, (buffer.back() + final_message_len), ONE_MB, 0))) {
				int error = WSAGetLastError();
				cout << "Receive message error. Error code : %d" << error << endl;
				return 1;
			}
			final_message_len += message_len; // ������(�����) ��� ����������� � ����� ���������.
											  // ����������� ����� �� ONE_MB. (� ������ ���������� � ����� ����� ������� ������ ����������� �� ONE_MB. ��. realloc() )
			buffer.back() = (char*)realloc(buffer.back(), final_message_len + ONE_MB);
		} while (message_len > 0); // �������� ���� �� ��������� ��� ������.

		*(buffer.back() + final_message_len) = '\0'; // � ����� Pure C ��, ���� ���.
		mainMut.unlock();
		closesocket(mysocket);

		extern int sleep_timeout;
		// ����� � � ������� "main", ������ ������������ �������� 
		// ��� ���������������� � ������ ������������������ ��� ��������.
		Sleep(sleep_timeout);
		DEBUG_6;
	}
	return 0;
}

