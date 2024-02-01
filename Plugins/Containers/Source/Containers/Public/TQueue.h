// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Queue.h"

#include "TQueue.generated.h"

/**
 * Struct to showcase the TQueue. 
 */
USTRUCT(BlueprintType)
struct FQueueTestStruct
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test Struct")
	int32 Number;

	FQueueTestStruct() : Name(""), Number(0)
	{
	}
};

/**
 * Delegates to indicate queue changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQueueChanged, bool, changed);


/**
 * Class to encapsulate TQueue
 * TQueue implements an unbounded non-intrusive queue using a lock-free linked
 * list that stores copies of the queued items. The template can operate in two modes:
 * Multiple-producers single-consumer (MPSC) and Single-producer single-consumer (SPSC).
 *
 * The queue is thread-safe in both modes. The Dequeue() method ensures thread-safety by
 * writing it in a way that does not depend on possible instruction reordering on the CPU.
 * The Enqueue() method uses an atomic compare-and-swap in multiple-producers scenarios.
 */
UCLASS(BlueprintType, Transient)
class UTQueue : public UObject
{
	GENERATED_BODY()

public:

	UTQueue()
	{
		BA_Queue.Empty();
	}

	#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate enqueue event on queue"))
	FOnQueueChanged OnQueue_Enqueue_Delegate;

	UPROPERTY(BlueprintAssignable, Category = "BA Container - Queue"
		, meta = (ToolTip = "Delegate to indicate dequeue event on queue"))
	FOnQueueChanged OnQueue_Dequeue_Delegate;

#pragma endregion Delegates

private:
	// Standard is Multiple-producers single-consumer (MPSC) 
	TQueue<FQueueTestStruct, EQueueMode::Mpsc> BA_Queue;

public:

	#pragma region Public Functions

	UFUNCTION(BlueprintCallable, Category = "BA Container - Queue"
		, meta = (CompactNodeTitle = "Enqueue to Queue"
			, ToolTip = "Adds an item to the head of the queue"))
	FORCEINLINE void Enqueue(UPARAM(ref) FQueueTestStruct& QueueItem)
	{
		this->BA_Queue.Enqueue(QueueItem);
		this->OnQueue_Enqueue_Delegate.Broadcast(true);
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Queue"
		, meta = (CompactNodeTitle = "Dequeue from Queue"
			, ToolTip = "Removes and returns the item from the tail of the queue"))
	FORCEINLINE FQueueTestStruct Dequeue()
	{
		FQueueTestStruct QueueItem;
		this->BA_Queue.Dequeue(QueueItem);
		this->OnQueue_Dequeue_Delegate.Broadcast(true);
		return QueueItem;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BA Container - Queue"
		, meta = (CompactNodeTitle = "Queue Empty?"
			, ToolTip = "Checks whether the queue is empty"))
	FORCEINLINE bool IsEmpty()
	{
		return this->BA_Queue.IsEmpty();
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Queue"
		, meta = (CompactNodeTitle = "Peek Queue"
			, ToolTip = "Peek at the queue's tail item without removing it"))
	FORCEINLINE FQueueTestStruct Peek()
	{
		FQueueTestStruct QueueItem;
		this->BA_Queue.Peek(QueueItem);
		return QueueItem;
	}

	UFUNCTION(BlueprintCallable, Category = "BA Container - Queue"
		, meta = (CompactNodeTitle = "Pop Queue"
			, ToolTip = "Removes the item from the tail of the queue. Returns true if a value was removed, false if the queue was empty"))
	FORCEINLINE bool Pop()
	{
		return this->BA_Queue.Pop();
	}

#pragma endregion Public Functions
};
