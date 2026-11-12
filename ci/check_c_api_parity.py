"""Checks that the C API covers the whole C++ interface.

- Every virtual method of the DLL's SdkManager has a JSR_* function in jsr_sdk.h.
- Every JSR_* function declared in jsr_sdk.h is defined in src/jsr_sdk_c.cpp.
- Every JSR_* function is used by the C++ wrapper in jsr_sdk.hpp.

Run from the repository root: python ci/check_c_api_parity.py
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# C++ methods whose C function has a different name.
RENAMED = {
    'replaceStatusChangeEventHandler': ['SetStatusChangeCallback'],
    'removeStatusChangeEventHandler': ['SetStatusChangeCallback'],
    'replaceNotifyEventHandler': ['SetNotifyCallback'],
    'removeNotifyEventHandler': ['SetNotifyCallback'],
    # Overloads get one name per signature.
    'GetPulserReceiverInfo': ['GetPulserReceiverInfoById', 'GetPulserReceiverInfoByModel'],
    'SetCurrentPulserReceiver': ['SetCurrentPulserReceiverById', 'SetCurrentPulserReceiverByModel'],
    'SetPulserPropertyValue': ['SetPulserPropertyValue', 'SetPulserPropertyValueWithRole'],
}

# Functions of the C API itself that the wrapper doesn't need to call by name.
WRAPPER_EXEMPT = set()


def read(path):
    return (ROOT / path).read_text(encoding='utf-8-sig')


def main():
    legacy = read('src/SdkManager.hpp')
    header = read('include/JSR-SDK/jsr_sdk.h')
    source = read('src/jsr_sdk_c.cpp')
    wrapper = read('include/JSR-SDK/jsr_sdk.hpp')

    methods = sorted(set(re.findall(r'virtual\s+[^;(]*?\b(\w+)\s*\(', legacy)) - {'SdkManager'})
    declared = set(re.findall(r'\bJSR_CALL\s+JSR_(\w+)\s*\(', header))
    defined = set(re.findall(r'\bJSR_CALL\s+JSR_(\w+)\s*\(', source))
    used = set(re.findall(r'\bJSR_(\w+)\s*\(', wrapper))

    errors = []
    for method in methods:
        expected = RENAMED.get(method, [method[0].upper() + method[1:]])
        for name in expected:
            if name not in declared:
                errors.append(f'SdkManager::{method} has no JSR_{name} in jsr_sdk.h')
    for name in sorted(declared - defined):
        errors.append(f'JSR_{name} is declared in jsr_sdk.h but not defined in jsr_sdk_c.cpp')
    for name in sorted(declared - used - WRAPPER_EXEMPT):
        errors.append(f'JSR_{name} is not used by jsr_sdk.hpp')

    print(f'{len(methods)} C++ methods, {len(declared)} C functions')
    for error in errors:
        print('ERROR:', error)
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
