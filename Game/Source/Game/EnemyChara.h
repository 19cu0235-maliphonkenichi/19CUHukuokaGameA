#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyChara.generated.h"

UENUM(BlueprintType)
enum class EEnemyMoveType : uint8
{
	Line			UMETA(DisplayName = "Line Move"),
	Sin				UMETA(DisplayName = "Sine Wave Move"),
	Overtake_Line	UMETA(DisplayName = "Overtake (Line)"),
	Overtake_Smooth	UMETA(DisplayName = "Overtake (Smooth)"),
	None			UMETA(DisplayName = "None Move"),
};

UCLASS()

class GAME_API AEnemyChara : public ACharacter
{
	GENERATED_BODY()
		
public:
	AEnemyChara();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float sinTime;
	float originPosY;

public:
	// �G�̈ړ��̎��
	UPROPERTY(EditAnywhere, Category = "Enemy Move Generic")
		EEnemyMoveType enemyType;
	
	// �����̈ړ��̑���
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Generic")
		float forwardSpeed;
	
	// �����g�ړ��̂Ƃ��̉��ړ��̑���
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Sine")
		float sinWaveSpeed;

	// �����g�ړ��̂Ƃ��̐U�ꕝ
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Sine")
		float sinWaveFrequency;

	// Player����
	UPROPERTY(EditAnywhere, Category = "Enemy Move Overtake")
		AActor* playerActor;

	// �ǂ������Ƃ��̑��x
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeSpeed;

	// ���ɓ���n�߂�I�t�Z�b�g�l
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeOffset;

	// �v���C���[�̈ʒu�ɗ��Ă�������ɓ���܂ł̋���
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeDistance;

	// �ǂ����������Ƃ�Y�����Ɉړ�����ʁi�����ɓ���ʁj
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeYAxisMove;

	// �����ړ�����
	//
	// _speed�F�ړ����x
	// _pos	 �F���݂̈ʒu
	void Move_Line(const float _speed, const FVector _pos);


	// �����g�ړ�����
	// 
	// _speedForward�F�O�ւ̈ړ����x
	// _speedSide	�F���ւ̈ړ����x
	// _frequency	�F���̐U�ꕝ
	// _deltaTime	�Fsin�̌v�Z�Ɏg�p����B�@Tick()��DeltaTime����
	// _pos			�F���݂̈ʒu
	void Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos);

	
	// �ǂ��z���E�����ł̈ړ�
	// �����ɓ��鎞�ɑ��x�����X�ɕ�ԁi_overtakeSpeed -> _speed�j���܂��B
	//
	// _overtakeSpeed�F�ǂ��z�����̃X�s�[�h
	// _speed		 �F�ǂ��z�����I��������̑��x
	// _ePos		 �F����Actor�̌��݂̈ʒu
	// _pPos		 �FPlayer�ɐݒ肵��Actor�̌��݂̈ʒu
	void Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// �ǂ��z���E�Ȃ߂炩�Ȉړ�
	// �����ɓ��鎞�ɑ��x�����X�ɕ�ԁi_overtakeSpeed -> _speed�j���܂��B
	//
	// _overtakeSpeed�F�ǂ��z�����̃X�s�[�h
	// _speed		 �F�ǂ��z�����I��������̑��x
	// _ePos		 �F����Actor�̌��݂̈ʒu
	// _pPos		 �FPlayer�ɐݒ肵��Actor�̌��݂̈ʒu
	void Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// �Ȃ߂炩�Ȉړ��̍ۂɎg�������i3�_�̍��W����Ȃ߂炩�ȋȐ���`���j
	//
	// _startPos		�F3�_�̂����A�J�n�n�_�ɓ�������W
	// _midPos			�F3�_�̂����A���Ԓn�_�ɓ�������W
	// _endPos			�F3�_�̂����A�ڕW�n�_�ɓ�������W
	// _interpolation	�F���ꂼ��̍��W��0�`1�Ƃ������̊���
	// _posX			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���X���W�݂̂�Ԃ�
	// _posY			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���Y���W�݂̂�Ԃ�
	// _posZ			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���Z���W�݂̂�Ԃ�
	FVector CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
						, float* _posX = NULL, float* _posY = NULL, float* _posZ = NULL);
};
