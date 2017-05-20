#pragma once
#include "mainh.h"
#define DEBUG if (argc >= 5) if (!strcmp(debug, "debug")) {
#define END_DEBUG }

#define DEBUG_1 DEBUG \
				cout << s_maddr->sin_addr.S_un.S_addr << endl; \
				cout << (int)s_maddr->sin_addr.S_un.S_un_b.s_b1 << endl; \
				cout << (int)s_maddr->sin_addr.S_un.S_un_b.s_b2 << endl; \
				cout << (int)s_maddr->sin_addr.S_un.S_un_b.s_b3 << endl; \
				cout << (int)s_maddr->sin_addr.S_un.S_un_b.s_b4 << endl; \
				char src[INET_ADDRSTRLEN]; \
				cout << inet_ntop(AF_INET, (void*)&(s_maddr->sin_addr), src, INET_ADDRSTRLEN) << endl << endl; /* Выводим адрес в "нормальном" формате. */ \
				END_DEBUG

#define DEBUG_2 DEBUG \
				cout << raw_req.str() << endl; \
				END_DEBUG

#define DEBUG_3 DEBUG \
				cout << "Yes 001 print." << endl; \
				END_DEBUG

#define DEBUG_4 DEBUG \
				cout << "Buffer size (1) : "; \
				cout << buffer.size() << endl; \
				END_DEBUG

#define DEBUG_5 DEBUG \
				cout << "Buffer size (2) : "; \
				cout << buffer.size() << endl; \
				END_DEBUG

#define DEBUG_6 DEBUG \
				cout << "Buffer size (3) : "; \
				cout << buffer.size() << endl; \
				END_DEBUG

#define STEP(step) printf("Step: %d\n", step);
