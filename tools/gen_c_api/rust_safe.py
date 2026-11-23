"""Generates the per-method part of the safe Rust crate
(rust/jsr-sdk/src/generated.rs): the Rust enums, from the enums of the
rendered jsr_sdk.h, and one Manager method per methods.py entry, the way
jsr_sdk.hpp gets one jsr::Manager method per entry.
"""
import re

import rust_sys

RUST_KEYWORDS = {'type', 'ref', 'mod', 'fn', 'match', 'move', 'self', 'use',
                 'in', 'loop', 'impl', 'trait', 'where', 'box', 'priv'}
# Enums whose values are bit flags: a newtype with constants instead.
FLAGS = {'JSR_DiscoveryStateFlags'}


def snake(name):
    """GetHVSupplyMin -> get_hv_supply_min, GetPulserMaxPRFs -> get_pulser_max_prfs."""
    # A word starts at a capital followed by lowercase, inside a run of
    # capitals, unless that lowercase is a plural "s" (PRFs).
    name = re.sub(r'(?<=[A-Z])(?=[A-Z](?!s(?:[A-Z]|$))[a-z])', '_', name)
    name = re.sub(r'(?<=[a-z0-9])(?=[A-Z])', '_', name)
    return name.lower()


def ident(name):
    name = snake(name)
    return f'r#{name}' if name in RUST_KEYWORDS else name


def camel(upper):
    """LOW_Z -> LowZ, NOT_STARTED -> NotStarted."""
    return ''.join(part.capitalize() for part in upper.split('_'))


def rust_enum_name(c_name):
    return c_name[len('JSR_'):]


def enum_values(header):
    """{C typedef: [(value name, int)]}"""
    out = {}
    for body, name in re.findall(r'typedef enum \w+ \{(.*?)\} (\w+);', header, re.S):
        values = []
        for item in rust_sys.strip_comments(body).split(','):
            match = re.fullmatch(r'(JSR_\w+) = (-?\d+)', item.strip())
            if match:
                values.append((match.group(1), int(match.group(2))))
        out[name] = values
    return out


def common_prefix(names):
    prefix = names[0]
    for name in names[1:]:
        while not name.startswith(prefix):
            prefix = prefix[:-1]
    return prefix[:prefix.rfind('_') + 1]


def gen_enum(c_name, values):
    name = rust_enum_name(c_name)
    if c_name == 'JSR_ErrorCode':
        name = 'ErrorCode'
        values = [(n, v) for n, v in values if n != 'JSR_OK']
    # LAST_VAL entries only mark the end of the .NET enums.
    values = [(n, v) for n, v in values if not n.endswith('_LAST_VAL')]
    prefix = common_prefix([n for n, _ in values])
    variants = [(camel(n[len(prefix):]), n, v) for n, v in values]

    if c_name in FLAGS:
        consts = '\n'.join(
            f'    /// `{n}`\n    pub const {n[len(prefix):]}: Self = Self({v});'
            for _, n, v in variants)
        return f'''/// `{c_name}`: bit flags. Test them with [`{name}::contains`].
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq, Hash)]
pub struct {name}(pub i32);

impl {name} {{
{consts}

    /// Whether every flag of `other` is set.
    pub fn contains(self, other: Self) -> bool {{
        self.0 & other.0 == other.0
    }}
}}
'''

    rows = '\n'.join(f'    /// `{n}`\n    {variant},' for variant, n, _ in variants)
    from_c = '\n'.join(f'            {v} => Self::{variant},' for variant, _, v in variants)
    to_c = '\n'.join(f'            {name}::{variant} => {v},' for variant, _, v in variants)
    return f'''/// `{c_name}`. `Unrecognized` holds a value this crate doesn't know, such as one a
/// newer JSR-SDK.dll added.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
#[non_exhaustive]
pub enum {name} {{
{rows}
    /// A value without a variant here.
    Unrecognized(i32),
}}

impl From<i32> for {name} {{
    fn from(value: i32) -> Self {{
        match value {{
{from_c}
            other => Self::Unrecognized(other),
        }}
    }}
}}

impl From<{name}> for i32 {{
    fn from(value: {name}) -> i32 {{
        match value {{
{to_c}
            {name}::Unrecognized(other) => other,
        }}
    }}
}}
'''


def enum_type(kind):
    """'enum:C_RECEIVER_MODE:JSR_ReceiverMode' -> 'ReceiverMode'."""
    return rust_enum_name(kind.split(':')[2])


def param(kind, name):
    if kind in ('str', 'optstr'):
        return f'{ident(name)}: &str'
    if kind == 'i32':
        return f'{ident(name)}: i32'
    if kind == 'bool':
        return f'{ident(name)}: bool'
    if kind == 'dbl':
        return f'{ident(name)}: f64'
    if kind == 'prid':
        return f'{ident(name)}: &PulserReceiverId'
    if kind.startswith('enum:'):
        return f'{ident(name)}: {enum_type(kind)}'
    raise ValueError(kind)


def prepare(kind, name):
    """Statements that turn a parameter into what the C function takes."""
    if kind in ('str', 'optstr'):
        return f'let {snake(name)}_c = c_string({ident(name)}, "{name}")?;'
    if kind == 'prid':
        return f'let {snake(name)}_c = {ident(name)}.to_c();'
    return None


def arg(kind, name):
    if kind in ('str', 'optstr'):
        return f'{snake(name)}_c.as_ptr()'
    if kind == 'i32' or kind == 'dbl':
        return ident(name)
    if kind == 'bool':
        return f'i32::from({ident(name)})'
    if kind == 'prid':
        return f'&{snake(name)}_c'
    if kind.startswith('enum:'):
        return f'i32::from({ident(name)})'
    raise ValueError(kind)


def method(entry, description):
    c_name, _, inputs, output, _ = entry
    name = snake(c_name)
    params = ', '.join(['&self'] + [param(k, n) for k, n in inputs])
    prep = [p for p in (prepare(k, n) for k, n in inputs) if p]
    args = ', '.join(['self.handle'] + [arg(k, n) for k, n in inputs])
    fn = f'sys::JSR_{c_name}'
    doc = f'    /// {description}\n    ///\n    /// Calls `JSR_{c_name}`.\n'
    body = ''.join(f'        {p}\n' for p in prep)

    if output is None:
        return (doc + f'    pub fn {name}({params}) -> Result<()> {{\n{body}'
                f'        check(unsafe {{ {fn}({args}) }})\n    }}\n')
    if output == 'bool' or output.startswith('retbool:'):
        return (doc + f'    pub fn {name}({params}) -> Result<bool> {{\n{body}'
                f'        let mut value = 0;\n'
                f'        check(unsafe {{ {fn}({args}, &mut value) }})?;\n'
                f'        Ok(value != 0)\n    }}\n')
    if output == 'i32':
        return (doc + f'    pub fn {name}({params}) -> Result<i32> {{\n{body}'
                f'        let mut value = 0;\n'
                f'        check(unsafe {{ {fn}({args}, &mut value) }})?;\n'
                f'        Ok(value)\n    }}\n')
    if output.startswith('enum:'):
        rtype = enum_type(output)
        return (doc + f'    pub fn {name}({params}) -> Result<{rtype}> {{\n{body}'
                f'        let mut value = 0;\n'
                f'        check(unsafe {{ {fn}({args}, &mut value) }})?;\n'
                f'        Ok({rtype}::from(value))\n    }}\n')
    if output == 'dbl':
        return (doc + f'    pub fn {name}({params}) -> Result<f64> {{\n{body}'
                f'        let mut value = 0.0;\n'
                f'        check(unsafe {{ {fn}({args}, &mut value) }})?;\n'
                f'        Ok(value)\n    }}\n')
    if output == 'str':
        return (doc + f'    pub fn {name}({params}) -> Result<String> {{\n{body}'
                f'        read_string(|buffer, size, required| unsafe {{\n'
                f'            {fn}({args}, buffer, size, required)\n'
                f'        }})\n    }}\n')
    if output == 'strs':
        return (doc + f'    pub fn {name}({params}) -> Result<Vec<String>> {{\n{body}'
                f'        read_strings(|buffers, count, length, available| unsafe {{\n'
                f'            {fn}({args}, buffers, count, length, available)\n'
                f'        }})\n    }}\n')
    if output == 'dbls':
        return (doc + f'    pub fn {name}({params}) -> Result<Vec<f64>> {{\n{body}'
                f'        read_list(0.0, |buffer, count, available| unsafe {{\n'
                f'            {fn}({args}, buffer, count, available)\n'
                f'        }})\n    }}\n')
    raise ValueError(output)


def render(template, header, sections, brief):
    enums = enum_values(header)
    enum_code = '\n'.join(gen_enum(name, values) for name, values in enums.items())
    methods = []
    for title, entries in sections:
        methods.append(f'    // --- {title} ---\n')
        for entry in entries:
            methods.append(method(entry, brief(entry)))
    return (template
            .replace('@ENUMS@', enum_code.rstrip())
            .replace('@METHODS@', '\n'.join(methods).rstrip()))
