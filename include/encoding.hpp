#if !defined(AM_CRYPTO_HEX_HPP)
    #define AM_CRYPTO_HEX_HPP

    #include "provider.hpp"

    #include <concepts>
    #include <string>
namespace am::crypto {
template <template <typename> typename D, typename T> class encoding {
    std::string data;

    encoding() = default;

  public:
    explicit encoding(D<T> const &data) : data(static_cast<D<T> *>(this)->encode(data)) { }
    static D<T> from_string(std::string const &data) {
        auto ret = D<T>();
        ret.data = data;
        return ret;
    }

    operator std::string() const {
        return data;
    }

    operator T() const {
        return static_cast<D<T> *>(this)->decode();
    }
}; // class encoding
} // namespace am::crypto

#endif // AM_CRYPTO_HEX_HPP