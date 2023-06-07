PATH_TO_WORKING_GB = r"C:\Users\apc09\Downloads\log.txt"
PATH_TO_GBC = r"C:\Users\apc09\Desktop\GBC\log1.txt"

working = open(PATH_TO_WORKING_GB)
mine = open(PATH_TO_GBC)

working_stuff = working.readlines()
mine_stuff = mine.readlines()

working_size = len(working_stuff)
mine_size = len(mine_stuff)

print("Starting line of GBC:", mine_stuff[0])
print("Starting line of wrk:", working_stuff[0])

def compare_mismatch(mg1, mg2, gb1, gb2):
    # Don't compare flag values
    # Instead, focus on reg values and PC's value
    pc_mg2 = mg2[:8]
    pc_gb2 = gb2[:8]
    
    return mg1 == gb1 and pc_mg2 == pc_gb2

def elaborate_mismatch(index):
    dispatch_count = index // 2
    print(f"-- DISPATCH {dispatch_count} --")
    
    megagbc1 = working_stuff[index].replace("\n","")
    megagbc2 = working_stuff[index + 1].replace("\n","")

    gbc1 = mine_stuff[index].replace("\n","")
    gbc2 = mine_stuff[index + 1].replace("\n","")

    print("(GBC)")
    print(megagbc1)
    print(megagbc2)

    print("(MINE)")
    print(gbc1)
    print(gbc2)
    print(megagbc1 == gbc1)
    print(megagbc2 == gbc2)
    print()

index = 0

while index < mine_size:
    megagbc1 = working_stuff[index].replace("\n","")
    megagbc2 = working_stuff[index + 1].replace("\n","")

    gbc1 = mine_stuff[index].replace("\n","")
    gbc2 = mine_stuff[index + 1].replace("\n","")
    
    if not compare_mismatch(megagbc1, megagbc2, gbc1, gbc2):
        elaborate_mismatch(index - 2)
        elaborate_mismatch(index)
        elaborate_mismatch(index + 2)
        break
    index += 2
    
working.close()
mine.close()
