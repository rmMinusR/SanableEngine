#include "TypedMemoryPool.hpp"

//std::vector<SafeDisposable*> allSafeDisposables = mkAllVec();

 //Awful fix to work around bad allocation in pre-main() context
SafeDisposable** mkAllArr() {
	SafeDisposable** d = new SafeDisposable * [allSafeDisposablesCount];
	memset(d, 0, sizeof(SafeDisposable*) * allSafeDisposablesCount);
	return d;
}
SafeDisposable** allSafeDisposables = nullptr;

void SafeDisposable::disposeAll()
{
	if (allSafeDisposables) {
		for (int i = 0; i < allSafeDisposablesCount; ++i) if(allSafeDisposables[i]) allSafeDisposables[i]->disposeContents();
		allSafeDisposables = nullptr;
	}
}

SafeDisposable::SafeDisposable()
{
	if (!allSafeDisposables) allSafeDisposables = mkAllArr();

	for (int i = 0; i < allSafeDisposablesCount; ++i) {
		if (!allSafeDisposables[i]) {
			allSafeDisposables[i] = this;
			return;
		}
	}
	
	assert(false);
}

SafeDisposable::~SafeDisposable()
{
	if (allSafeDisposables) {
		for (int i = 0; i < allSafeDisposablesCount; ++i) if (allSafeDisposables[i] == this) allSafeDisposables[i] = nullptr;
	}
}
