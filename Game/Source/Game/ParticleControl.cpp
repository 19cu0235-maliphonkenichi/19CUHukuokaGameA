#include "ParticleControl.h"
#include <time.h>
#include <stdlib.h>

AParticleControl::AParticleControl()
	: m_pVolt(NULL)
	, m_MoveSpeed(100.0f)
	, m_FrequencyMin(0.0f)
	, m_FrequencyMax(100.0f)
	, m_FrequencyTime(0.5f)
	, m_originPos(FVector::ZeroVector)
	, m_timer(0.0f)
	, m_isDirectionUp(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AParticleControl::BeginPlay()
{
	// �U�ꕝ�̐ݒ�`�F�b�N
	if (m_FrequencyMin < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("m_FrequencyMin more less 0 (value = %f). set value 0."), m_FrequencyMin);
		m_FrequencyMin = 0.0f;
	}

	if (m_FrequencyMax < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("m_FrequencyMax more less 0 (value = %f). set value 0."), m_FrequencyMax);
		m_FrequencyMax = 0.0f;
	}

	if (m_FrequencyMax < m_FrequencyMin)
	{
		UE_LOG(LogTemp, Warning, TEXT("m_FrequencyMax more less m_FrequencyMin (Max = %f, Min = %f). set value m_FrequencyMin."), m_FrequencyMax, m_FrequencyMin);
		m_FrequencyMax = m_FrequencyMin;
	}

	// �ʒu�ۑ�
	if (m_pVolt != NULL)
	{
		m_originPos = m_pVolt->GetActorLocation();
	}
	
	// �����V�[�h�l�̌���
	srand(time(NULL));

	Super::BeginPlay();
}

void AParticleControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveParticle(DeltaTime);
}

void AParticleControl::MoveParticle(float _deltaTime)
{
	if (m_pVolt == NULL)
	{
		return;
	}

	static float newPosZ;
	static float targetPosZ;

	m_timer += _deltaTime;

	// ��Ɍ������Ĉړ�
	if (m_isDirectionUp)
	{
		if (m_timer <= _deltaTime)
		{
			targetPosZ = m_originPos.Z + (float)(rand() % (int)(m_FrequencyMax - m_FrequencyMin) + m_FrequencyMin + 1);
		}
	}
	// ���Ɍ������Ĉړ�
	else
	{
		if (m_timer <= _deltaTime)
		{
			targetPosZ = m_originPos.Z - (float)(rand() % (int)(m_FrequencyMax - m_FrequencyMin) + m_FrequencyMin + 1);
		}
	}

	if (m_timer < (m_FrequencyTime / 2.0f))
	{
		newPosZ = FMath::Lerp(m_originPos.Z, targetPosZ, m_timer / (m_FrequencyTime / 2.0f));
	}
	else
	{
		newPosZ = FMath::Lerp(targetPosZ, m_originPos.Z, (m_timer - (m_FrequencyTime / 2.0f)) / (m_FrequencyTime / 2.0f));
	}

	if (m_timer >= m_FrequencyTime)
	{
		m_isDirectionUp = !m_isDirectionUp;
		m_timer = 0.0f;
	}

	// �ʒu�̔��f
	FVector newPos(m_pVolt->GetActorLocation().X + m_MoveSpeed, m_originPos.Y, newPosZ);
	m_pVolt->SetActorLocation(newPos);
}