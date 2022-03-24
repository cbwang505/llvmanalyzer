// RetDec: This file was created by the RetDec authors by moving its code from
// source *.cpp files to this header file.
// RetDec: This file was taken from LLVM commit: 438784aaf3397778212bd41bf5333f86e04b4814.
//

#ifndef RETDEC_ALLOCATOR_H
#define RETDEC_ALLOCATOR_H

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

namespace llvm {
namespace itanium_demangle {

class BumpPointerAllocator {
private:
	struct BlockMeta {
		BlockMeta* Next;
		size_t Current;
	};

// RetDec: insrease alloc size to prevent segfaults.
//	static constexpr size_t AllocSize = 4096; // RetDec: old code
	static constexpr size_t AllocSize = 4096 * 4; // RetDec: new code
	static constexpr size_t UsableAllocSize = AllocSize - sizeof(BlockMeta);

	alignas(long double) char InitialBuffer[AllocSize];
	BlockMeta* BlockList = nullptr;

	void grow();

	void* allocateMassive(size_t NBytes);

public:
	BumpPointerAllocator();

	void* allocate(size_t N);

	void reset();

	~BumpPointerAllocator();
};

class DefaultAllocator {
	BumpPointerAllocator Alloc;

public:
	void reset();

	template<typename T, typename ...Args> T *makeNode(Args &&...args) {
		return new (Alloc.allocate(sizeof(T)))
			T(std::forward<Args>(args)...);
	}

	void *allocateNodeArray(size_t sz);

	void *allocateBytes(size_t sz);
};

}
}

#endif //RETDEC_ALLOCATOR_H
