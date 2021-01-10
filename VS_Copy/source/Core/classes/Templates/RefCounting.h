#pragma once


/**
 * A smart pointer to an object which implements AddRef/Release.
 */
template<typename ReferencedType>
class TRefCountPtr
{
	typedef ReferencedType* ReferenceType;

public:

	__forceinline TRefCountPtr() :
		Reference(nullptr)
	{ }

	TRefCountPtr(ReferencedType* InReference, bool bAddRef = true)
	{
		Reference = InReference;
		if (Reference && bAddRef)
		{
			Reference->AddRef();
		}
	}

	TRefCountPtr(const TRefCountPtr& Copy)
	{
		Reference = Copy.Reference;
		if (Reference)
		{
			Reference->AddRef();
		}
	}

	__forceinline TRefCountPtr(TRefCountPtr&& Copy)
	{
		Reference = Copy.Reference;
		Copy.Reference = nullptr;
	}

	~TRefCountPtr()
	{
		if (Reference)
		{
			//Reference->Release();
		}
	}

	TRefCountPtr& operator=(ReferencedType* InReference)
	{
		// Call AddRef before Release, in case the new reference is the same as the old reference.
		ReferencedType* OldReference = Reference;
		Reference = InReference;
		if (Reference)
		{
			Reference->AddRef();
		}
		if (OldReference)
		{
			OldReference->Release();
		}
		return *this;
	}

	__forceinline TRefCountPtr& operator=(const TRefCountPtr& InPtr)
	{
		return *this = InPtr.Reference;
	}

	TRefCountPtr& operator=(TRefCountPtr&& InPtr)
	{
		if (this != &InPtr)
		{
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = nullptr;
			if (OldReference)
			{
				OldReference->Release();
			}
		}
		return *this;
	}

	__forceinline ReferencedType* operator->() const
	{
		return Reference;
	}

	__forceinline operator ReferenceType() const
	{
		return Reference;
	}

	__forceinline friend uint32_t GetTypeHash(const TRefCountPtr& InPtr)
	{
		return GetTypeHash(InPtr.Reference);
	}

	__forceinline ReferencedType** GetInitReference()
	{
		*this = nullptr;
		return &Reference;
	}

	__forceinline ReferencedType* GetReference() const
	{
		return Reference;
	}

	__forceinline friend bool IsValidRef(const TRefCountPtr& InReference)
	{
		return InReference.Reference != nullptr;
	}

	__forceinline bool IsValid() const
	{
		return Reference != nullptr;
	}

	__forceinline void SafeRelease()
	{
		*this = nullptr;
	}

	uint32_t GetRefCount()
	{
		uint32 Result = 0;
		if (Reference)
		{
			Result = Reference->GetRefCount();
			check(Result > 0); // you should never have a zero ref count if there is a live ref counted pointer (*this is live)
		}
		return Result;
	}

	__forceinline void Swap(TRefCountPtr& InPtr) // this does not change the reference count, and so is faster
	{
		ReferencedType* OldReference = Reference;
		Reference = InPtr.Reference;
		InPtr.Reference = OldReference;
	}

	/*friend FArchive& operator<<(FArchive& Ar, TRefCountPtr& Ptr)
	{
		ReferenceType PtrReference = Ptr.Reference;
		Ar << PtrReference;
		if (Ar.IsLoading())
		{
			Ptr = PtrReference;
		}
		return Ar;
	}*/

private:

	ReferencedType* Reference;
};

template<typename ReferencedType>
__forceinline bool operator==(const TRefCountPtr<ReferencedType>& A, const TRefCountPtr<ReferencedType>& B)
{
	return A.GetReference() == B.GetReference();
}

template<typename ReferencedType>
__forceinline bool operator==(const TRefCountPtr<ReferencedType>& A, ReferencedType* B)
{
	return A.GetReference() == B;
}

template<typename ReferencedType>
__forceinline bool operator==(ReferencedType* A, const TRefCountPtr<ReferencedType>& B)
{
	return A == B.GetReference();
}
