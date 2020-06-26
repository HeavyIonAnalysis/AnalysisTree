# Contribution Guide 

https://google.github.io/styleguide/cppguide.html

## Doxygen documentation
Always document your code, e. g. put at least a brief class description

    /** @file MyClass.h
    @author John Snow
    @date 01.01.1991
    @class MyClass
    @brief A class for ...
    */

Methods description

    /**
    * Do something
    * @param par1 first parameter description
    * @return something
    */
    int foo(int par1)

Members description

    int var; ///< description after the member

## Namespaces

AnalysisTree classes use a namespace:

* AnalysisTree

## Include Files

In header files only use include which MUST be there. Unnecessary includes increase the compilation time. Most of the time all necessary includes can be moved to the implementation.

To avoid including header files more than once, use the preprocessor macro _nsp_ClassName_h_ to mark a header as already included. Usage of these inclusion guards is nonnegotiable.

    // file: .../include/nsp/ClassName.h

    #ifndef _nsp_ClassName_h_
    #define _nsp_ClassName_h_

    class ClassName {
    };

    #endif

Since with the introduction of namespaces, a class can exist in more than one namespace, we can have header files with the same name to define the same class. We have to add the namespace both to the path for locating the file and to the preprocessor macro preventing multiple inclusion of a header.

## Pass by const reference
* Complex objects should be passed by reference. 
* Don’t use it for simple types (int, float, . . . ), it can be an overhead
* Prefer over pointer: This makes sure you get a valid object
    
    void func(const myClass& c); // good
    void func(const myClass* c); // ok, if you know why (needed only in rare cases)
    void func(myClass c); // not so good, unless you know why

## Pointers
 * Only use pointer if you have a strong reason!
 * If you use objects on the stack you don’t have to care about deletion
 * If you need pointers, prefer smart pointers over plain pointers

## Using namespace statement

Due to the global effect on all code that includes the particular header, using and using namespace statement should never be given in header files. NEVER do that in header:
    
    using namespace std;

This means all files including your header will have the std namespace. You could do it in the implementation, not considered nice style. Prefer importing only single function, only do it in your implementation
    
    using std::cout;

Furthermore, since using namespace statement has an immediate effect also outside of the file scope, in .cpp files all such statements should come after all includes.
You could also define an alias, this can even be done locally in a function body

    using cout = std::cout;

consider using modern ‘typedef’

    using vecType = std::vector<int>;
    typedef std::vector<int> vecType

## Abbreviations

In general, avoid abbreviations in variable and class names.

# Const correctness and constness issues
It is of utmost importance that all classes, functions and methods respect the correct and logical use of const modifier. Methods that do not change class data members should be declared const. Function and method parameters should be declared const if they are not intended to be changed inside of the function body. This is even more true when references are passed through the parameters.

In this respect the use of const_cast is strongly discouraged since it can unravel weird compiler bugs.

The rule of a thumb for each programmer should be that first of all a new variable (if possible) is declared not before the value for it is available (i.e. no C-style declarations of variables at the beginning of the code block) and that every new local variable is declared const by default. A forward declaration of a local variable should be used only in special cases when its value will be known only after complex processing (possibly taking place inside of a nested scope).

# NULL vs. nullptr
Note that NULL is a (macro) remainder from C and should be avoided. nullptr is meant as a replacement to NULL. nullptr provides a typesafe pointer value representing an empty (null) pointer. The general rule of thumb that I recommend is that you should start using nullptr whenever you would have used NULL in the past. Even better: avoid usage of pointers completely. Almost everything can be done with references instead and is more bug-resistant.

# Pointer * and reference & placement
The * and & modifiers are part of the type and there thus they belong:

    Core::Track& foo;

# Constructor initializer list
If possible put it directly after member declaration in the header

    Floating_t fE{0}; // track energy in GeV

# Increment, decrement
As default, only pre-increment (++i) and pre-decrement (--i) operators should be used, unless the post-increment/-decrement (i++) feature is explicitly required. The reason is that the post- versions require temporary storage of the previous value of a variable. The compiler will (most probably) optimize such waste away for the built-in types (int etc) but this does not happen for custom code of the various iterators (STL, Offline). The bad habit of placing ++ after the variables can be avoided by forcing yourself to use pre- versions of operators everywhere.

# Forbidden practices
Generally, the C/FORTRAN-hackish style should be avoided. In particular, the following practices are absolutely forbidden.

Do not use fixed size C arrays when variable size arrays are needed, such as

    // A "probably big enough" fixed size array in a place where variable size array would be needed:
    double a[20];
     
Use STL vector instead, such as

    std::vector<double> a(20);
     
Do not use dynamical C arrays, such as

    // Allocate:
    double* a = (double*)calloc(30, sizeof(double));
    // De-allocate:
    free(a);
     
Use STL vector instead, see previous point.
Do not pass large data to function arguments via pointers, such as

    int function(int a, double* array); 

or

    int function(int a, double[] array);
     
Use instead C++ reference:

    int function(int a, std::vector<double>& array);
     
Do not use unnamed constants, such as

    std::vector<double> array(23);
     
Use instead enums so that we know what 23 is:

    // Declared somewhere at the beginning:
    enum ENParameters {
    eNParameters = 23
    };
    // Then, in the code:
    std::vector<double> parameters(eNParameters);




