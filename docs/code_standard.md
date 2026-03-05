# Code standard

## Naming convention

### Macros - Upper case
```c++
#define FOO_BAR
```

### Classes and Structs - Pascal case
```c++
class Foo;
struct Bar;
```

### Free functions - Snake case
```c++
Foo create_foo();
```

### Variables - Snake case

```c++
// Regular
int counter;
// Global
int g_counter;

// Arguments
void foo(bool bar);

// Member variables
class Foo {
public:
    int visible;
private:
    int m_invisible;
};
```