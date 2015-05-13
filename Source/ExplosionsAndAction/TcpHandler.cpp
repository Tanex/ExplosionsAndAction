// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosionsAndAction.h"
#include "TcpHandler.h"

TSharedRef<FInternetAddr> UTcpHandler::_addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
FSocket* UTcpHandler::_socket = 0;

void UTcpHandler::TcpHandlerConnect()
{
	//get socket
	_socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	//prepare for connect
	FString address = TEXT("127.0.0.1");
	int32 port = 9059;
	FIPv4Address ip;
	FIPv4Address::Parse(address, ip);
	_addr->SetIp(ip.GetValue());
	_addr->SetPort(port);

	//connect
	bool connected = _socket->Connect(*_addr);
}

void UTcpHandler::TcpHandlerSendMsg(FString msg)
{
	TCHAR *serializedChar = msg.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	bool successful = _socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
}

void UTcpHandler::TcpHandlerAttemptLogin(FString username, FString password)
{
	//build login string
	FString loginMsg = FString(TEXT("Login:"));
	loginMsg += username;
	loginMsg += FString(TEXT(":"));
	loginMsg += password;

	//Send
	TcpHandlerSendMsg(loginMsg);
}

void UTcpHandler::TcpHandlerCreateAccount(FString username, FString password)
{
	//build login string
	FString loginMsg = FString(TEXT("CreateAccount:"));
	loginMsg += username;
	loginMsg += FString(TEXT(":"));
	loginMsg += password;

	//Send
	TcpHandlerSendMsg(loginMsg);
}
