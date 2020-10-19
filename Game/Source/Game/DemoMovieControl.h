//----------------------------------------------------------
// �t�@�C����		�FDemoMovieControl.h
// �T�v				�F�f�����[�r�[�𐶐�
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/09 �쐬 �f�����[�r�[�𐶐�
//					�F
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "DemoMovieControl.generated.h"

UCLASS()
class GAME_API ADemoMovieControl : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoMovieControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> DemoMovieUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		UUserWidget* DemoMovieUI;

	UFUNCTION()
		void OnLeftClicked();

	// The time to start to play the DemoMovie
	UPROPERTY(EditAnywhere, Category = "Duration")
		float duration;

private:
	FString currentLevelName;

	void CountDown(float _deltaTime);
	
};
