#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory_resource>
#include <scoped_allocator>


// scc = scoped container
namespace scc
{

template <typename T, typename C = std::pmr::polymorphic_allocator<T>>
using vector = std::vector<T, std::scoped_allocator_adaptor<C>>;

template <typename K, typename V, typename Less = std::less<K>, typename Alloc = std::pmr::polymorphic_allocator<std::pair<const K, V>>>
using map = std::map<K, V, Less, std::scoped_allocator_adaptor<Alloc>>;

template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::pmr::polymorphic_allocator<CharT>>
using basic_string = std::basic_string<CharT, Traits, Alloc>;

using string = basic_string<char>;

using u16string = basic_string<char16_t>;

using u32string = basic_string<char32_t>;

}
