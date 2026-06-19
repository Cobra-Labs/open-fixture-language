from ofl_types import *

INDENT = "    "

class Writer:

    @staticmethod
    def write(fixture):
        lines = [f"fixture {fixture.name}:", INDENT + f"manufacturer: {fixture.manufacturer}"]

        if fixture.fixture_type:
            lines.append(INDENT + f"type: {fixture.fixture_type.value}")
        if fixture.weight_kg:
            lines.append(INDENT + f"weight: {fixture.weight_kg}kg")
        if fixture.power_w:
            lines.append(INDENT + f"power: {fixture.power_w}W")

        for ch in fixture.channels:
            lines.extend(Writer._write_channel(ch))

        for mode in fixture.modes:
            lines.extend(Writer._write_mode(mode))

        return "\n".join(lines)

    @staticmethod
    def _write_channel(channels: FixtureChannel):
        lines = [INDENT + f"channel {channels.name}:", INDENT * 2 + f"attribute: {channels.attribute.value}",
                 INDENT * 2 + f"resolution: {'16bit' if channels.resolution == 16 else '8bit'}"]
        for cap in channels.capabilities:
            lines.append(Writer._write_cap(cap))
        return lines

    @staticmethod
    def _write_cap(cap: Capability):

        cap_str = INDENT * 2 + f"cap {cap.range_start}..{cap.range_end}: {cap.cap_type.value}"
        if cap.params:
            params_str = ", ".join(f"{k}={v}" for k, v in cap.params.items())
            cap_str += f"({params_str})"
        return cap_str

    @staticmethod
    def _write_mode(mode: FixtureMode):
        content = [
            INDENT + f"mode {mode.name}({mode.channel_count}ch):"
        ]

        for slot, ch_name in mode.mappings.items():
            content.append(INDENT * 2 + f"{slot}: {ch_name}")

        return content