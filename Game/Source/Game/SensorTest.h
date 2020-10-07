//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�FArduino����Z���T�[�̏���ǂݎ��AActor��Rotator�ɔ��f������
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/08/31 �n粗��� �쐬
//					�F2020/09/17 �n粗��� �N���X�Ƃ��Ďg���₷���悤�ɁE�f�b�h�]�[���̒ǉ�
//					�F2020/10/07 �n粗��� �Z���T�[�̃V���A���|�[�g���������o�ł���
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

private:
	// Arduino�̃V���A���ʐM�ۑ��p
	USerial* m_pArduinoSerial;

	// ��]�ʂ̕ۑ��i�Ȃ߂炩�Ɉړ�����悤�Ɂj
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	
protected:
	// �Z���T�[�̒l��Rotator�ɕϊ�
	FRotator SensorToRotator();

	// �Z���T�[�̒l��SetActorRotation�ɐݒ肷��
	void SetActorRotationFromSensor(AActor* _setActor);

public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	// (10/07 �n�)-------------------------------------------------------
	// Is Serialport Automatic detection
	UPROPERTY(EditAnywhere, Category = "Sensor Ports")
		bool isAutoSerialPort;
	
	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor Ports")
		int serialPort;
	//--------------------------------------------------------------------

	// Dead zone for sensor
	UPROPERTY(EditAnywhere, Category = "Sensor")
		float deadZone;

};