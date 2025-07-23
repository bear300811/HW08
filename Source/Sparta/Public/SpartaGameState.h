#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

USTRUCT(BlueprintType)
struct FLevelSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LevelDuration;
};

UCLASS()
class SPARTA_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
public:

	ASpartaGameState();

	virtual void BeginPlay() override;

	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FLevelSettings> LevelSettings;
	
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	
	void OnCoinCollected();
	void OnGameOver();
	
	void OnLevelTimeUp();
	void EndLevel();
	void StartLevel();

	void UpdateHUD();
};
