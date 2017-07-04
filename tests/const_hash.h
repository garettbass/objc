#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string>

namespace xtd {

    //==========================================================================

    class const_hash
    {
    public: // constants

        enum : uint32_t { DEFAULT_SEED = 0 };

    private: // constants

        enum : uint32_t {
            C1 = 0xcc9e2d51,
            C2 = 0x1b873593,
            C3 = 0xe6546b69,
            M1 = 0x85ebca6b,
            M2 = 0xc2b2ae35,
        };

    private: // fields

        const char* _data;
        size_t      _size;

    public: // structors

        constexpr const_hash(const char* data, size_t size)
        : _data(data)
        , _size(size)
        {}

        template<size_t size>
        constexpr const_hash(const char (&data)[size])
        : _data(data)
        , _size(size)
        {}

    public: // operators

        constexpr operator uint32_t() const { return hash32(); }

        constexpr operator uint64_t() const { return hash64(); }

    public: // methods

        constexpr uint32_t hash32(uint32_t seed = DEFAULT_SEED) const {
            return
                finalize(
                    _size,
                    hash_tail_quad(
                        tailQuad(),
                        hash_body_quads(*this, 0, seed)));
        }

        constexpr uint64_t hash64(uint32_t seed = DEFAULT_SEED) const {
            return (uint64_t(hash32(seed)) << 32) + hash32(~seed);
        }

    private: // implementation

        constexpr uint8_t byte(size_t i) const { return uint8_t(_data[i]); }

        constexpr size_t bodyQuadCount() const { return _size / 4; }

        constexpr uint32_t bodyQuad(size_t i) const { return quad4(i); }

        constexpr uint32_t tailQuad() const {
            return (tailByteCount() == 3)
                 ? quad3(bodyQuadCount())
                 : (tailByteCount() == 2)
                 ? quad2(bodyQuadCount())
                 : (tailByteCount() == 1)
                 ? quad1(bodyQuadCount())
                 : 0;
        }

        constexpr size_t tailByteCount() const {
            return _size - (bodyQuadCount() * 4);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr uint32_t quad4(size_t i) const {
            return
                byte(4 * i + 0) <<  0 |
                byte(4 * i + 1) <<  8 |
                byte(4 * i + 2) << 16 |
                byte(4 * i + 3) << 24;
        }

        constexpr uint32_t quad3(size_t i) const {
            return
                byte(4 * i + 0) <<  0 |
                byte(4 * i + 1) <<  8 |
                byte(4 * i + 2) << 16;
        }

        constexpr uint32_t quad2(size_t i) const {
            return
                byte(4 * i + 0) << 0 |
                byte(4 * i + 1) << 8;
        }

        constexpr uint32_t quad1(size_t i) const {
            return
                byte(4 * i + 0) << 0;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t hash_body_quads(
            const const_hash data,
            const size_t     quadIndex,
            const uint32_t   hash
        ) {
            return
                (quadIndex == data.bodyQuadCount())
                ? hash
                : hash_body_quads(
                    data,
                    quadIndex + 1,
                    hash_body_quad(
                        data.bodyQuad(quadIndex),
                        hash));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t hash_body_quad(
            const uint32_t quad,
            const uint32_t hash)
        {
            return ((rotl32((hash ^ (rotl32((quad * C1), 15) * C2)), 13)) * C3);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t hash_tail_quad(
            const uint32_t quad,
            const uint32_t hash)
        {
            return (hash ^ (rotl32((quad * C1), 15) * C2));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t finalize(
            const size_t   size,
            const uint32_t hash)
        {
            return fmix32(hash ^ uint32_t(size));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t shift_xor(uint32_t h, uint32_t shift)
        {
            return ((h >> shift) ^ h);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t fmix32(uint32_t h)
        {
            return shift_xor((shift_xor((shift_xor(h, 16) * M1), 13) * M2), 16);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        constexpr static uint32_t rotl32(uint32_t x, uint32_t k)
        {
            return ((x << k) | (x >> (32 - k)));
        }

    };

    //--------------------------------------------------------------------------

    constexpr
    const_hash operator "" _hash(const char* data, size_t size)
    {
        return const_hash(data, size);
    }

    //--------------------------------------------------------------------------

    inline
    constexpr
    uint32_t const_hash32(
        const char* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash(data, size).hash32(seed);
    }

    template <size_t SIZE>
    constexpr
    uint32_t const_hash32(
        const char (&data)[SIZE],
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash32(data, SIZE, seed);
    }

    inline
    constexpr
    uint32_t const_hash32(
        const void* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash32((const char*)data, size, seed);
    }

    constexpr
    uint32_t operator "" _hash32(const char* data, size_t size)
    {
        return const_hash32(data, size);
    }

    //--------------------------------------------------------------------------

    inline
    constexpr
    uint64_t const_hash64(
        const char* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash(data, size).hash64(seed);
    }

    template <size_t SIZE>
    constexpr uint64_t const_hash64(
        const char (&data)[SIZE],
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash64(data, SIZE, seed);
    }

    inline
    constexpr
    uint64_t const_hash64(
        const void* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash64((const char*)data, size, seed);
    }

    constexpr
    uint64_t operator "" _hash64(const char* data, size_t size)
    {
        return const_hash64(data, size);
    }

    //--------------------------------------------------------------------------

    inline
    constexpr
    size_t const_hash_size_t(
        const char* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return size_t(const_hash(data, size).hash64(seed));
    }

    template <size_t SIZE>
    constexpr size_t const_hash_size_t(
        const char (&data)[SIZE],
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash_size_t(data, SIZE, seed);
    }

    inline
    constexpr
    size_t const_hash_size_t(
        const void* data,
        size_t      size,
        uint32_t    seed = const_hash::DEFAULT_SEED)
    {
        return const_hash_size_t((const char*)data, size, seed);
    }

    constexpr
    size_t operator "" _hash_size_t(const char* data, size_t size)
    {
        return const_hash_size_t(data, size);
    }

    //==========================================================================

} // namespace xtd