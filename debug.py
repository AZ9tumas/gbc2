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
    Handles two possible formats:
      Format 1:
         [0x0100][Z1 N0 H0 C0]      NOP
      Format 2:
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
    Parse the text of a disassembly file into a list of instructions.
    Each instruction is represented as a dict with keys:
      'raw_reg' : original register state line,
      'raw_instr': original instruction line,
      'registers' : parsed register state dict,
      'instruction': parsed instruction details dict.
    Assumes each instruction is represented by two consecutive non-empty lines.
    """
    lines = [line for line in text.strip().splitlines() if line.strip()]
    instructions = []
    i = 0
    while i < len(lines):
        # Expect register state line.
        if re.match(r'^\[.*\|.*\]$', lines[i]):
            raw_reg = lines[i].rstrip()
            reg_dict = parse_register_line(raw_reg)
            i += 1
        else:
            print(f"Warning: Expected register state line at line {i+1}: {lines[i]}")
            i += 1
            continue
        # Next, instruction detail line.
        if i < len(lines) and lines[i].startswith('['):
            raw_instr = lines[i].rstrip()
            instr_dict = parse_instruction_line(raw_instr)
            i += 1
        else:
            print(f"Warning: Expected instruction detail line at line {i+1}")
            break
        instructions.append({
            'raw_reg': raw_reg,
            'raw_instr': raw_instr,
            'registers': reg_dict,
            'instruction': instr_dict
        })
    return instructions

def compare_instructions(instr1, instr2):
    """
    Compare two instruction dictionaries.
    Returns a list of difference strings for crucial elements (registers, PC, flags, mnemonic).
    """
    diffs = []
    # Compare registers.
    regs1 = instr1.get('registers', {})
    regs2 = instr2.get('registers', {})
    for reg in set(regs1.keys()).union(regs2.keys()):
        val1 = regs1.get(reg)
        val2 = regs2.get(reg)
        if val1 != val2:
            diffs.append(f"Register {reg} differs: {val1} vs {val2}")
    
    inst1 = instr1.get('instruction', {})
    inst2 = instr2.get('instruction', {})
    # Compare PC.
    if inst1.get('pc') != inst2.get('pc'):
        diffs.append(f"PC differs: {inst1.get('pc')} vs {inst2.get('pc')}")
    # Compare flags.
    if inst1.get('flags') != inst2.get('flags'):
        diffs.append(f"Flags differ at PC {inst1.get('pc')}: {inst1.get('flags')} vs {inst2.get('flags')}")
    # Compare mnemonic (and operands).
    if inst1.get('mnemonic') != inst2.get('mnemonic'):
        diffs.append(f"Mnemonic differs at PC {inst1.get('pc')}: '{inst1.get('mnemonic')}' vs '{inst2.get('mnemonic')}'")
    # Compare opcode field if both exist.
    op1 = inst1.get('opcode')
    op2 = inst2.get('opcode')
    if op1 and op2 and op1.lower() != op2.lower():
        diffs.append(f"Opcode differs at PC {inst1.get('pc')}: {op1} vs {op2}")
    return diffs

def print_instruction_context(instr_list, index, filename):
    """
    Print previous, current, and next instructions from the list for a given index.
    """
    print(f"=== {filename} ===")
    if index > 0:
        print(f"Previous instruction (#{index}):")
        print(instr_list[index-1]['raw_reg'])
        print(instr_list[index-1]['raw_instr'])
    else:
        print("Previous instruction: None")
    
    print(f"\nCurrent instruction (#{index+1}):")
    print(instr_list[index]['raw_reg'])
    print(instr_list[index]['raw_instr'])
    
    if index+1 < len(instr_list):
        print(f"\nNext instruction (#{index+2}):")
        print(instr_list[index+1]['raw_reg'])
        print(instr_list[index+1]['raw_instr'])
    else:
        print("\nNext instruction: None")
    print("\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: python compare_disasm.py <disasm_file1> <disasm_file2>")
        sys.exit(1)
    file1, file2 = sys.argv[1], sys.argv[2]
    with open(file1, 'r') as f:
        text1 = f.read()
    with open(file2, 'r') as f:
        text2 = f.read()
    
    instrs1 = parse_disassembly(text1)
    instrs2 = parse_disassembly(text2)
    
    min_len = min(len(instrs1), len(instrs2))
    for i in range(min_len):
        diffs = compare_instructions(instrs1[i], instrs2[i])
        if diffs:
            print(f"Mismatch found at instruction #{i+1}:")
            for diff in diffs:
                print("  ", diff)
            print("\nContext from file1:")
            print_instruction_context(instrs1, i, file1)
            print("Context from file2:")
            print_instruction_context(instrs2, i, file2)
            sys.exit(0)
    
    # If no mismatch is found in the overlapping instructions.
    if len(instrs1) != len(instrs2):
        print("No mismatches in crucial elements were found, but the number of instructions differs.")
    else:
        print("No mismatches found in the crucial elements of the disassembly.")

if __name__ == "__main__":
    main()

