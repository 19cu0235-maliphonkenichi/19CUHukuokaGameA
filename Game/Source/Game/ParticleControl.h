#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleControl.generated.h"

UCLASS()
class GAME_API AParticleControl : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticleControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// �p�[�e�B�N����Actor
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		AActor* m_pVolt;

	// �ړ����x
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_MoveSpeed;

	// �U�ꕝ�̍Œ�
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyMin;

	// �U�ꕝ�̍ō�
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyMax;

	// ���b�ň���g���邩
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyTime;

	// ���Ƃ��Ƃ̈ʒu
	FVector m_originPos;

	// ���Ԍv���p�^�C�}�[
	float m_timer;

	// �������������
	bool m_isDirectionUp;

private:
	// Actor�ړ�����
	void MoveParticle(float _deltaTime);
};
