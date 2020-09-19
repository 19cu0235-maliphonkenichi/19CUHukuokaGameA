//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�F�R�C���̑�����s��
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/09/17 �n粗��� �쐬
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class GAME_API ACoinItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoinItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// �I�[�o�[���b�v���ɌĂ΂��
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 Otherbodyindex, bool bFromSweep, const FHitResult& sweepResult);

public:
	// �R���W����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_pBoxComp;

	// ���b�V��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* m_pItemMesh;

protected:
	// ���[�g�R���|�[�l���g�p
	UPROPERTY()
		USceneComponent* m_pRoot;

private:
	// ���Z�X�R�A
	UPROPERTY(EditAnyWhere)
		int m_AddScore;

	// ���b�ŃR�C�����擾�ł��邩
	UPROPERTY(EditAnyWhere)
		float m_GetCoinTime;

	// �R�C���擾�p�^�C�}�[
	float m_CoinTimer;

	// Player��Actor
	AActor* m_playerActor;

	// ���Ƃ��Ƃ�Scale
	FVector m_OriginScale;
};
