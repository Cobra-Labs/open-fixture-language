from lexer import Token, TokenType
from ofl_nodes import *

class Parser:
    def __init__(self, tokens):
        self.token = None
        self.tokens = tokens
        self.index = 0
        
    def peek(self):
        return self.tokens[self.index]

    def advance(self):
        self.token = self.tokens[self.index]
        self.index += 1
        return self.token

    def expect(self, tk_type):
        if self.tokens[self.index].type == tk_type:
            return self.advance()
        else:
            raise SyntaxError(
                f"Syntax Error in line {self.tokens[self.index].line}! Wrong type {self.tokens[self.index].type}, expected {tk_type}! Line {self.tokens[self.index].line}")

    def parse(self):
        token = self.expect(TokenType.KEYWORD)
        if token.value == "fixture":
            name = self.expect(TokenType.IDENTIFIER)
            self.expect(TokenType.COLON)
            self.expect(TokenType.NEWLINE)
            meta, children = self._parse_fixture_body()
            return FixtureNode(name=name.value, line=token.line, meta=meta, children=children)
        else:
            raise SyntaxError(f"Line {self.tokens[self.index].line}: expected 'fixture'")

    def _parse_fixture_body(self):
        meta = {}
        children = []

        self.expect(TokenType.INDENT)

        while self.peek().type != TokenType.DEDENT and self.peek().type != TokenType.EOF:
            token = self.peek()

            if token.type == TokenType.NEWLINE:
                self.advance()

            elif token.type == TokenType.AT:
                t = self.advance()
                children.append(IncludeNode(raw_path=t.value, line=t.line))

            elif token.type == TokenType.KEYWORD and token.value == "channel":
                children.append(self._channel_parser())

            elif token.type == TokenType.KEYWORD and token.value == "mode":
                children.append(self._parse_mode())

            elif token.type == TokenType.KEYWORD and token.value in ("manufacturer", "type", "weight", "power"):
                keyword = self.advance()
                self.expect(TokenType.COLON)
                wert = self.advance()
                meta[keyword.value] = wert.value

            else:
                raise SyntaxError(f"Line {self.tokens[self.index].line}: unexpected token '{self.tokens[self.index].value}'")

        self.expect(TokenType.DEDENT)
        return meta, children

    def _channel_parser(self):
        self.advance()

        attribute, resolution = "", ""

        caps = []

        name = self.expect(TokenType.IDENTIFIER)
        self.expect(TokenType.COLON)
        self.expect(TokenType.NEWLINE)
        self.expect(TokenType.INDENT)

        while self.peek().type != TokenType.DEDENT:

            if self.peek().type == TokenType.NEWLINE:
                self.advance()

            if self.peek().type == TokenType.KEYWORD:
                if self.peek().value == "attribute":
                    self.advance()
                    self.expect(TokenType.COLON)
                    attribute = self.advance().value
                if self.peek().value == "resolution":
                    self.advance()
                    self.expect(TokenType.COLON)
                    resolution = self.advance().value
                elif self.peek().value == "cap":
                    caps.append(self._parse_cap())
                else:
                    pass
            else:
                raise SyntaxError(f"Line {self.tokens[self.index].line}: unexpected '{self.peek().value}'")

        self.expect(TokenType.DEDENT)
        return ChannelNode(name=name.value, attribute=attribute, resolution=resolution, caps=caps, line=name.line)

    def _parse_mode(self):
        self.advance()

        mappings = {}

        name = self.expect(TokenType.IDENTIFIER)
        self.expect(TokenType.LPAREN)
        channel_count = self.expect(TokenType.KEYWORD)
        self.expect(TokenType.RPAREN)
        self.expect(TokenType.COLON)
        self.expect(TokenType.NEWLINE)
        self.expect(TokenType.INDENT)
        while self.peek().type != TokenType.DEDENT:
            if self.peek().type == TokenType.NEWLINE:
                self.advance()
            if self.peek().type == TokenType.NUMBER:
                s1 = self.expect(TokenType.NUMBER)
                s2 = None
            if self.peek().type == TokenType.COMMA:
                self.advance()
                s2 = self.expect(TokenType.NUMBER)
            self.expect(TokenType.COLON)
            ch = self.expect(TokenType.IDENTIFIER)
            mappings[int(s1.value)] = ch.value
            if s2:
                mappings[int(s2.value)] = ch.value
            if self.peek().type != TokenType.DEDENT:
                self.expect(TokenType.NEWLINE)

        self.expect(TokenType.DEDENT)
        return ModeNode(name=name.value, channel_count=int(channel_count.value.replace("ch", "")), line=name.line, mappings=mappings)

    def _parse_cap(self):
        self.advance()
        cap_range = self.expect(TokenType.RANGE)
        params = {}

        self.expect(TokenType.COLON)
        if self.peek().type in {TokenType.KEYWORD, TokenType.IDENTIFIER}:
            cap_type = self.advance().value
            if self.peek().type == TokenType.LPAREN:
                while self.peek().type != TokenType.RPAREN:
                    name = self.advance().value
                    if self.peek().type == TokenType.EQUALS:
                        self.advance()
                        params[name] = self.advance().value
                    if self.peek().type == TokenType.COMMA:
                        self.advance()

                self.expect(TokenType.RPAREN)
        self.expect(TokenType.NEWLINE)
        parts = cap_range.value.split("..")
        range_start = int(parts[0])
        range_end = int(parts[1])
        return CapNode(range_start=range_start, range_end=range_end, cap_type=cap_type, params=params, line=cap_range.line)

if __name__ == "__main__":
    from lexer import Lexer
    source = """
fixture TestFixture:
    manufacturer: Robe
    channel pan:
        attribute: intensity
        resolution: 16bit
        cap 0..255: open
    mode standard(8ch):
        1: pan
""".strip()
    tokens = Lexer(source).tokenize()
    result = Parser(tokens).parse()
    print(result)