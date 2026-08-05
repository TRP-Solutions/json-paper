Import("env")

from pathlib import Path
import sys

project = Path(env.subst("$PROJECT_DIR"))
sys.path.insert(0, str(project / "scripts"))
from font_codegen import generate

generate(project)
