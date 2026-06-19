# ofl/ofl_nodes.py
from dataclasses import dataclass, field

# ─── Capability ───────────────────────────────────────────

@dataclass
class CapNode:
    range_start: int
    range_end: int
    cap_type: str           # "open", "closed", "gobo", "color", "strobe"
    params: dict            # {"slot": 1, "spin": "cw", "name": "Red"}
    line: int               # für Fehlermeldungen

# ─── Channel ──────────────────────────────────────────────

@dataclass
class ChannelNode:
    name: str
    attribute: str
    resolution: str         # "8bit" oder "16bit"
    caps: list[CapNode]     = field(default_factory=list)
    extra: dict             = field(default_factory=dict)
    # extra: range_degrees, default_value, etc.
    line: int               = 0

# ─── Mode ─────────────────────────────────────────────────

@dataclass
class ModeNode:
    name: str
    channel_count: int
    mappings: dict[int, str]    # {1: "pan", 2: "pan", 3: "tilt"}
    line: int = 0

# ─── Include ──────────────────────────────────────────────

@dataclass
class IncludeNode:
    raw_path: str           # exakt was nach @ steht
    line: int = 0

# ─── Fixture (Root) ───────────────────────────────────────

@dataclass
class FixtureNode:
    name: str
    meta: dict              # manufacturer, type, weight, power
    children: list          # ChannelNode | ModeNode | IncludeNode
                            # Reihenfolge bleibt erhalten!
    line: int = 0