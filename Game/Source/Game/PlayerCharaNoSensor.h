// �C���N���[�h�K�[�h
#pragma once

// �C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacter���p�����Ă��邽��
#include "Blueprint/UserWidget.h"
#include "PlayerCharaNoSensor.generated.h"

//	�O���錾
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API APlayerCharaNoSensor : public ACharacter
{
	// UE4�̃I�u�W�F�N�g�N���X�ŕK���擪�ɏ����}�N��
	GENERATED_BODY()

public:
	// �R���X�g���N�^
	APlayerCharaNoSensor();

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

	//	�W�����v����
	void UpdateJump(float _deltaTime);

	//	�K�[�h����
	void UpdateGuard(float _deltaTime);
private:
	//	�y���̓o�C���h�z�J������]:Pitch�iY���j
	void Cam_RotatePitch(float _axisValue);
	//	�y���̓o�C���h�z�J������]:Yaw�iZ���j
	void Cam_RotateYaw(float _axisValue);

	//	�y���̓o�C���h�z�L�����ړ�:�O��
	void Chara_MoveForward(float _axisValue);
	//	�y���̓o�C���h�z�L�����ړ�:���E
	void Chara_MoveRight(float _axisValue);

	//	�y���̓o�C���h�z�W�����v�J�n
	void JumpStart();

	//	�y���̓o�C���h�z�K�[�h�J�n
	void GuardStart(float _axisValue);
private:
	//	UPROPERTY�ɂ��邱�ƂŁA�u���[�v�����g��ŕϐ��̊m�F�A�ҏW�Ȃǂ��ł���
	//	�uBlueprintReadOnly�v�Ɏw�肵�Ă��邽�߁A�u���[�v�����g�Ō��邱�Ƃ����\�ŁA�ҏW�͂ł��Ȃ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;			//	�X�v�����O�A�[��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;				//	�J����

	FVector2D m_charaMoveInput;						//	Pawn�ړ����͗�
	FVector2D m_cameraRotateInput;					//	�J������]���͗�
	FVector2D m_charaRotateInput;

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	�J�����̃s�b�`�͈�

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_moveSpeed;							//	�ړ���

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	�d��

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_jumpPower;							//	�W�����v��

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> PlayerGuardUIClass;

	UUserWidget* PlayerGuardUI;

	float m_jumpTime;								//	�W�����v����
	float m_nowJumpHeight;							//	���݃t���[���̃W�����v��
	float m_prevJumpHeight;							//	�O�t���[���̃W�����v��

	bool m_bJumping;								//	�W�����v���t���O
	FVector m_posBeforeJump;						//	�W�������J�n�O�̃L�����N�^�[���W

	bool m_bGuarding;								//	�K�[�h���t���O
	bool m_bCanGuard;
	float m_GuardRechargeTime;
	float m_GuardCostTime;

	float m_bDashing;
	float m_bCanDash;
	float m_DashRechargeTime;
	float m_DashCostTime;

	bool m_bCanControl;								//	����\�ȏ�Ԃ�?

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_GuardValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_DashValue;
};