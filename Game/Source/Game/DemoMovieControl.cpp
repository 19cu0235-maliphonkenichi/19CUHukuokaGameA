//----------------------------------------------------------
// �t�@�C����		�FDemoMovieControl.cpp
// �T�v				�F�f�����[�r�[�𐶐�
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/09 �쐬 �f�����[�r�[�𐶐�
//					�F
//----------------------------------------------------------

#include "DemoMovieControl.h"

// Sets default values
ADemoMovieControl::ADemoMovieControl():
	currentLevelName(""),
	duration(5.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADemoMovieControl::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;	
}

// Called every frame
void ADemoMovieControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��莞�Ԍo�߂����UI��\��
	CountDown(DeltaTime);
}

// DemoMovieUI�̕\��
void ADemoMovieControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &ADemoMovieControl::OnLeftClicked);
}

void ADemoMovieControl::OnLeftClicked()
{
	if (duration <= 0.0f) {
		DemoMovieUI->RemoveFromParent();
		DemoMovieUI = NULL;
	}
	duration = 5.0f;
}

void ADemoMovieControl::CountDown(float _deltaTime)
{
	currentLevelName = GetWorld()->GetName();

	if (duration > 0.0f)duration -= _deltaTime * 1.0f;
	else duration = 0.0f;	

	if (duration <= 0.0f && currentLevelName == "Title") {
		//UE_LOG(LogTemp, Warning, TEXT("duration:%s"), *(FString::SanitizeFloat(duration)));
		if (DemoMovieUIClass != nullptr && !DemoMovieUI) {
			DemoMovieUI = CreateWidget(GetWorld(), DemoMovieUIClass);
			DemoMovieUI->AddToViewport();
		}
	}
}

