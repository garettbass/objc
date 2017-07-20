#pragma once
#include <dlfcn.h>

namespace OBJC_NAMESPACE {

    struct library final {

        struct symbol_t {
            void* const pointer = nullptr;

            symbol_t(void* pointer) : pointer(pointer) {}

            explicit operator bool() const { return pointer != nullptr; }

            template<typename T> operator T*() const { return (T*)pointer; }

            symbol_t operator *() const { return {*(void**)pointer}; }
        };

        void* const pointer = nullptr;

        library() = default;

        library(decltype(nullptr)) : library() {}

        library(const char* path, int mode = RTLD_NOW | RTLD_LOCAL) :
            pointer(dlopen(path,mode))
        {
            if (not pointer) {
                const char* error = dlerror();
                printf("library \"%s\" %s\n",path,error?error:"not found");
            }
            // else { printf("library \"%s\" @ %p\n",path,pointer); }
        }

        library(library&& rhs) : pointer(rhs.pointer) { new(&rhs) library(); }

        library& operator =(library&& rhs) {
            if (this != &rhs) { new(this) library(std::move(rhs)); }
            return *this;
        }

        ~library() { if (pointer) { dlclose(pointer); } }

        explicit operator bool() const { return pointer != nullptr; }

        symbol_t operator ()(const char* name) const { return symbol(name); }

        symbol_t constant(const char* name) const { return *symbol(name); }

        symbol_t function(const char* name) const { return symbol(name); }

        symbol_t symbol(const char* name) const {
            symbol_t sym { pointer ? dlsym(pointer,name) : nullptr };
            if (not sym) { printf("symbol \"%s\" not found\n", name); }
            // else { printf("symbol \"%s\" @ %p\n",name,sym.pointer); }
            return sym;
        }

    };

} // namespace OBJC_NAMESPACE