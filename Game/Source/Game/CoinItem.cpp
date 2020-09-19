//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�F�R�C���̑�����s��
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/09/17 �n粗��� �쐬
//----------------------------------------------------------

#include "CoinItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ACoinItem::ACoinItem()
	: m_AddScore(100)
	, m_GetCoinTime(5.0f)
	, m_CoinTimer(0.0f)
	, m_playerActor(NULL)
	, m_OriginScale(FVector::ZeroVector)
{
	PrimaryActorTick.bCanEverTick = true;

	// ���[�g�R���|�[�l���g����
	m_pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = m_pRoot;

	// �R���W�����p�{�b�N�X�R���|�[�l���g����
	m_pBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	m_pBoxComp->SetupAttachment(RootComponent);

	// ���b�V���R���|�[�l���g����
	m_pItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	m_pItemMesh->SetupAttachment(RootComponent);
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();	

	if (m_pBoxComp != NULL)
	{
		m_pBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACoinItem::OnOverlapBegin);
	}

	// Player�ƐG���܂ł͖��t���[����������K�v���Ȃ�
	PrimaryActorTick.SetTickFunctionEnable(false);

	// ���݂̃X�P�[����ۑ�
	m_OriginScale = GetActorScale();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_playerActor != NULL)
	{
		m_CoinTimer += DeltaTime;

		if (m_CoinTimer >= m_GetCoinTime)
		{
			this->Destroy();
		}

		// �擾���Ă���̎��Ԃ̊������v�Z
		float ratio = FMath::Clamp((m_CoinTimer / m_GetCoinTime), 0.0f, 1.0f);

		// �ʒu���X�V
		FVector newPos = FMath::Lerp(GetActorLocation(), m_playerActor->GetActorLocation(), ratio);

		// �T�C�Y�̍X�V
		FVector newScale = FMath::Lerp(m_OriginScale, FVector::ZeroVector, ratio);

		SetActorLocation(newPos);
		SetActorScale3D(newScale);
	}
}

void ACoinItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 Otherbodyindex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (OtherActor && (OtherActor != (AActor*)this) && OtherComp)
	{
		// Actor�̃^�O��Player�ł����
		if (OtherActor->ActorHasTag("Player"))
		{
			// �v���C���[��Actor��ۑ�
			m_playerActor = OtherActor;

			// Tick()�L����
			PrimaryActorTick.SetTickFunctionEnable(true);

			// �R���W����������
			this->SetActorEnableCollision(false);
		}
	}
}