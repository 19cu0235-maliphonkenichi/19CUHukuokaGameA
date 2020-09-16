//  �C���N���[�h�K�[�h
#pragma once

// �C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacter���p�����Ă��邽��
#include "PlayerChara.generated.h"

//	�O���錾
class USerial;
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:
	// ���t���[���̍X�V����
	virtual void Tick(float DeltaTime) override;

	// �Z���T�[�̒l��Rotator�ɕϊ�
	FRotator SensorToRotator();

private:
	//	�J�����X�V����
	void UpdateCamera(float _deltaTime);

	//	�ړ�����
	void UpdateMove(float _deltaTime);

	//	�W�����v����
	void UpdateJump(float _deltaTime);

	//	�K�[�h����
	void UpdateGuard();

	//	��������
	void UpdateAccelerate();
private:
	//	�y���̓o�C���h�z�W�����v�J�n
	void JumpStart();

	//	�y���̓o�C���h�z�K�[�h�J�n
	void GuardStart(float _axisValue);
private:
	// Arduino�̃V���A���ʐM�ۑ��p
	USerial* m_pArduinoSerial;

	// ��]�ʂ̕ۑ��i�Ȃ߂炩�Ɉړ�����悤�Ɂj
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor")
		int serialPort;

	//	UPROPERTY�ɂ��邱�ƂŁA�u���[�v�����g��ŕϐ��̊m�F�A�ҏW�Ȃǂ��ł���
	//	�uBlueprintReadOnly�v�Ɏw�肵�Ă��邽�߁A�u���[�v�����g�Ō��邱�Ƃ����\�ŁA�ҏW�͂ł��Ȃ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;			//	�X�v�����O�A�[��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;				//	�J����

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	�J�����̃s�b�`�͈�

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	�d��

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_jumpPower;							//	�W�����v��

	float m_jumpTime;								//	�W�����v����
	float m_nowJumpHeight;							//	���݃t���[���̃W�����v��
	float m_prevJumpHeight;							//	�O�t���[���̃W�����v��

	bool m_bJumping;								//	�W�����v���t���O
	FVector m_posBeforeJump;						//	�W�������J�n�O�̃L�����N�^�[���W

	bool m_bGuarding;								//	�K�[�h���t���O

	bool m_bAccelerate;

	bool m_bCanControl;								//	����\�ȏ�Ԃ�?
public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	float tempRoll;
	float tempPitch;
	float tempYaw;
};