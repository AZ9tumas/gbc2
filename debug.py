#!/usr/bin/env python3
import re
import sys

def parse_register_line(line):
    """
    Parse a register state line like:
    [A11|B00|C00|Dff|E56|H00|L0d|SPfffe]
    and return a dict mapping register names to values.
    """
    content = line.strip()[1:-1]  # remove surrounding brackets
    parts = content.split('|')
    reg_dict = {}
    for part in parts:
        m = re.match(r'([A-Z]+)([0-9A-Fa-f]+)', part)
        if m:
            reg, val = m.groups()
            reg_dict[reg] = val
    return reg_dict

def parse_instruction_line(line):
    """
    Parse an instruction detail line.
    Supports two formats:
      Format 1 (no opcode field):
         [0x0100][Z1 N0 H0 C0]      NOP
      Format 2 (with opcode field):
         [0x0100][0x00][Z0 N0 H0 C0]      NOP
    Returns a dict with keys: pc, opcode (or None), flags, mnemonic.
    """
    line = line.strip()
    pattern = r'^\[(0x[0-9A-Fa-f]+)\](?:\[(0x[0-9A-Fa-f]+)\])?\[(Z[0-9]\s+N[0-9]\s+H[0-9]\s+C[0-9])\]\s+(.*)$'
    m = re.match(pattern, line)
    if m:
        pc, opcode, flags, mnemonic = m.groups()
        return {
            'pc': pc,
            'opcode': opcode,  # may be None
            'flags': flags.strip(),
            'mnemonic': mnemonic.strip()
        }
    else:
        return {}

def parse_disassembly(text):
    """
    Parse the disassembly text into a list of entries.
    Each entry is either:
      - An instruction entry with type "instruction", which has:
          'raw_reg': the register state line,
          'raw_instr': the instruction detail line,
          'registers': parsed registers dict,
          'instruction': parsed instruction dict.
      - A writing entry with type "writing", which has:
          'raw': the full writing line.
    """
    lines = [line.rstrip() for line in text.strip().splitlines() if line.strip()]
    entries = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("Writing"):
            # Writing entry.
            entries.append({
                'type': 'writing',
                'raw': line
            })
            i += 1
        elif line.startswith("[") and "|" in line and line.endswith("]"):
            # Expect a register state line.
            raw_reg = line
            reg_dict = parse_register_line(raw_reg)
            if i+1 < len(lines) and lines[i+1].startswith("["):
                raw_instr = lines[i+1]
                instr_dict = parse_instruction_line(raw_instr)
                entries.append({
                    'type': 'instruction',
                    'raw_reg': raw_reg,
                    'raw_instr': raw_instr,
                    'registers': reg_dict,
                    'instruction': instr_dict
                })
                i += 2
            else:
                print(f"Warning: Missing instruction detail line after line {i+1}")
                i += 1
        else:
            print(f"Warning: Unrecognized line format at line {i+1}: {line}")
            i += 1
    return entries

def compare_instruction_entries(e1, e2):
    """
    Compare two instruction entries.
    Returns a list of difference strings for registers, PC, flags, mnemonic, and opcode.
    """
    diffs = []
    regs1 = e1.get('registers', {})
    regs2 = e2.get('registers', {})
    for reg in set(regs1.keys()).union(regs2.keys()):
        val1 = regs1.get(reg)
        val2 = regs2.get(reg)
        if val1 != val2:
            diffs.append(f"Register {reg} differs: {val1} vs {val2}")
    
    inst1 = e1.get('instruction', {})
    inst2 = e2.get('instruction', {})
    if inst1.get('pc') != inst2.get('pc'):
        diffs.append(f"PC differs: {inst1.get('pc')} vs {inst2.get('pc')}")
    if inst1.get('flags') != inst2.get('flags'):
        diffs.append(f"Flags differ at PC {inst1.get('pc')}: {inst1.get('flags')} vs {inst2.get('flags')}")
    if inst1.get('mnemonic') != inst2.get('mnemonic'):
        diffs.append(f"Mnemonic differs at PC {inst1.get('pc')}: '{inst1.get('mnemonic')}' vs '{inst2.get('mnemonic')}'")
    op1 = inst1.get('opcode')
    op2 = inst2.get('opcode')
    if op1 and op2 and op1.lower() != op2.lower():
        diffs.append(f"Opcode differs at PC {inst1.get('pc')}: {op1} vs {op2}")
    return diffs

def compare_entries(e1, e2):
    """
    Compare two entries (which can be of type "instruction" or "writing").
    Returns a list of differences. If types differ, report that.
    """
    diffs = []
    if e1.get('type') != e2.get('type'):
        diffs.append(f"Entry type differs: {e1.get('type')} vs {e2.get('type')}")
        return diffs
    
    if e1.get('type') == 'instruction':
        diffs.extend(compare_instruction_entries(e1, e2))
    elif e1.get('type') == 'writing':
        # Compare writing lines directly.
        if e1.get('raw').strip() != e2.get('raw').strip():
            diffs.append(f"Writing line differs: '{e1.get('raw').strip()}' vs '{e2.get('raw').strip()}'")
    return diffs

def print_entry_context(entries, index, filename):
    """
    Print the previous, current, and next entries (raw) from entries list for context.
    """
    print(f"=== {filename} ===")
    if index > 0:
        print(f"Previous entry (#{index}):")
        print_entry(entries[index-1])
    else:
        print("Previous entry: None")
    print(f"\nCurrent entry (#{index+1}):")
    print_entry(entries[index])
    if index+1 < len(entries):
        print(f"\nNext entry (#{index+2}):")
        print_entry(entries[index+1])
    else:
        print("\nNext entry: None")
    print("\n")

def print_entry(entry):
    """
    Print an entry. For an instruction entry, print both register and instruction lines.
    For a writing entry, print the raw line.
    """
    if entry.get('type') == 'instruction':
        print(entry.get('raw_reg'))
        print(entry.get('raw_instr'))
    elif entry.get('type') == 'writing':
        print(entry.get('raw'))
    else:
        print("Unknown entry type.")

def main():
    if len(sys.argv) != 3:
        print("Usage: python debug.py <disasm_file1> <disasm_file2>")
        sys.exit(1)
    file1, file2 = sys.argv[1], sys.argv[2]
    with open(file1, 'r') as f:
        text1 = f.read()
    with open(file2, 'r') as f:
        text2 = f.read()
    
    entries1 = parse_disassembly(text1)
    entries2 = parse_disassembly(text2)
    
    min_len = min(len(entries1), len(entries2))
    for i in range(min_len):
        diffs = compare_entries(entries1[i], entries2[i])
        if diffs:
            print(f"Mismatch found at entry #{i+1}:")
            for diff in diffs:
                print("  ", diff)
            print("\nContext from file1:")
            print_entry_context(entries1, i, file1)
            print("Context from file2:")
            print_entry_context(entries2, i, file2)
            sys.exit(0)
    
    if len(entries1) != len(entries2):
        print("No mismatches in crucial elements were found, but the number of entries differs.")
    else:
        print("No mismatches found in the crucial elements of the disassembly.")

if __name__ == "__main__":
    main()

