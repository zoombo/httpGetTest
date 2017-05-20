#pragma once
#include "mainh.h"

// Это для задействования Dll библиотеки винды отвечающей за сокеты.
int init_WSAlib(int a, int b) {
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(a, b);
	if (FAILED(WSAStartup(DllVersion, &wsaData))) { // Если все норм. WSAStartup вернет 0 если ошибка !=0.
		cout << "Error init winsock." << endl;
		return WSAGetLastError();
	}
	return 0;
}

int get_data(int argc, char **argv, vector<char*> &buffer) {

	// Это для задействования Dll библиотеки винды отвечающей за сокеты.
	if (FAILED(init_WSAlib(2, 1)))
		return WSAGetLastError();

	// Установим параметры по умолчанию чтобы не ловить segfault'ы.
	char *host = "localhost", *port = "80", *URI = "/", *debug = "no_debug";
	// Тут меняем умолчальные параметры на заданные, если они есть.
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

	//Основной цикл.
	for (;;) {

		extern int exit_flag;

		// Проверка нажатия клавиши.
		if (exit_flag)
			break;

		// Создаем сокет.
		SOCKET mysocket;
		// MSDN: Если всё норм, socket() вернет дескриптор сокета, если ошибка, вернется INVALID_SOCKET. 
		if (INVALID_SOCKET == (mysocket = socket(AF_INET, SOCK_STREAM, 0))) {
			int error = WSAGetLastError();
			cout << "Error create socket. Error code : %d" << error << endl;
			exit_flag = 1;
			return 1;
		}

		// Резолвим hostname.
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
		В структуре addrinfo поле типа ai_addr, является, указателем на структуру типа sockaddr
		которая, может быть приведена к структуре типа sockaddr_in которая, используется вызовом connect.
		Нагляднее будет создать еще одну структуру sockaddr_in и скопировать поля, но это будет лишняя трата памяти.
		*/

		// Соединяемся с сервером используя созданный сокет.
		sockaddr_in *s_maddr = (sockaddr_in*)servinfo->ai_addr; DEBUG_1
			// MSDN: If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR...
			if (SOCKET_ERROR == (connect(mysocket, (sockaddr*)s_maddr, sizeof(*s_maddr)))) {
				int error = WSAGetLastError();
				cout << "Error connection. Error code : %d" << error << endl;
				exit_flag = 1;
				return 1;
			}

		// Формируем HTTP-запрос.
		stringstream raw_req;
		string final_req;
		raw_req << "GET " << URI << " HTTP/1.1\r\n";
		raw_req << "Host: " << host << "\r\n\r\n";
		final_req = raw_req.str();

		DEBUG_2;

		// Отправляем...
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
		// Принимаем ответ.
		vector<char*>::iterator new_item(buffer.end()); // Можно просто "buffer.insert(buffer.begin(), "")" , но так нагладянее. Наверное...
		buffer.insert(new_item, ""); // Резервируем в векторе один элемент типа char*.
		// Запрашиваем 1 Mb памяти у системы и указатель на начало выделенной 
		// памяти присваиваем 0-му элементу вектора.
		buffer.back() = (char*)calloc(ONE_MB, sizeof(char));
		unsigned long int final_message_len = 0; // Финальный размер буфера.
		int message_len = 0; DEBUG_4; // Длинна ответа. (Количество считанных байт.)

		do {
			// Передаем в функцию указатель на начало участка памяти(массива), достаточного чтобы принять ONE_MB данных. 
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
			final_message_len += message_len; // Длинна(общая) уже записанного в буфер сообщения.
			// Увеличиваем буфер на ONE_MB. (А точнее перемещаем в новое место которое больше предыдущего на ONE_MB. См. realloc() )
			buffer.back() = (char*)realloc(buffer.back(), final_message_len + ONE_MB);
		} while (message_len > 0); // Работать пока не считаются все данные.

		*(buffer.back() + final_message_len) = '\0'; // В стиле Pure C но, пока так.
		mainMut.unlock();
		closesocket(mysocket);

		extern int sleep_timeout; 
		// Сдесь и в функции "main", задать произвольные значения 
		// или закомментировать в разной последовательности для проверки.
		Sleep(sleep_timeout); 
		DEBUG_6;
	}
	return 0;
}
