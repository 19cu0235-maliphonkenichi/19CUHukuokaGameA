// �C���N���[�h�K�[�h
#pragma once

// �C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacter���p�����Ă��邽��
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "PlayerChara.generated.h"

//	�O���錾
class USerial;
class USpringArmComponent;
class UCameraComponent;
class AActor;

UENUM(BlueprintType)
enum class PPlayerAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

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
	void UpdateSensor(float _deltaTime);

	//	�ړ�����
	void UpdateMove(float _deltaTime);

	//	�W�����v����
	void UpdateJump(float _deltaTime);

	//	�K�[�h����
	void UpdateGuard();

	//	��������
	void UpdateAccelerate();

	//	���X�^�[�g
	void RestartGame();

	//���ˊJ�n
	void Shooting(float DeltaTime);

	//	���S�J�E���g
	void DeadCount();
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

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	�J�����̃s�b�`�͈�

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	�d��

	UPROPERTY(EditAnywhere, Category = "Jump")		//	�W�����v��
		float m_jumpPower;

	float m_jumpTime;								//	�W�����v����
	float m_nowJumpHeight;							//	���݃t���[���̃W�����v��
	float m_prevJumpHeight;							//	�O�t���[���̃W�����v��

	bool m_bCanJump;
	bool m_bJumping;								//	�W�����v���t���O
	FVector m_posBeforeJump;						//	�W�������J�n�O�̃L�����N�^�[���W

	float tempRotate;								//�@����Ԃɖ߂��̉�]�p�x

	bool m_bHaveGuardEnergy;

	// ���ˊԊu�J�E���g�_�E��
	float bulletTimeCount;

	float tempSpeed;
	bool m_bIsDamageOver;

	bool m_bDashing;
	bool m_bHaveDashEnergy;

	float m_bTempDamageFrame;
	bool m_bCanDamage;

	bool m_bDead;									//	���S�t���O

	bool m_bIsGoal;

	float tempRoll;
	float tempPitch;
	float tempYaw;
public:
	// Attacking type (�U���^�C�v)
	UPROPERTY(EditAnywhere, Category = "Attack")
		PPlayerAttackType playerATKType;

	// Bullet type (�e�̎g�p�^�C�v)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(���ˊԊu)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(�e�̐����ʒu�␳)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	HP��UI
		TSubclassOf<UUserWidget> Player_HP_Widget_Class;
	UUserWidget* Player_HP_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Score��UI
		TSubclassOf<UUserWidget> Player_Score_Widget_Class;
	UUserWidget* Player_Score_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Dead��UI
		TSubclassOf<UUserWidget> Player_Select_Widget_Class;
	UUserWidget* Player_Select_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Guard��UI
		TSubclassOf<UUserWidget> Player_Guard_Widget_Class;
	UUserWidget* Player_Guard_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Dash��UI
		TSubclassOf<UUserWidget> Player_Dash_Widget_Class;
	UUserWidget* Player_Dash_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Damage��UI
		TSubclassOf<UUserWidget> Player_Damage_Widget_Class;
	UUserWidget* Player_Damage_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Goal��UI
		TSubclassOf<UUserWidget> Player_Goal_Widget_Class;
	UUserWidget* Player_Goal_Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 selectPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float guardBulletUIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Guard_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dash_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GoalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CountShootEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float GuardEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)			//	CoinScore
		float CoinScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)			//	EnemyScore
		float EnemyScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float PlayerScore;								//	Player�l����Score

	UPROPERTY(EditAnywhere)
		float Fence_FilmDmg;
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool m_bGuarding;
};