// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include <string>
#include "Runtime/Networking/Public/Networking.h"
//#include "Runtime/Sockets/Public/Sockets.h"
//#include "Runtime/Sockets/Public/SocketSubsystem.h"
//#include "Runtime/Sockets/Public/IPAddress.h"
//#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "TcpHandler.generated.h"

/**
 * 
 */
UCLASS()
class EXPLOSIONSANDACTION_API UTcpHandler : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Connect to server */
	UFUNCTION(BlueprintCallable, Category = "LoginServ")
	static void TcpHandlerConnect();

	/** Send message to server */
	UFUNCTION(BlueprintCallable, Category = "LoginServ")
	static void TcpHandlerSendMsg(FString msg);

	/** Attempt login */
	UFUNCTION(BlueprintCallable, Category = "LoginServ")
	static void TcpHandlerAttemptLogin(FString username, FString password);

	/** Create New Account */
	UFUNCTION(BlueprintCallable, Category = "LoginServ")
	static void TcpHandlerCreateAccount(FString username, FString password);


	
private:
	static FSocket* _socket;
	static TSharedRef<FInternetAddr> _addr;
};
