#include "details/ForwardDecl.h"

namespace Ubpa {
	template<typename Impl, typename Data, typename Value>
	class TLoopIteratorBase {
	public:
		using value_type = Value;
		using difference_type = std::ptrdiff_t;
		using pointer = std::add_pointer_t<value_type>;
		using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using iterator_category = std::forward_iterator_tag;

		constexpr TLoopIteratorBase(Data data, bool is_begin) noexcept : data{ data }, is_begin{ is_begin } {}
		constexpr TLoopIteratorBase() noexcept : data{ nullptr }, is_begin{ false } {}
		friend constexpr bool operator==(const Impl& lhs, const Impl& rhs) noexcept { return lhs.data == rhs.data && lhs.is_begin == rhs.is_begin; }
		friend constexpr bool operator!=(const Impl &lhs, const Impl &rhs) noexcept { return !(lhs == rhs); }
		value_type operator->() const noexcept { return Impl::ToValue(data); }
		value_type operator*() const noexcept { return Impl::ToValue(data); }
		Impl& operator++() noexcept { data = Impl::Next(data); is_begin = false; return *static_cast<Impl*>(this); }
		Impl operator++(int) noexcept { Impl rst = *static_cast<Impl*>(this); this->operator++(); return rst; }
	private:
		Data data;
		bool is_begin;
	};

	template<typename Impl, typename Data, typename Value>
	class TLoopViewBase {
	public:
		class Iterator : public TLoopIteratorBase<Iterator, Data, Value> {
		public:
			using TLoopIteratorBase<Iterator, Data, Value>::TLoopIteratorBase;
		private:
			friend TLoopIteratorBase<Iterator, Data, Value>;
			static Value ToValue(Data data) noexcept { return Impl::ToValue(data); }
			static Data Next(Data data) noexcept { return Impl::Next(data); }
		};

		constexpr TLoopViewBase(Data begin_value, Data end_value) noexcept :
			begin_value{ begin_value }, end_value{ end_value } {}
		constexpr TLoopViewBase() noexcept :
			begin_value{ nullptr }, end_value{ nullptr }{}

		Iterator begin() const noexcept { return begin_value ? Iterator{ begin_value, true } : Iterator{}; }
		Iterator end() const noexcept { return { end_value, false }; }

	private:
		Data begin_value;
		Data end_value;
	};
}
