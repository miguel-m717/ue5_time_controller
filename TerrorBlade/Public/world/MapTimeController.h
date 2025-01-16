// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "MapTimeController.generated.h"

UENUM(BlueprintType)
enum class EWeatherState : uint8
{
	Clear UMETA(DisplayName = "Clear"),
	Cloudy UMETA(DisplayName = "Cloudy"),
	Rainy UMETA(DisplayName = "Rainy"),
	Windy UMETA(DisplayName = "Windy")
};


UCLASS()
class TERRORBLADE_API AMapTimeController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapTimeController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Time properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeSpeed; // Multiplier for time progression

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float CurrentTimeOfDay; // 0-24 range for hours



    // Weather state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    EWeatherState CurrentWeather;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WeatherTransitionSpeed;






    // Sun and moon lights
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    ADirectionalLight* SunLight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    ADirectionalLight* MoonLight;

    




    // atimosfera
    /** Exponential Height Fog */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    AExponentialHeightFog* ExponentialHeightFog;

    /** Sky Light */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    ASkyLight* SkyLight;

    /** Sky Atmosphere */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    USkyAtmosphereComponent* SkyAtmosphere;

    /** Partículas de chuva */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    UNiagaraSystem* RainParticleSystem;

    /** Partículas de vento */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    UNiagaraSystem* WindParticleSystem;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float RainIntensity;  // Controla a intensidade da chuva

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WindIntensity;  // Controla a intensidade da chuva




    // Functions
    UFUNCTION(BlueprintCallable, Category = "Time")
    void SetTimeSpeed(float NewSpeed);

    UFUNCTION(BlueprintCallable, Category = "Time")
    void SetCurrentTime(float NewTime);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void ChangeWeather(EWeatherState NewWeather);







private:
    void UpdateTime(float DeltaTime);
    float GetLightIntensityBasedOnWeather(EWeatherState Weather, bool IsSun);
    void UpdateLighting();
    void TransitionWeather(float DeltaTime);
    void InitializeLights();
    void SetClearWeather();
    void SetCloudyWeather();
    void SetRainyWeather();
    void SetWindyWeather();
    void UpdateWeather();
};
