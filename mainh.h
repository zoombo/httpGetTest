#pragma once
#pragma comment (lib, "Ws2_32.lib")
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include <vector>
#include <conio.h>
#include "debugsh.h"

#define MYBUFFER_LEN 10485760 // 10 Mb - размер буфера для ответа.
#define ONE_MB 1048576 // 1 Mb
#define ESC 27
#define TEST
#define END_TEST

#define TEST_ASOI 0

using namespace std;

int init_WSAlib(int lib_version_H, int lib_version_L);
int get_data(int argc, char **argv, vector<char*> &buffer);
