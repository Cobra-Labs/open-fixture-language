from enum import Enum, auto
from dataclasses import dataclass

class TokenType(Enum):
    # Struktur
    KEYWORD    = auto()
    IDENTIFIER = auto()
    COLON      = auto()
    NEWLINE    = auto()
    INDENT     = auto()
    DEDENT     = auto()
    EOF        = auto()

    # Werte
    NUMBER     = auto()
    STRING     = auto()
    COLOR      = auto()
    RANGE      = auto()
    UNIT       = auto()

    # Sonderzeichen
    AT         = auto()
    COMMA      = auto()
    LPAREN     = auto()
    RPAREN     = auto()
    EQUALS     = auto()

@dataclass
class Token:
    type:  TokenType
    value: str
    line:  int

    def __repr__(self):
        return f"Token({self.type.name}, {self.value!r}, line={self.line})"

KEYWORDS = {
    "fixture", "channel", "mode", "cap",
    "attribute", "resolution", "manufacturer",
    "type", "weight", "power", "range",
    "open", "closed", "variable", "8bit", "16bit", "8ch", "16ch"
}

class Lexer:
    def __init__(self, source: str):
        self.source = source
        self.pos    = 0          # wo wir gerade sind
        self.line   = 1          # aktuelle Zeile
        self.tokens = []         # gesammelte Token
        self.indent_stack = [0]  # Einrückungshistorie

    def _read_while(self, condition) -> str:
        start = self.pos
        while self.pos < len(self.source) and condition(self.source[self.pos]):
            self.pos += 1
        return self.source[start:self.pos]


    def tokenize(self) -> list[Token]:
        while self.pos < len(self.source):
            self._next_token()

        # Alle noch offenen Einrückungen schließen
        while len(self.indent_stack) > 1:
            self.indent_stack.pop()
            self.tokens.append(Token(TokenType.DEDENT, "", self.line))

        self.tokens.append(Token(TokenType.EOF, "", self.line))
        return self.tokens

    def _next_token(self):
        c = self.source[self.pos]

        # ── Kommentar und Farbe #FF0000─────────────
        if c == "#":
            if self.pos + 1 < len(self.source) and self.source[self.pos + 1] in "#0123456789abcdefABCDEF":
                color = self._read_while(lambda x: x in "#0123456789ABCDEFabcdef")
                self.tokens.append(Token(TokenType.COLOR, color, self.line))
            else:
                self._read_while(lambda x: x != "\n")
                return
            return

        # ── Zeilenumbruch ──────────────────────────
        if c == "\n":
            self.tokens.append(Token(TokenType.NEWLINE, "\n", self.line))
            self.line += 1
            self.pos += 1
            self._handle_indent()
            return

        # ── Leerzeichen überspringen ───────────────
        if c in " \t":
            self.pos += 1
            return

        # ── @ Include ──────────────────────────────
        if c == "@":
            self.pos += 1
            path = self._read_while(lambda x: x not in " \n\t")
            self.tokens.append(Token(TokenType.AT, path, self.line))
            return


        # ── String "..." ───────────────────────────
        if c == '"':
            self.pos += 1
            s = self._read_while(lambda x: x != '"')
            self.pos += 1
            self.tokens.append(Token(TokenType.STRING, s, self.line))
            return

        # ── Zahl, Range, oder Zahl+Einheit ─────────
        if c.isdigit():
            self._read_number()
            return

        # ── Keyword oder Identifier ────────────────
        if c.isalpha() or c == "_":
            word = self._read_while(lambda x: x.isalnum() or x == "_")
            if word in KEYWORDS:
                self.tokens.append(Token(TokenType.KEYWORD, word, self.line))
            else:
                self.tokens.append(Token(TokenType.IDENTIFIER, word, self.line))
            return

        # ── Einzelzeichen ──────────────────────────
        single = {
            ":": TokenType.COLON,
            ",": TokenType.COMMA,
            "(": TokenType.LPAREN,
            ")": TokenType.RPAREN,
            "=": TokenType.EQUALS,
        }
        if c in single:
            self.tokens.append(Token(single[c], c, self.line))
            self.pos += 1
            return

        # ── Unbekannt ──────────────────────────────
        raise SyntaxError(
            f"Unbekanntes Zeichen '{c}' in Zeile {self.line}"
        )

    def _read_number(self):
        num = self._read_while(lambda c: c.isdigit())

        # Range? 0..9
        if self.pos + 1 < len(self.source) and self.source[self.pos:self.pos+2] == "..":
            self.pos += 2
            num2 = self._read_while(lambda c: c.isdigit())
            self.tokens.append(Token(TokenType.RANGE, f"{num}..{num2}", self.line))
            return

        elif self.pos + 1 < len(self.source) and self.source[self.pos:self.pos+1] == ".":
            self.pos += 1
            num3 = self._read_while(lambda c: c.isdigit())
            num = f"{num}.{num3}"

        # Einheit direkt dahinter? 540deg, 480W, 19.5kg
        unit = self._read_while(lambda c: c.isalpha())
        if unit:
            combined = num + unit
            if combined in KEYWORDS or unit == "ch":
                self.tokens.append(Token(TokenType.KEYWORD, combined, self.line))
                return
            self.tokens.append(Token(TokenType.NUMBER, num, self.line))
            self.tokens.append(Token(TokenType.UNIT, unit, self.line))
            return

        self.tokens.append(Token(TokenType.NUMBER, num, self.line))

    def _handle_indent(self):
        # Leerzeichen am Zeilenanfang zählen
        spaces = 0
        while self.pos < len(self.source):
            if self.source[self.pos] == " ":
                spaces += 1
                self.pos += 1
            elif self.source[self.pos] == "\t":
                spaces += 4
                self.pos += 1
            else: break

        # Leere Zeile – ignorieren
        if self.pos < len(self.source) and self.source[self.pos] == "\n":
            return

        current = self.indent_stack[-1]

        if spaces > current:
            self.indent_stack.append(spaces)
            self.tokens.append(Token(TokenType.INDENT, "", self.line))

        elif spaces < current:
            while self.indent_stack[-1] > spaces:
                self.indent_stack.pop()
                self.tokens.append(Token(TokenType.DEDENT, "", self.line))

if __name__ == "__main__":
    source = """
fixture TestFixture:
    @fixture.ofl
    manufacturer: Test
    channel pan:
        resolution: 16bit
        cap 0..255: open
    channel color:
        color: #FF0000
""".strip()

    tokens = Lexer(source).tokenize()
    for t in tokens:
        print(t)