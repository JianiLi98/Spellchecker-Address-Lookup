# Patricia Tree Spellchecker for Victorian Address Lookup

This project extends a linked list-based dictionary to a **Patricia tree** dictionary that handles **misspelled queries**. It reads a CSV dataset of Victorian addresses and allows users to search by `EZI_ADD`. Queries return **exact matches**, or if not found, the **closest recommended key**.

---

## Features

- Exact match lookup
- Similar match lookup (spellchecker)
- Handles no match with `NOTFOUND`
- Tracks search performance (comparisons, node accesses)
- Works with linked list (Assignment 1) for performance comparison

---

## Implementation

- **Language:** C
- **Data Structure:** Patricia tree
- **Input CSV:** Victorian addresses dataset
- **Executable:** `dict2`

**Command-line Usage:**

```bash
./dict2 <stage> <input_file> <output_file>
