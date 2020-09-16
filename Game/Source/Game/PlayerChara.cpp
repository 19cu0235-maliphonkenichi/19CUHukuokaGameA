// �C���N���[�h
#include "PlayerChara.h"
#include "Engine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include <time.h>

//	define�}�N��
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (m_jumpTime * m_jumpTime * (-m_gravity) / 2) + (m_jumpTime * m_jumpPower)

// �R���X�g���N�^
APlayerChara::APlayerChara()
	: m_pArduinoSerial(NULL)
	, serialPort(4)
	, isOpen(false)
	, m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_gravity(600.f)
	, m_jumpPower(1800.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false) 
	, m_bGuarding(false)
	, m_bAccelerate(false)
	, m_bCanControl(true)
	, tempRoll(0.f)
	, tempPitch(0.f)
	, tempYaw(0.f)
{
	// ���t���[���A���̃N���X��Tick()���ĂԂ��ǂ��������߂�t���O�B�K�v�ɉ����āA�p�t�H�[�}���X����̂��߂ɐ؂邱�Ƃ��ł���B
	PrimaryActorTick.bCanEverTick = true;

	// ��]�ʂ̕ۑ��p�z��̏�����
	prevRotator.Reset();

	//	�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//	�X�v�����O�A�[���̃I�u�W�F�N�g�𐶐�
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL)
	{
		m_pSpringArm->SetupAttachment(RootComponent);

		//	�A�[���̒�����ݒ�
		//	�J�����̎q���W�����e�X�g���s������ݒ�
		m_pSpringArm->bDoCollisionTest = false;
		//	�J�����Ǐ]���O���g������ݒ�
		m_pSpringArm->bEnableCameraLag = true;
		//	�J�����Ǐ]���O�̑��x��ݒ�
		m_pSpringArm->CameraLagSpeed = 20.f;
		//	�J������]���O���g������ݒ�
		m_pSpringArm->bEnableCameraRotationLag = true;
		//	�J������]���O�̑��x��ݒ�
		m_pSpringArm->CameraRotationLagSpeed = 20.f;		
	}

	//	�J�����̃I�u�W�F�N�g�𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL)
	{
		//	�J�������X�v�����O�A�[���ɃA�^�b�`������
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

// �Q�[���X�^�[�g���A�܂��͐������ɌĂ΂�鏈��
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//	PlayerChara�������Ă��郁�b�V���R���|�[�l���g�̑��Έʒu��ύX
	USkeletalMeshComponent* pMeshComp = GetMesh();
	if (pMeshComp != NULL)
	{
		//	Z���W��������
		pMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	}

	UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
	if (pCharMoveComp != NULL)
	{
		//	�W�����v���ɂ����������ւ̈ړ��������悤�Ɂi0�`1�̊Ԃɐݒ肷�邱�Ƃňړ������𒲐��j
		pCharMoveComp->AirControl = 0.8f;
	}

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

// ���t���[���̍X�V����
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	�J�����X�V����
	UpdateCamera(DeltaTime);

	//	�ړ�����
	UpdateMove(DeltaTime);

	//	�W�����v����
	UpdateJump(DeltaTime);

	//	�K�[�h����
	UpdateGuard();

	UpdateAccelerate();

	tempRoll = 0.f;
	tempPitch = 0.f;
	tempYaw = 0.f;

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

	//GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Red, FString::FromInt(tempYaw));

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

//	�J�����X�V����
void APlayerChara::UpdateCamera(float _deltaTime)
{
	//	���[�g�I�u�W�F�N�g�𒆐S�ɁA�X�v�����O�A�[���ɂ��Ă���J��������]������
	USpringArmComponent* pSpringArm = m_pSpringArm;
	if (pSpringArm != NULL)
	{
		//	���݂�FPS�𑪒�
		float fps = 1.0f / _deltaTime;

		//	�����������Ă��A��葬�x�ŃJ���������悤�ɕ␳
		float rotateCorrection = 60.f / fps;
	}
}

//	�ړ�����
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	//	�O�Ɍ��������ƈړ�����
	if (m_bAccelerate)
	{
		NewLocation.X += 30.f;
	}
	else if (!m_bGuarding && !m_bAccelerate)
	{
		NewLocation.X += 20.f;
	}
	else
	{
		NewLocation.X += 12.f;
	}

	//	�L�����N�^�[�̈ړ�
	{
		//	SpringArm�����������ɁA���͂ɂ��ړ��ʂ�PawnMovementComponent�ɓn��
		NewLocation.Y += tempRoll * 0.2f;
		SetActorLocation(NewLocation);
	}
}

//	�W�����v����
void APlayerChara::UpdateJump(float _deltaTime)
{
	if(tempPitch > 45.f)
	{
		//	�W�����v�ʂ��v�Z
		m_nowJumpHeight = JUMP_HEIGHT;

		//	�W�����v���Ԃ𑝉�
		m_jumpTime += _deltaTime;

		//	Actor�̌��݂̍��W���擾
		FVector nowPos = GetActorLocation();

		//	���n���i=�W�����v�ʂ��}�C�i�X�ɓ]�������j�A�W�����v�O��Ԃɖ߂�
		if (m_nowJumpHeight < 0.0f)
		{
			m_bJumping = false;
			m_jumpTime = 0.0f;

			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z));
		}
		//	����ȊO�́A�W�����v���Ă��邽�ߍ��W�𔽉f
		else
		{
			//	���݂̍��W�ɃW�����v�ʂ𑫂�
			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z + m_nowJumpHeight), true);
		}

		//	�W�����v�ʂ�ێ�
		m_prevJumpHeight = m_nowJumpHeight;
	}
}

//	�K�[�h����
void APlayerChara::UpdateGuard()
{
		FRotator nowRot = GetActorRotation();
		if (tempYaw < -30.f)
		{
			m_bGuarding = true;
		}
		else
		{
			m_bGuarding = false;
		}
}

void APlayerChara::UpdateAccelerate()
{
	FRotator nowRot = GetActorRotation();
	if (tempPitch < -30.f || tempPitch > 30.f)
	{
		m_bAccelerate = true;
	}
	else
	{
		m_bAccelerate = false;
	}
}

void APlayerChara::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

FRotator APlayerChara::SensorToRotator()
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