# Code standard

- [Programming patterns](#programming-patterns)
- [Naming convention](#naming-convention)

__________________________________________________
## Programming patterns

### CreateInfo struct initialization

Pros:  
- Less verbose than a builder pattern  
- named arguments
Cons:  
- 

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

