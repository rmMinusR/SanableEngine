#include "ThunkCapture.hpp"

thunk_marker_t scanForNearCall(_Fn<void> toScan, _Fn<void> searchFor, thunk_marker_t prevCall)
{
	thunk_marker_t diff = prevCall;
	int32_t relAddr;
	do
	{
		++diff;
		relAddr = searchFor.data-toScan.data-diff-5; //CALL instruction is one byte, plus 4 for relAddr
	} while (memcmp(toScan.data+diff+1, &relAddr, sizeof(relAddr)) != 0);

	return diff;
}

_Fn<void>::ptr_t getNearCallFnPtr(_Fn<void> thunk, thunk_marker_t marker)
{
	int32_t relAddr = *(int32_t*)(thunk.data+marker+1);
	uint8_t* absAddr = thunk.data+marker+relAddr+5;
	return _Fn<void>(absAddr).fn;
}
