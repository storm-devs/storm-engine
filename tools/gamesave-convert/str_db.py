import logging

HASH_TABLE_SIZE = 512


def hash_str(s, str_encoding):
    h = 0
    str_bytes = s.encode(str_encoding)
    for b in str_bytes:
        if ord('A') <= b <= ord('Z'):
            b += ord('a') - ord('A')
        h = (h << 4) + b
        g = h & 0xf0000000
        if g != 0:
            h ^= g >> 24
            h ^= g

    return h


def get_int(db, s, str_encoding):
    h = hash_str(s, str_encoding)
    row_id = h % HASH_TABLE_SIZE
    if row_id >= HASH_TABLE_SIZE or s not in db[row_id]:
        logging.warning(f"couldn't find string {s} in the string database")
        return -1
    elem_id = db[row_id].index(s)
    return row_id << 16 | elem_id
    

def get_str(db, i):
    row_id = i >> 16
    elem_id = i & 0xffff
    if row_id >= HASH_TABLE_SIZE or elem_id >= len(db[row_id]):
        logging.warning(f"couldn't find name code {i} in the string database")
        return f'str_db[{hex(i)}]'
    entry = db[row_id][elem_id]
    return entry


def create_db(str_list, str_encoding):
    db = []
    for i in range(HASH_TABLE_SIZE):
        db.append([])

    for s in str_list:
        h = hash_str(s, str_encoding)
        row_id = h % HASH_TABLE_SIZE
        db[row_id].append(s)

    return db


def remove_unused(db, used_str, str_encoding):
    unused_str = []
    for row in db:
        for elem in row:
            if elem not in used_str:
                unused_str.append(elem)

    for s in unused_str:
        h = hash_str(s, str_encoding)
        row_id = h % HASH_TABLE_SIZE
        elem_id = db[row_id].index(s)
        del db[row_id][elem_id]

    return db