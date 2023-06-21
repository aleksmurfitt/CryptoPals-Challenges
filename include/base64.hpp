#if !defined(AM_CRYPTO_BASE64_HPP)
    #define AM_CRYPTO_BASE64_HPP

    #include "provider.hpp"

    #include <concepts>
    #include <string>
namespace am::crypto {

template <typename T> class base64 {
    std::string data;
    static inline const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    static std::string encode(T const &data) {
        std::string result;
        size_t size = sizeof(data);
        auto *ptr = reinterpret_cast<const unsigned char *>(const_cast<T *>(&data));

        result.reserve(size * 4 / 3);

        for (int i = 0; i + 3 < size; i += 3) {
            result.push_back(base64_chars[(ptr[i] & 0xFC) >> 2]);
            result.push_back(base64_chars[((ptr[i] & 0x03) << 4) | ((ptr[i + 1] & 0xF0) >> 4)]);
            result.push_back(base64_chars[((ptr[i + 1] & 0x0F) << 2) | ((ptr[i + 2] & 0xC0) >> 6)]);
            result.push_back(base64_chars[ptr[i + 2] & 0x3F]);
        }

        if (size % 3 == 1) {
            result.push_back(base64_chars[(ptr[size - 1] & 0xFC) >> 2]);
            result.push_back(base64_chars[(ptr[size - 1] & 0x03) << 4]);
            result.push_back('=');
            result.push_back('=');
        } else if (size % 3 == 2) {
            result.push_back(base64_chars[(ptr[size - 2] & 0xFC) >> 2]);
            result.push_back(base64_chars[((ptr[size - 2] & 0x03) << 4) | ((ptr[size - 1] & 0xF0) >> 4)]);
            result.push_back(base64_chars[(ptr[size - 1] & 0x0F) << 2]);
            result.push_back('=');
        }
        return result;
    }

    T decode() const {
        T result;
        auto *ptr = reinterpret_cast<unsigned char *>(&result);

        const auto to_byte = [](char c) {
            if (c >= 'A' && c <= 'Z')
                return c - 'A';
            if (c >= 'a' && c <= 'z')
                return c - 'a' + 26;
            if (c >= '0' && c <= '9')
                return c - '0' + 52;
            if (c == '+')
                return 62;
            if (c == '/')
                return 63;
            return 0;
        };

        for (int i = 0; i < data.size() / 4; ++i) {
            *ptr++ = to_byte(data[4 * i]) << 2 | to_byte(data[4 * i + 1]) >> 4;
            *ptr++ = to_byte(data[4 * i + 1]) << 4 | to_byte(data[4 * i + 2]) >> 2;
            *ptr++ = to_byte(data[4 * i + 2]) << 6 | to_byte(data[4 * i + 3]);
        }

        if (data.size() > 0 && data[data.size() - 2] == '=') {
            *ptr++ = to_byte(data[data.size() - 4]) << 2 | to_byte(data[data.size() - 3]) >> 4;
        } else if (data.size() > 1 && data[data.size() - 1] == '=') {
            *ptr++ = to_byte(data[data.size() - 4]) << 2 | to_byte(data[data.size() - 3]) >> 4;
            *ptr++ = to_byte(data[data.size() - 3]) << 4 | to_byte(data[data.size() - 2]) >> 2;
        }
        return result;
    }

    base64() = default;

  public:
    explicit base64(T const &data) : data(encode(data)) { }
    static base64 from_string(std::string const &data) {
        auto ret = base64();
        ret.data = encode(data);
        return ret;
    }

    operator std::string() const {
        return data;
    }
    operator T() const {
        return decode();
    }
}; // class base64

} // namespace am::crypto

#endif // AM_CRYPTO_BASE64_HPP
