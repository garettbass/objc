
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

objc::class_id
CustomClass {
    "CustomClass","DerivedClass",

    objc::protocol("NSApplicationDelegate"),

    objc::variable<double>("doubleVariable"),

    objc::method(
        "init",[](id self,SEL sel){
            self = objc::message<id()>{sel}(objc::super(self));
            objc::accessor<double>(self,"doubleVariable")(self) = 1.0;
            return self;
        }
    ),

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

    const objc::accessor<int>
    intVariable { obj, "intVariable" };
    echo(intVariable(obj));
    intVariable(obj) = 2;
    echo(intVariable(obj));

    const objc::accessor<float>
    floatVariable { obj, "floatVariable" };
    echo(floatVariable(obj));
    floatVariable(obj) = 2.f;
    echo(floatVariable(obj));

    const objc::accessor<double>
    doubleVariable { obj, "doubleVariable" };
    echo(doubleVariable(obj));
    doubleVariable(obj) = 3.0;
    echo(doubleVariable(obj));

    objc::message
    <long double()>
    getLongDouble { "getLongDouble" };

    objc::message
    <double()>
    getDouble { "getDouble" };

    objc::message
    <float()>
    getFloat { "getFloat" };

    objc::message
    <small_struct()>
    getSmallStruct { "getSmallStruct" };

    objc::message
    <medium_struct()>
    getMediumStruct { "getMediumStruct" };

    objc::message
    <large_struct()>
    getLargeStruct { "getLargeStruct" };

    objc::message
    <void()>
    method { "method" };

    objc::message
    <void(int,float,double,long double,small_struct,medium_struct,large_struct,int*)>
    methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt {
        "methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:"
    };

    objc::implementation
    <void(int,float,double,long double,small_struct,medium_struct,large_struct,int*)>
    CustomClass_methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt {
        CustomClass,
        "methodWithInt:float32:float64:float128:smallStruct:mediumStruct:largeStruct:andPointerToInt:"
    };

    std::cout << '\n';
    const auto super = objc::super(obj);
    echo(getLongDouble(super));
    echo(getDouble(super));
    echo(getFloat(super));
    echo(getSmallStruct(super));
    echo(getMediumStruct(super));
    echo(getLargeStruct(super));
    method(super);
    methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt(
        super,
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
    echo(getLongDouble(obj));
    echo(getDouble(obj));
    echo(getFloat(obj));
    echo(getSmallStruct(obj));
    echo(getMediumStruct(obj));
    echo(getLargeStruct(obj));
    method(obj);
    methodWithInt_float32_float64_float128_smallStruct_mediumStruct_largeStruct_andPointerToInt(
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

    return 0;
}