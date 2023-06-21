#if !defined(AM_CRYPTO_PROVIDER_HPP)
    #define AM_CRYPTO_PROVIDER_HPP
    #include <concepts>
    #include <ranges>
    #include <span>
    #include <vector>

namespace am::crypto {
template <typename T, typename U> struct copy_const {
    using type = std::conditional_t<std::is_const_v<T>, const U, U>;
};

template <typename T> class provider;

template <typename T>
requires std::is_trivially_copyable_v<T>
class provider<T> {
    T *data;

  public:
    [[nodiscard]] constexpr auto *begin() {
        return reinterpret_cast<typename copy_const<std::remove_pointer_t<T>, unsigned char>::type *>(data);
    };
    [[nodiscard]] constexpr auto *end() const {
        return begin() + sizeof(T);
    };
    [[nodiscard]] constexpr size_t size() const {
        return sizeof(T);
    };

    explicit provider(T &data) : data(&data){};
    provider() : data(nullptr){};
};

template <std::ranges::forward_range T>
requires(!std::is_trivially_copyable_v<T>)
class provider<T> {
    using range_type = std::ranges::range_value_t<T>;

    size_t index = 0;
    const std::ranges::iterator_t<T> begin_it;
    std::ranges::iterator_t<T> current_it;
    const std::ranges::sentinel_t<T> sentinel;


  public:
    using value_type = unsigned char;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = typename std::iterator_traits<std::ranges::iterator_t<T>>::iterator_category;

    provider &operator++() {
        size_t size = provider<range_type>().size();
        if(index == size - 1) {
            ++current_it;
            index = 0;
        }
        else ++index;
        return *this;
    }

    [[nodiscard]] provider operator++(int) & { // lvalue only to avoid bugs like (x++)++
        auto tmp = *this;
        ++this;
        return tmp;
    }

    template <std::bidirectional_iterator V = provider> V &operator--() {
        size_t size = provider<range_type>().size();
        if(index == 0) {
            --current_it;
            index = size - 1;
        }
        else --index;
        return *this;
    }

    template <std::random_access_iterator V = provider> V &operator+=(difference_type n) {
        // Being a consteval method, this doesn't actually create an object.
        size_t size = provider<range_type>().size();
        size_t rem = (n % size);

        std::advance(current_it, n / size + (index + rem >= size ? 1 : 0));
        index = (index + rem) % size;

        return *this;
    }

    template <std::random_access_iterator V = provider> V &operator-=(difference_type n) {
        size_t size = provider<range_type>().size();
        size_t rem = (n % size);

        std::advance(current_it, -(n / size + (index - rem <= 0 ? 1 : 0)));
        index = (index - rem) % size;
        return *this;
    }

    value_type operator*() {
        provider<range_type> tmp{*current_it};
        return *std::next(tmp.begin(), index);
    }

    bool operator==(const provider &rhs) const = default;
    auto operator<=>(const provider &rhs) const = default;

    [[nodiscard]] auto begin() {
        return provider{T{begin_it, sentinel}};
    };
    [[nodiscard]] auto end() {
        return provider{T{sentinel, sentinel}};
    }
    [[nodiscard]] constexpr size_t size() const requires std::ranges::sized_range<const T>{
        size_t size = provider<range_type>().size();
        return std::ranges::size(T{begin_it, sentinel}) * size;
    }

    // Clang-tidy is incorrect here because we explicitly make this constructor invalid for the same type
    template <typename U = T>
    requires(!std::same_as<std::ranges::borrowed_iterator_t<U>, std::ranges::dangling> && !std::same_as<std::remove_cvref<U>, provider<U>>)
    explicit provider(U&& range) : begin_it(std::ranges::begin(range)), current_it(begin_it), sentinel(std::ranges::end(range)) {};
};
template <typename T> provider(T&& t) -> provider<T>;
} // namespace am::crypto
#endif // AM_CRYPTO_PROVIDER_HPP
