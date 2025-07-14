# Notes from Compiler Lecture

1. Compile with `-std=c++20 -Wall -Wextra -Wsuggest-override -Werror`
2. Pointer vs Reference: pointer is just an optional reference (can be `null`, but reference cannot)

### Virtual Functions
- If you assign a derived class without a pointer `Shape s = Square{1.0};`, the compiler will "slice" away the components not defined in the `Shape` object. This will result in a linker error, when you try to run a `Square` method, because it is not defined.

- This "slicing" is done because the `Shape` instances uses compiler managed stack memory. Compiler only allocates enough space for the object: not possible for the compiler to allocate memory enough for every single shape. Compiler prevents you from making illegal memory access.

- You can assign the virtual functions to 0: `virtual double GetArea() const = 0;`. This creates a pure virtual class tells the compiler that it belongs to an abstract class and that it should not ever be instantiated as is.

- Virtual functions are pointer to derived class methods using a "vtable". Under the hood, class objects are just `structs` with functions that point to itself. Inheritance works by creating a constructor (or vtable) `struct` like `Square_Init` that assigns pointers to the various derived class methods and variables.

- When using a base class pointer to a derived class object, you must have a `virtual` destructor. Otherwise, there will be memory mismanagement.

- `final` keyword: tells the compiler that this is a final concrete object, there should not be any further derived classes from this. e.g. `class Square final: public Shape {}`

- `override` keyword: tells the compiler that this method intends to override the `virtual` function in the base class.

### Compiler API Design
```cpp
class Node
{
    virtual ~Node() = default;
    virtual void EmitRISC(std::ostream&, Context&) const = 0;
    virtual void Print(std::ostream&) const = 0;
};
```
- `EmitRISC`: can be called to generate work RISC code
- `Print`: debugging -> print out the corresponding AST

- Don't try and shoehorn everything to be a node
- Use type-safe `enum class` for things like `TypeSpecifier` to prevent implicit type casting (to int value): enum useful for representing a finite set of labelled values, better implementation than pure string because it prevents typos and unhandled cases.
- Use specialised objects `Expression` and `Node` to distinguish different kind of functionality: expression pointers can be arithmetically added tgt for example.
- Pointers add in fours ie (ptr_x + 1 => increment ptr_x by 4). Hence in a binary add, need to scale by 4.

### RAII: Resource Allocation Is Initialisation
Shifts the responsibility of resource manamgent from programmer to compiler. Essentially the compiler acquires resources on construction and relinquishes when destroyed or out of scope
- `std::unique_ptr`: automatically relinquishes memory when out of scope. Ideally never use `new` or `delete *` again. you can never copy `unique_ptrs` because the pointer may get destroyed before the second dereferencing.
- Move semantics: How do we pass around `unique_ptrs`? - `&&` is an Rvalue reference: refers to temporary value (not a named variable) and cannot modify the value at memory location, `&` is an Lvalue reference: refers to a named variable. When we want to create a new `unique_ptr` and destroy the old pointer, we can swap ownership with the subsequent pointer using Rvalue reference to ensure we can destroy the old pointer safely. Don't use `shared_ptr`!!
- `std::move` casts Lvalue into an Rvalue. You should not use the old pointer after moving.
- make sure not to leak registers. Transform registers into an object so that it is destroyed naturally when out of scope.
