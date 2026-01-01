# tree-sitter-cobol

A [tree-sitter](https://tree-sitter.github.io/tree-sitter/) grammar for the COBOL programming language.

This project is based on the implementation by Yutaro Sakamoto (https://github.com/yutaro-sakamoto/tree-sitter-cobol.git) but includes some personal decisions and fixes that are supposed to facilitate the usage of the parser in context like a new mode for your favorite text editor.

## Features

This grammar provides comprehensive parsing support for COBOL, including:

- **Free format COBOL** - Support for both fixed and free format source code
- **All four COBOL divisions:**
- **Extensive statement support:**
- **Data description clauses:**
- **Special features:**
- **SQL/Embedded SQL support** (OpenESQL syntax)

## Installation

### As a Node.js Package

```bash
npm install tree-sitter-cobol
```

### As a tree-sitter Parser

```bash
git clone https://github.com/SamuelVanie/tree-sitter-cobol.git
cd tree-sitter-cobol
npm install
npm run build
```

### For Use with Editors

Many editors support tree-sitter grammars. The typical installation method depends on your editor:

- **Neovim**: Use `nvim-treesitter` plugin
- **Emacs**: Use `treesit-auto` or configure manually
- **VS Code**: Use a tree-sitter-based extension
- **Other editors**: Check your editor's tree-sitter integration documentation

## Usage

### Command Line

Parse a COBOL file:

```bash
tree-sitter parse your_program.cbl
```

Generate the parser:

```bash
tree-sitter generate
```

Run tests:

```bash
npm test
```

### Programmatic Usage (Node.js)

```javascript
const Parser = require('tree-sitter');
const Cobol = require('tree-sitter-cobol'); // could be tree-sitter-COBOL in some cases

const parser = new Parser();
parser.setLanguage(Cobol);

const sourceCode = `
       IDENTIFICATION DIVISION.
       PROGRAM-ID. HELLO.
       PROCEDURE DIVISION.
           DISPLAY "Hello, World!".
           STOP RUN.
`;

const tree = parser.parse(sourceCode);
console.log(tree.rootNode.toString());
```

## Testing

This project includes extensive test coverage:

### NIST COBOL85 Test Suite

The project includes the full NIST COBOL85 test suite with over 400 test programs covering various COBOL features.

Run all tests:

```bash
npm run nist
```

This will:
- Parse all test files in `test/cobol85/src/`
- Generate a summary report in `test/cobol85/summary.txt`
- Output detailed results to `test/cobol85/result/`

### Corpus Tests

Unit tests for specific grammar constructs are in the `test/corpus/` directory:

```bash
npm test
```

Or run individual corpus tests:

```bash
tree-sitter test test/corpus/minimal-cobol.txt
tree-sitter test test/corpus/evaluate.txt
tree-sitter test test/corpus/perform.txt
```

### OpenESQL Tests

SQL embedded COBOL tests are available in `test/ocesql/`.

## Development

### Building from Source

```bash
# Clone the repository
git clone https://github.com/SamuelVanie/tree-sitter-cobol.git
cd tree-sitter-cobol

# Install dependencies
npm install

# Generate the parser
tree-sitter generate

# Build the native module
npm run build
```

### Development Workflow

1. Modify `grammar.js` to update the grammar rules
2. Run `tree-sitter generate` to regenerate the parser
3. Run `tree-sitter build` or `npm run build` to compile
4. Test with `npm test` or `tree-sitter parse <file>`

### Adding New Tests

Add test cases to `test/corpus/` for specific grammar features:

```txt
==================================================
IDENTIFICATION DIVISION.
PROGRAM-ID. test.
PROCEDURE DIVISION.
    MOVE "hello" TO var.
    MOVE var TO var2.
==================================================

(program_definition
  (identification_division)
  (procedure_division
    (move_statement)
    (move_statement)))
```

## Project Structure

```
tree-sitter-cobol/
├── grammar.js           # Main grammar definition
├── src/                 # Generated parser files
│   ├── parser.c        # Generated C parser
│   ├── scanner.c       # External scanner implementation by myself (includes format detection)
│   └── tree_sitter/    # Tree-sitter headers
├── bindings/           # Language bindings
│   ├── node/          # Node.js bindings
│   └── rust/          # Rust bindings
├── test/              # Test suites
│   ├── corpus/        # Unit tests
│   ├── cobol85/       # NIST COBOL85 test suite
│   └── ocesql/        # OpenESQL tests
├── queries/           # Tree-sitter query examples
└── sample/            # Sample COBOL programs
```

## Supported COBOL Dialects

This grammar is designed to support COBOL-85 and is compatible with major COBOL compilers including:

- GNU COBOL (GnuCOBOL/OpenCOBOL)
- IBM COBOL
- Micro Focus COBOL
- OpenESQL embedded SQL

## License

MIT License - See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

When contributing:
1. Ensure all tests pass
2. Add tests for new grammar features
3. Follow the existing code style
4. Update this README if needed

## Acknowledgments

- NIST COBOL85 test suite for comprehensive test coverage
- tree-sitter project for the excellent parser generator
- The COBOL community for feedback and testing
