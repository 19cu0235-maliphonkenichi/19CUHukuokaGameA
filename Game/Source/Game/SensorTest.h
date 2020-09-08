//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�FArduino����Z���T�[�̏���ǂݎ��AActor��Rotator�ɔ��f������
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/08/31 �n粗��� �쐬
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SensorTest.generated.h"

class USerial;

UCLASS()
class GAME_API ASensorTest : public AActor
{
	GENERATED_BODY()

public:
	ASensorTest();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

	// �Z���T�[�̒l��Rotator�ɕϊ�
	FRotator SensorToRotator();

private:
	// Arduino�̃V���A���ʐM�ۑ��p
	USerial* m_pArduinoSerial;

	// ��]�ʂ̕ۑ��i�Ȃ߂炩�Ɉړ�����悤�Ɂj
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor")
		int serialPort;

public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;
};