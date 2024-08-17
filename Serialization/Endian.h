#pragma once

inline std::uint16_t ByteSwap2(std::uint16_t data)
{
	return (data >> 8) | (data << 8);
}

inline std::uint32_t ByteSwap4(std::uint32_t data)
{
	return ((data >> 24) & 0x000000FF) |
		((data >> 8) & 0x0000FF00) |
		((data << 8) & 0x00FF0000) |
		((data << 24) & 0xFF000000);
}

inline std::uint64_t ByteSwap8(std::uint64_t data)
{
	return	((data >> 56) & 0x00000000000000FF) |
		((data >> 40) & 0x000000000000FF00) |
		((data >> 24) & 0x0000000000FF0000) |
		((data >> 8) & 0x00000000FF000000) |
		((data << 8) & 0x000000FF00000000) |
		((data << 24) & 0x0000FF0000000000) |
		((data << 40) & 0x00FF000000000000) |
		((data << 56) & 0xFF00000000000000);
}

template <typename tFrom, typename tTo>
class TypeAliaser
{
public:
	TypeAliaser(tFrom from) : m_fromType{ from } {}
	tTo& Get() { return m_toType; }

private:
	union
	{
		tFrom m_fromType;
		tTo m_toType;
	};
};

template<typename T, size_t tSize> class ByteSwapper;	//나중에 작성할 템플릿 클래스 특수화를 위해서 선언만 함

template<typename T>
class ByteSwapper<T, 2>
{
public:
	T Swap(T data) const
	{
		std::uint16_t result = ByteSwap2(TypeAliaser<T, std::uint16_t>(data).Get());
		return TypeAliaser<std::uint16_t, T>(result).Get();
	}
};

template<typename T>
class ByteSwapper<T, 4>
{
public:
	T Swap(T data) const
	{
		std::uint32_t result = ByteSwap4(TypeAliaser<T, std::uint32_t>(data).Get());
		return TypeAliaser<std::uint32_t, T>(result).Get();
	}
};

template<typename T>
class ByteSwapper<T, 8>
{
public:
	T Swap(T data) const
	{
		std::uint64_t result = ByteSwap8(TypeAliaser<T, std::uint64_t>(data).Get());
		return TypeAliaser<std::uint64_t, T>(result).Get();
	}
};

template<typename T>
T ByteSwap(T data)
{
	return ByteSwapper<T, sizeof(T)>().Swap(data);
}