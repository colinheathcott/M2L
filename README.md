# Dagni

A programming language made for scientific computing, simulation, statistical modeling, and data science.

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
    - [x] Pretty printer
- [ ] Scanning
    - [x] Operators
    - [x] String literals
        - [ ] Escape sequences
    - [ ] Raw string literals
    - [x] Symbols
    - [x] Integers
    - [x] Floats
    - [x] Keywords
    - [ ] Comments
    - [ ] Labels

## Diagnostics

- [x] Diagnostic
    - [x] Issues
    - [x] Levels
    - [x] Reports
    - [x] Pretty printing
- [x] Diagnostic buffer

## Parsing

- [ ] Expressions
  - [x] Atoms/Literals
    - [x] Integers
    - [x] Floats
    - [x] Strings
    - [x] Booleans
    - [x] Symbols
  - [x] Unary Expressions
    - [x] Prefix
    - [x] Postfix
  - [x] Binary Expressions
    - [x] Arithmetic
    - [ ] Comparison
    - [ ] Logical
    - [ ] Equality
    - [ ] Assignment
- [ ] Statements
  - [ ] Bindings
  - [ ] Control Flow
    - [ ] Continue
    - [ ] Break
    - [ ] Return
    - [ ] If
    - [ ] For
    - [ ] While
    - [ ] Match
- [ ] Declarations
  - [ ] Globals
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
- [ ] Document things that are missing documentation.