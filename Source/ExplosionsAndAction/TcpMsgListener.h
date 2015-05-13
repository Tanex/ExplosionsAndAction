// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include <string>
#include "Runtime/Networking/Public/Networking.h"
#include "TcpMsgListener.generated.h"

//DECLARE_DYNAMIC_DELEGATE(FTcpMsgListenerDelegate);

UCLASS()
class EXPLOSIONSANDACTION_API ATcpMsgListener : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATcpMsgListener();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Start TCP Receiver */
	UFUNCTION(BlueprintCallable, Category = "LoginServ")
	void TcpMsgListenerStart();

	/** Start TCP Receiver */
	bool StartTcpReceiver(const FString& socketName, const FString& ip, const int32 port);

	/** Create listener */
	FSocket* CreateTcpConnectionListener(const FString& socketName, const FString& ip, const int32 port, const int32 receiveBufferSize = 2 * 1024 * 1024);

	/** Connection Listener Thread */
	void TcpConnectionListener();

	/** Socket Listener Thread */
	void TcpSocketListener();

	//static bool FormatIp4ToNumber(const FString& ip, uint8(&out)[4]);

	/** Forms ue4 string from received bytes */
	FString StringFromBinaryArray(const TArray<uint8>& binaryArray);

	/** Called when Actor is destroyed */
	void EndPlay(const EEndPlayReason::Type EndPlayReason);

private:
	void receivedLoginSuccessful();
	void receivedLoginRefused();
	

	FSocket* _connectionSocket;
	FSocket* _listenerSocket;
	FIPv4Endpoint _remoteAddressForConnection;
};
