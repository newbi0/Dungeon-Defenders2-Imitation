#pragma once

namespace Engine
{

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const _tchar* pTag) : m_pTag(pTag) {}
		virtual ~CTag_Finder(void) {}
	public:
		template <typename T>
		_bool operator () (T& Pair)
		{
			return !lstrcmp(Pair.first, m_pTag);
		}
	private:
		const _tchar*			m_pTag = nullptr;

	};

}