// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "Containers/Array.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "Misc/Guid.h"
#include "Timer.h"
#include "TArray.generated.h"


/**
 * Enum to structure sorting for FTArrayTestStruct.
 */
UENUM(BlueprintType)
	enum class ETestArraySorting : uint8 {
		E_NumberAsc		UMETA(DisplayName = "Number - Ascending"),
		E_NumberDesc	UMETA(DisplayName = "Number - Descending"),
		E_NameAsc		UMETA(DisplayName = "Name - Ascending"),
		E_NameDesc		UMETA(DisplayName = "Name - Descending")
	};

/**
 * Struct to showcase the TArray. 
 */
USTRUCT(BlueprintType)
struct FTArrayTestStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Array Test Struct")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Array Test Struct")
	int32 Number;

	FTArrayTestStruct() : Name(""), Number(0)
	{
	}

	#pragma region Sorting
	static bool CompareNumberAscending(const FTArrayTestStruct& A, const FTArrayTestStruct& B)
	{
		return A.Number < B.Number;
	}

	static bool CompareNumberDescending(const FTArrayTestStruct& A, const FTArrayTestStruct& B)
	{
		return A.Number > B.Number;
	}

	static bool CompareNameAscending(const FTArrayTestStruct& A, const FTArrayTestStruct& B)
	{
		return A.Name < B.Name;
	}

	static bool CompareNameDescending(const FTArrayTestStruct& A, const FTArrayTestStruct& B)
	{
		return A.Name > B.Name;
	}
	#pragma endregion Sorting

	#pragma region Mandatory Functions
// generates a hash from the struct
	// MANDATORY FOR MOST TArray FUNCTIONS!
	friend uint32 GetTypeHash(const FTArrayTestStruct& Struct)
	{
		return HashCombine(GetTypeHash(Struct.Name), Struct.Number);
	}

	// define a value that acts as comparison between two struct
	// MANDATORY FOR MOST TArray FUNCTIONS!
	FORCEINLINE bool operator==(const FTArrayTestStruct& StructToCompare) const
	{
		return Number == StructToCompare.Number;
	}
#pragma endregion Mandatory Functions
};

/**
 * Delegates to indicate queue changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArrayChanged, bool, Changed);

/**
 * Class to encapsulate TMultiMap
 * A TMapBase specialization that allows multiple values to be associated with each key.
 */
UCLASS(BlueprintType, Transient)
class UTArray : public UObject
{
	GENERATED_BODY()

public:

	UTArray()
	{

	}

	#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Array"
		, meta = (ToolTip = "Delegate to indicate add vent to array"))
	FOnArrayChanged OnArrayAdd_Delegate;

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Array"
		, meta = (ToolTip = "Delegate to indicate remove event from array"))
	FOnArrayChanged OnArrayRemove_Delegate;

#pragma endregion Delegates

private:
	TArray<FTArrayTestStruct> BA_Array;

public:
	#pragma region Public Functions

	#pragma region Adding Elements
	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Add - Emplace"
			, ToolTip = "Add an item to the Array"))
	FORCEINLINE void Array_Add(UPARAM(ref) FTArrayTestStruct& Value, bool Broadcast)
	{
		// Emplace avoids creating a temporary variable, 
		// which is often undesirable for non-trivial value types.
		// As a rule of thumb, use Add for trivial types and Emplace otherwise. 
		// Emplace will never be less efficient than Add.
		this->BA_Array.Emplace(Value);
		if (Broadcast)
			this->OnArrayAdd_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Add - Move Temp"
			, ToolTip = "Add an item to the Array"))
	FORCEINLINE void Array_AddMoveTemp(UPARAM(ref) FTArrayTestStruct& Value, bool Broadcast)
	{
		// MoveTemp will cast a reference to an rvalue reference. 
		// It essentially just shifts points instead of doing a Value copy to a new address.
		this->BA_Array.Add(MoveTemp(Value));
		if (Broadcast)
			this->OnArrayAdd_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Add - Push"
			, ToolTip = "Add an item to the end of the Array. Wil use MoveTemp if possible"))
	FORCEINLINE void Array_Push(UPARAM(ref) FTArrayTestStruct& Value, bool Broadcast)
	{
		if (Broadcast)
			this->OnArrayAdd_Delegate.Broadcast(true);
		// tries to use MoveTemp internally. 
		this->BA_Array.Push(Value);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Add - Unique"
			, ToolTip = "Add an item to the Array while checking uniqueness"))
	FORCEINLINE void Array_AddUnique(UPARAM(ref) FTArrayTestStruct& Value, bool Broadcast)
	{
		// AddUnique only adds a new element to the container if an equivalent element doesn't already exist. 
		// Equivalence is checked by using the element type's operator==:
		// 
		// AddUnique will have to parse the entire array checking for duplicates!
		this->BA_Array.AddUnique(Value);
		if (Broadcast)
			this->OnArrayAdd_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Insert At"
			, ToolTip = "Insert an item to the Array into a given index. Will preserve order"))
	FORCEINLINE void Array_InsertAt(UPARAM(ref) FTArrayTestStruct& Value, int32 Position, bool Broadcast)
	{
		this->BA_Array.Insert(Value, Position);
		if (Broadcast)
			this->OnArrayAdd_Delegate.Broadcast(true);
	}

#pragma endregion Adding Elements

	#pragma region Removing Elements
	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Remove"
			, ToolTip = "Removes an item from the array"))
	FORCEINLINE void Array_Remove(UPARAM(ref) FTArrayTestStruct& Value, bool Broadcast)
	{
		this->BA_Array.Remove(Value);
		if (Broadcast)
			this->OnArrayRemove_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Remove At"
			, ToolTip = "Removes an item from the array on a given position. Will return true on successful removal, or false if position is not valid"))
	FORCEINLINE bool Array_RemoveAt(int32 Position, bool Broadcast)
	{
		if (this->BA_Array.IsValidIndex(Position))
		{
			this->BA_Array.RemoveAt(Position);
			if (Broadcast)
				this->OnArrayRemove_Delegate.Broadcast(true);
			return true;
		}
		else
			return false;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Pop"
			, ToolTip = "Removes first item from array. If this is standard access pattern, think about using a queue"))
	FORCEINLINE FTArrayTestStruct Array_Pop(int32 Position, bool Broadcast)
	{
		return this->BA_Array.Pop(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Remove All Starting With"
			, ToolTip = "Removes all elements that match a defined predicate"))
	FORCEINLINE void Array_RemoveAllStartingWith(FString StartsWith, bool Broadcast)
	{
		// Example for using a predicate to filter all items matching the predicate condition
		this->BA_Array.RemoveAll(
			[StartsWith](const FTArrayTestStruct& A) {
				return A.Name.StartsWith(StartsWith, ESearchCase::IgnoreCase);
			}
		);
		if (Broadcast)
			this->OnArrayRemove_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Empty"
			, ToolTip = "Empties the array - set NewCapacity to zero if you dont need to reserve space for new content, otherwise provide the expected capacity"))
	FORCEINLINE void Array_Empty(int32 NewCapacity, bool Broadcast)
	{
		this->BA_Array.Empty(NewCapacity);
		if (Broadcast)
			this->OnArrayRemove_Delegate.Broadcast(true);
	}
#pragma endregion Removing Elements
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "No of values"
			, ToolTip = "Returns the number of values within this Array"))
	FORCEINLINE int32 Array_NumberOfValues()
	{
		return this->BA_Array.Num();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Contains"
			, ToolTip = "Check if a given value exists"))
	FORCEINLINE bool Array_Contains(UPARAM(ref) FTArrayTestStruct& Value)
	{
		return this->BA_Array.Contains(Value);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Values"
			, ToolTip = "Returns all array values"))
	FORCEINLINE TArray<FTArrayTestStruct> Array_Values()
	{
		return this->BA_Array;
	}

	#pragma region Searching

	/**
	 * Example of a Filter Function
	 * Add your custom filter criteria in lambda function within FilterByPredicate
	 *
	 * @StartsWith String that the Values needs to start with
	 * @returns TArray<FTArrayTestStruct>
	 */
	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Find Name Start"
			, ToolTip = "Returns all items with name starting like parameter given"))
	FORCEINLINE TArray<FTArrayTestStruct> Array_GetNamesStartingWith(const FString& StartsWith)
	{
		return this->BA_Array.FilterByPredicate(
			[StartsWith](const FTArrayTestStruct& A) {
				return A.Name.StartsWith(StartsWith, ESearchCase::IgnoreCase);
			}
		);
	}

#pragma endregion Searching

	#pragma region Sorting
	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Sort Array"
			, ToolTip = "Sort the Values as FTArrayTestStruct by Enum ETestArraySorting"))
	FORCEINLINE void Array_Sort(ETestArraySorting Sort)
	{
		// Sorting with Lambda
		//this->BA_Array.Sort([](const FTArrayTestStruct& A, const FTArrayTestStruct& B) {
		//	return A.Number > B.Number;
		//	}
		//);
		
		// Better to make the search logic part of your Struct class and reference this:
		switch (Sort)
		{
		case ETestArraySorting::E_NumberAsc:
			this->BA_Array.Sort(FTArrayTestStruct::CompareNumberAscending);
			break;
		case ETestArraySorting::E_NumberDesc :
			this->BA_Array.Sort(FTArrayTestStruct::CompareNumberDescending);
			break;
		case ETestArraySorting::E_NameAsc:
			this->BA_Array.Sort(FTArrayTestStruct::CompareNameAscending);
			break;
		case ETestArraySorting::E_NameDesc:
			this->BA_Array.Sort(FTArrayTestStruct::CompareNameDescending);
			break;

		default: break;
		}
	}
#pragma endregion Sorting

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Iterate Array"
			, ToolTip = "Example to iterate over array, adding a prefix to all Struct.Names"))
	FORCEINLINE float Array_Iterate(UPARAM(ref) FString& Prefix)
	{
		// for demonstration, we set a timer and report total time needed for operation
		Timer t;
		t.Start();
		for (FTArrayTestStruct& Value : BA_Array)
		{
			Value.Name.InsertAt(0, Prefix);
		}
		return t.Stop();
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Array"
		, meta = (CompactNodeTitle = "Parallel Iterate Array"
			, ToolTip = "Example to parallel-foreach iterate over array, adding a prefix to all Struct.Names"))
	FORCEINLINE float Array_IterateParallel(UPARAM(ref) FString& Prefix)
	{
		// for demonstration, we set a timer and report total time needed for operation
		Timer t;
		t.Start();
		FCriticalSection Mutex;
		ParallelFor(BA_Array.Num(), [&, Prefix](int32 i)
		{
			FScopeLock Lock(&Mutex);
			// whatever logic you need to do with this element
			BA_Array[i].Name.InsertAt(0, Prefix);
		});
		return t.Stop();
	}

#pragma endregion Public Functions

	

};
