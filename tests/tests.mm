
#include <iostream>
#include "../objc"
#include "echo.hpp"

//------------------------------------------------------------------------------

struct small_struct {
    float a;
};

std::ostream& operator <<(std::ostream& o, const small_struct s) {
    return o << "{ a:"<<s.a<<" }";
}

//------------------------------------------------------------------------------

struct medium_struct {
    float a, b, c, d;
};

std::ostream& operator <<(std::ostream& o, const medium_struct s) {
    return o << "{"
        <<" a:"<<s.a<<", b:"<<s.b<<", c:"<<s.c<<", d:"<<s.d
    <<" }";
}

//------------------------------------------------------------------------------

struct large_struct {
    float a, b, c, d, e, f, g, h;
};

std::ostream& operator <<(std::ostream& o, const large_struct s) {
    return o << "{"
        <<" a:"<<s.a<<", b:"<<s.b<<", c:"<<s.c<<", d:"<<s.d
        <<" e:"<<s.e<<", f:"<<s.f<<", g:"<<s.g<<", h:"<<s.h
    <<" }";
}

//------------------------------------------------------------------------------

@interface BaseClass : NSObject {
    int intVariable;
}

    -(long double)getLongDouble;

    -(double)getDouble;

    -(float)getFloat;

    -(small_struct)getSmallStruct;

    -(medium_struct)getMediumStruct;

    -(large_struct)getLargeStruct;

    -(void)method;

    -(void)
    methodWithInt:(int)a
    float32:(float)b
    float64:(double)c
    float128:(long double)d
    smallStruct:(small_struct)e
    mediumStruct:(medium_struct)f
    largeStruct:(large_struct)g
    andPointerToInt:(int*)h;

@end

@implementation BaseClass

    -(long double)getLongDouble { return 0.12l; }

    -(double)getDouble { return 1.23; }

    -(float)getFloat { return 2.34f; }

    -(small_struct)getSmallStruct {
        return { 0.f };
    }

    -(medium_struct)getMediumStruct {
        return { 0.f, 1.f, 2.f, 3.f };
    }

    -(large_struct)getLargeStruct {
        return { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f };
    }

    -(void)method {
        std::cout << __func__ << "()\n";
    }

    -(void)
    methodWithInt:(int)a
    float32:(float)b
    float64:(double)c
    float128:(long double)d
    smallStruct:(small_struct)e
    mediumStruct:(medium_struct)f
    largeStruct:(large_struct)g
    andPointerToInt:(int*)h {
        std::cout
        <<__func__<<"(\n"
        <<"    a:"<<a<<", b:"<<b<<", c:"<<c<<", d:"<<d<<",\n"
        <<"    e:"<<e<<",\n"
        <<"    f:"<<f<<",\n"
        <<"    g:"<<g<<",\n"
        <<"    h:"<<h<<",\n"
        <<")\n";
    }

@end

//------------------------------------------------------------------------------

@interface DerivedClass : BaseClass {
    float floatVariable;
}

    -(long double)getLongDouble;

    -(double)getDouble;

    -(float)getFloat;

    -(small_struct)getSmallStruct;

    -(medium_struct)getMediumStruct;

    -(large_struct)getLargeStruct;

    -(void)method;

    -(void)
    methodWithInt:(int)a
    float32:(float)b
    float64:(double)c
    float128:(long double)d
    smallStruct:(small_struct)e
    mediumStruct:(medium_struct)f
    largeStruct:(large_struct)g
    andPointerToInt:(int*)h;

@end

@implementation DerivedClass

    -(id)init {
        self = [super init];
        self->intVariable = 1;
        self->floatVariable = 1.f;
        return self;
    }

    -(long double)getLongDouble { return 1.23l; }

    -(double)getDouble { return 2.34; }

    -(float)getFloat { return 3.45f; }

    -(small_struct)getSmallStruct {
        return { 1.f };
    }

    -(medium_struct)getMediumStruct {
        return { 1.f, 2.f, 3.f, 4.f };
    }

    -(large_struct)getLargeStruct {
        return { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f };
    }

    -(void)method {
        std::cout << __func__ << '\n';
    }

    -(void)
    methodWithInt:(int)a
    float32:(float)b
    float64:(double)c
    float128:(long double)d
    smallStruct:(small_struct)e
    mediumStruct:(medium_struct)f
    largeStruct:(large_struct)g
    andPointerToInt:(int*)h {
        std::cout
        <<__func__<<"(\n"
        <<"    a:"<<a<<", b:"<<b<<", c:"<<c<<", d:"<<d<<",\n"
        <<"    e:"<<e<<",\n"
        <<"    f:"<<f<<",\n"
        <<"    g:"<<g<<",\n"
        <<"    h:"<<h<<",\n"
        <<")\n";
    }

@end

//------------------------------------------------------------------------------

objc::class_definition
CustomClass {
    "CustomClass","DerivedClass",

    objc::protocol("NSApplicationDelegate"),

    objc::member<double>("doubleVariable"),

    objc::method(
        "method",[](id,SEL){
            std::cout << "-[CustomClass method]\n";
        }
    ),

    objc::method(
        "methodWithInt:"
        "float32:"
        "float64:"
        "float128:"
        "smallStruct:"
        "mediumStruct:"
        "largeStruct:"
        "andPointerToInt:",
        [](id,SEL,
            int a,
            float b,
            double c,
            long double d,
            small_struct e,
            medium_struct f,
            large_struct g,
            int* h
        ) {
            std::cout
            <<"-[CustomClass methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:](\n"
            <<"    a:"<<a<<", b:"<<b<<", c:"<<c<<", d:"<<d<<",\n"
            <<"    e:"<<e<<",\n"
            <<"    f:"<<f<<",\n"
            <<"    g:"<<g<<",\n"
            <<"    h:"<<h<<",\n"
            <<")\n";
        }
    )
};

//------------------------------------------------------------------------------

int main() {
    const objc::autoreleasepool autoreleasepool;

    const objc::unique_ptr obj = objc::make_unique(CustomClass);
    echo(objc::get_retain_count(obj));

    const objc::member_accessor<int>
    intVariable { obj, "intVariable" };
    echo(intVariable(obj));
    intVariable(obj) = 2;
    echo(intVariable(obj));

    const objc::member_accessor<float>
    floatVariable { obj, "floatVariable" };
    echo(floatVariable(obj));
    floatVariable(obj) = 2.f;
    echo(floatVariable(obj));

    const objc::member_accessor<double>
    doubleVariable { obj, "doubleVariable" };
    echo(doubleVariable(obj));
    doubleVariable(obj) = 3.0;
    echo(doubleVariable(obj));

    const objc::selector
    methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt {
        "methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:"
    };

    const objc::method_implementation
    <void(int,float,double,long double,small_struct,medium_struct,large_struct,int*)>
    CustomClass_methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt {
        CustomClass,
        "methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:"
    };

    const objc::method_interface
    <void(int,float,double,long double,small_struct,medium_struct,large_struct,int*)>
    virtual_methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt {
        "methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:"
    };

    std::cout << '\n';
    echo(objc::call<long double>(objc::super(obj),"getLongDouble"));
    echo(objc::call<double>(objc::super(obj),"getDouble"));
    echo(objc::call<float>(objc::super(obj),"getFloat"));
    echo(objc::call<small_struct>(objc::super(obj),"getSmallStruct"));
    echo(objc::call<medium_struct>(objc::super(obj),"getMediumStruct"));
    echo(objc::call<large_struct>(objc::super(obj),"getLargeStruct"));
    objc::call<void>(objc::super(obj),"method");
    objc::call<void>(objc::super(obj),
        methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt,
        1,
        2.f,
        3.0,
        4.0L,
        small_struct{1.f},
        medium_struct{1.f,2.f,3.f,4.f},
        large_struct{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f},
        nullptr
    );

    std::cout << '\n';
    echo(objc::call<long double>(obj,"getLongDouble"));
    echo(objc::call<double>(obj,"getDouble"));
    echo(objc::call<float>(obj,"getFloat"));
    echo(objc::call<small_struct>(obj,"getSmallStruct"));
    echo(objc::call<medium_struct>(obj,"getMediumStruct"));
    echo(objc::call<large_struct>(obj,"getLargeStruct"));
    objc::call<void>(obj,"method");
    objc::call<void>(obj,
        methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt,
        1,
        2.f,
        3.0,
        4.0L,
        small_struct{1.f},
        medium_struct{1.f,2.f,3.f,4.f},
        large_struct{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f},
        nullptr
    );
    CustomClass_methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt(
        obj,
        1,
        2.f,
        3.0,
        4.0L,
        small_struct{1.f},
        medium_struct{1.f,2.f,3.f,4.f},
        large_struct{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f},
        nullptr
    );
    virtual_methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt(
        obj,
        1,
        2.f,
        3.0,
        4.0L,
        small_struct{1.f},
        medium_struct{1.f,2.f,3.f,4.f},
        large_struct{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f},
        nullptr
    );

    return 0;
}