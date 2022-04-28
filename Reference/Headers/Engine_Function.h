#pragma once

namespace Engine
{
	template <typename T>
	void Safe_Delete(T& pInstance)
	{
		if (nullptr != pInstance)
		{
			delete pInstance;
			pInstance = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pInstance)
	{
		if (nullptr != pInstance)
		{
			delete[] pInstance;
			pInstance = nullptr;
		}
	}


	template <typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();
			if (0 == dwRefCnt)
				pInstance = nullptr;
		}
		return dwRefCnt;
	}
}

