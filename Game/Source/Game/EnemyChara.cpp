#include "EnemyChara.h"
#include "DrawDebugHelpers.h"

AEnemyChara::AEnemyChara()
	: sinTime(0.0f)
	, originPosY(0.0f)
	, enemyType(EEnemyMoveType::None)
	, playerActor(NULL)
	, forwardSpeed(1.0f)
	, sinWaveSpeed(1.0f)
	, sinWaveFrequency(1.0f)
	, overtakeSpeed(2.0f)
	, overtakeOffset(0.0f)
	, overtakeDistance(300.0f)
	, overtakeYAxisMove(-100.0f)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyChara::BeginPlay()
{
	Super::BeginPlay();

	// �J�n���_��Y���W��ۑ�
	originPosY = GetActorLocation().Y; 
}

void AEnemyChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// ����Actor�̈ʒu�̍X�V
	FVector pos = GetActorLocation();

	// Player�Ƃ��Đݒ肵��Actor�̈ʒu�̍X�V�i�ݒ肵���ꍇ�̂݁j
	FVector playerPos = FVector::ZeroVector;
	if (playerActor != NULL)
	{
		playerPos = playerActor->GetActorLocation();
	}

	// �ړ��̎�ނɉ����ĕ���
	switch (enemyType)
	{
	// �����ړ��^�C�v
	case EEnemyMoveType::Line:

		Move_Line(forwardSpeed, pos);
		break;


	// �����g�ړ��^�C�v
	case EEnemyMoveType::Sin:

		Move_Sin(forwardSpeed, sinWaveSpeed, sinWaveFrequency, DeltaTime, pos);
		break;


	// �ǂ��z���ē����ɓ���^�C�v
	case EEnemyMoveType::Overtake_Line:
	case EEnemyMoveType::Overtake_Smooth:

		// Player���ݒ肳��Ă��Ȃ���΃G���[���O��\�����֐����I��
		if (playerActor == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("AEnemyChara::Tick(): playerActor is NULL. Need to set Actor."));
			return;
		}

		// �v���C���[�����ɂ���i���������ړ��j
		if (pos.X < (playerPos.X + overtakeOffset))
		{
			Move_Line(overtakeSpeed, pos);
		}
		// �v���C���[���O�ɂ���i�����ɓ���j
		else if (pos.X < (playerPos.X + overtakeOffset + overtakeDistance))
		{
			// �����ɓ��铮���������I�̏ꍇ
			if (enemyType == EEnemyMoveType::Overtake_Line)
			{
				Move_Overtake_Line(overtakeSpeed, forwardSpeed, pos, playerPos);
			}

			// �����ɓ��铮�����Ȃ߂炩�̏ꍇ
			else if (enemyType == EEnemyMoveType::Overtake_Smooth)
			{
				Move_Overtake_Lerp(overtakeSpeed, forwardSpeed, pos, playerPos);
			}
		}
		// ��������
		else
		{
			Move_Line(forwardSpeed, pos);
		}
		break;

	// �^�C�v�w��Ȃ�
	default:
		break;
	}
}

void AEnemyChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// �����ړ�����
void AEnemyChara::Move_Line(const float _speed, const FVector _pos)
{
	// X���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	FVector nextPos((_pos.X + _speed), _pos.Y, _pos.Z);

	SetActorLocation(nextPos);
}

// �����g�ړ�����
void AEnemyChara::Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos)
{
	// �o�ߎ��Ԃ��v�����Asin�̒l�Ɏg�p����
	// sin�̒l��Y���ɔ��f�����AX���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	sinTime += _deltaTime;

	FVector nextPos(_pos.X + _speedForward, ((_pos.Y + (FMath::Sin(sinTime * _speedSide) * _frequency)) / 2.0f), _pos.Z);

	SetActorLocation(nextPos);
}

// �ǂ��z���E�����ł̈ړ�
void AEnemyChara::Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// 0�Ŋ��邱�Ƃ�h��
	float t = 0.5f;
	if ((overtakeOffset + overtakeDistance) > 0)
	{
		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		t = (_ePos.X - _pPos.X - overtakeOffset) / overtakeDistance;
	}

	// �ړ����x�EY���̍��W��X���̈ړ��ʂɑ΂��Ă�����Ƃ��ψڂ�����
	float nextPosY = FMath::Lerp(originPosY, (originPosY + overtakeYAxisMove), t);
	float speed = FMath::Lerp(_overtakeSpeed, _speed, t);

	// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
	FVector nextPos(_ePos.X + speed, nextPosY, _ePos.Z);

	// �ʒu�̍X�V
	SetActorLocation(nextPos);
}

// �ǂ��z���E�Ȃ߂炩�Ȉړ�
void AEnemyChara::Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// �ړ����x
	float speed = 0.0f;

	// Y���̈ړ���̍��W
	float nextPosY = 0.0f;

	// �ړ���̍��W
	FVector nextPos = FVector::ZeroVector;

	// �Ȃ߂炩�Ɉړ����邽�߂̊J�n�_�A�ڕW�_�A2�̒��ԓ_�̕ۑ��p
	FVector point[3];

	// �����ɓ���؂邽�߂̋����iovertakeDistance�j�̔������O�ɂ����
	if (_ePos.X < (_pPos.X + overtakeOffset + (overtakeDistance / 2.0f)))
	{
		// 3�_�̕ۑ�������
		point[0] = FVector(_pPos.X + overtakeOffset								, originPosY								, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY								, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + (overtakeYAxisMove / 2.0f)	, _ePos.Z);

		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		float t = (_ePos.X - _pPos.X - overtakeOffset) / (overtakeDistance / 2.0f);

		// �Ȃ߂炩�Ɉړ��������̍��W�́AY���W�݂̂�ۑ�
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// �ړ����x�̕��
		speed = FMath::Lerp(_overtakeSpeed, _speed, t);

		// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}
	else
	{
		// 3�_�̕ۑ�
		point[0] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + (overtakeYAxisMove / 2.0f)	, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + overtakeYAxisMove			, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + overtakeDistance			, originPosY + overtakeYAxisMove			, _ePos.Z);

		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		float t = (_ePos.X - _pPos.X - overtakeOffset - (overtakeDistance / 2.0f)) / (overtakeDistance / 2.0f);

		// �Ȃ߂炩�Ɉړ��������̍��W�́AY���W�݂̂�ۑ�
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// �ړ����x�̕��
		speed = FMath::Lerp(_overtakeSpeed, _speed, (t * 2.0f));

		// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}

	// �ʒu�̍X�V
	SetActorLocation(nextPos);
}

// �Ȃ߂炩�Ȉړ��̍ۂɎg�������i3�_�̍��W����Ȃ߂炩�ȋȐ���`���j
FVector AEnemyChara::CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
								, float* _posX /* = NULL */, float* _posY /* = NULL */, float* _posZ /* = NULL */)
{
	FVector lerpPos[3];

	// ��Ԃ̒l��0���� ���� 1���傫���l�ɂ��Ȃ�
	float t = FMath::Clamp(_interpolation, 0.0f, 1.0f);

	// �J�n�_���璆�ԓ_�ia�j�A���ԓ_����I���_�ib�j�����Ԃ����l��ۑ�
	lerpPos[0] = FMath::Lerp(_startPos,	 _midPos,	 _interpolation);
	lerpPos[1] = FMath::Lerp(_midPos,	 _endPos,	 _interpolation);

	// ��La, b�����Ԃ����l��ۑ��i�Ȃ߂炩�Ɉړ�����ۂ̍��W�ɂȂ�j
	lerpPos[2] = FMath::Lerp(lerpPos[0], lerpPos[1], _interpolation);

	// ���ꂼ��̃|�C���^�[�ɃA�h���X���i�[����Ă���ΑΉ������l����
	if (_posX != NULL)
	{
		*_posX = lerpPos[2].X;
	}
	if (_posY != NULL)
	{
		*_posY = lerpPos[2].Y;
	}
	if (_posZ != NULL)
	{
		*_posZ = lerpPos[2].Z;
	}

	return lerpPos[2];
}

