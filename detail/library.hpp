#pragma once
#include <dlfcn.h>

namespace OBJC_NAMESPACE {

    struct library final {

        struct symbol_t {
            void* const handle = nullptr;

            symbol_t(void* handle) : handle(handle) {}

            explicit operator bool() const { return handle != nullptr; }

            template<typename T> operator T*() const { return (T*)handle; }

            symbol_t operator *() const { return {*(void**)handle}; }
        };

        void* const handle = nullptr;

        library() = default;

        library(decltype(nullptr)) : library() {}

        library(const char* path, int mode = RTLD_NOW | RTLD_LOCAL) :
            handle(dlopen(path,mode))
        {
            if (not handle) {
                const char* error = dlerror();
                printf("library \"%s\" %s\n",path,error?error:"not found");
            }
            // else { printf("library \"%s\" @ %p\n",path,handle); }
        }

        library(library&& rhs) : handle(rhs.handle) { new(&rhs) library(); }

        library& operator =(library&& rhs) {
            if (this != &rhs) { new(this) library(std::move(rhs)); }
            return *this;
        }

        ~library() { if (handle) { dlclose(handle); } }

        explicit operator bool() const { return handle != nullptr; }

        symbol_t operator ()(const char* name) const { return symbol(name); }

        symbol_t constant(const char* name) const { return *symbol(name); }

        symbol_t function(const char* name) const { return symbol(name); }

        symbol_t symbol(const char* name) const {
            symbol_t sym { handle ? dlsym(handle,name) : nullptr };
            if (not sym) { printf("symbol \"%s\" not found\n", name); }
            // else { printf("symbol \"%s\" @ %p\n",name,sym.handle); }
            return sym;
        }

    };

} // namespace OBJC_NAMESPACE