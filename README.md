# Compiler - Lexical Analyzer and Predictive Parser

A two-phase compiler front-end implementation in C++ that performs lexical analysis (tokenization) and syntax analysis (predictive parsing) for the ML++ (minilang++) programming language.

---

## Overview

This project implements the first two layers of a compiler:

1. **Lexical Analyzer (Lexer)** - Converts raw source code text into a stream of tokens
2. **Syntax Analyzer (Parser)** - Validates the token stream against the language grammar using an LL(1) predictive parsing algorithm

The implementation uses a hand-written scanner and a table-driven predictive parser. Grammar analysis is performed by computing FIRST and FOLLOW sets, which are then used to construct a parsing table.

---

## Project Structure

```
.
├── main.cpp                    # Entry point - orchestrates lexer and parser
├── grammar.txt                 # LL(1) grammar definition file
├── test.mlpp                   # Sample ML++ source file for testing
├── testcases.txt               # Test case traces with expected output
├── lexer/
│   ├── lexer.hpp               # Lexer function declarations
│   └── lexer.cpp               # Lexer implementation (tokenization, validation)
├── token/
│   ├── token_types.hpp         # Token type definitions and data structures
│   ├── token_types.cpp         # Global token storage variables
│   ├── token_utils.hpp         # Token utility function declarations
│   └── token_utils.cpp         # Token classification and validation utilities
├── tables/
│   ├── language_tables.hpp     # Language table declarations
│   └── language_tables.cpp     # Operator, keyword, delimiter, and character tables
├── firstFollow/
│   ├── firstFollow.hpp         # FIRST/FOLLOW set declarations
│   └── firstFollow.cpp         # Grammar loading, FIRST/FOLLOW set computation
└── parser/
    ├── parser.hpp              # Parser function declarations
    └── parser.cpp              # LL(1) parsing table construction and predictive parser
```

---

## Compilation

### Using g++ (Linux / macOS)

```bash
g++ -std=c++17 -o compiler main.cpp \
    lexer/lexer.cpp \
    token/token_types.cpp \
    token/token_utils.cpp \
    tables/language_tables.cpp \
    firstFollow/firstFollow.cpp \
    parser/parser.cpp
```

### Using g++ via MinGW (Windows)

```bash
g++ -std=c++17 -o compiler.exe main.cpp ^
    lexer/lexer.cpp ^
    token/token_types.cpp ^
    token/token_utils.cpp ^
    tables/language_tables.cpp ^
    firstFollow/firstFollow.cpp ^
    parser/parser.cpp
```

## Running

After compilation, execute the program with:

```bash
./compiler          # Linux / macOS
compiler.exe        # Windows
```

The program reads the source file `test.mlpp` and the grammar file `grammar.txt` from the current working directory. Ensure both files are present in the directory where the binary is executed.

---

## How It Works

### Phase 1: Lexical Analysis

The lexical analyzer reads the source file character by character and produces a sequence of tokens. The process involves three stages:

#### Stage 1: Raw Token Generation (`generateTokensFromFile`)

- Reads the source file line by line
- Classifies each character as whitespace, delimiter, operator, or part of a multi-character token
- Handles string literals (including escape sequences: `\n`, `\t`, `\\`, `\"`, `\xNN` for hex escapes)
- Handles block comments (`/* ... */`) with nesting support
- Handles doc comments (`/*** ... ***/` and `/// ...`)
- Handles single-line comments (`// ...`)
- Reports unterminated strings and comments as errors

#### Stage 2: Operator Merging (`mergeTokens`)

- Applies the **maximal munch** strategy for multi-character operators
- Merges adjacent operator characters into the longest valid operator (e.g., `+` and `=` become `+=`)
- Reports invalid operator sequences as lexical errors

#### Stage 3: Token Validation (`validateTokens`)

Each merged token is classified into one of the following categories:

| Token Type     | Examples                          |
|----------------|-----------------------------------|
| Keyword        | `if`, `else`, `while`, `return`   |
| Identifier     | `a`, `myVar`, `_count`            |
| Numeric Literal| `42`, `0xFF`, `3.14`, `1.5e10`   |
| String Literal | `"hello world"`                   |
| Operator       | `+`, `-`, `*`, `/`, `==`, `!=`   |
| Delimiter      | `(`, `)`, `{`, `}`, `;`, `,`     |

Numeric literals support:
- Decimal integers: `42`
- Binary literals: `0b1010`
- Octal literals: `0o52`
- Hexadecimal literals: `0x2A`
- Floating-point: `3.14`, `1.5e10`, `2.5E-3`

Identifiers must:
- Start with an alphabetic character or underscore
- Contain only alphanumeric characters or underscores
- Be no longer than 64 characters

A **symbol table** is built mapping each identifier to its first line of occurrence.

The validated tokens are mapped to grammar terminal symbols (e.g., `+` becomes `PLUS`, `a` becomes `ID`, `42` becomes `NUM`) and assembled into a `tokenStream` for the parser, terminated by `$`.

---

### Phase 2: Syntax Analysis

The parser implements a **table-driven LL(1) predictive parsing** algorithm.

#### Grammar Definition

The grammar is defined in `grammar.txt`. The current grammar supports assignment statements with arithmetic expressions:

```
Stmt     -> ID ASSIGN Expr SEMICOLON
Expr     -> Term ExprP
ExprP    -> PLUS Term ExprP | MINUS Term ExprP | eps
Term     -> Factor TermP
TermP    -> MULTIPLY Factor TermP | DIVIDE Factor TermP | eps
Factor   -> LPAREN Expr RPAREN | ID | NUM
```

This is an LL(1)-compatible grammar that enforces operator precedence:
- Multiplication and division have higher precedence than addition and subtraction
- Parentheses override precedence
- Left recursion is eliminated using the `ExprP` and `TermP` tail productions

#### FIRST and FOLLOW Set Computation

The `computeFirst()` function computes the FIRST set for each non-terminal:

- **FIRST(X)** is the set of terminals that can begin strings derived from X
- If a production can derive epsilon (`eps`), then `eps` is included in FIRST(X)
- Computed iteratively until a fixed point is reached

The `computeFollowSets()` function computes the FOLLOW set for each non-terminal:

- **FOLLOW(A)** is the set of terminals that can appear immediately after A in some sentential form
- The end-of-input marker `$` is placed in FOLLOW of the start symbol
- Uses a fixed-point iteration algorithm

#### Parsing Table Construction

The `buildParsingTable()` function constructs a 2D table M[A, a] where:
- Rows correspond to non-terminals
- Columns correspond to terminals
- Each cell contains the production rule to use (or is empty, indicating a syntax error)

Table construction rules:
- For each production `A -> alpha`, add `A -> alpha` to `M[A, a]` for every terminal `a` in FIRST(alpha)
- If `eps` is in FIRST(alpha), add `A -> alpha` to `M[A, b]` for every terminal `b` in FOLLOW(A)

#### Predictive Parsing Algorithm

The `predictiveParser()` function performs a top-down parse using a stack:

```
1. Initialize: stack = [$, Stmt], input pointer = 0
2. While stack is not empty:
   a. Let X = top of stack, a = current input token
   b. If X == a (both are terminals):
        Pop X from stack, advance input pointer
   c. Else if M[X, a] exists (parsing table lookup):
        Pop X from stack
        Push production body onto stack (right-to-left)
   d. Else:
        Report syntax error with position information
3. If stack is empty and input is exhausted:
        Report "Parsing Successful"
```

The parser prints a trace table at each step showing:
- The current stack contents
- The remaining input
- The action taken (match or production applied)

---

## Grammar File Format

The grammar file (`grammar.txt`) uses the following format:

```
<number_of_rules>
<start_symbol>
<LHS> -> <RHS symbol 1> <RHS symbol 2> ... <RHS symbol N>
```

- The first line specifies the number of production rules
- The second line specifies the start symbol (used as the root of the parse)
- Each subsequent line defines a production rule
- Use `|` to separate multiple alternatives on the same non-terminal
- Use `eps` to denote epsilon (empty) productions

---

## Supported Language Tokens

### Operators

| Symbol | Token Name       | Symbol | Token Name         |
|--------|------------------|--------|--------------------|
| `+`    | PLUS             | `==`   | EQUAL              |
| `-`    | MINUS            | `!=`   | NOT_EQUAL          |
| `*`    | MULTIPLY         | `<`    | LESS               |
| `/`    | DIVIDE           | `<=`   | LESS_EQUAL         |
| `%`    | MODULO           | `>`    | GREATER            |
| `=`    | ASSIGN           | `>=`   | GREATER_EQUAL      |
| `+=`   | PLUS_ASSIGN      | `&&`   | LOGICAL_AND        |
| `-=`   | MINUS_ASSIGN     | `\|\|` | LOGICAL_OR         |
| `*=`   | MULTIPLY_ASSIGN  | `!`    | LOGICAL_NOT        |
| `/=`   | DIVIDE_ASSIGN    | `->`   | ARROW              |
| `::`   | SCOPE            |        |                    |

### Delimiters

| Symbol | Token Name   |
|--------|-------------|
| `(`    | LPAREN      |
| `)`    | RPAREN      |
| `{`    | LBRACE      |
| `}`    | RBRACE      |
| `[`    | LBRACKET    |
| `]`    | RBRACKET    |
| `,`    | COMMA       |
| `;`    | SEMICOLON   |
| `:`    | COLON       |

### Keywords

`if`, `else`, `while`, `for`, `return`, `break`, `continue`, `fn`, `let`, `const`, `struct`, `enum`, `match`

---

## Example Input and Output

### Input (`test.mlpp`)

```
a = b + c * d;
```

### Token Stream Output

```
Token Stream :
ID ASSIGN ID PLUS ID MULTIPLY ID SEMICOLON $
```

### Parser Trace (abbreviated)

```
Stack                                             Input                                             Action
----------------------------------------------------------------------------------------------------------------------------------
$ Stmt                                            ID ASSIGN ID PLUS ID MULTIPLY ID SEMICOLON $      Stmt -> ID ASSIGN Expr SEMICOLON
$ SEMICOLON Expr ASSIGN ID                        ID ASSIGN ID PLUS ID MULTIPLY ID SEMICOLON $      Match ID
$ SEMICOLON Expr ASSIGN                           ASSIGN ID PLUS ID MULTIPLY ID SEMICOLON $         Match ASSIGN
$ SEMICOLON Expr                                  ID PLUS ID MULTIPLY ID SEMICOLON $                Expr -> Term ExprP
...
$                                                 $                                                 Match $
Parsing Successful
```

---

## Error Handling

### Lexical Errors

- **Invalid token**: Unrecognized character sequence
- **Unterminated string**: String literal without closing `"`
- **Unterminated comment**: Block comment without closing `*/`
- **Invalid escape sequence**: Invalid character after `\` in string literal
- **Invalid numeric literal**: Malformed number (e.g., `0xGG`, invalid binary digit)
- **Invalid identifier**: Identifier starting with a digit, containing invalid characters, or exceeding 64 characters
- **Invalid operator**: Unknown or malformed operator sequence

### Syntax Errors

- The parser reports the token where the error was detected, its position, and the preceding token for context
- Parsing stops immediately upon encountering a syntax error

---

## Data Structures

| Type            | Definition                                              | Purpose                                      |
|-----------------|----------------------------------------------------------|----------------------------------------------|
| `TRC`           | `pair<string, pair<int, int>>`                          | Token with row and column position           |
| `tokenTable`    | `pair<pair<string, string>, pair<int, int>>`            | Lexeme, token type, and source position      |
| `tokenStream`   | `vector<string>`                                        | Sequence of grammar terminal symbols         |
| `symboleTable`  | `unordered_map<string, int>`                            | Identifier to first-occurrence line mapping   |
| `grammar`       | `map<string, vector<vector<string>>>`                   | Non-terminal to production rules mapping     |
| `parsingTable`  | `unordered_map<string, unordered_map<string, vector<string>>>` | LL(1) parsing table                 |

---

## Extending the Grammar

To add new language constructs:

1. Define new production rules in `grammar.txt` ensuring the grammar remains LL(1)-compatible (no left recursion, no common left factors)
2. Add any new terminal symbols to the token validation and classification logic in `lexer/lexer.cpp`
3. If new keywords are required, add them to the `keywords` set in `tables/language_tables.cpp`
4. Update the `tokenStream` generation in `validateTokens()` to map new tokens to their grammar terminal names
