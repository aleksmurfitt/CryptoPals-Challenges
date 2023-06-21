#if !defined(AM_CRYPTO_HEX_HPP)
    #define AM_CRYPTO_HEX_HPP

    #include "provider.hpp"

    #include <concepts>
    #include <string>
namespace am::crypto {
template <typename T> class hex {
    std::string data;
    static inline const char *hex_chars = "0123456789ABCDEF";

    static std::string encode(const T &object) {
        std::string result;
        auto buffer = provider(object);

        result.reserve(buffer.size() * 2);

        for (const auto e : buffer) {
            result.push_back(hex_chars[(e & 0xF0) >> 4]);
            result.push_back(hex_chars[e & 0x0F]);
        }
        return result;
    }

    T decode() const {
        T result;
        auto buffer = provider(result);
        auto it = std::begin(buffer);
        const auto to_byte = [](char c) {
            if (c >= '0' && c <= '9')
                return c - '0';
            else
                return c - 'A' + 10;
        };

        for (int i = 0; i < data.size() / 2; ++i)
            *it++ = to_byte(data[2 * i]) << 4 | to_byte(data[2 * i + 1]);

        return result;
    }


  public:
    explicit hex(const T &data) : data(encode(data)){};

    // This is redundant, but I prefer to be explicit for readability
    hex() = delete;

    /**
     * Deserializes an existing hexadecimal string into a hex object.
     * Throws on failure.
     * @param data
     * @return
     */
    static hex from_string(std::string const &data) {
        auto ret = hex();
        ret.data = data;
        return ret;
    }

    explicit operator std::string() const {
        return data;
    }
    explicit operator T() const {
        return decode();
    }
}; // class hex

} // namespace am::crypto

#endif // AM_CRYPTO_HEX_HPP
