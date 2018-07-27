// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AP_PawnBase.generated.h"

class UProceduralMeshComponent;
class UPawnMovementComponent;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class PROJECT_API AP_PawnBase : public APawn
{
	GENERATED_BODY()
private:
    TArray<int> m_triangles;
    TArray<FVector> m_vertices;
    TArray<FVector> m_normals;
    TArray<FVector2D> m_uvmapping;
    float m_radius = 0.0;
    float m_masterVertex;

public:
    static FName CollisionComponentName;
    static FName MovementComponentName;
    static FName MeshComponentName;

private:
    UPROPERTY(Category = PPawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* MeshComponent;
    UPROPERTY(Category = PPawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPawnMovementComponent* MovementComponent;
    UPROPERTY(Category = PPawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

    bool hasSphereData();
    bool hasRadius();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "PPawn",meta=(BlueprintProtected = "true"))
    void ConstructSphereRunOnce();

    UFUNCTION(BlueprintCallable, Category = "PPawn",meta=(BlueprintProtected = "true"))
    void ConstructSphere();

    UFUNCTION(BlueprintCallable, Category = "PPawn",meta=(BlueprintProtected = "true"))
    void MakeMesh();

    UFUNCTION(BlueprintCallable, Category = "PPawn",meta=(BlueprintProtected = "true"))
    void SetRadius(float radius);

    UFUNCTION(BlueprintCallable, Category = "PPawn",meta=(BlueprintProtected = "true"))
    void SetMaterial(UMaterialInterface* material);
public: 
    // Sets default values for this pawn's properties
    AP_PawnBase();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
};
