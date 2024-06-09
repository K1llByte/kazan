# Code standard

- [Programming patterns](#programming-patterns)
- [Naming convention](#naming-convention)
- [Error handling](#error-handling)


__________________________________________________
## Programming patterns

### CreateInfo struct initialization

Pros:  
- Less verbose than a builder pattern  
- named arguments  

Cons:  
- ...

TBD: Choose a consistent name for the struct (CreateInfo or Params)
```c++
Device({
    .with_validation_layers = true,
})
```

__________________________________________________
## Naming convention

### Macros
- Upper case
```c++
#define FOO_BAR
```


### Classes
- Pascal case
```c++
class FooBar;
```


### Free functions
- Snake case
```c++
auto create_foo();
```


### Variables
- Snake case
#### 
```c++
// Regular
int counter;
// Global
int g_counter;

// Arguments
void foo(bool bar);

// Member variables
class Foo {
    private:
    int m_invisible;
    public:
    int visible;
};
```

__________________________________________________
## Error handling

- Always use exceptions (To be consistent and avoid ignoring exceptions).
- However exceptions are not to be spammed everywhere, their usage must be kept to a minimal.
- Use simple bool or std::optional as return values when possible and where it makes sense.