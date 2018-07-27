// Fill out your copyright notice in the Description page of Project Settings.

#include "P_PawnBase.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"

#include "Geometry/icosphere.h"
#include "core.h"
#include <mutex>

//global to file
icosphere unitsphere;
float epsilon = 0.000015f;

void construct_icosphere(){
    static const uint8 subdivisions = 9;
    if( unitsphere.get_vert_count() == 0 ){
        logInfoC(Geometry,DColor::Green,true,"Generating cached icosphere [run once](in a perfect world)");
        unitsphere.make_icosphere(subdivisions);
    }
    /*static std::once_flag constructed_flag;
    std::call_once(constructed_flag, [](){ 
        logInfoC(Geometry,DColor::Green,true,"Generating icosphere [run once]");
        unitsphere.make_icosphere(subdivisions);
    });*/
}


FName AP_PawnBase::CollisionComponentName(TEXT("PPawn_CollisionComponent"));
FName AP_PawnBase::MeshComponentName(TEXT("PPawn_MeshComponent"));
FName AP_PawnBase::MovementComponentName(TEXT("PPawn_MovementComponent"));

// Sets default values
AP_PawnBase::AP_PawnBase()
{
    LOGINIT(DColor::Green);
	PrimaryActorTick.bCanEverTick = true;
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
    NetPriority = 3.f;

    bCanBeDamaged = false;
    BaseEyeHeight = 0.f;
    bCollideWhenPlacing = true;
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(AP_PawnBase::CollisionComponentName);
	CollisionComponent->InitSphereRadius(40.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->bShouldUpdatePhysicsVolume = true;
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;

	RootComponent = CollisionComponent;

    MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(AP_PawnBase::MovementComponentName);
    MovementComponent->UpdatedComponent = CollisionComponent;

    MeshComponent = CreateOptionalDefaultSubobject<UProceduralMeshComponent>( AP_PawnBase::MeshComponentName );
    /*MeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules(EAttachmentRule::KeepRelative,true));*/
    if( MeshComponent ){
        MeshComponent->SetupAttachment(RootComponent);
        MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    }
}

// Called when the game starts or when spawned
void AP_PawnBase::BeginPlay()
{
    Super::BeginPlay();
    LOGINIT(DColor::Green);
}

bool AP_PawnBase::hasSphereData(){
    if( m_vertices.Num() == 0 ){
        logVerboseC(Geometry,DColor::Purple,false,"\nVertex count: %d",m_vertices.Num());
        return false;
    }
    auto &v = m_vertices[m_vertices.Num() / 2];
    logVerboseC(Geometry,DColor::Purple,false,"\nVertex count: %d\nVertex length: %f",m_vertices.Num(),v.Size());
    if( m_vertices.Num() >= 20 ){
        if( !v.IsNearlyZero() ){
            return true;
        }
    }
    return false;
}

bool AP_PawnBase::hasRadius(){
    if( hasSphereData() ){
        auto &v = m_vertices[m_vertices.Num() / 2];
        float radius = v.Size();
        logVerboseC(Geometry,DColor::Purple,false,"\nExpected radius: %f\nActual radius: %f",m_radius,radius);
        if( FMath::IsNearlyEqual(m_radius,radius,epsilon) ){
            return true;
        }
        logWarning(Geometry,"Radius is invalid. {expected: %f, actual: %f}",m_radius,radius);
    }
    return false;
}

void AP_PawnBase::ConstructSphereRunOnce(){
    if ( !hasSphereData() ) {
        construct_icosphere();
        m_triangles = TArray<int>( unitsphere.get_triangles_raw(), unitsphere.get_index_count() );
        m_normals = TArray<FVector>( unitsphere.get_vertices() );
        m_vertices = TArray<FVector>( unitsphere.get_vertices() );
        m_uvmapping = TArray<FVector2D>( unitsphere.get_uvmapping() );
        MakeMesh();
    }
}

void AP_PawnBase::ConstructSphere(){
    construct_icosphere();
    m_triangles = TArray<int>( unitsphere.get_triangles_raw(), unitsphere.get_index_count() );
    m_normals = TArray<FVector>( unitsphere.get_vertices() );
    m_vertices = TArray<FVector>( unitsphere.get_vertices() );
    m_uvmapping = TArray<FVector2D>( unitsphere.get_uvmapping() );
    MakeMesh();
}

void AP_PawnBase::MakeMesh(){
    if( !MeshComponent || !hasSphereData() ) //only checking cause DefaultPawn checks a StaticMesh
    {
        logError(Geometry,"Cannot make mesh (MeshComponent is null, or SphereData is non-existant)");
        return;
    }
    static TArray<FVector2D> dummy_uv;
    static TArray<FColor> dummy_color;
    static TArray<FProcMeshTangent> dummy_tangents;
    //logWarning(Geometry,"Still using `dummy_uv` for CreateMeshSection");
    MeshComponent->CreateMeshSection( 0, m_vertices, m_triangles, m_normals, m_uvmapping, dummy_color, dummy_tangents, true );
    CollisionComponent->SetSphereRadius(m_radius);
}

void AP_PawnBase::SetRadius(float radius){
    if( !hasSphereData() )
    {
        logError(Geometry,"Invalid sphere data present.");
        return;
    }
    
    float factor;
    if( FMath::IsNearlyZero(m_radius) )
    {
        m_vertices = TArray<FVector>( m_normals );
        m_radius = 1.f;
        factor = radius;
    }
    else
    {
        if( FMath::IsNearlyEqual(m_radius,radius,epsilon) ) {
            float curRadius = m_vertices[15].Size();
            logWarning(Geometry,"Ignoring request. {radius: %f, requested-radius: %f}",curRadius,radius);
            return;
        }
        factor = radius / m_radius;
    }
    logInfoC(Geometry,DColor::Cyan,true,"Setting radius {vertices: %d, radius: %f, new radius: %f, scaling factor: %f}",m_vertices.Num(),m_radius,radius,factor);
    for( FVector &each : m_vertices )
    {
        each *= factor;
        logVerbose(Geometry,"v = {%s}",*each.ToString());
    }
    m_radius = radius;
    MakeMesh();
}

void AP_PawnBase::SetMaterial(UMaterialInterface* material){
    if( !MeshComponent || !material ){
        logError(Materials, "%s", !MeshComponent ? "MeshComponent is NULL" : "Material instance is NULL");
        return;
    }
    MeshComponent->SetMaterial(0,material);
}

// Called every frame
void AP_PawnBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AP_PawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
	//check(PlayerInputComponent);

	/* todo: implement controls
    if (bAddDefaultMovementBindings)
	{
		InitializeP_PawnInputBindings();

		PlayerInputComponent->BindAxis("P_Pawn_MoveForward", this, &AP_PawnBase::MoveForward);
		PlayerInputComponent->BindAxis("P_Pawn_MoveRight", this, &AP_PawnBase::MoveRight);
		PlayerInputComponent->BindAxis("P_Pawn_MoveUp", this, &AP_PawnBase::MoveUp_World);
		PlayerInputComponent->BindAxis("P_Pawn_Turn", this, &AP_PawnBase::AddControllerYawInput);
		PlayerInputComponent->BindAxis("P_Pawn_TurnRate", this, &AP_PawnBase::TurnAtRate);
		PlayerInputComponent->BindAxis("P_Pawn_LookUp", this, &AP_PawnBase::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("P_Pawn_LookUpRate", this, &AP_PawnBase::LookUpAtRate);
	}*/
}



void InitializeP_PawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveForward", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveForward", EKeys::Up, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveForward", EKeys::Down, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveForward", EKeys::Gamepad_LeftY, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveRight", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveRight", EKeys::Gamepad_LeftX, 1.f));

		// HACK: Android controller bindings in ini files seem to not work
		//  Direct overrides here some to work
#if !PLATFORM_ANDROID
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Gamepad_LeftThumbstick, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Gamepad_RightThumbstick, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Gamepad_FaceButton_Bottom, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::LeftControl, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::SpaceBar, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::C, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::E, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Q, -1.f));
#else
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Gamepad_LeftTriggerAxis, -0.5f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_MoveUp", EKeys::Gamepad_RightTriggerAxis, 0.5f));
#endif

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_TurnRate", EKeys::Left, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_TurnRate", EKeys::Right, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_Turn", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_LookUpRate", EKeys::Gamepad_RightY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("P_Pawn_LookUp", EKeys::MouseY, -1.f));
	}
}