HASH_TABLE_SIZE = 512

def hash(str):
    h = 0
    str = str.lower()
    for c in str:
        h = (h << 4) + ord(c)
        g = h & 0xf0000000
        if g != 0:
            h ^= g >> 24
            h ^= g

    return h


def get_int(db, str):
    h = hash(str)
    row_id = h % HASH_TABLE_SIZE
    elem_id = db[row_id].index(str)
    return row_id << 16 | elem_id


def get_str(db, i):
    row_id = i >> 16
    elem_id = i & 0xffff
    if row_id >= HASH_TABLE_SIZE or elem_id >= len(db[row_id]):
        return f'str_db[{hex(i)}]'
    return db[row_id][elem_id]


def create_db(str_list):
    db = []
    for i in range(HASH_TABLE_SIZE):
        db.append([])

    for str in str_list:
        h = hash(str)
        row_id = h % HASH_TABLE_SIZE
        db[row_id].append(str)

    return db
