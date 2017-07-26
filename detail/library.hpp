#pragma once
#include <cstdio>

namespace OBJC_NAMESPACE {

    extern "C" {

        enum : int { RTLD_LAZY = 0x1, RTLD_LOCAL = 0x4 };

        void* dlopen(const char*,int);

        void dlclose(void*);

        const char* dlerror(void);

        void* dlsym(void*,const char*);

    } // extern "C"

    struct library final {

        struct symbol_t {
            void* const address = nullptr;

            symbol_t(void* address) : address(address) {}

            explicit operator bool() const { return address != nullptr; }

            template<typename T> operator T*() const { return (T*)address; }

            symbol_t operator *() const { return {*(void**)address}; }
        };

        template<typename>
        struct function_t;

        template<typename Result, typename... Args>
        struct function_t<Result(Args...)> {
            using pointer = Result(*)(Args...);
        };

        void* const address = nullptr;

        library() = default;

        library(decltype(nullptr)) : library() {}

        library(const char* path, int mode = RTLD_LAZY | RTLD_LOCAL)
        : address(dlopen(path,mode))
        {
            if (not address) {
                fprintf(stderr,"library \"%s\" not found\n",path);
            }
            // else { printf("library \"%s\" @ %p\n",path,address); }
        }

        library(library&& rhs) : address(rhs.address) { new(&rhs) library(); }

        library& operator =(library&& rhs) {
            if (this != &rhs) { new(this) library(std::move(rhs)); }
            return *this;
        }

        ~library() { if (address) { dlclose(address); } }

        explicit operator bool() const { return address != nullptr; }

        symbol_t operator ()(const char* name) const { return symbol(name); }

        symbol_t constant(const char* name) const { return *symbol(name); }

        symbol_t function(const char* name) const { return symbol(name); }

        symbol_t symbol(const char* name) const {
            symbol_t symbol { address ? dlsym(address,name) : nullptr };
            if (not symbol) {
                fprintf(stderr,"symbol \"%s\" not found\n",name);
            }
            // else { printf("symbol \"%s\" @ %p\n",name,sym.address); }
            return symbol;
        }

    };

} // namespace OBJC_NAMESPACE