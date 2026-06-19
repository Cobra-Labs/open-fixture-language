# ofl/ofl_types.py
from dataclasses import dataclass, field
from enum import Enum

# ─── Enums ────────────────────────────────────────────────

class FixtureType(str, Enum):
    MOVING_HEAD  = "moving_head"
    LED_PAR      = "led_par"
    STROBE       = "strobe"
    CONVENTIONAL = "conventional"
    LED_BAR      = "led_bar"
    SMOKE        = "smoke"

class Attribute(str, Enum):
    INTENSITY = "intensity"
    PAN       = "pan"
    TILT      = "tilt"
    COLOR     = "color"
    GOBO      = "gobo"
    STROBE    = "strobe"
    ZOOM      = "zoom"
    FOCUS     = "focus"
    IRIS      = "iris"
    PRISM     = "prism"
    FROST     = "frost"

class CapType(str, Enum):
    OPEN     = "open"
    CLOSED   = "closed"
    GOBO     = "gobo"
    COLOR    = "color"
    STROBE   = "strobe"
    VARIABLE = "variable"

# ─── Capability ───────────────────────────────────────────

@dataclass
class Capability:
    range_start: int
    range_end:   int
    cap_type:    CapType
    label:       str
    params:      dict = field(default_factory=dict)
    # params enthält z.B.:
    # {"slot": 1, "spin": "cw"}          → gobo
    # {"hex": "#FF0000"}                  → color
    # {"hz_start": 1, "hz_end": 30}      → strobe

# ─── Channel ──────────────────────────────────────────────

@dataclass
class FixtureChannel:
    name:         str
    attribute:    Attribute
    resolution:   int           # 8 oder 16
    capabilities: list[Capability] = field(default_factory=list)
    default:      int = 0
    range_degrees: float | None = None

# ─── Mode ─────────────────────────────────────────────────

@dataclass
class FixtureMode:
    name:          str
    channel_count: int
    mappings:      dict[int, str]   # {1: "pan", 2: "pan"}

# ─── Fixture (fertig) ─────────────────────────────────────

@dataclass
class Fixture:
    id:           str
    name:         str
    manufacturer: str
    fixture_type: FixtureType
    channels:     list[FixtureChannel] = field(default_factory=list)
    modes:        list[FixtureMode]    = field(default_factory=list)
    weight_kg:    float | None = None
    power_w:      float | None = None

    def channel(self, name: str) -> FixtureChannel | None:
        """Kanal per Name holen"""
        return next((c for c in self.channels if c.name == name), None)

    def mode(self, name: str) -> FixtureMode | None:
        """Mode per Name holen"""
        return next((m for m in self.modes if m.name == name), None)