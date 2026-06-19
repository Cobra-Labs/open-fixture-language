from pathlib import Path
from ofl_nodes import *
from lexer import Lexer
from parser import Parser

class Resolver:
    def __init__(self, fixture_root: Path):
        self.fixture_root = fixture_root
        self.currently_resolving = set()

    def resolve(self, node, current_file):
        channels = {}
        other_children = []
        for entry in node.children:
            if isinstance(entry, IncludeNode):
                load_path = self._resolve_path(raw_path=entry.raw_path, current_file=current_file)
                if load_path in self.currently_resolving: raise RecursionError("Recursion Error!")
                self.currently_resolving.add(load_path)
                source = load_path.read_text(encoding="utf-8")
                tokens = Lexer(source).tokenize()
                included_node = Parser(tokens).parse()
                for ch in included_node.children:
                    if isinstance(ch, ChannelNode): channels[ch.name] = ch
                self.currently_resolving.remove(load_path)


            elif isinstance(entry, ChannelNode):
                channels[entry.name] = entry
            elif isinstance(entry, ModeNode):
                other_children.append(entry)

        return FixtureNode(
            name = node.name,
            meta = node.meta,
            children = list(channels.values()) + other_children,
            line = node.line
        )

    def _resolve_path(self, raw_path, current_file):
        if raw_path.startswith("/"):
            return Path(raw_path)
        if raw_path.startswith("./") or raw_path.startswith("../"):
            return (current_file.parent / raw_path).resolve()
        else:
            fpath = next(self.fixture_root.rglob(raw_path + ".ofl"), None)
            if fpath is not None:
                return fpath
            else:
                raise FileNotFoundError(f"{raw_path} not found!")