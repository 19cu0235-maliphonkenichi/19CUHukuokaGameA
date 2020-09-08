//----------------------------------------------------------
// �t�@�C����		�FSensorTest.cpp
// �T�v				�FArduino����Z���T�[�̏���ǂݎ��AActor��Rotator�ɔ��f������
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/08/31 �n粗��� �쐬
//----------------------------------------------------------

#pragma once

#include "SensorTest.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include "Engine.h"
#include <time.h>

// prevRotator�̗v�f�����w��
// �傫���قǂȂ߂炩�Ɉړ����܂����A�������ׂ������A�l�̔�����������ɂȂ�܂�
#define ROTATOR_ARRAY_SIZE 1

ASensorTest::ASensorTest() :
	m_pArduinoSerial(NULL),
	serialPort(4),
	isOpen(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// ��]�ʂ̕ۑ��p�z��̏�����
	prevRotator.Reset();
}

void ASensorTest::BeginPlay()
{
	Super::BeginPlay();

	// �V���A���|�[�g���J����
	m_pArduinoSerial = USerial::OpenComPort(isOpen, serialPort, 115200);

	if (isOpen == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): COM Port:%d is failed open. Please check the connection and COM Port number."), serialPort);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ASensorTest::BeginPlay(): COM Port:%d is Successfully Open."), serialPort);
	}

	// 10�񕪂̃f�[�^������
	int errorCount = 0;
	for (int i = 0; i < ROTATOR_ARRAY_SIZE; ++i)
	{
		FRotator rotTemp;
		rotTemp = SensorToRotator();

		// �Z���T�[�̒l���ǂݎ��Ă��Ȃ���΂�蒼��
		if (rotTemp == FRotator::ZeroRotator)
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

	// ��]�ʕۑ��p

	float tempRoll = 0.0f;
	float tempPitch = 0.0f;
	float tempYaw = 0.0f;

	// ���Z
	for (int i = 0; i < prevRotator.Num(); ++i)
	{
		tempRoll += prevRotator[i].Roll;
		tempPitch += prevRotator[i].Pitch;
		tempYaw += prevRotator[i].Yaw;
	}

	// ���ϒl���Z�o
	tempRoll /= prevRotator.Num();
	tempPitch /= prevRotator.Num();
	tempYaw /= prevRotator.Num();

	// Actor�ɉ�]�ʂ𔽉f
	FRotator rot(tempPitch, tempYaw, tempRoll);

	UE_LOG(LogTemp, Error, TEXT("Roll : %f Pitch : %f Yaw : %f"), (rot - prevDiffRot).Roll, (rot - prevDiffRot).Pitch, (rot - prevDiffRot).Yaw);

	float angle = 5.0f;
	if (FMath::Abs((rot - prevDiffRot).Roll) < angle && FMath::Abs((rot - prevDiffRot).Pitch) < angle && FMath::Abs((rot - prevDiffRot).Yaw) < angle)
	{
		rot = prevDiffRot;
	}

	SetActorRotation(rot);

	// ���X�g���X�V
	if (prevRotator.IsValidIndex(0) == true)
	{
		// �C���f�b�N�X�ԍ�0�̗v�f���폜
		prevRotator.RemoveAt(0);

		FRotator rotTemp = SensorToRotator();

		// �Z���T�[����̒l�����S��0������
		if (rotTemp == FRotator::ZeroRotator)
		{
			// ���݂̕��ϒl����
			prevRotator.Add(rot);
		}
		else
		{
			// �Z���T�[����̒l����
			prevRotator.Add(rotTemp);
		}
	}

	prevDiffRot = rot;
}

void ASensorTest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_pArduinoSerial != NULL)
	{
		m_pArduinoSerial->Close();
		m_pArduinoSerial = NULL;
	}

	// ��]�ʂ̕ۑ��p�z��̏�����
	prevRotator.Reset();
}

FRotator ASensorTest::SensorToRotator()
{
	bool isRead = false;		// �f�[�^��ǂݎ�ꂽ���H
	FString fStr;				// �ǂݎ��f�[�^�i�[�p
	int tryCnt = 0;				// �ǂݎ�낤�Ƃ�����
	const int tryCntMax = 500;	// �ő�̓ǂݎ���

	// �V���A���̃I�u�W�F�N�g�������
	if (m_pArduinoSerial != NULL)
	{
		// �f�[�^�̓ǂݎ��
		// �f�[�^���ǂݎ��邩�A�ő�ǂݎ��񐔂ɂȂ�܂ŌJ��Ԃ�
		do
		{
			m_pArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_pArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < tryCntMax);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("ASensorTest::SensorToRotator(): Try Read Count: %d / %d"), tryCnt, tryCntMax);

		// �ǂݎ��Ȃ�������ZeroRotator��Ԃ��ďI��
		if (isRead == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): No Data From Sensor. return ZeroRotator."));
			return FRotator::ZeroRotator;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Get Data From Sensor."));
		}

		// �Z���T�[�f�[�^���J���}��؂��splitTextArray�ɓ����
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		// ���ꂼ���int�^�ɕϊ�����
		TArray<float> rotatorAxis;
		rotatorAxis.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			rotatorAxis.Add(FCString::Atof(*splitTextArray[i]));
		}

		// Roll(X), Pitch(Y), Yaw(Z)�̗v�f�i3���j�ǂݎ��Ă��Ȃ����ZeroRotator��Ԃ�
		if (rotatorAxis.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): Failed Add TArray<float> elements. return ZeroRotator."));
			return FRotator::ZeroRotator;
		}

		UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Rotator Roll:%f Pitch:%f Yaw:%f"), rotatorAxis[0], rotatorAxis[1], rotatorAxis[2]);

		// FRotator�^�̕ϐ���float�^���g�p���ď�����
		FRotator rot(rotatorAxis[1], rotatorAxis[2], -rotatorAxis[0]);

		return rot;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ASensorTest::SensorToRotator(): ASensorTest::m_pArduinoSerial is NULL."));
		return FRotator::ZeroRotator;
	}
}

