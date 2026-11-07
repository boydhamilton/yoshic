

#pragma once

#include<cstddef>
#include<cstdlib>
#include<memory>
#include<new>

/*
ensure things are next to each other in memory so cpu cache hits
*/

class ArenaAllocator {

    public: 
        inline explicit ArenaAllocator(size_t bytes)
            : m_size(bytes)
        {
            m_buffer = (std::byte*)malloc(m_size);
            m_offset = m_buffer;
        }


        template<typename T> inline T* alloc(){
            // align for bit ol 64 bit types
            std::size_t space = m_size - (m_offset - m_buffer);
            void* ptr = m_offset;

            if (!std::align(alignof(T), sizeof(T), ptr, space)) {
                return nullptr; // out of memory
            }

            m_offset = static_cast<std::byte*>(ptr) + sizeof(T);
            // placement-new: the buffer is raw malloc'd bytes, so T must
            // actually be constructed here. Without this, members like
            // std::optional/std::variant/std::vector start out as
            // whatever bytes happened to be at this address (fresh mmap
            // pages read as zero, so it looks fine on a single run - but
            // once the arena's memory is reused across repeated
            // allocations, as in a benchmark loop, those bytes are stale
            // and assigning into them is undefined behavior).
            return new (ptr) T();
        }

        inline void reset(){
            m_offset = m_buffer;
        }

       inline ~ArenaAllocator(){
            free(m_buffer);
       }

        // move-only: the arena owns a single malloc'd buffer, so copying it
        // would leave two owners of the same memory (double free on the
        // second destructor, use-after-free on whichever dies first). A
        // Parser holds one of these by value, and needs to be movable so
        // its AST can safely be handed to a caller that outlives it (see
        // ParseResult in game.hpp) - so move, rather than delete outright.
        inline ArenaAllocator(ArenaAllocator&& other) noexcept
            : m_size(other.m_size), m_buffer(other.m_buffer), m_offset(other.m_offset)
        {
            other.m_buffer = nullptr;
            other.m_offset = nullptr;
            other.m_size = 0;
        }

        inline ArenaAllocator& operator=(ArenaAllocator&& other) noexcept
        {
            if (this != &other) {
                free(m_buffer);
                m_size = other.m_size;
                m_buffer = other.m_buffer;
                m_offset = other.m_offset;
                other.m_buffer = nullptr;
                other.m_offset = nullptr;
                other.m_size = 0;
            }
            return *this;
        }

        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

    private:
        size_t m_size;
        std::byte* m_buffer;
        std::byte* m_offset; // pointer where we are through the piece of memory; always set by whichever ctor runs
};