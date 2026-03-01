

#pragma once

#include<cstddef>
#include<cstdlib>
#include<memory>

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
            return new (ptr) T();
        }
        
        inline void reset(){
            m_offset = m_buffer;
        }

       inline ArenaAllocator(const ArenaAllocator& other) = delete;

       inline ~ArenaAllocator(){
            free(m_buffer);
       }

       inline ArenaAllocator operator=(const ArenaAllocator& other) = delete;

    private:
        size_t m_size;
        std::byte* m_buffer;
        std::byte* m_offset = m_buffer; // pointer where we are through the piece of memory
};