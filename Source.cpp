/*
� ���������� �������� ������� 2 ������.
1) ��������� ������ �� ����������� � ���������� ������ ���������,
�� �������� ��� �� ����������� � ��������� �����(���������������� � ������� INVALID_SOCKET).
2) 2� ������ ��� ������� vector(� ���� FIFO-�������) �� ���������� ���������,
�� �������� ��������� � ��������(��������) �������� �� ������� ( mybuffer[0] ),
� ����� ������, ���� ����� ����������� �������� � vector ����� �������� �������
������ ��������� � ���������� �������� vector'�, �� ��� �� 2� �������� �����
������ ���������� �������� ��� �� �������������� ��������.
����� ������� ���������� ����� 2 ������:
1) ������ ����� ����������;
2) ������ ������ �� �� ���������� ���������� (��. free()).
*/

/*
* ����� ������� �������� 1� ���������� � ��������� ������.
* ���� �������� 2� ����������, ���� ��� ��� �� �� ��������� ����� 80.
* URI �������� 3� ����������, ���� ��� ��� �� �� ��������� ����� "/".
* 4� ����������, ����� �������� "����" debug � �������� ��������� ���������� ���������� ����. ������ ����-����...
* ���� ������� �������� � ������ ��������� ������ �� ������.
* ����� ������ ����� ���� ���� �������������, ����� ��������� ������ ��� ���������� �����. ���� ���.
* �����, ����, ������ GET.
* ���� ������ IPv4.
* ����� �� ��������� �������������� ������� �������� ESC.
*/
#pragma once
#include "mainh.h"

/*
*
*/

int exit_flag = 0; // ���� ������� ����� ������������ � 1 ����� ������ ������� ESC.
int sleep_timeout = 3000; // 3 �������.
mutex mainMut;

int main(int argc, char** argv) {

	if (argc <= 1) {
		cout << "Usage: <host> <port> <URI> <debug> <timeout>" << endl;
		return 0;
	}
	else if (argc >= 6)
		sleep_timeout = atoi(argv[5]) * 1000;

	vector<char*> mybuffer;

	// std::ref() - ��� �������� ���������� � ����� �� ������! https://habrahabr.ru/post/182610/
	// �������� �� ������ ��� � ������� ������� �� ���������, � �� ��������� ������ �������� �� ���������.
	thread thr1(get_data, argc, argv, ref(mybuffer));

	for (;;) {
		if (exit_flag)
			break;
		// �������� ������� �������.
		if (_kbhit())
			if (_getch() == ESC) {
				exit_flag = 1;
				break;
			}
		// ����� � �������.
		if (mybuffer.size()) { // ���� ���������� ��������� � ������� �� 0.
			mainMut.lock(); // ��������� ������ � �������.
							// �������� ��������(���������) ������� �������� (� ������ �������� ��� ��������).
							// ������� ���������� ������������� �� ��������� �� "\0".
			cout << mybuffer.front();
			free(mybuffer.front()); // ����������� ������ �� ���������.
			mybuffer.erase(mybuffer.begin()); // ������� ������ ������� (� ������ ������� ��� �������).
			mainMut.unlock(); // ������������ ������ � �������.
			Sleep(sleep_timeout); // ����� � � ������� "get_data", ������ ������������ �������� ��� ���������������� � ������ ������������������ ��� ��������.
		}
	}
	thr1.join();

	return 0;
}
