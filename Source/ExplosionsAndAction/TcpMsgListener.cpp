// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosionsAndAction.h"
#include "TcpMsgListener.h"


// Sets default values
ATcpMsgListener::ATcpMsgListener()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATcpMsgListener::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATcpMsgListener::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATcpMsgListener::TcpMsgListenerStart()
{
	TArray<TSharedPtr<FInternetAddr>> localAdress;
	if (ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalAdapterAddresses(localAdress))
	{
		for (int32 i = 0; i < localAdress.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(i, 5.f, FColor::Red, localAdress[i]->ToString(false));
		}
	}
	//IP = 127.0.0.1, Port = 8890 for my Python test case
	if (!StartTcpReceiver("RamaSocketListener", localAdress[0]->ToString(false), 9058))
	{
		//UE_LOG  "TCP Socket Listener Created!"
		return;
	}

	//UE_LOG  "TCP Socket Listener Created! Yay!"
}

bool ATcpMsgListener::StartTcpReceiver(const FString& socketName, const FString& ip, const int32 port)
{
	//Rama's CreateTCPConnectionListener
	_listenerSocket = CreateTcpConnectionListener(socketName, ip, port);

	//Not created?
	if (!_listenerSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *ip, port));
		return false;
	}

	//Start the Listener! //thread this eventually
	GetWorldTimerManager().SetTimer(this, &ATcpMsgListener::TcpConnectionListener, 0.01, true);

	return true;
}

//Format IP String as Number Parts
//bool ATcpMsgListener::FormatIp4ToNumber(const FString& ip, uint8(&Out)[4])
//{
//	//IP Formatting
//	ip.Replace(TEXT(" "), TEXT(""));
//
//	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	//						   IP 4 Parts
//
//	//String Parts
//	TArray<FString> Parts;
//	ip.ParseIntoArray(&Parts, TEXT("."), true);
//	if (Parts.Num() != 4)
//		return false;
//
//	//String to Number Parts
//	for (int32 i = 0; i < 4; ++i)
//	{
//		Out[i] = FCString::Atoi(*Parts[i]);
//	}
//
//	return true;
//}

//Rama's Create TCP Connection Listener
FSocket* ATcpMsgListener::CreateTcpConnectionListener(const FString& socket, const FString& ip, const int32 port, const int32 receiveBufferSize)
{
	//uint8 IP4Nums[4];
	/*if (!FormatIP4ToNumber(TheIP, IP4Nums))
	{
	VShow("Invalid IP! Expecting 4 parts separated by .");
	return false;
	}*/

	FIPv4Address address;
	FIPv4Address::Parse(ip, address);
	FIPv4Endpoint endpoint(address, port);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create Socket
	//FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
	FSocket* listenSocket = FTcpSocketBuilder(*socket)
		.AsReusable()
		.BoundToEndpoint(endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	listenSocket->SetReceiveBufferSize(receiveBufferSize, NewSize);

	//Done!
	return listenSocket;
}

//Rama's TCP Connection Listener
void ATcpMsgListener::TcpConnectionListener()
{
	//~~~~~~~~~~~~~
	if (!_listenerSocket) return;
	//~~~~~~~~~~~~~

	//Remote address
	TSharedRef<FInternetAddr> remoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool pending;

	// handle incoming connections
	if (_listenerSocket->HasPendingConnection(pending) && pending)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//Already have a Connection? destroy previous
		if (_connectionSocket)
		{
			_connectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(_connectionSocket);
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//New Connection receive!
		_connectionSocket = _listenerSocket->Accept(*remoteAddress, TEXT("RamaTCP Received Socket Connection"));

		if (_connectionSocket != NULL)
		{
			//Global cache of current Remote Address
			_remoteAddressForConnection = FIPv4Endpoint(remoteAddress);

			//UE_LOG "Accepted Connection! WOOOHOOOO!!!";

			//can thread this too
			GetWorldTimerManager().SetTimer(this, &ATcpMsgListener::TcpSocketListener, 0.01, true);
		}
	}
}

//Rama's String From Binary Array
//This function requires 
//		#include <string>
FString ATcpMsgListener::StringFromBinaryArray(const TArray<uint8>& binaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(binaryArray.GetData()), binaryArray.Num());
	return FString(cstr.c_str());
}

//Rama's TCP Socket Listener
void ATcpMsgListener::TcpSocketListener()
{
	//~~~~~~~~~~~~~
	if (!_connectionSocket) return;
	//~~~~~~~~~~~~~


	//Binary Array!
	TArray<uint8> receivedData;

	uint32 size;
	while (_connectionSocket->HasPendingData(size))
	{
		receivedData.Init(FMath::Min(size, 65507u));

		int32 read = 0;
		_connectionSocket->Recv(receivedData.GetData(), receivedData.Num(), read);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (receivedData.Num() <= 0)
	{
		//No Data Received
		return;
	}

	//VShow("Total Data read!", receivedData.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), receivedData.Num()));


	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						Rama's String From Binary Array
	const FString receivedUE4String = StringFromBinaryArray(receivedData);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	if (receivedUE4String == TEXT("LoginSuccessful"))
	{
		receivedLoginSuccessful();
	}
	else if (receivedUE4String == TEXT("LoginRefused"))
	{
		receivedLoginRefused();
	}

	//VShow("As String!!!!! ~>", receivedUE4String);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *receivedUE4String));
}

void ATcpMsgListener::receivedLoginSuccessful_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Successful!")));
}

void ATcpMsgListener::receivedLoginRefused_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Refused!")));
}

void ATcpMsgListener::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (_listenerSocket)
	{
		_listenerSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(_listenerSocket);
	}

	if (_connectionSocket)
	{
		_connectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(_connectionSocket);
	}

	Super::EndPlay(EndPlayReason);
		
}