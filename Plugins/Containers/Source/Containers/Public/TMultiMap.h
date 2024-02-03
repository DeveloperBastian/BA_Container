// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "Misc/Guid.h"

#include "TMultiMap.generated.h"

/**
 * Struct to showcase the TMultiMap. 
 */
USTRUCT(BlueprintType)
struct FTMultiMapTestStruct
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FGuid Guid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	int32 Number;

	FTMultiMapTestStruct() : Guid(FGuid()), Name(""), Number(0)
	{
	}

	// define a value that acts as comparison between two struct
	// THIS IS MANDATORY FOR THE RemoveSingle Function to work
	FORCEINLINE bool operator==(const FTMultiMapTestStruct& StructToCompare) const
	{
		return Number == StructToCompare.Number;
	}
};

/**
 * Delegates to indicate queue changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiMapChanged, FGuid, ChangedKey);


/**
 * Class to encapsulate TMultiMap
 * A TMapBase specialization that allows multiple values to be associated with each key.
 */
UCLASS(BlueprintType, Transient)
class UTMultiMap : public UObject
{
	GENERATED_BODY()

public:

	UTMultiMap()
	{

	}

	#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate enqueue event on queue"))
	FOnMultiMapChanged OnMultiMapAddKey_Delegate;

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate dequeue event on queue"))
	FOnMultiMapChanged OnMultiMapRemoveFromKey_Delegate;

#pragma endregion Delegates

private:
	TMultiMap<FGuid, FTMultiMapTestStruct> BA_MultiMap;

public:

	#pragma region Public Functions

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Add key-value pair"
			, ToolTip = "Add a key-value association to the multi map."))
	FORCEINLINE void MM_Add(UPARAM(ref) FGuid& Key, UPARAM(ref) FTMultiMapTestStruct& Value)
	{
		this->BA_MultiMap.Add(Key, Value);
		this->OnMultiMapAddKey_Delegate.Broadcast(Key);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Number of values"
			, ToolTip = "Returns the number of values within this multi map associated with the specified key"))
	FORCEINLINE int32 MM_NumberOfValues(UPARAM(ref) FGuid& Key)
	{
		return this->BA_MultiMap.Num(Key);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Multi Find"
			, ToolTip = "Finds all values associated with the specified key"))
	FORCEINLINE TArray<FTMultiMapTestStruct> MM_MultiFind(UPARAM(ref) FGuid& Key)
	{
		TArray<FTMultiMapTestStruct> FoundValues;
		this->BA_MultiMap.MultiFind(Key, FoundValues);
		return FoundValues;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Remove All"
			, ToolTip = "Remove all associations between the specified key and value from the multi map"))
	FORCEINLINE int32 MM_RemoveAll(UPARAM(ref) FGuid& Key)
	{
		this->OnMultiMapRemoveFromKey_Delegate.Broadcast(Key);
		return this->BA_MultiMap.Remove(Key);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Remove First"
			, ToolTip = "Remove the first association between the specified key and value from the map"))
	FORCEINLINE int32 MM_RemoveFirst(UPARAM(ref) FGuid& Key, UPARAM(ref) FTMultiMapTestStruct& Value)
	{
		this->OnMultiMapRemoveFromKey_Delegate.Broadcast(Key);
		return this->BA_MultiMap.RemoveSingle(Key, Value);
		return 1;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Get Keys"
			, ToolTip = "Gets all keys of the multi map"))
	FORCEINLINE TArray<FGuid> MM_GetKeys()
	{
		TArray<FGuid> keys;
		this->BA_MultiMap.GetKeys(keys);
		return keys;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Empty"
			, ToolTip = "Empties the multi map - set NewCapacity to zero if you dont need to reserve space for new content, otherwise provide the expected capacity"))
	FORCEINLINE void MM_Empty(int32 NewCapacity)
	{
		this->BA_MultiMap.Empty(NewCapacity);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "KvP Exists"
			, ToolTip = "Check if a given key-value pair exists"))
	FORCEINLINE bool MM_KeyValueExist(UPARAM(ref) FGuid& Key, UPARAM(ref) FTMultiMapTestStruct& Value)
	{
		const FTMultiMapTestStruct* FoundValuePtr = this->BA_MultiMap.FindPair(Key, Value);
		if (FoundValuePtr == nullptr)
			return false;
		else
			return true;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - MultiMap"
		, meta = (CompactNodeTitle = "Get All Values"
			, ToolTip = "Gets all values of the multi map"))
	FORCEINLINE TArray<FTMultiMapTestStruct> MM_GetAllValues()
	{
		TSet<FGuid> keys;
		TArray<FTMultiMapTestStruct> values;
		// get all keys from multi map and iterate		
		this->BA_MultiMap.GetKeys(keys);
		for (auto& guid : keys) 
		{ 
			// find all values per key
			TArray<FTMultiMapTestStruct> FoundValues;
			this->BA_MultiMap.MultiFind(guid, FoundValues);
			// add all values to return array
			for (auto& value: FoundValues)
				values.Add(value);
		}
		return values;
	}

#pragma endregion Public Functions
};
