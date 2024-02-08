// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)

#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "Misc/Guid.h"
#include "Misc/SpinLock.h"
#include "Containers/Map.h"
#include "Timer.h"

#include "TMap.generated.h"

#pragma region Enum for Sorting
/**
 * Enum to structure sorting for FTSetTestStruct.
 */
UENUM(BlueprintType)
	enum class ETestMapSorting : uint8 {
		E_NumberAsc		UMETA(DisplayName = "Number - Ascending"),
		E_NumberDesc	UMETA(DisplayName = "Number - Descending"),
		E_NameAsc		UMETA(DisplayName = "Name - Ascending"),
		E_NameDesc		UMETA(DisplayName = "Name - Descending")
	};
#pragma endregion Enum for Sorting

#pragma region Struct
/**
 * Struct to showcase the TCircularQueue. 
 */
USTRUCT(BlueprintType)
struct FMapTestStruct
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FGuid Guid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	int32 Number;

	FMapTestStruct() : Name(""), Number(0)
	{
	}
	
#pragma region Sorting
	static bool CompareNumberAscending(const FMapTestStruct& A, const FMapTestStruct& B)
	{
		return A.Number < B.Number;
	}

	static bool CompareNumberDescending(const FMapTestStruct& A, const FMapTestStruct& B)
	{
		return A.Number > B.Number;
	}

	static bool CompareNameAscending(const FMapTestStruct& A, const FMapTestStruct& B)
	{
		return A.Name < B.Name;
	}

	static bool CompareNameDescending(const FMapTestStruct& A, const FMapTestStruct& B)
	{
		return A.Name > B.Name;
	}
#pragma endregion Sorting
};
#pragma endregion Struct

/**
 * Delegates to indicate map changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapChanged, FMapTestStruct, changed);


/**
 * Class to encapsulate TMap - A Key-Value Store
 */
UCLASS(BlueprintType, Transient)
class UTMap : public UObject
{
	GENERATED_BODY()

public:

	UTMap()
	{}

	~UTMap()
	{
		this->BA_Map.Reset();
	}

	#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Map"
		, meta = (ToolTip = "Delegate to indicate a value was added to map"))
	FOnMapChanged OnMapAdd_Delegate;

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Map"
		, meta = (ToolTip = "Delegate to indicate a value was removed from map"))
	FOnMapChanged OnMapDelete_Delegate;

#pragma endregion Delegates

private:
	TMap<FGuid, FMapTestStruct> BA_Map;

public:

	#pragma region Public Functions

	#pragma region Add and Remove
	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Add Item"
			, ToolTip = "Add one Key-Value pair to the map"))
	FORCEINLINE void Map_Add(UPARAM(ref) FMapTestStruct& Value, bool Broadcast)
	{
		BA_Map.Add(Value.Guid, Value);
		if (Broadcast)
			this->OnMapAdd_Delegate.Broadcast(Value);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Remove Item"
			, ToolTip = "Remove all associations between the specified key and value from the multi map"))
	FORCEINLINE FMapTestStruct Map_Remove(UPARAM(ref) FGuid& Key, bool Broadcast)
	{
		FMapTestStruct tmpValue;
		bool found = this->BA_Map.RemoveAndCopyValue(Key, tmpValue);
		if (Broadcast && found)
			this->OnMapDelete_Delegate.Broadcast(tmpValue);
		return tmpValue;
	}
#pragma endregion Add and Remove

	#pragma region Map Misc
	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Number of values"
			, ToolTip = "Returns the number of values within this map"))
	FORCEINLINE int32 Map_NumberOfValues()
	{
		return this->BA_Map.Num();
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Empty"
			, ToolTip = "Empties the map - set NewCapacity to zero if you dont need to reserve space for new content, otherwise provide the expected capacity"))
	FORCEINLINE void Map_Empty(int32 NewCapacity)
	{
		this->BA_Map.Empty(NewCapacity);
	}
#pragma endregion Map Misc

	#pragma region Get Values and Keys
	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Get Value"
			, ToolTip = "Returns a values matching the given key - or an empty default struct if key is not found"))
	FORCEINLINE FMapTestStruct Map_GetValue(UPARAM(ref) FGuid& Key)
	{
		return this->BA_Map.FindRef(Key);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Get Keys"
			, ToolTip = "Gets all keys of the map"))
	FORCEINLINE TArray<FGuid> Map_GetKeys()
	{
		TArray<FGuid> keys;
		this->BA_Map.GenerateKeyArray(keys);
		return keys;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Get Values"
			, ToolTip = "Gets all values of the map"))
	FORCEINLINE TArray<FMapTestStruct> Map_GetValues()
	{
		TArray<FMapTestStruct> values;
		this->BA_Map.GenerateValueArray(values);
		return values;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Filter Cities"
			, ToolTip = "Gets all cities with population larger than parameter"))
	FORCEINLINE TMap<FGuid, FMapTestStruct> Map_FilterCities(int32 Population)
	{
		TMap<FGuid, FMapTestStruct> filtered = this->BA_Map.FilterByPredicate(
			[Population](const TPair<FGuid, FMapTestStruct>& KvP)
			{
				return KvP.Value.Number > Population;
			}
		);
		return filtered;
	}
#pragma endregion Get Values and Keys

	#pragma region Sorting Values and Keys
UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Value Sort"
			, ToolTip = "Sort the values of the map using ETestMapSorting and static functions of the value struct"))
	FORCEINLINE void Map_ValueSort(ETestMapSorting Sorting)
	{
		switch (Sorting)
		{
		case ETestMapSorting::E_NumberAsc:
			this->BA_Map.ValueSort(FMapTestStruct::CompareNumberAscending);
			break;
		case ETestMapSorting::E_NumberDesc:
			this->BA_Map.ValueSort(FMapTestStruct::CompareNumberDescending);
			break;
		case ETestMapSorting::E_NameAsc:
			this->BA_Map.ValueSort(FMapTestStruct::CompareNameAscending);
			break;
		case ETestMapSorting::E_NameDesc:
			this->BA_Map.ValueSort(FMapTestStruct::CompareNameDescending);
			break;
		default: break;
		}	
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Sort Keys"
			, ToolTip = "Sort the keys of the map"))
	FORCEINLINE void Map_KeySort()
	{
		this->BA_Map.KeySort([](const FGuid& A, const FGuid& B) 
		{
			return A.ToString() > B.ToString();
		}
		);
	}
#pragma endregion Sorting Values and Kesys

	#pragma region Iteration Examples
UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Iterate Map"
			, ToolTip = "Example to iterate over map values, adding a prefix to all Struct.Names"))
	FORCEINLINE float Map_Iterate(UPARAM(ref) FString& Prefix)
	{
		// make parameter local
		FString lPrefix = Prefix;
		// for demonstration, we set a timer and report total time needed for operation
		Timer t; t.Start();
		TArray<FGuid> keys;
		TArray<FMapTestStruct> values;
		// get all keys from map and iterate		
		this->BA_Map.GenerateKeyArray(keys);
		for (FGuid& guid : keys)
		{
			FMapTestStruct* value = this->BA_Map.Find(guid);
			if (value)
				value->Name.InsertAt(0, lPrefix);
		}
		return t.Stop();
		// ==> took about 3200 CPU cycles
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Map"
		, meta = (CompactNodeTitle = "Parallel Iterate Map"
			, ToolTip = "Example to parallel iterate over map values, adding a prefix to all Struct.Names"))
	FORCEINLINE float Map_ParallelIterate(UPARAM(ref) FString& Prefix)
	{
		// make parameter local
		FString lPrefix = Prefix;
		// for demonstration, we set a timer and report total time needed for operation
		Timer t; t.Start();
		// create vars
		TArray<FGuid> keys;
		TArray<FMapTestStruct> values;
		// get all keys from map 		
		this->BA_Map.GenerateKeyArray(keys);
		// Iteration
		FCriticalSection Mutex;
		ParallelFor(keys.Num(), [&, lPrefix](int32 i)
			{
				FMapTestStruct* value = this->BA_Map.Find(keys[i]);
				if (value)
				{
					//FScopeLock Lock(&Mutex);
					value->Name.InsertAt(0, lPrefix);
				}
			}, EParallelForFlags::None);
		return t.Stop();
		// ==> took about 330 CPU cycles without lock
		// ==> took about 26000 CPU cycles with lock
	}
#pragma endregion Iteration Examples
	

#pragma endregion Public Functions
};