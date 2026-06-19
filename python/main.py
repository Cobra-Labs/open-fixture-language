from pathlib import Path
from lexer import Lexer
from parser import Parser
from resolver import Resolver
from validator import Validator
from builder import Builder
from writer import Writer

source = Path("test.ofl").read_text(encoding="utf-8")
tokens = Lexer(source).tokenize()
ast = Parser(tokens).parse()
resolved = Resolver(Path(".")).resolve(ast, Path("test.ofl"))
Validator().validate(resolved)
fixture = Builder.build(resolved)
print(fixture)
out = Writer.write(fixture)
print("-" * 50)
print(out)