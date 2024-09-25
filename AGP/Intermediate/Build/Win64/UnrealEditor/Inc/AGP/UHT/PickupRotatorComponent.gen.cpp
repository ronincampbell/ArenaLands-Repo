// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AGP/PickupRotatorComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePickupRotatorComponent() {}
// Cross Module References
	AGP_API UClass* Z_Construct_UClass_UPickupRotatorComponent();
	AGP_API UClass* Z_Construct_UClass_UPickupRotatorComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
	UPackage* Z_Construct_UPackage__Script_AGP();
// End Cross Module References
	void UPickupRotatorComponent::StaticRegisterNativesUPickupRotatorComponent()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPickupRotatorComponent);
	UClass* Z_Construct_UClass_UPickupRotatorComponent_NoRegister()
	{
		return UPickupRotatorComponent::StaticClass();
	}
	struct Z_Construct_UClass_UPickupRotatorComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_RotationSpeed_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_RotationSpeed;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UPickupRotatorComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UActorComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_AGP,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPickupRotatorComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Custom" },
		{ "IncludePath", "PickupRotatorComponent.h" },
		{ "ModuleRelativePath", "PickupRotatorComponent.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPickupRotatorComponent_Statics::NewProp_RotationSpeed_MetaData[] = {
		{ "Category", "PickupRotatorComponent" },
		{ "ModuleRelativePath", "PickupRotatorComponent.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UPickupRotatorComponent_Statics::NewProp_RotationSpeed = { "RotationSpeed", nullptr, (EPropertyFlags)0x0040000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UPickupRotatorComponent, RotationSpeed), METADATA_PARAMS(Z_Construct_UClass_UPickupRotatorComponent_Statics::NewProp_RotationSpeed_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UPickupRotatorComponent_Statics::NewProp_RotationSpeed_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UPickupRotatorComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPickupRotatorComponent_Statics::NewProp_RotationSpeed,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UPickupRotatorComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPickupRotatorComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UPickupRotatorComponent_Statics::ClassParams = {
		&UPickupRotatorComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UPickupRotatorComponent_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UPickupRotatorComponent_Statics::PropPointers),
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UPickupRotatorComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UPickupRotatorComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UPickupRotatorComponent()
	{
		if (!Z_Registration_Info_UClass_UPickupRotatorComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPickupRotatorComponent.OuterSingleton, Z_Construct_UClass_UPickupRotatorComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UPickupRotatorComponent.OuterSingleton;
	}
	template<> AGP_API UClass* StaticClass<UPickupRotatorComponent>()
	{
		return UPickupRotatorComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPickupRotatorComponent);
	UPickupRotatorComponent::~UPickupRotatorComponent() {}
	struct Z_CompiledInDeferFile_FID_Users_Fuzzy_Documents_GitHub_ArenaLands_ArenaLands_Repo_AGP_Source_AGP_PickupRotatorComponent_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Fuzzy_Documents_GitHub_ArenaLands_ArenaLands_Repo_AGP_Source_AGP_PickupRotatorComponent_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UPickupRotatorComponent, UPickupRotatorComponent::StaticClass, TEXT("UPickupRotatorComponent"), &Z_Registration_Info_UClass_UPickupRotatorComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPickupRotatorComponent), 4223933404U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Fuzzy_Documents_GitHub_ArenaLands_ArenaLands_Repo_AGP_Source_AGP_PickupRotatorComponent_h_2553309328(TEXT("/Script/AGP"),
		Z_CompiledInDeferFile_FID_Users_Fuzzy_Documents_GitHub_ArenaLands_ArenaLands_Repo_AGP_Source_AGP_PickupRotatorComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Fuzzy_Documents_GitHub_ArenaLands_ArenaLands_Repo_AGP_Source_AGP_PickupRotatorComponent_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
