// Fill out your copyright notice in the Description page of Project Settings.


#include "world/MapTimeController.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h" 
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

AMapTimeController::AMapTimeController()
{
    PrimaryActorTick.bCanEverTick = true;

    TimeSpeed = 1.0f;
    CurrentTimeOfDay = 12.0f; // Midday
    CurrentWeather = EWeatherState::Clear;
    WeatherTransitionSpeed = 0.1f;

    SunLight = nullptr;
    MoonLight = nullptr;


    ExponentialHeightFog = nullptr;
    SkyLight = nullptr;
    SkyAtmosphere = nullptr;
    RainParticleSystem = nullptr;
    WindParticleSystem = nullptr;
}

void AMapTimeController::BeginPlay()
{
    Super::BeginPlay();

    InitializeLights();
}

void AMapTimeController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateTime(DeltaTime);
    UpdateLighting();
    TransitionWeather(DeltaTime);
}

void AMapTimeController::SetTimeSpeed(float NewSpeed)
{
    TimeSpeed = FMath::Max(NewSpeed, 0.1f); // Avoid negative or zero speed
}

void AMapTimeController::SetCurrentTime(float NewTime)
{
    CurrentTimeOfDay = FMath::Fmod(NewTime, 24.0f); // Clamp between 0-24
}

void AMapTimeController::ChangeWeather(EWeatherState NewWeather)
{
    CurrentWeather = NewWeather;
    UpdateWeather();
}



void AMapTimeController::UpdateWeather() {
    switch (CurrentWeather) {
    case EWeatherState::Clear:
        SetClearWeather();
        break;
    case EWeatherState::Cloudy:
        SetCloudyWeather();
        break;
    case EWeatherState::Rainy:
        SetRainyWeather();
        break;
    case EWeatherState::Windy:
        SetWindyWeather();
        break;
    default:
        SetClearWeather();
        break;
    }
}


void AMapTimeController::UpdateTime(float DeltaTime)
{
    CurrentTimeOfDay += TimeSpeed * DeltaTime;
    if (CurrentTimeOfDay >= 24.0f) CurrentTimeOfDay -= 24.0f; // Loop the clock
}

float AMapTimeController::GetLightIntensityBasedOnWeather(EWeatherState Weather, bool IsSun)
{
    switch (Weather)
    {
    case EWeatherState::Clear:
        return IsSun ? 10.0f : 0.10f; // Sol forte e Lua fraca
    case EWeatherState::Cloudy:
        return IsSun ? 5.0f : 0.050f; // Sol moderado e Lua fraca
    case EWeatherState::Rainy:
        return IsSun ? 0.50f : 0.025f; // Sol fraco e Lua fraca
    case EWeatherState::Windy:
        return IsSun ? 7.0f : 0.075f; // Sol moderado e Lua moderada
    default:
        return 0.0f; // Condição não especificada
    }
}


void AMapTimeController::UpdateLighting()
{
    if (!SunLight) return;

    float SunAngle = (CurrentTimeOfDay / 24.0f) * 360.0f - 90.0f;
    SunLight->SetActorRotation(FRotator(SunAngle+7.920133f, 1.275106f, 88.364398f));

    //float MoonAngle = SunAngle + 180.0f;
    //MoonLight->SetActorRotation(FRotator(MoonAngle, 0.0f, 0.0f));


    float SunIntensity = GetLightIntensityBasedOnWeather(CurrentWeather, true);/*
    float MoonIntensity = GetLightIntensityBasedOnWeather(CurrentWeather, false);*/

    UDirectionalLightComponent* SunLightComponent = Cast<UDirectionalLightComponent>(SunLight->GetLightComponent());
    /*UDirectionalLightComponent* MoonLightComponent = Cast<UDirectionalLightComponent>(MoonLight->GetLightComponent());*/
    if (SunLightComponent)
    {
        SunLightComponent->SetIntensity(SunIntensity);
    }
}

void AMapTimeController::TransitionWeather(float DeltaTime)
{
    // Implementation for transitioning weather conditions
    // This could include blending skybox materials, adjusting fog density, etc.
}

void AMapTimeController::InitializeLights()
{
    if (!SunLight)
    {
        SunLight = GetWorld()->SpawnActor<ADirectionalLight>();
        if (SunLight)
        {
            SunLight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f)); // Warm sunlight
            ULightComponent* SunLightComponent = SunLight->GetLightComponent();
            if (SunLightComponent)
            {
                SunLightComponent->SetMobility(EComponentMobility::Movable);
            }
        } // Warm sunlight
    }
    UpdateWeather();

}






void AMapTimeController::SetClearWeather()
{

    if (SkyAtmosphere)
    {
        SkyAtmosphere->SetRayleighScatteringScale(0.10f);
        SkyAtmosphere->SetMieScatteringScale(0.01f);
    }

    if (ExponentialHeightFog)
    {
        UExponentialHeightFogComponent* FogComponent = Cast<UExponentialHeightFogComponent>(ExponentialHeightFog->GetComponentByClass(UExponentialHeightFogComponent::StaticClass()));
        if (FogComponent)
        {
            FogComponent->SetFogDensity(0.005f);
            FogComponent->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 1.0f));
        }
    }

    if (SkyLight)
    {
        USkyLightComponent* SkyLightComponent = Cast<USkyLightComponent>(SkyLight->GetComponentByClass(USkyLightComponent::StaticClass()));
        if (SkyLightComponent)
        {
            SkyLightComponent->SetIntensity(1.5f);
            SkyLightComponent->RecaptureSky();
        }
    }

    // Desativar partículas existentes
    TArray<AActor*> ParticleActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEmitter::StaticClass(), ParticleActors);
    for (AActor* Actor : ParticleActors)
    {
        if (UParticleSystemComponent* ParticleComponent = Actor->FindComponentByClass<UParticleSystemComponent>())
        {
            ParticleComponent->DeactivateSystem();
        }
    }
}

void AMapTimeController::SetCloudyWeather()
{

    if (SkyAtmosphere)
    {
        SkyAtmosphere->SetRayleighScatteringScale(0.10f); // Ajuste de dispersão
        SkyAtmosphere->SetMieScatteringScale(0.01f);      // Ajuste de embaçamento
    }

    if (ExponentialHeightFog)
    {
        ExponentialHeightFog->GetComponent()->SetFogDensity(0.05f); // Neblina mais densa
        ExponentialHeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.5f, 0.5f, 0.6f)); // Cinza frio
    }

    if (SkyLight)
    {
        USkyLightComponent* SkyLightComponent = Cast<USkyLightComponent>(SkyLight->GetComponentByClass(USkyLightComponent::StaticClass()));
        if (SkyLightComponent){
            SkyLightComponent->SetIntensity(0.8f); // Luz ambiente reduzida
            SkyLightComponent->RecaptureSky(); // Atualizar reflexos
        }
    }

    // Opcional: Adicionar nuvens volumétricas ou efeitos de partículas
}

void AMapTimeController::SetRainyWeather()
{
    if (SkyAtmosphere)
    {
        UE_LOG(LogTemp, Warning, TEXT("Atmosphere system..."));
        SkyAtmosphere->SetRayleighScatteringScale(0.10f);
        SkyAtmosphere->SetMieScatteringScale(0.01f);
    }

    if (ExponentialHeightFog)
    {
        ExponentialHeightFog->GetComponent()->SetFogDensity(0.2f);
        ExponentialHeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.3f, 0.3f, 0.4f));
    }

    if (SkyLight)
    {
        USkyLightComponent* SkyLightComponent = Cast<USkyLightComponent>(SkyLight->GetComponentByClass(USkyLightComponent::StaticClass()));
        if (SkyLightComponent)
        {   
            SkyLightComponent->SetIntensity(0.5f); 
            SkyLightComponent->RecaptureSky(); 
        }
    }

    // Ativar partículas de chuva usando Niagara
    if (RainParticleSystem){
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
        if (PlayerPawn)
        {
            // Cria um componente de partículas de chuva e anexa ao jogador
            UNiagaraComponent* RainNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
                RainParticleSystem, 
                PlayerPawn->GetRootComponent(), 
                NAME_None, 
                FVector(0.0f, 0.0f, 200.0f), 
                FRotator::ZeroRotator, 
                EAttachLocation::KeepRelativeOffset, 
                true // Mantenha as partículas ao anexar
            );

            if (RainNiagaraComponent)
            {
                // Ajuste da intensidade das partículas com base na RainIntensity
                RainNiagaraComponent->SetFloatParameter(FName("RainIntensity"), RainIntensity);
            }
        }
    }
}

void AMapTimeController::SetWindyWeather()
{
    if (SkyAtmosphere)
    {
        SkyAtmosphere->SetRayleighScatteringScale(0.09f);
        SkyAtmosphere->SetMieScatteringScale(0.01f);
    }

    if (ExponentialHeightFog)
    {
        ExponentialHeightFog->GetComponent()->SetFogDensity(0.05f);
        ExponentialHeightFog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.6f, 0.6f, 0.7f));
    }

    if (SkyLight)
    {
        USkyLightComponent* SkyLightComponent = Cast<USkyLightComponent>(SkyLight->GetComponentByClass(USkyLightComponent::StaticClass()));
        if (SkyLightComponent)
        {
            SkyLightComponent->RecaptureSky();
        }
    }

    // Ativar partículas de vento usando Niagara
    if (WindParticleSystem)
    {
        UNiagaraComponent* WindNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WindParticleSystem, GetActorLocation(), FRotator::ZeroRotator, FVector(WindIntensity));
        if (WindNiagaraComponent)
        {
            // Ajuste da intensidade das partículas com base na WindIntensity
            WindNiagaraComponent->SetFloatParameter(FName("WindIntensity"), WindIntensity);
        }
    }
}
