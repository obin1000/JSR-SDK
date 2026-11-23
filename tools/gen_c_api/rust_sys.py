"""Generates the raw Rust bindings (rust/jsr-sdk-sys/src/bindings.rs) from
the rendered jsr_sdk.h, so they always match the C API exactly.

It parses only what jsr_sdk.h contains: #define constants, enums, structs,
the opaque JSR_Manager, the callback typedefs and the JSR_API functions.
A construct it doesn't know stops the generator rather than producing wrong
bindings.
"""
import re

# C type -> Rust type, for fields and parameters.
SCALARS = {
    'int32_t': 'i32',
    'size_t': 'usize',
    'double': 'f64',
    'char': 'c_char',
    'void': 'c_void',
}
STRUCTS = ('JSR_InstrumentID', 'JSR_PulserReceiverID', 'JSR_LibMetadata',
           'JSR_StatusChangedEvent', 'JSR_NotifyEvent', 'JSR_Manager')
CALLBACKS = ('JSR_StatusChangeCallback', 'JSR_NotifyCallback')


def strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    return re.sub(r'//[^\n]*', '', text)


def base_type(name):
    if name in SCALARS:
        return SCALARS[name]
    if name in STRUCTS or name in CALLBACKS or re.fullmatch(r'JSR_\w+', name):
        return name
    raise ValueError(f'unknown C type: {name}')


def rust_type(c_type):
    """'const char *' -> '*const c_char', 'JSR_Manager **' -> '*mut *mut JSR_Manager'."""
    c_type = ' '.join(c_type.split())
    const = c_type.startswith('const ')
    if const:
        c_type = c_type[len('const '):]
    stars = c_type.count('*')
    name = c_type.replace('*', '').strip()
    rust = base_type(name)
    for level in range(stars):
        # The const of 'const T *' belongs to the innermost pointer.
        rust = ('*const ' if const and level == 0 else '*mut ') + rust
    if const and stars == 0:
        raise ValueError(f'const by value: {c_type}')
    return rust


def split_param(param):
    """'const char *pluginName' -> ('const char *', 'pluginName')."""
    param = ' '.join(param.split())
    match = re.fullmatch(r'(.*?[\s*])(\w+)', param)
    if not match:
        raise ValueError(f'cannot parse parameter: {param}')
    return match.group(1).strip(), match.group(2)


def snake(name):
    return re.sub(r'(?<=[a-z0-9])(?=[A-Z])', '_', name).lower()


def gen_defines(header):
    out = []
    for name, value in re.findall(r'^#define (JSR_\w+) (-?\d+)\s*$', header, re.M):
        kind = 'i32' if name == 'JSR_API_VERSION' else 'usize'
        out.append(f'pub const {name}: {kind} = {value};')
    return '\n'.join(out)


def gen_enums(header):
    out = []
    for body, name in re.findall(r'typedef enum \w+ \{(.*?)\} (\w+);', header, re.S):
        out.append(f'pub type {name} = i32;')
        for item in strip_comments(body).split(','):
            item = item.strip()
            if not item:
                continue
            match = re.fullmatch(r'(JSR_\w+) = (-?\d+)', item)
            if not match:
                raise ValueError(f'cannot parse enum value in {name}: {item}')
            out.append(f'pub const {match.group(1)}: {name} = {match.group(2)};')
        out.append('')
    return '\n'.join(out)


def gen_structs(header):
    out = []
    for body, name in re.findall(r'typedef struct \w+ \{(.*?)\} (\w+);', header, re.S):
        fields = []
        for field in strip_comments(body).split(';'):
            field = ' '.join(field.split())
            if not field:
                continue
            match = re.fullmatch(r'(\w+) (\w+)((?:\[\w+\])*)', field)
            if not match:
                raise ValueError(f'cannot parse field of {name}: {field}')
            c_type, fname, dims = match.groups()
            rust = base_type(c_type)
            for dim in reversed(re.findall(r'\[(\w+)\]', dims)):
                rust = f'[{rust}; {dim}]'
            fields.append(f'    pub {fname}: {rust},')
        out.append('#[repr(C)]\n#[derive(Clone, Copy)]\n'
                   f'pub struct {name} {{\n' + '\n'.join(fields) + '\n}\n')
    if not re.search(r'typedef struct JSR_Manager JSR_Manager;', header):
        raise ValueError('JSR_Manager is no longer opaque')
    out.append('/// Opaque handle to a manager.\n#[repr(C)]\n'
               'pub struct JSR_Manager {\n    _private: [u8; 0],\n}\n')
    return '\n'.join(out)


def gen_callbacks(header):
    out = []
    pattern = r'typedef void\s*\(JSR_CALL \*(\w+)\)\s*\(([^)]*)\);'
    for name, params in re.findall(pattern, header, re.S):
        args = []
        for param in params.split(','):
            c_type, pname = split_param(param)
            args.append(f'{snake(pname)}: {rust_type(c_type)}')
        out.append(f'pub type {name} =\n'
                   f'    Option<unsafe extern "C" fn({", ".join(args)})>;')
    found = {name for name, _ in re.findall(pattern, header, re.S)}
    if found != set(CALLBACKS):
        raise ValueError(f'callback typedefs changed: {sorted(found)}')
    return '\n'.join(out)


def gen_functions(header):
    out = []
    pattern = r'JSR_API\s+(\w+)\s+JSR_CALL\s+(JSR_\w+)\s*\(([^)]*)\)\s*;'
    for ret, name, params in re.findall(pattern, header, re.S):
        args = []
        for param in params.split(','):
            if not param.strip() or param.strip() == 'void':
                continue
            c_type, pname = split_param(param)
            args.append(f'{snake(pname)}: {rust_type(c_type)}')
        out.append(f'    pub fn {name}({", ".join(args)}) -> {base_type(ret)};')
    return '\n'.join(out), len(out)


def render(template, header):
    """Fills templates/jsr_sdk_sys.rs.in from the rendered jsr_sdk.h text."""
    functions, count = gen_functions(header)
    if count < 100:
        raise ValueError(f'only {count} functions found in jsr_sdk.h')
    return (template
            .replace('@DEFINES@', gen_defines(header))
            .replace('@ENUMS@', gen_enums(header).rstrip())
            .replace('@STRUCTS@', gen_structs(header).rstrip())
            .replace('@CALLBACKS@', gen_callbacks(header))
            .replace('@FUNCTIONS@', functions))
