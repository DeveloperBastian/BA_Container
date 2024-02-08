// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "Containers/Set.h"
#include "Misc/Guid.h"

#include "TSet.generated.h"


/**
 * Enum to structure sorting for FTSetTestStruct.
 */
UENUM(BlueprintType)
	enum class ETestStructSorting : uint8 {
		E_NumberAsc		UMETA(DisplayName = "Number - Ascending"),
		E_NumberDesc	UMETA(DisplayName = "Number - Descending"),
		E_NameAsc		UMETA(DisplayName = "Name - Ascending"),
		E_NameDesc		UMETA(DisplayName = "Name - Descending")
	};

/**
 * Struct to showcase the TSet. 
 */
USTRUCT(BlueprintType)
struct FTSetTestStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	int32 Number;

	FTSetTestStruct() : Name(""), Number(0)
	{
	}

	#pragma region Sorting
	static bool CompareNumberAscending(const FTSetTestStruct& A, const FTSetTestStruct& B)
	{
		return A.Number < B.Number;
	}

	static bool CompareNumberDescending(const FTSetTestStruct& A, const FTSetTestStruct& B)
	{
		return A.Number > B.Number;
	}

	static bool CompareNameAscending(const FTSetTestStruct& A, const FTSetTestStruct& B)
	{
		return A.Name < B.Name;
	}

	static bool CompareNameDescending(const FTSetTestStruct& A, const FTSetTestStruct& B)
	{
		return A.Name > B.Name;
	}
	#pragma endregion Sorting

	#pragma region Mandatory Functions
// generates a hash from the struct
	// MANDATORY FOR MOST TSet FUNCTIONS!
	friend uint32 GetTypeHash(const FTSetTestStruct& Struct)
	{
		return HashCombine(GetTypeHash(Struct.Name), Struct.Number);
	}

	// define a value that acts as comparison between two struct
	// MANDATORY FOR MOST TSet FUNCTIONS!
	FORCEINLINE bool operator==(const FTSetTestStruct& StructToCompare) const
	{
		return Number == StructToCompare.Number;
	}
#pragma endregion Mandatory Functions
};

/**
 * Delegates to indicate queue changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetChanged, bool, Changed);


/**
 * Class to encapsulate TMultiMap
 * A TMapBase specialization that allows multiple values to be associated with each key.
 */
UCLASS(BlueprintType, Transient)
class UTSet : public UObject
{
	GENERATED_BODY()

public:

	UTSet()
	{}

	#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate enqueue event on queue"))
	FOnSetChanged OnSetAdd_Delegate;

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate dequeue event on queue"))
	FOnSetChanged OnSetRemove_Delegate;

#pragma endregion Delegates

private:
	TSet<FTSetTestStruct> BA_Set;

public:
	#pragma region Public Functions

	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Add"
			, ToolTip = "Add an item to the Set"))
	FORCEINLINE void Set_Add(UPARAM(ref) FTSetTestStruct& Value)
	{
		this->BA_Set.Add(Value);
		this->OnSetAdd_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Number of values"
			, ToolTip = "Returns the number of values within this set"))
	FORCEINLINE int32 Set_NumberOfValues()
	{
		return this->BA_Set.Num();
	}
	
	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Remove All"
			, ToolTip = "Remove an item from the set"))
	FORCEINLINE void Set_Remove(UPARAM(ref) FTSetTestStruct& Value)
	{
		this->OnSetRemove_Delegate.Broadcast(true);
		this->BA_Set.Remove(Value);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Empty"
			, ToolTip = "Empties the set - set NewCapacity to zero if you dont need to reserve space for new content, otherwise provide the expected capacity"))
	FORCEINLINE void Set_Empty(int32 NewCapacity)
	{
		this->BA_Set.Empty(NewCapacity);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Value Exists"
			, ToolTip = "Check if a given value exists"))
	FORCEINLINE bool Set_ItemExists(UPARAM(ref) FTSetTestStruct& Value)
	{
		return this->BA_Set.Contains(Value);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Get All Values"
			, ToolTip = "Gets all values of the set"))
	FORCEINLINE TArray<FTSetTestStruct> Set_GetAllValues()
	{
		return this->BA_Set.Array();
	}

	#pragma region Searching

	/**
	 * Example of a Filter Function
	 * Add your custom filter criteria in lambda function within FilterByPredicate
	 *
	 * @StartsWith String that the Values needs to start with
	 * @returns TArray<FTSetTestStruct>
	 */
	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Find Name Start"
			, ToolTip = "Returns all items with name starting like parameter given"))
	FORCEINLINE TArray<FTSetTestStruct> Set_GetNamesStartingWith(const FString& StartsWith)
	{
		return this->BA_Set.Array().FilterByPredicate(
			[StartsWith](const FTSetTestStruct& A) {
				return A.Name.StartsWith(StartsWith, ESearchCase::IgnoreCase);
			}
		);
	}

#pragma endregion Searching

#pragma endregion Public Functions

	UFUNCTION(BlueprintCallable, Category = "BA Container - Set"
		, meta = (CompactNodeTitle = "Sort Set"
			, ToolTip = "Sort the Values as FTSetTestStruct by Enum ETestStructSorting"))
	FORCEINLINE void Set_Sort(ETestStructSorting Sort)
	{
		// Sorting with Lambda
		//this->BA_Set.Sort([](const FTSetTestStruct& A, const FTSetTestStruct& B) {
		//	return A.Number > B.Number;
		//	}
		//);
		
		// Better to make the search logic part of your Struct class and reference this:
		switch (Sort)
		{
		case ETestStructSorting::E_NumberAsc:
			this->BA_Set.Sort(FTSetTestStruct::CompareNumberAscending);
			break;
		case ETestStructSorting::E_NumberDesc :
			this->BA_Set.Sort(FTSetTestStruct::CompareNumberDescending);
			break;
		case ETestStructSorting::E_NameAsc:
			this->BA_Set.Sort(FTSetTestStruct::CompareNameAscending);
			break;
		case ETestStructSorting::E_NameDesc:
			this->BA_Set.Sort(FTSetTestStruct::CompareNameDescending);
			break;

		default: break;
		}
	}

};
