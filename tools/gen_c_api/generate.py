"""Generates the C API's per-method code from methods.py and templates/.

Writes, relative to the repository root:
  include/JSR-SDK/jsr_sdk.h     the C API
  src/jsr_sdk_c.cpp             its implementation
  include/JSR-SDK/jsr_sdk.hpp   the jsr::Manager wrapper
  test/CApiParityTest.cpp       parity tests against JSRSDKManager
and formats them with clang-format and the repository's .clang-format.

Usage:
  python tools/gen_c_api/generate.py           regenerate the files
  python tools/gen_c_api/generate.py --check   fail if they are out of date

Set CLANG_FORMAT to the clang-format executable if it isn't on PATH.
"""
import argparse
import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
import methods  # noqa: E402

# Template -> generated file, relative to the repository root.
OUTPUTS = {
    'jsr_sdk.h.in': 'include/JSR-SDK/jsr_sdk.h',
    'jsr_sdk_c.cpp.in': 'src/jsr_sdk_c.cpp',
    'jsr_sdk.hpp.in': 'include/JSR-SDK/jsr_sdk.hpp',
    'CApiParityTest.cpp.in': 'test/CApiParityTest.cpp',
    'EnumParityTest.cpp.in': 'test/EnumParityTest.cpp',
}


def read(path):
    return (ROOT / path).read_text(encoding='utf-8-sig')


ENTRIES = [e for _, es in methods.SECTIONS for e in es]

# --- Briefs from JSRSDKManager.hpp ------------------------------------------
BRIEFS = {}
for _doc, _name in re.findall(r'/\*\*(.*?)\*/\s*virtual\s+[^;(]*?\b(\w+)\s*\(',
                              read('include/JSR-SDK/JSRSDKManager.hpp'), re.S):
    _match = re.search(r'@brief\s+(.*?)(?=\n\s*\*\s*@|\Z)', _doc, re.S)
    if _match:
        _text = ' '.join(l.strip().lstrip('*').strip() for l in _match.group(1).splitlines())
        BRIEFS.setdefault(_name, ' '.join(_text.split()))


def brief(entry):
    c_name, cpp_name, _, _, doc = entry
    return doc or BRIEFS.get(cpp_name) or c_name


# --- Enums ------------------------------------------------------------------
# (C++ enum, C typedef, C value prefix, header in include/JSR-SDK/enums)
ENUMS = [
    ('C_ERROR_CODE', 'JSR_ErrorCode', 'JSR_ERROR_', 'C_ERROR_CODE.h'),
    ('C_CONNECTION_TYPE', 'JSR_ConnectionType', 'JSR_CONNECTION_TYPE_', 'C_CONNECTION_TYPE.h'),
    ('C_DISCOVERY_STATE_FLAGS', 'JSR_DiscoveryStateFlags', 'JSR_DISCOVERY_STATE_', 'C_DISCOVERY_STATE_FLAGS.h'),
    ('C_IS_PULSING', 'JSR_IsPulsing', 'JSR_IS_PULSING_', 'C_IS_PULSING.h'),
    ('C_MANAGER_STATE', 'JSR_ManagerState', 'JSR_MANAGER_STATE_', 'C_MANAGER_STATE.h'),
    ('C_NOTIFY_TYPE', 'JSR_NotifyType', 'JSR_NOTIFY_TYPE_', 'C_NOTIFY_TYPE.h'),
    ('C_POWER_LIMIT', 'JSR_PowerLimit', 'JSR_POWER_LIMIT_', 'C_POWER_LIMIT.h'),
    ('C_PROPERTY_UNITS', 'JSR_PropertyUnits', 'JSR_PROPERTY_UNITS_', 'C_PROPERTY_UNITS.h'),
    ('C_PULSER_IMPEDANCE', 'JSR_PulserImpedance', 'JSR_PULSER_IMPEDANCE_', 'C_PULSER_IMPEDANCE.h'),
    ('C_PULSER_PROPERTY_DATA_TYPE', 'JSR_PulserPropertyDataType', 'JSR_DATA_TYPE_', 'C_PULSER_PROPERTY_DATA_TYPE.h'),
    ('C_PULSER_PROPERTY_ROLE', 'JSR_PulserPropertyRole', 'JSR_PROPERTY_ROLE_', 'C_PULSER_PROPERTY_ROLE.h'),
    ('C_PULSER_RECEIVER_STATE', 'JSR_PulserReceiverState', 'JSR_PULSER_RECEIVER_STATE_', 'C_PULSER_RECEIVER_STATE.h'),
    ('C_RECEIVER_MODE', 'JSR_ReceiverMode', 'JSR_RECEIVER_MODE_', 'C_RECEIVER_MODE.h'),
    ('C_STATUS_CHANGE', 'JSR_StatusChange', 'JSR_STATUS_CHANGE_', 'C_STATUS_CHANGE.h'),
    ('C_TRIGGER_IMPEDANCE', 'JSR_TriggerImpedance', 'JSR_TRIGGER_IMPEDANCE_', 'C_TRIGGER_IMPEDANCE.h'),
    ('C_TRIGGER_POLARITY', 'JSR_TriggerPolarity', 'JSR_TRIGGER_POLARITY_', 'C_TRIGGER_POLARITY.h'),
    ('C_TRIGGER_SOURCE', 'JSR_TriggerSource', 'JSR_TRIGGER_SOURCE_', 'C_TRIGGER_SOURCE.h'),
]
RENAME = {'ERRORC': 'ERROR'}

# The .NET SDK enum each C++ enum mirrors (full type name in JSRDotNETSDK.dll).
# src/marshals/MarshalEnums.h casts between them, so test/EnumParityTest.cpp
# checks that they have the same names and values.
MANAGED_ENUMS = {
    'C_ERROR_CODE': 'JSRDotNETSDK.ERROR_CODE',
    'C_CONNECTION_TYPE': 'JSRDotNETSDK.CONNECTION_TYPE',
    'C_DISCOVERY_STATE_FLAGS': 'JSRDotNETSDK.DiscoveryStateFlags',
    'C_IS_PULSING': 'JSRDotNETSDK.IS_PULSING',
    'C_MANAGER_STATE': 'JSRDotNETSDK.JSRDotNETManager+MANAGER_STATE',
    'C_NOTIFY_TYPE': 'JSRDotNETSDK.NOTIFY_TYPE',
    'C_POWER_LIMIT': 'JSRDotNETSDK.POWER_LIMIT',
    'C_PROPERTY_UNITS': 'JSRDotNETSDK.PROPERTY_UNITS',
    'C_PULSER_IMPEDANCE': 'JSRDotNETSDK.PULSER_IMPEDANCE',
    'C_PULSER_PROPERTY_DATA_TYPE': 'JSRDotNETSDK.PulserPropertyDataType',
    'C_PULSER_PROPERTY_ROLE': 'JSRDotNETSDK.PulserPropertyRole',
    'C_PULSER_RECEIVER_STATE': 'JSRDotNETSDK.PulserReceiverState',
    'C_RECEIVER_MODE': 'JSRDotNETSDK.RECEIVER_MODE',
    'C_STATUS_CHANGE': 'JSRDotNETSDK.STATUS_CHANGE',
    'C_TRIGGER_IMPEDANCE': 'JSRDotNETSDK.TRIGGER_IMPEDANCE',
    'C_TRIGGER_POLARITY': 'JSRDotNETSDK.TRIGGER_POLARITY',
    'C_TRIGGER_SOURCE': 'JSRDotNETSDK.TRIGGER_SOURCE',
}


def enum_values(cpp, prefix, header):
    body = read('include/JSR-SDK/enums/' + header)
    values = []
    for name, value in re.findall(r'^\s*(\w+)\s*=\s*(-?\d+)', body, re.M):
        if cpp == 'C_ERROR_CODE' and name == 'SUCCESS':
            values.append((name, 'JSR_OK', int(value)))
            continue
        c_name = RENAME.get(name, name)
        if c_name.startswith('UNIT_'):
            c_name = c_name[len('UNIT_'):]
        values.append((name, prefix + c_name, int(value)))
    return values


def gen_enum_parity_tests():
    """One test per C++ enum: the SDK enum has exactly its names and values."""
    tests = []
    for cpp, _, prefix, header in ENUMS:
        values = enum_values(cpp, prefix, header)
        rows = ',\n'.join(
            f'      {{"{RENAME.get(name, name)}", static_cast<int>({cpp}::{name})}}'
            for name, _, _ in values)
        tests.append(f'TEST(EnumParity, {cpp}) {{\n'
                     f'  expectSameEnum("{MANAGED_ENUMS[cpp]}", {{\n{rows}}});\n'
                     f'}}\n')
    return '\n'.join(tests)


def gen_enums():
    lines, asserts = [], []
    for cpp, ctype, prefix, header in ENUMS:
        values = enum_values(cpp, prefix, header)
        lines.append(f'/** Mirrors {cpp}. */')
        lines.append(f'typedef enum {ctype} {{')
        for i, (cpp_value, c_value, value) in enumerate(values):
            sep = ',' if i + 1 < len(values) else ''
            lines.append(f'  {c_value} = {value}{sep}')
            asserts.append(f'static_assert({c_value} == static_cast<int>({cpp}::{cpp_value}), "{c_value}");')
        lines.append(f'}} {ctype};')
        lines.append('')
    return '\n'.join(lines).rstrip() + '\n', '\n'.join(asserts) + '\n'


# --- C API: declarations and implementation --------------------------------
def is_enum(kind):
    return kind is not None and kind.startswith('enum:')


def enum_parts(kind):
    _, cpp, ctype = kind.split(':')
    return cpp, ctype


def c_param(kind, name):
    if kind in ('str', 'optstr'):
        return f'const char *{name}'
    if kind in ('i32', 'bool') or is_enum(kind):
        return f'int32_t {name}'
    if kind == 'dbl':
        return f'double {name}'
    if kind == 'prid':
        return f'const JSR_PulserReceiverID *{name}'
    raise ValueError(kind)


def c_arg(kind, name):
    """C++ expression that turns a C parameter into the C++ method's argument."""
    if kind == 'str':
        return f'text({name}, "{name}")'
    if kind == 'optstr':
        return f'({name} != nullptr ? {name} : "")'
    if kind == 'i32':
        return f'static_cast<int>({name})'
    if kind == 'bool':
        return f'{name} != 0'
    if kind == 'dbl':
        return name
    if kind == 'prid':
        return f'toNative(*required({name}, "{name}"))'
    if is_enum(kind):
        cpp, _ = enum_parts(kind)
        return f'static_cast<{cpp}>({name})'
    raise ValueError(kind)


def c_out_params(output):
    if output is None:
        return []
    if output in ('bool', 'i32') or is_enum(output):
        return ['int32_t *value']
    if output == 'dbl':
        return ['double *value']
    if output == 'str':
        return ['char *buffer', 'size_t bufferSize', 'size_t *required']
    if output == 'strs':
        return ['char **buffers', 'size_t bufferCount', 'size_t stringLength', 'size_t *count']
    if output == 'dbls':
        return ['double *buffer', 'size_t bufferCount', 'size_t *count']
    if output.startswith('retbool:'):
        return [f'int32_t *{output.split(":")[1]}']
    raise ValueError(output)


def c_signature(entry):
    c_name, _, inputs, output, _ = entry
    params = ['JSR_Manager *manager'] + [c_param(k, n) for k, n in inputs] + c_out_params(output)
    return f'JSR_API int32_t JSR_CALL JSR_{c_name}({", ".join(params)})'


def c_body(entry):
    c_name, cpp_name, inputs, output, _ = entry
    args = [c_arg(k, n) for k, n in inputs]
    a = ', '.join(args)
    if output is None:
        stmt = f'm.{cpp_name}({a});'
    elif output == 'bool':
        stmt = f'*required(value, "value") = m.{cpp_name}({a}) ? 1 : 0;'
    elif output == 'i32' or is_enum(output):
        stmt = f'*required(value, "value") = static_cast<int32_t>(m.{cpp_name}({a}));'
    elif output == 'dbl':
        stmt = f'*required(value, "value") = m.{cpp_name}({a});'
    elif output == 'str':
        stmt = f'report(required, m.{cpp_name}({", ".join(args + ["buffer", "bufferSize"])}));'
    elif output == 'strs':
        stmt = f'report(count, m.{cpp_name}({", ".join(args + ["buffers", "bufferCount", "stringLength"])}));'
    elif output == 'dbls':
        stmt = f'report(count, m.{cpp_name}({", ".join(args + ["buffer", "bufferCount"])}));'
    elif output.startswith('retbool:'):
        out = output.split(':')[1]
        stmt = f'*required({out}, "{out}") = m.{cpp_name}({a}) ? 1 : 0;'
    else:
        raise ValueError(output)
    return (f'{c_signature(entry)} {{\n'
            f'  return call(manager, [&](JSRSDKManager &m) {{ {stmt} }});\n'
            f'}}\n')


def h_decl(entry):
    sig = c_signature(entry)
    if len(sig) > 80:
        head, params = sig.split('(', 1)
        params = params[:-1].split(', ')
        sig = head + '(\n' + ',\n'.join('    ' + p for p in params) + ')'
    return f'/** {brief(entry)} */\n{sig};\n'


# --- C++ wrapper ------------------------------------------------------------
def w_param(kind, name):
    if kind == 'str':
        return f'const std::string &{name}'
    if kind == 'optstr':
        return f'const std::string &{name} = ""'
    if kind == 'i32':
        return f'int {name}'
    if kind == 'bool':
        return f'bool {name}'
    if kind == 'dbl':
        return f'double {name}'
    if kind == 'prid':
        return f'const PulserReceiverID &{name}'
    if is_enum(kind):
        _, ctype = enum_parts(kind)
        return f'{ctype} {name}'
    raise ValueError(kind)


def w_arg(kind, name):
    if kind in ('str', 'optstr'):
        return f'{name}.c_str()'
    if kind == 'i32':
        return f'static_cast<int32_t>({name})'
    if kind == 'bool':
        return f'{name} ? 1 : 0'
    if kind == 'dbl':
        return name
    if kind == 'prid':
        return f'&{name}'
    if is_enum(kind):
        return f'static_cast<int32_t>({name})'
    raise ValueError(kind)


def w_method(entry):
    c_name, cpp_name, inputs, output, _ = entry
    params = ', '.join(w_param(k, n) for k, n in inputs)
    a = ', '.join(['m_handle'] + [w_arg(k, n) for k, n in inputs])
    fn = f'JSR_{c_name}'
    doc = f'  /** {brief(entry)} */\n'
    if output is None:
        return doc + f'  void {cpp_name}({params}) {{ detail::check({fn}({a})); }}\n'
    if output == 'bool' or output.startswith('retbool:'):
        return doc + (f'  bool {cpp_name}({params}) {{\n'
                      f'    int32_t value = 0;\n'
                      f'    detail::check({fn}({a}, &value));\n'
                      f'    return value != 0;\n'
                      f'  }}\n')
    if output == 'i32':
        return doc + (f'  int {cpp_name}({params}) {{\n'
                      f'    int32_t value = 0;\n'
                      f'    detail::check({fn}({a}, &value));\n'
                      f'    return static_cast<int>(value);\n'
                      f'  }}\n')
    if is_enum(output):
        _, ctype = enum_parts(output)
        return doc + (f'  {ctype} {cpp_name}({params}) {{\n'
                      f'    int32_t value = 0;\n'
                      f'    detail::check({fn}({a}, &value));\n'
                      f'    return static_cast<{ctype}>(value);\n'
                      f'  }}\n')
    if output == 'dbl':
        return doc + (f'  double {cpp_name}({params}) {{\n'
                      f'    double value = 0.0;\n'
                      f'    detail::check({fn}({a}, &value));\n'
                      f'    return value;\n'
                      f'  }}\n')
    if output == 'str':
        return doc + (f'  std::string {cpp_name}({params}) {{\n'
                      f'    return detail::readString([&](char *buffer, size_t size, size_t *required) {{\n'
                      f'      return {fn}({a}, buffer, size, required);\n'
                      f'    }});\n'
                      f'  }}\n')
    if output == 'strs':
        return doc + (f'  std::vector<std::string> {cpp_name}({params}) {{\n'
                      f'    return detail::readStrings(\n'
                      f'        [&](char **buffers, size_t bufferCount, size_t length, size_t *count) {{\n'
                      f'          return {fn}({a}, buffers, bufferCount, length, count);\n'
                      f'        }});\n'
                      f'  }}\n')
    if output == 'dbls':
        return doc + (f'  std::vector<double> {cpp_name}({params}) {{\n'
                      f'    return detail::readArray<double>([&](double *buffer, size_t bufferCount, size_t *count) {{\n'
                      f'      return {fn}({a}, buffer, bufferCount, count);\n'
                      f'    }});\n'
                      f'  }}\n')
    raise ValueError(output)


# --- Parity tests -----------------------------------------------------------
BY_CPP = {}
for _entry in ENTRIES:
    BY_CPP.setdefault(_entry[1], []).append(_entry)


def getter_test(entry):
    c_name, cpp, inputs, output, _ = entry
    if inputs or output is None or c_name in methods.VOLATILE:
        return None
    if output in ('bool', 'i32') or is_enum(output):
        is_bool = 'true' if output == 'bool' else 'false'
        return (f'TEST_F(CApiParityTest, {c_name}) {{\n'
                f'  int32_t value = 0;\n'
                f'  const int32_t rc = JSR_{c_name}(cManager, &value);\n'
                f'  expectSame(legacyOutcome([&] {{ return text(legacy->{cpp}()); }}),\n'
                f'             cOutcome(rc, [&] {{ return text(value, {is_bool}); }}));\n'
                f'}}\n')
    if output == 'dbl':
        return (f'TEST_F(CApiParityTest, {c_name}) {{\n'
                f'  double value = 0.0;\n'
                f'  const int32_t rc = JSR_{c_name}(cManager, &value);\n'
                f'  expectSame(legacyOutcome([&] {{ return text(legacy->{cpp}()); }}),\n'
                f'             cOutcome(rc, [&] {{ return text(value); }}));\n'
                f'}}\n')
    if output == 'str':
        return (f'TEST_F(CApiParityTest, {c_name}) {{\n'
                f'  expectSame(legacyOutcome([&] {{\n'
                f'               return legacyString([&](char *b, size_t n) {{ return legacy->{cpp}(b, n); }});\n'
                f'             }}),\n'
                f'             cString([&](char *b, size_t n, size_t *r) {{ return JSR_{c_name}(cManager, b, n, r); }}));\n'
                f'}}\n')
    if output == 'strs':
        return (f'TEST_F(CApiParityTest, {c_name}) {{\n'
                f'  expectSame(legacyOutcome([&] {{\n'
                f'               return legacyStrings([&](char **b, size_t n, size_t len) {{ return legacy->{cpp}(b, n, len); }});\n'
                f'             }}),\n'
                f'             cStrings([&](char **b, size_t n, size_t len, size_t *c) {{ return JSR_{c_name}(cManager, b, n, len, c); }}));\n'
                f'}}\n')
    if output == 'dbls':
        return (f'TEST_F(CApiParityTest, {c_name}) {{\n'
                f'  expectSame(legacyOutcome([&] {{\n'
                f'               return legacyDoubles([&](double *b, size_t n) {{ return legacy->{cpp}(b, n); }});\n'
                f'             }}),\n'
                f'             cDoubles([&](double *b, size_t n, size_t *c) {{ return JSR_{c_name}(cManager, b, n, c); }}));\n'
                f'}}\n')
    return None


def setter_test(entry):
    """Calls a setter with the device's current value through both interfaces."""
    c_name, cpp, inputs, output, _ = entry
    if output is not None or len(inputs) != 1 or not cpp.startswith('set'):
        return None
    kind, _ = inputs[0]
    getters = BY_CPP.get('get' + cpp[3:])
    if not getters or getters[0][2]:
        return None
    g_cpp, g_out = getters[0][1], getters[0][3]
    if kind == 'str' and g_out == 'str':
        vtype, default = 'std::string', 'std::string()'
        current = f'legacyString([&](char *b, size_t n) {{ return legacy->{g_cpp}(b, n); }})'
        legacy_set, c_set = f'legacy->{cpp}(v.c_str())', f'JSR_{c_name}(cManager, v.c_str())'
    elif kind == 'i32' and g_out == 'i32':
        vtype, default, current = 'int', '0', f'legacy->{g_cpp}()'
        legacy_set, c_set = f'legacy->{cpp}(v)', f'JSR_{c_name}(cManager, v)'
    elif kind == 'dbl' and g_out == 'dbl':
        vtype, default, current = 'double', '0.0', f'legacy->{g_cpp}()'
        legacy_set, c_set = f'legacy->{cpp}(v)', f'JSR_{c_name}(cManager, v)'
    elif kind == 'bool' and g_out == 'bool':
        vtype, default, current = 'bool', 'false', f'legacy->{g_cpp}()'
        legacy_set, c_set = f'legacy->{cpp}(v)', f'JSR_{c_name}(cManager, v ? 1 : 0)'
    elif is_enum(kind) and g_out == kind:
        cpp_enum = kind.split(':')[1]
        vtype, default, current = cpp_enum, f'{cpp_enum}{{}}', f'legacy->{g_cpp}()'
        legacy_set = f'legacy->{cpp}(v)'
        c_set = f'JSR_{c_name}(cManager, static_cast<int32_t>(v))'
    else:
        return None
    return (f'TEST_F(CApiParityTest, {c_name}_WithCurrentValue) {{\n'
            f'  {vtype} v = {default};\n'
            f'  try {{\n'
            f'    v = {current};\n'
            f'  }} catch (const std::exception &) {{\n'
            f'    // Unsupported here: both interfaces must still fail alike.\n'
            f'  }}\n'
            f'  expectSame(legacyOutcome([&] {{ {legacy_set}; return std::string(); }}),\n'
            f'             cOutcome({c_set}, [] {{ return std::string(); }}));\n'
            f'}}\n')


# --- Rendering --------------------------------------------------------------
def template(name):
    return (HERE / 'templates' / name).read_text(encoding='utf-8')


def render():
    """Returns {relative output path: text} for every generated file."""
    enums_h, enum_asserts = gen_enums()
    h_funcs, c_funcs, w_funcs = [], [], []
    for title, entries in methods.SECTIONS:
        h_funcs.append(f'/* --- {title} --- */\n')
        c_funcs.append(f'// --- {title} ---\n')
        w_funcs.append(f'  // --- {title} ---\n')
        for e in entries:
            h_funcs.append(h_decl(e))
            c_funcs.append(c_body(e))
            w_funcs.append(w_method(e))
        h_funcs.append('')
        c_funcs.append('')
        w_funcs.append('')
    tests = [t for e in ENTRIES for t in (getter_test(e), setter_test(e)) if t]
    return {
        OUTPUTS['jsr_sdk.h.in']: template('jsr_sdk.h.in')
            .replace('@ENUMS@', enums_h)
            .replace('@FUNCTIONS@', '\n'.join(h_funcs).rstrip() + '\n'),
        OUTPUTS['jsr_sdk_c.cpp.in']: template('jsr_sdk_c.cpp.in')
            .replace('@ENUM_ASSERTS@', enum_asserts)
            .replace('@FUNCTIONS@', '\n'.join(c_funcs).rstrip() + '\n'),
        OUTPUTS['jsr_sdk.hpp.in']: template('jsr_sdk.hpp.in')
            .replace('@METHODS@', '\n'.join(w_funcs).rstrip() + '\n'),
        OUTPUTS['CApiParityTest.cpp.in']: template('CApiParityTest.cpp.in')
            .replace('@TESTS@', '\n'.join(tests)),
        OUTPUTS['EnumParityTest.cpp.in']: template('EnumParityTest.cpp.in')
            .replace('@TESTS@', gen_enum_parity_tests()),
    }


def clang_format():
    exe = os.environ.get('CLANG_FORMAT') or shutil.which('clang-format')
    if not exe and os.name == 'nt':
        candidate = Path(os.environ.get('ProgramFiles', r'C:\Program Files')) / 'LLVM/bin/clang-format.exe'
        exe = str(candidate) if candidate.exists() else None
    if not exe:
        sys.exit('clang-format not found: install LLVM or set CLANG_FORMAT')
    return exe


def format_files(paths):
    style = f'--style=file:{ROOT / ".clang-format"}'
    subprocess.run([clang_format(), '-i', style, *map(str, paths)], check=True)


def line_ending(path):
    """Keeps a file's existing line endings, so git sees no change."""
    if path.exists() and b'\r\n' in path.read_bytes()[:4096]:
        return '\r\n'
    return '\n'


def normalized(path):
    return path.read_text(encoding='utf-8-sig').replace('\r\n', '\n')


def generate():
    for rel, text in render().items():
        path = ROOT / rel
        with open(path, 'w', encoding='utf-8', newline=line_ending(path)) as f:
            f.write(text)
        print('wrote', rel)
    format_files(ROOT / rel for rel in OUTPUTS.values())


def check():
    """Formats the generated and the committed files the same way and compares
    them, so a different clang-format version doesn't cause false failures."""
    outputs = render()
    stale = []
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        pairs = []
        for i, (rel, text) in enumerate(outputs.items()):
            generated = tmp / f'gen{i}_{Path(rel).name}'
            committed = tmp / f'com{i}_{Path(rel).name}'
            generated.write_text(text, encoding='utf-8')
            shutil.copyfile(ROOT / rel, committed)
            pairs.append((rel, generated, committed))
        format_files([p for _, g, c in pairs for p in (g, c)])
        for rel, generated, committed in pairs:
            if normalized(generated) != normalized(committed):
                stale.append(rel)
                diff = difflib.unified_diff(
                    normalized(committed).splitlines(), normalized(generated).splitlines(),
                    f'{rel} (committed)', f'{rel} (generated)', lineterm='', n=1)
                print('\n'.join(list(diff)[:40]))
    if stale:
        print('\nOut of date: ' + ', '.join(stale))
        print('Run: python tools/gen_c_api/generate.py')
        return 1
    print('All generated files are up to date.')
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--check', action='store_true',
                        help='fail if the generated files are out of date')
    args = parser.parse_args()
    return check() if args.check else generate() or 0


if __name__ == '__main__':
    sys.exit(main())
