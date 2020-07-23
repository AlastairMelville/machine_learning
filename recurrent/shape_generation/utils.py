
a_idx = ord('a')

def idx_to_element(index : int):
    return chr(a_idx + index)

def element_to_idx(element) -> int:
    return ord(element) - a_idx

def get_newline_id() -> int:
    return element_to_idx('z') + 1
