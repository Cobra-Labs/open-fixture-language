from ofl_nodes import *

VALID_ATTRIBUTES = {"intensity", "pan", "tilt", "color", "gobo", "strobe", "zoom", "focus", "iris", "prism", "frost"}
VALID_CAP_TYPES = {"open", "closed", "gobo", "color", "strobe"}

class ValidationError(Exception):
    def __init__(self, errors):
        if isinstance(errors, str):
            errors = [errors]
        self.errors = errors
        super().__init__("\n".join(errors))

class Validator:

    def validate(self, node):
        errors = []
        self._validate_attributes(node, errors)
        self._validate_caps(node, errors)
        self._validate_modes(node, errors)
        if errors:
            raise ValidationError(errors)

    @staticmethod
    def _validate_attributes(node, errors):
        for entry in node.children:
            if isinstance(entry, ChannelNode):
                if entry.attribute not in VALID_ATTRIBUTES: errors.append(
                    f"Invalid attribute '{entry.attribute}' in channel '{entry.name}' - valid attributes: {VALID_ATTRIBUTES}")
    @staticmethod
    def _validate_caps(node, errors):
        for entry in node.children:
            if isinstance(entry, ChannelNode):
                for caps_entry in entry.caps:
                    if caps_entry.cap_type not in VALID_CAP_TYPES: errors.append(
                        f"Invalid cap type '{caps_entry}' in line '{caps_entry.line}' - valid cap_types: {VALID_CAP_TYPES}")

    @staticmethod
    def _validate_modes(node, errors):
        channel_names = {entry.name for entry in node.children if isinstance(entry, ChannelNode)}
        for entry in node.children:
            if isinstance(entry, ModeNode):
                for ch_name in entry.mappings.values():
                    if ch_name not in channel_names:
                        errors.append(f"Mode '{entry.name}': channel '{ch_name}' not defined")