//----------------------------------------------------------
// ファイル名		：SensorTest.cpp
// 概要				：Arduinoからセンサーの情報を読み取り、ActorのRotatorに反映させる
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/08/31 渡邊龍音 作成
//					：2020/09/17 渡邊龍音 クラスとして使いやすいように・デッドゾーンの追加
//					：2020/10/07 渡邊龍音 センサーのシリアルポートを自動検出できる
//----------------------------------------------------------

#pragma once

#include "SensorTest.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include "Engine.h"
#include <time.h>

// prevRotatorの要素数を指定
// 大きいほどなめらかに移動しますが、処理負荷が増え、値の判定もゆっくりになります
#define ROTATOR_ARRAY_SIZE 1

#define SENSOR_ERROR FRotator(999.9f, 999.9f, 999.9f)

ASensorTest::ASensorTest()
	: m_pArduinoSerial(NULL)
	, serialPort(4)
	, isOpen(false)
	, isAutoSerialPort(true)
	, deadZone(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// 回転量の保存用配列の初期化
	prevRotator.Reset();
}

void ASensorTest::BeginPlay()
{
	Super::BeginPlay();
	
	// (10/07 渡邊)-------------------------------------------------------
	// シリアルポート自動検出
	if (isAutoSerialPort)
	{
		for (int port = 0; port < 10; ++port)
		{
			m_pArduinoSerial = USerial::OpenComPort(isOpen, port, 115200);

			if (isOpen == true && m_pArduinoSerial != NULL)
			{
				serialPort = port;
				break;
			}
		}
	}
	else
	{
		// 指定したシリアルポートを開ける
		m_pArduinoSerial = USerial::OpenComPort(isOpen, serialPort, 115200);
	}
	//--------------------------------------------------------------------

	if (isOpen == false)
	{
		// (10/07 渡邊)-------------------------------------------------------
		if (isAutoSerialPort)
		{
			UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): Could not COM Port open automatically. Try the manual setting."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): COM Port:%d is failed open. Please check the connection and COM Port number."), serialPort);
		}
		//--------------------------------------------------------------------
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ASensorTest::BeginPlay(): COM Port:%d is Successfully Open."), serialPort);
	}

	// 10回分のデータを入れる
	int errorCount = 0;
	for (int i = 0; i < ROTATOR_ARRAY_SIZE; ++i)
	{
		FRotator rotTemp;
		rotTemp = SensorToRotator();

		// センサーの値が読み取れていなければやり直し
		if (rotTemp == SENSOR_ERROR)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::BeginPlay(): Failed Read."));
			++errorCount;
			if (errorCount >= 10)
			{
				UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): Failed to read the sensor more than 10 times. Please check the connection."));
				break;
			}
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::BeginPlay(): SuccessFully Read."));
		}

		prevRotator.Add(rotTemp);
	}
}

void ASensorTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ASensorTest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_pArduinoSerial != NULL)
	{
		m_pArduinoSerial->Close();
		m_pArduinoSerial = NULL;
	}

	// 回転量の保存用配列の初期化
	prevRotator.Reset();
}

FRotator ASensorTest::SensorToRotator()
{
	bool isRead = false;		// データを読み取れたか？
	FString fStr;				// 読み取りデータ格納用
	int tryCnt = 0;				// 読み取ろうとした回数
	const int tryCntMax = 500;	// 最大の読み取る回数

	// シリアルのオブジェクトがあれば
	if (m_pArduinoSerial != NULL)
	{
		// データの読み取り
		// データが読み取れるか、最大読み取り回数になるまで繰り返す
		do
		{
			m_pArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_pArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < tryCntMax);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("ASensorTest::SensorToRotator(): Try Read Count: %d / %d"), tryCnt, tryCntMax);

		// 読み取れなかったらSENSOR_ERRORを返して終了
		if (isRead == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): No Data From Sensor. return ZeroRotator."));
			return SENSOR_ERROR;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Get Data From Sensor."));
		}

		// センサーデータをカンマ区切りでsplitTextArrayに入れる
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		// それぞれをint型に変換する
		TArray<float> rotatorAxis;
		rotatorAxis.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			rotatorAxis.Add(FCString::Atof(*splitTextArray[i]));
		}

		// Roll(X), Pitch(Y), Yaw(Z)の要素（3個分）読み取れていなければZeroRotatorを返す
		if (rotatorAxis.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): Failed Add TArray<float> elements. return ZeroRotator."));
			return SENSOR_ERROR;
		}

		UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Rotator Roll:%f Pitch:%f Yaw:%f"), rotatorAxis[0], rotatorAxis[1], rotatorAxis[2]);

		// FRotator型の変数をfloat型を使用して初期化
		FRotator rot(rotatorAxis[1], rotatorAxis[2], -rotatorAxis[0]);

		// デッドゾーン以上の移動があるか？
		for (int i = 0; i < rotatorAxis.Num(); ++i)
		{
			// デッドゾーンより入力が少なければZeroRotatorを返す
			if (rotatorAxis[i] < deadZone && rotatorAxis[i] > -deadZone)
			{
				return FRotator::ZeroRotator;
			}
		}

		return rot;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ASensorTest::SensorToRotator(): ASensorTest::m_pArduinoSerial is NULL."));
		return SENSOR_ERROR;
	}
}

void ASensorTest::SetActorRotationFromSensor(AActor* _setActor)
{
	// 回転量保存用

	float tempRoll = 0.0f;
	float tempPitch = 0.0f;
	float tempYaw = 0.0f;

	// 加算
	for (int i = 0; i < prevRotator.Num(); ++i)
	{
		tempRoll += prevRotator[i].Roll;
		tempPitch += prevRotator[i].Pitch;
		tempYaw += prevRotator[i].Yaw;
	}

	// 平均値を算出
	tempRoll /= prevRotator.Num();
	tempPitch /= prevRotator.Num();
	tempYaw /= prevRotator.Num();

	// Actorに回転量を反映
	FRotator rot(tempPitch, tempYaw, tempRoll);

	UE_LOG(LogTemp, Error, TEXT("Roll : %f Pitch : %f Yaw : %f"), (rot - prevDiffRot).Roll, (rot - prevDiffRot).Pitch, (rot - prevDiffRot).Yaw);

	float angle = 5.0f;
	if (FMath::Abs((rot - prevDiffRot).Roll) < angle && FMath::Abs((rot - prevDiffRot).Pitch) < angle && FMath::Abs((rot - prevDiffRot).Yaw) < angle)
	{
		rot = prevDiffRot;
	}

	_setActor->SetActorRotation(rot);

	// リストを更新
	if (prevRotator.IsValidIndex(0) == true)
	{
		// インデックス番号0の要素を削除
		prevRotator.RemoveAt(0);

		FRotator rotTemp = SensorToRotator();

		// センサーからの値がエラー値か判別
		if (rotTemp == SENSOR_ERROR)
		{
			// 現在の平均値を代入
			prevRotator.Add(rot);
		}
		else
		{
			// センサーからの値を代入
			prevRotator.Add(rotTemp);
		}
	}

	prevDiffRot = rot;
}

