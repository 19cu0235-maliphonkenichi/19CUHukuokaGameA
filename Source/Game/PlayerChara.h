//----------------------------------------------------------
// �t�@�C����		�FPlayerChara.h
// �T�v				�F�v���C���[�L�����𐧌䂷��Character�I�u�W�F�N�g
// �쐬��			�F19CU0220 ���@��
//----------------------------------------------------------

// �C���N���[�h�K�[�h
#pragma once

// �C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacter���p�����Ă��邽��
#include "PlayerChara.generated.h"

//	�O���錾
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API APlayerChara : public ACharacter
{
	// UE4�̃I�u�W�F�N�g�N���X�ŕK���擪�ɏ����}�N��
	GENERATED_BODY()

public:
	// �R���X�g���N�^
	APlayerChara();

protected:
	// �Q�[���X�^�[�g���A�܂��͐������ɌĂ΂�鏈��
	virtual void BeginPlay() override;

public:
	// ���t���[���̍X�V����
	virtual void Tick(float DeltaTime) override;

	// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//	�J�����X�V����
	void UpdateCamera(float _deltaTime);

	//	�ړ�����
	void UpdateMove(float _deltaTime);

	void UpdateRotate(float _deltaTime);

	//	�W�����v����
	void UpdateJump(float _deltaTime);

	void UpdateSpeedUp(float _deltaTime);


private:
	//	�y���̓o�C���h�z�L�����ړ�:���E
	void Chara_MoveRight(float _axisValue);

	void Chara_RotateRight(float _axisValue);

	//	�y���̓o�C���h�z�W�����v�J�n
	void JumpStart();

	void SpeedUpStart();
private:
	//	UPROPERTY�ɂ��邱�ƂŁA�u���[�v�����g��ŕϐ��̊m�F�A�ҏW�Ȃǂ��ł���
	//	�uBlueprintReadOnly�v�Ɏw�肵�Ă��邽�߁A�u���[�v�����g�Ō��邱�Ƃ����\�ŁA�ҏW�͂ł��Ȃ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;			//	�X�v�����O�A�[��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;				//	�J����

	FVector2D m_charaMoveInput;						//	Pawn�ړ����͗�
	FVector2D m_cameraRotateInput;					//	�J������]���͗�

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	�J�����̃s�b�`�͈�

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_moveSpeed;							//	�ړ���

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	�d��

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_jumpPower;							//	�W�����v��

	bool isRotate;

	float m_jumpTime;								//	�W�����v����
	float m_nowJumpHeight;							//	���݃t���[���̃W�����v��
	float m_prevJumpHeight;							//	�O�t���[���̃W�����v��

	bool m_bJumping;								//	�W�����v���t���O
	FVector m_posBeforeJump;						//	�W�������J�n�O�̃L�����N�^�[���W

	float m_speedUpTime;

	bool m_bSpeedUp;

	bool m_bCanControl;								//	����\�ȏ�Ԃ�?

	FRotator sen_Rotator;

	// =====================
public:
	UFUNCTION(BlueprintCallable, Category = "SensorMovement")
		void PassRotatorNum( FRotator rot1 );

};