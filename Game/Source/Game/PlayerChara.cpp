//----------------------------------------------------------
// �t�@�C����		�FPlayerChara.cpp
// �T�v				�F�v���C���[�L�����𐧌䂷��Character�I�u�W�F�N�g
// �쐬��			�F19CU0220 ���@��
// �X�V���e			�F2020/08/07 �쐬
//----------------------------------------------------------

// �C���N���[�h
#include "PlayerChara.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

//	define�}�N��
#define JUMP_HEIGHT (m_jumpTime * m_jumpTime * (-m_gravity) / 2) + (m_jumpTime * m_jumpPower)

// �R���X�g���N�^
APlayerChara::APlayerChara()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_charaMoveInput(FVector2D::ZeroVector)
	, m_moveSpeed(50.f)
	, m_gravity(1200.f)
	, m_jumpPower(900.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false)
	, m_bCanControl(true)
{
	// ���t���[���A���̃N���X��Tick()���ĂԂ��ǂ��������߂�t���O�B�K�v�ɉ����āA�p�t�H�[�}���X����̂��߂ɐ؂邱�Ƃ��ł���B
	PrimaryActorTick.bCanEverTick = true;

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
}

// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	�ړ�
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::Chara_MoveRight);

	//	�W������
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerChara::JumpStart);
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

		//	�J�����̐V�����p�x�����߂�
		//	���݂̊p�x���擾
		FRotator NewRotation = pSpringArm->GetRelativeRotation();

		//	Yaw�͓��͂�������
		NewRotation.Yaw += m_cameraRotateInput.X * rotateCorrection;

		//	Pitch�Ɋւ��ẮA�㉺�̐����p�x�͈͓̔��Ő؂�
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + (m_cameraRotateInput.Y * rotateCorrection), m_cameraPitchLimit.X, m_cameraPitchLimit.Y);

		//	�V�����p�x�𔽉f
		pSpringArm->SetRelativeRotation(NewRotation);
	}
}

//	�ړ�����
void APlayerChara::UpdateMove(float _deltaTime)
{
	//	�O�Ɍ��������ƈړ�����
	FVector NewLocation = GetActorLocation();
	NewLocation.X += 20.f;
	SetActorLocation(NewLocation);

	//	�ړ����͂�����ꍇ
	if (!m_charaMoveInput.IsZero())
	{
		//	�R���g���[���\�̏ꍇ�̂�
		if (m_bCanControl == false) { return; }

		//	���͂ɍ��킹�āAActor�����E�O��Ɉړ�
		USpringArmComponent* pSpringArm = m_pSpringArm;
		if (pSpringArm != NULL)
		{
			//	�L�����N�^�[�̈ړ�
			{
				//	SpringArm�����������ɁA���͂ɂ��ړ��ʂ�PawnMovementComponent�ɓn��
				NewLocation.Y += (m_charaMoveInput.Y * m_moveSpeed);
				SetActorLocation(NewLocation);
			}
		}
	}
}

//	�W�����v����
void APlayerChara::UpdateJump(float _deltaTime)
{
	//	�W�����v���t���O���m�F���Ă���
	if (m_bJumping)
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

		////	�f�o�b�O�m�F�p
		//nowPos = GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Jump > PosZ %f / Height %f"), nowPos.Z, m_nowJumpHeight);
	}
}

//	�y���̓o�C���h�z�L�����ړ�:���E
void APlayerChara::Chara_MoveRight(float _axisValue)
{
	//	�R���g���[���\�̏ꍇ�̂�
	if (m_bCanControl == false) { return; }

	m_charaMoveInput.Y = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 0.5f;
}

void APlayerChara::JumpStart()
{
	//	�R���g���[���\�̏ꍇ�̂�
	if (m_bCanControl == false) { return; }

	//	�W�����v���ł͂Ȃ��ꍇ
	if (m_bJumping == false)
	{
		//	�W�����v����Ԃɂ���
		m_bJumping = true;

		//	�W�������O��Actor���W��ێ�
		m_posBeforeJump = GetActorLocation();
	}
}

