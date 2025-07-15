import yaml
import json
import sys
from pathlib import Path
from jsonschema import validate, ValidationError

SCHEMA_PATH = Path(__file__).parent / "schema.json"
ASM_PATH = Path(__file__).parent / "asm.yaml"

def load_schema():
    with open(SCHEMA_PATH, "r", encoding="utf-8") as f:
        return json.load(f)

def load_asm():
    with open(ASM_PATH, "r", encoding="utf-8") as f:
        return yaml.safe_load(f)

def load_and_validate():
    schema = load_schema()
    config = load_asm()
    try:
        validate(instance=config, schema=schema)
    except ValidationError as e:
        print(f"Validation error:\n{e.message}", file=sys.stderr)
        sys.exit(1)
    return config

if __name__ == "__main__":
    config = load_and_validate()
    print("Loaded config:")
    print(json.dumps(config, indent=2))
