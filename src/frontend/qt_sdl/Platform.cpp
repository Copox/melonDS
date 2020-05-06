/*
    Copyright 2016-2020 Arisotura

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QStandardPaths>
#include <QDir>
#include <QThread>
#include <QSemaphore>
#include <QOpenGLContext>

#include "Platform.h"
#include "PlatformConfig.h"
//#include "LAN_Socket.h"
//#include "LAN_PCap.h"

#ifdef __WIN32__
#define NTDDI_VERSION		0x06000000 // GROSS FUCKING HACK
#include <windows.h>
//#include <knownfolders.h> // FUCK THAT SHIT
#include <shlobj.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define socket_t    SOCKET
#define sockaddr_t  SOCKADDR
#else

#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#define socket_t    int
#define sockaddr_t  struct sockaddr
#define closesocket close
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (socket_t)-1
#endif


extern char* EmuDirectory;

void Stop(bool internal);


namespace Platform
{

socket_t MPSocket;
sockaddr_t MPSendAddr;
u8 PacketBuffer[2048];

#define NIFI_VER 1


void StopEmu()
{
	//Stop(true);
}

FILE* OpenFile(const char* path, const char* mode, bool mustexist)
{
	QFile f(path);

	if (!mustexist && !f.exists())
		return nullptr;

	f.open(QIODevice::ReadOnly);
	FILE* file = fdopen(dup(f.handle()), mode);
	f.close();

	return file;
}

FILE* OpenLocalFile(const char* path, const char* mode)
{
	QString fullpath;

	if (path[0] == '/')
	{
		// If it's an absolute path, just open that.
		fullpath = path;
	}
	else
	{
#ifdef PORTABLE
		fullpath = QString("./") + path;
#else
		// Check user configuration directory
		fullpath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/melonDS/";
		fullpath.append(path);
#endif
	}

	return OpenFile(fullpath.toUtf8(), mode, mode[0] != 'w');
}

FILE* OpenDataFile(const char* path)
{
#ifdef PORTABLE
	return OpenLocalFile(path);
#else
	QString melondir = "/melonDS/";
	QStringList sys_dirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	QString found = nullptr;

	for (int i = 0; i < sys_dirs.size(); i++)
	{
		QString f = sys_dirs.at(i) + melondir + path;

		if (QFile::exists(f))
		{
			found = f;
			break;
		}
	}

	if (found == nullptr)
		return nullptr;

	FILE* f = OpenFile(found.toUtf8(), "rb", false);
	if (f)
		return f;

	return nullptr;
#endif
}

void* Thread_Create(void (* func)())
{
	QThread* t = QThread::create(func);
	t->start();
	return (void*) t;
}

void Thread_Free(void* thread)
{
	QThread* t = (QThread*) thread;
	t->terminate();
	delete t;
}

void Thread_Wait(void* thread)
{
	((QThread*) thread)->wait();
}


void* Semaphore_Create()
{
	return new QSemaphore();
}

void Semaphore_Free(void* sema)
{
	delete (QSemaphore*) sema;
}

void Semaphore_Reset(void* sema)
{
	QSemaphore* s = (QSemaphore*) sema;

	s->acquire(s->available());
}

void Semaphore_Wait(void* sema)
{
	((QSemaphore*) sema)->acquire();
}

void Semaphore_Post(void* sema)
{
	((QSemaphore*) sema)->release();
}


void* GL_GetProcAddress(const char* proc)
{
	return (void*) QOpenGLContext::globalShareContext()->getProcAddress(proc);
}


bool MP_Init()
{
	int opt_true = 1;
	int res;

#ifdef __WIN32__
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		return false;
	}
#endif // __WIN32__

	MPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (MPSocket < 0)
	{
		return false;
	}

	res = setsockopt(MPSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &opt_true, sizeof(int));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	sockaddr_t saddr;
	saddr.sa_family = AF_INET;
	*(u32*) &saddr.sa_data[2] = htonl(Config::SocketBindAnyAddr ? INADDR_ANY : INADDR_LOOPBACK);
	*(u16*) &saddr.sa_data[0] = htons(7064);
	res = bind(MPSocket, &saddr, sizeof(sockaddr_t));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	res = setsockopt(MPSocket, SOL_SOCKET, SO_BROADCAST, (const char*) &opt_true, sizeof(int));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	MPSendAddr.sa_family = AF_INET;
	*(u32*) &MPSendAddr.sa_data[2] = htonl(INADDR_BROADCAST);
	*(u16*) &MPSendAddr.sa_data[0] = htons(7064);

	return true;
}

void MP_DeInit()
{
	if (MPSocket >= 0)
		closesocket(MPSocket);

#ifdef __WIN32__
	WSACleanup();
#endif // __WIN32__
}

int MP_SendPacket(u8* data, int len)
{
	if (MPSocket < 0)
		return 0;

	if (len > 2048 - 8)
	{
		printf("MP_SendPacket: error: packet too long (%d)\n", len);
		return 0;
	}

	*(u32*) &PacketBuffer[0] = htonl(0x4946494E); // NIFI
	PacketBuffer[4] = NIFI_VER;
	PacketBuffer[5] = 0;
	*(u16*) &PacketBuffer[6] = htons(len);
	memcpy(&PacketBuffer[8], data, len);

	int slen = sendto(MPSocket, (const char*) PacketBuffer, len + 8, 0, &MPSendAddr, sizeof(sockaddr_t));
	if (slen < 8) return 0;
	return slen - 8;
}

int MP_RecvPacket(u8* data, bool block)
{
	if (MPSocket < 0)
		return 0;

	fd_set fd;
	struct timeval tv;

	FD_ZERO(&fd);
	FD_SET(MPSocket, &fd);
	tv.tv_sec = 0;
	tv.tv_usec = block ? 5000 : 0;

	if (!select(MPSocket + 1, &fd, 0, 0, &tv))
	{
		return 0;
	}

	sockaddr_t fromAddr;
	socklen_t fromLen = sizeof(sockaddr_t);
	int rlen = recvfrom(MPSocket, (char*) PacketBuffer, 2048, 0, &fromAddr, &fromLen);
	if (rlen < 8 + 24)
	{
		return 0;
	}
	rlen -= 8;

	if (ntohl(*(u32*) &PacketBuffer[0]) != 0x4946494E)
	{
		return 0;
	}

	if (PacketBuffer[4] != NIFI_VER)
	{
		return 0;
	}

	if (ntohs(*(u16*) &PacketBuffer[6]) != rlen)
	{
		return 0;
	}

	memcpy(data, &PacketBuffer[8], rlen);
	return rlen;
}


bool LAN_Init()
{
	/*if (Config::DirectLAN)
	{
		if (!LAN_PCap::Init(true))
			return false;
	}
	else
	{
		if (!LAN_Socket::Init())
			return false;
	}*/

	return true;
}

void LAN_DeInit()
{
	// checkme. blarg
	//if (Config::DirectLAN)
	//    LAN_PCap::DeInit();
	//else
	//    LAN_Socket::DeInit();
	/*LAN_PCap::DeInit();
	LAN_Socket::DeInit();*/
}

int LAN_SendPacket(u8* data, int len)
{
	/*if (Config::DirectLAN)
		return LAN_PCap::SendPacket(data, len);
	else
		return LAN_Socket::SendPacket(data, len);*/
	return 0;
}

int LAN_RecvPacket(u8* data)
{
	/*if (Config::DirectLAN)
		return LAN_PCap::RecvPacket(data);
	else
		return LAN_Socket::RecvPacket(data);*/
	return 0;
}


}
