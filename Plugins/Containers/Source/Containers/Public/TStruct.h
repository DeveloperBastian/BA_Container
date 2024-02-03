#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TStruct.generated.h"

/**
* Describe your struct here
* See https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/GameplayArchitecture/Structs/Specifiers/ for specifiers
*/
USTRUCT(BlueprintType
	, meta = (DisplayName = "Template Struct"))
struct FTemplateStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Struct Test Template")
	int32 Int32Template;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Test Template")
	FString StringTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Struct Test Template")
	AActor* ActorTemplate;

#pragma region Constructor - Initialize all values
public:
	/**
	* Better initialize all properties with zero values or null pointer
	* The default constructor is especially useful, if data structures like TArray or TMap will be used, since they might need to preallocate internal space
	*/

	FTemplateStruct() : Int32Template(0), StringTemplate(""), ActorTemplate(nullptr)
	{
	}

#pragma endregion Constructor - Initialize all values

#pragma region Helper functions
    /**
	* Your struct will need a Hash function to be effectively usable e.g. in TMap as key
	* Just combine multiple fields of it into one Has value
	* HashCombine: Combines two int32 values into one hash: https://docs.unrealengine.com/4.27/en-US/API/Runtime/Core/Templates/HashCombine/
	* GetTypeHash: Offers overloads for multiple data types to calculate an int32 hash value: https://docs.unrealengine.com/4.27/en-US/API/Runtime/Core/Templates/GetTypeHash/
	*/
	friend uint32 GetTypeHash(const FTemplateStruct& Struct)
	{
		return HashCombine(GetTypeHash(Struct.StringTemplate), Struct.Int32Template);
	}

	/**
	* Override the comparison operator for sorting in C++
	*/
	FORCEINLINE bool operator==(const FTemplateStruct& StructToCompare) const
	{
		return Int32Template == StructToCompare.Int32Template;
	}

	/**
	* We use this Equals function, to implement an override of the == operator, associated with our custom struct
	* This will enable any code, using the == operator, to compare two instances of the struct.
	*/
	//FORCEINLINE bool Equals(const FString& Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const
	FORCEINLINE bool Equals(const FTemplateStruct& Other) const
    {
		return Int32Template == Other.Int32Template;
    }

	/**
	* This UObject pointer is not accessible to Blueprint Graphs, but
	* is visible to UE's reflection, smart pointer, and garbage collection
	* systems.
	*/
	UPROPERTY()
	UObject* SafeObjectPointer;

#pragma endregion Helper functions

};
