# SMPL

> *Statistical Modeling Programming Language*
>
> "Simple"

## Utilities

- [x] Generic growable arrays
- [ ] Source
    - [ ] Read from file
    - [x] Static source from string
    - [x] Substrings
- [ ] Interner (?)

## Scanning

- [x] Token
    - [x] Token kinds
- [ ] Scanning
    - [ ] Operators
    - [ ] String literals
    - [ ] Raw string literals
    - [ ] Symbols
    - [ ] Integers
    - [ ] Floats
    - [ ] Keywords

## Diagnostics

- [x] Diagnostic
    - [x] Issues
    - [x] Levels
    - [x] Reports
    - [x] Pretty printing
- [x] Diagnostic buffer

## Parsing

- [ ] Expressions
    - [ ] Atoms/Literals
        - [ ] Integers
        - [ ] Floats
        - [ ] Strings
        - [ ] Booleans
    - [ ] Unary Expressions
        - [ ] Negative
        - [ ] Not
        - [ ] Increment/decrement
    - [ ] Binary Expressions
        - [ ] Arithmetics
        - [ ] Comparison
        - [ ] Logical
        - [ ] Equality
        - [ ] Assignment
    - [ ] Complex Expressions
        - [ ] If/Else blocks
        - [ ] For loops
        - [ ] While loops
        - [ ] Match blocks
- [ ] Declarations
    - [ ] Bindings
    - [ ] Enums
        - [ ] Tagged unions
    - [ ] Types
    - [ ] Protocols
    - [ ] Impl blocks
        - [ ] Impl type
        - [ ] Impl protocl for type
    - [ ] Function prototypes
    - [ ] Function definitions 

## Backlog

- [ ] Write safe fatal error and diagnostic raiser for violated invariants.