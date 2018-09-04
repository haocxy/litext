#pragma once

#include <bitset>
#include <cstring>


template <int Bits>
class FlagSet
{
public:
	template <typename ...T>
	FlagSet(int flag, T ...flags):FlagSet(flags...)
	{
		set(flag);
	}

	FlagSet()
	{
#ifndef NDEBUG
		std::memset(m_debug_val, 0, sizeof(m_debug_val));
#endif
	}

	bool has(int n) const
	{
		return m_bitset.test(n);
	}

	void set(int n)
	{
		m_bitset.set(n);
#ifndef NDEBUG
		m_debug_val[n] = 1;
#endif
	}

	void unset(int n)
	{
		m_bitset.set(n, false);
#ifndef NDEBUG
		m_debug_val[n] = 0;
#endif
	}

	void clear()
	{
		m_bitset.reset();
#ifndef NDEBUG
		std::memset(m_debug_val, 0, sizeof(m_debug_val));
#endif
	}

private:
	std::bitset<Bits> m_bitset;

#ifndef NDEBUG
	short m_debug_val[Bits];
#endif

};
