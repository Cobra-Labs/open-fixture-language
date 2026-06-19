from ofl_types import *
from ofl_nodes import *
from uuid import uuid4

class Builder:
    @staticmethod
    def build(node: FixtureNode) -> Fixture:
        channels = [Builder._build_channel(e) for e in node.children if isinstance(e, ChannelNode)]
        modes = [Builder._build_mode(e) for e in node.children if isinstance(e, ModeNode)]

        return Fixture(
            id=str(uuid4()),
            name=node.name,
            manufacturer=node.meta.get("manufacturer", ""),
            fixture_type=FixtureType(node.meta.get("type", "")),
            channels=channels,
            modes=modes,
            weight_kg=float(node.meta["weight"]) if "weight" in node.meta else None,
            power_w=float(node.meta["power"]) if "power" in node.meta else None,
        )

    @staticmethod
    def _build_channel(node: ChannelNode):
        name = node.name
        attribute = Attribute(node.attribute)
        resolution = 16 if node.resolution == "16bit" else 8
        capabilities = [Builder._build_cap(cap) for cap in node.caps]
        default = node.extra.get("default") if node.extra.get("default") else None
        range_degrees = float(node.extra.get("range")) if node.extra.get("range") else None

        return FixtureChannel(
            name = name,
            attribute = attribute,
            resolution = resolution,
            capabilities = capabilities,
            default = default,
            range_degrees = range_degrees
        )

    @staticmethod
    def _build_cap(node: CapNode):
        start = node.range_start
        end = node.range_end
        cap_type = node.cap_type
        params = node.params
        label = params.get("name", cap_type)

        return Capability(
            range_start = start,
            range_end = end,
            cap_type = CapType(cap_type),
            label = label,
            params = params
        )

    @staticmethod
    def _build_mode(node: ModeNode):
        name = node.name
        channel_count = node.channel_count
        mappings = node.mappings

        return FixtureMode(
            name = name,
            channel_count = channel_count,
            mappings = mappings
        )
