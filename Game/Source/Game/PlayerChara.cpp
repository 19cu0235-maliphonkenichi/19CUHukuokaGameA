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
	, tempSpeed(0.f)
	, bulletTimeCount(0.0f)
	, bulletDuration(1.0f)
	, bulletXOffset(10.0f)
	, playerSpeed(10.f)
	, m_gravity(700.f)
	, m_jumpPower(1200.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bCanJump(false)
	, m_bJumping(false)
	, tempRotate(0.f)
	, m_bGuarding(false)
	, m_bDashing(false)
	, m_bTempDamageFrame(0.f)
	, m_bCanDamage(true)
	, m_bHaveGuardEnergy(true)
	, m_bHaveDashEnergy(true)
	, m_bDead(false)
	, m_bIsGoal(false)
	, m_bIsDamageOver(false)
	, isStart(false)
	, GoalTime(0.f)
	, HP(100.f)
	, CoinCount(0)
	, CountShootEnemy(0)
	, GuardEnergy(100.f)
	, DashEnergy(100.f)
	, guardBulletUIDownSpeed(10.f)
	, Guard_UIDownSpeed(0.5f)
	, Dash_UIDownSpeed(0.5f)
	, DamageFrame(50.f)
	, CoinScore(1000.f)
	, EnemyScore(2000.f)
	, PlayerScore(0.f)
	, Fence_FilmDmg(10.f)
	, selectPlay(0)
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
}

// �Q�[���X�^�[�g���A�܂��͐������ɌĂ΂�鏈��
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	m_bTempDamageFrame = DamageFrame;
	tempSpeed = playerSpeed;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OverlapEnds);;

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

	if (Player_HP_Widget_Class != nullptr)
	{
		Player_HP_Widget = CreateWidget(GetWorld(), Player_HP_Widget_Class);
		Player_HP_Widget->AddToViewport();
	}

	/*if (Player_Guard_Widget_Class != nullptr)
	{
		Player_Guard_Widget = CreateWidget(GetWorld(), Player_Guard_Widget_Class);
		Player_Guard_Widget->AddToViewport();
	}

	if (Player_Dash_Widget_Class != nullptr)
	{
		Player_Dash_Widget = CreateWidget(GetWorld(), Player_Dash_Widget_Class);
		Player_Dash_Widget->AddToViewport();
	}

	if (Player_Dash_Widget_Class != nullptr)
	{
		Player_Score_Widget = CreateWidget(GetWorld(), Player_Score_Widget_Class);
		Player_Score_Widget->AddToViewport();
	}*/
}

// ���t���[���̍X�V����
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DeltaTime));

	if (!m_bDead && !m_bIsGoal && isStart)
	{
		GoalTime += DeltaTime;

		//	�J�����X�V����
		UpdateSensor(DeltaTime);

		//	�ړ�����
		UpdateMove(DeltaTime);

		//	�W�����v����
		UpdateJump(DeltaTime);

		//	�K�[�h����
		UpdateGuard();

		UpdateAccelerate();

		Shooting(DeltaTime);

		if (!m_bCanDamage)
		{
			DamageFrame -= (DeltaTime * 60);

			if (DamageFrame <= 0.f)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DamageFrame));
				m_bCanDamage = true;
				Player_Damage_Widget->RemoveFromViewport();
				DamageFrame = m_bTempDamageFrame;
				m_bIsDamageOver = true;
			}
		}
	}

	if (selectPlay == 1)
	{
		RestartGame();
	}

	DeadCount();
}

//	�Z���T�[�̍X�V����
void APlayerChara::UpdateSensor(float _deltaTime)
{
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

	//	guardEnergy��0�ɂȂ�����A����]�̊p�x��������0�ɖ߂�
	if (!m_bHaveGuardEnergy)
	{
		if (tempRotate >= 85.f || tempRotate <= -85.f)
		{
			tempYaw = 0.f;

		}
		else if (tempRotate < 85.f && tempYaw < 0.f)
		{
			tempRotate += 2.f;
			tempYaw += tempRotate;
		}
		else if (tempRotate > -85.f && tempYaw > 0.f)
		{
			tempRotate -= 2.f;
			tempYaw += tempRotate;
		}
	}

	// Actor�ɉ�]�ʂ𔽉f
	if (m_bGuarding)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(tempRoll)); = tempPitch;
		tempPitch = 0.f;
	}
	else if (m_bDashing)
	{
		tempYaw = 0.f;
	}
	FRotator rot(tempPitch, tempYaw, tempRoll);

	float angle = 5.f;
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

//	�ړ�����
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	//	�O�Ɍ��������ƈړ�����
	if (m_bDashing)
	{
		NewLocation.X += playerSpeed * 1.15f;
	}
	else if (!m_bGuarding && !m_bDashing)
	{
		NewLocation.X += playerSpeed;
	}
	else if (m_bGuarding)
	{
		NewLocation.X += playerSpeed * 0.8f;
	}

	//	�L�����N�^�[��Y���ړ�
	{
		YRotation.Y = 0.f;
		NewLocation.Y += 0.2f * tempRoll;
		SetActorLocation(NewLocation);
	}

	if (m_bIsDamageOver)
	{
		if (playerSpeed >= tempSpeed)
		{
			m_bIsDamageOver = false;
			playerSpeed = tempSpeed;
		}
		else
		{
			playerSpeed += tempSpeed * 0.02f;
		}
	}
}

//	�W�����v����
void APlayerChara::UpdateJump(float _deltaTime)
{
	if (tempPitch > 45.f && !m_bJumping && m_bCanJump && !m_bGuarding)
	{
		m_bJumping = true;
		m_posBeforeJump = GetActorLocation();
	}

	if (m_bJumping)
	{
		//	�W�����v�ʂ��v�Z
		m_nowJumpHeight = JUMP_HEIGHT;

		FVector nowPos = GetActorLocation();

		m_jumpTime += _deltaTime;

		if (m_nowJumpHeight < 0.0f)
		{
			m_jumpTime = 0.f;
			m_bJumping = false;
			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z));
		}
		else
		{
			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z + m_nowJumpHeight), true);
		}

		m_prevJumpHeight = m_nowJumpHeight;
	}
}

//	�K�[�h����
void APlayerChara::UpdateGuard()
{
	if (GuardEnergy <= 0.f)
	{
		m_bGuarding = false;
		m_bHaveGuardEnergy = false;
		tempRotate = 0.f;
	}

	FRotator nowRot = GetActorRotation();
	if (tempYaw < -30.f || tempYaw > 30.f)
	{
		m_bGuarding = true;
	}
	else
	{
		m_bGuarding = false;
	}

	if (m_bGuarding && m_bHaveGuardEnergy)
	{
		GuardEnergy -= 0.05f;
		//GuardEnergy -= Guard_UIDownSpeed;
	}
	else
	{
		if (GuardEnergy <= 100.f)
		{
			GuardEnergy += Guard_UIDownSpeed;
		}
		else
		{
			m_bHaveGuardEnergy = true;
		}
	}
}

void APlayerChara::UpdateAccelerate()
{
	FRotator nowRot = GetActorRotation();
	if (tempPitch < -30.f && m_bHaveDashEnergy)
	{
		m_bDashing = true;
	}

	if (DashEnergy <= 0.f)
	{
		m_bDashing = false;
		m_bHaveDashEnergy = false;
	}

	if (m_bDashing && m_bHaveDashEnergy)
	{
		DashEnergy -= Dash_UIDownSpeed;
	}
	else
	{
		if (DashEnergy <= 100.f)
		{
			DashEnergy += Dash_UIDownSpeed;
		}
		else
		{
			m_bHaveDashEnergy = true;
		}
	}
}

void APlayerChara::RestartGame()
{
	FVector restartLocation = GetActorLocation();

	SetActorLocation(FVector(restartLocation.X - 3000.f, -10.f, 30.f));

	HP = 100.f;

	m_bDead = false;

	Player_Select_Widget->RemoveFromViewport();

	selectPlay = 0;
}

//���ˊJ�n
void APlayerChara::Shooting(float DeltaTime)
{
	bulletTimeCount += DeltaTime;

	FVector currentVector = GetActorLocation();
	if (bulletTimeCount >= bulletDuration) {
		// �e�̍쐬�FSpawnActor<AActor>(��������N���X�A�n�_���W�A�n�_��]���W)
		GetWorld()->SpawnActor<AActor>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, FRotator().ZeroRotator);
		bulletTimeCount = 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
	}
}

void APlayerChara::DeadCount()
{
	if (HP <= 0)
	{
		if (!m_bDead)
		{
			m_bDead = true;

			if (Player_Select_Widget_Class != nullptr)
			{
				Player_Select_Widget = CreateWidget(GetWorld(), Player_Select_Widget_Class);
				Player_Select_Widget->AddToViewport();
			}

			GetMesh()->SetSimulatePhysics(true);
		}
	}
}

void APlayerChara::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		m_bCanJump = true;
	}

	if (OtherActor->ActorHasTag("Fence_Film") && m_bCanDamage && !m_bDashing && !m_bGuarding)
	{
		if (Player_Damage_Widget_Class != nullptr)
		{
			Player_Damage_Widget = CreateWidget(GetWorld(), Player_Damage_Widget_Class);
			Player_Damage_Widget->AddToViewport();
		}

		playerSpeed *= 0.5f;
		m_bCanDamage = false;
		HP -= Fence_FilmDmg;
	}

	if (OtherActor->ActorHasTag("EnemyBullet") && m_bCanDamage && !m_bDashing)
	{
		if (!m_bGuarding)
		{
			if (Player_Damage_Widget_Class != nullptr)
			{
				Player_Damage_Widget = CreateWidget(GetWorld(), Player_Damage_Widget_Class);
				Player_Damage_Widget->AddToViewport();
			}
			playerSpeed *= 0.5f;
			m_bCanDamage = false;
			HP -= Fence_FilmDmg;
		}
		else
		{
			GuardEnergy -= guardBulletUIDownSpeed;
		}
	}

	if (OtherActor->ActorHasTag("Coin"))
	{
		PlayerScore += CoinScore;

		CoinCount += 1;
	}

	if (OtherActor->ActorHasTag("Goal"))
	{
		if (Player_Goal_Widget_Class != nullptr)
		{
			Player_Goal_Widget = CreateWidget(GetWorld(), Player_Goal_Widget_Class);
			Player_Goal_Widget->AddToViewport();
		}

		if (Player_HP_Widget_Class != nullptr)
		{
			Player_HP_Widget->RemoveFromViewport();
		}

		if (Player_Guard_Widget_Class != nullptr)
		{
			Player_Guard_Widget->RemoveFromViewport();
		}

		if (Player_Dash_Widget_Class != nullptr)
		{
			Player_Dash_Widget->RemoveFromViewport();
		}

		if (Player_Dash_Widget_Class != nullptr)
		{
			Player_Score_Widget->RemoveFromViewport();
		}

		m_bIsGoal = true;
	}
}

void APlayerChara::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		m_bCanJump = false;
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