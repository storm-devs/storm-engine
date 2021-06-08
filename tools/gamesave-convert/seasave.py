import struct

def read_buffer(buffer, cur_ptr):
    size = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    buf = struct.unpack_from(f'{size}s', buffer, cur_ptr)[0]
    cur_ptr += size
    return buf, cur_ptr


def read_string(buffer, cur_ptr, encoding):
    str_len = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    if str_len == 0:
        return '', cur_ptr
    s = struct.unpack_from(f'{str_len-1}s', buffer, cur_ptr)[0]  # str_len-1 to skip trailing '\0'
    s = s.decode(encoding)
    cur_ptr += str_len
    return s, cur_ptr


def read_attr_ptr(buffer, cur_ptr):    
    index = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    s, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    attr_ptr = {
        'index' : index,
        'str': s
    }
    return attr_ptr, cur_ptr


def read_aiballs(buffer, cur_ptr):
    return


def read_aicannon():
    return


def read_aifort():
    return


def read_aigroup():
    return


def read_aihelper(buffer, cur_ptr):
    gravity = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    num_relations = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    relations, cur_ptr = read_buffer(buffer, cur_ptr)
    sea_cameras, cur_ptr = read_attr_ptr(buffer, cur_ptr)

    num_characters = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    characters = []
    for _ in range(num_characters):
        char_ptr, cur_ptr = read_attr_ptr(buffer, cur_ptr)
        characters.append(char_ptr)

    num_main_characters = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    main_characters = []
    for _ in range(num_main_characters):
        char_ptr, cur_ptr = read_attr_ptr(buffer, cur_ptr)
        main_characters.append(char_ptr)

    aihelper = {
        'gravity': gravity,
        'num_relations': num_relations,
        'relations': relations,
        'sea_cameras': sea_cameras,
        'characters': characters,
        'main_characters': main_characters
    }
    return aihelper, cur_ptr


def read_ship():
    return


def read_aiship():
    return


def read_aiship_camera_controller():
    return


def read_aiship_cannon_controller():
    return


def read_aiship_move_controller():
    return


def read_aiship_rotate_controller():
    return


def read_aiship_speed_controller():
    return


def read_aiship_task_controller():
    return


def read_aiship_touch_controller():
    return


def read_deck_cam(buffer, cur_ptr):
    vertices, cur_ptr = read_buffer(buffer, cur_ptr)

    format = '13f3f3f3f3f3ff'
    data = struct.unpack_from(format, buffer, cur_ptr)
    cur_ptr += struct.calcsize(format)

    deck_cam = {
        'distance_sensivity': data[0],
        'height_angle_sensivity': data[1],
        'azimuth_angle_sensivity': data[2],
        'rocking_x': data[3],
        'rocking_z': data[4],
        'men_step_up': data[5],
        'men_step_min': data[6],
        'height_max': data[7],
        'height_min': data[8],
        'height_step': data[9],
        'cam_max_x': data[10],
        'cam_min_x': data[11],
        'default_height': data[12],
        'g_vec1': { 'x': data[13], 'y': data[14], 'z': data[15] },
        'g_vec2': { 'x': data[16], 'y': data[17], 'z': data[18] },
        'g_vec3': { 'x': data[19], 'y': data[20], 'z': data[21] },
        'cam_pos': { 'x': data[22], 'y': data[23], 'z': data[24] },
        'cam_angle': { 'x': data[25], 'y': data[26], 'z': data[27] },
        'eye_height': data[28]
    }
    assert(len(data) == 29)

    deck_cam['screen_rect'], cur_ptr = read_buffer(buffer, cur_ptr)

    format = '4If'
    data = struct.unpack_from(format, buffer, cur_ptr)
    cur_ptr += struct.calcsize(format)
    deck_cam['lock_x'] = data[0]
    deck_cam['lock_y'] = data[1]
    deck_cam['is_on'] = data[2]
    deck_cam['is_active'] = data[3]
    deck_cam['perspective'] = data[4]
    assert(len(data) == 5)

    deck_cam['character'], cur_ptr = read_attr_ptr(buffer, cur_ptr)

    return deck_cam, cur_ptr


def read_free_cam(buffer, cur_ptr):
    format = '3f3ffIIIf'
    data = struct.unpack_from(format, buffer, cur_ptr)
    cur_ptr += struct.calcsize(format)

    free_cam = {
        'pos': { 'x': data[0], 'y': data[1], 'z': data[2] },
        'angle': { 'x': data[3], 'y': data[4], 'z': data[5] },
        'fov': data[6],
        'lock_x': data[7],
        'lock_y': data[8],
        'is_onland': data[9],
        'cam_onland_height': data[10]
    }
    assert(len(data) == 11)
    
    return free_cam, cur_ptr


def read_ship_cam(buffer, cur_ptr):
    format = 'II19f3f3ff4If'
    data = struct.unpack_from(format, buffer, cur_ptr)
    cur_ptr += struct.calcsize(format)

    ship_cam = {
        'lock_x': data[0],
        'lock_y': data[1],
        'min_height_on_sea': data[2],
        'max_height_on_sea': data[3],
        'distance': data[4],
        'max_distance': data[5],
        'min_distance': data[6],
        'distance_delta': data[7],
        'distance_inertia': data[8],
        'min_angle_x': data[9],
        'max_angle_x': data[10],
        'angle_x_delta': data[11],
        'angle_x_inertia': data[12],
        'angle_y_delta': data[13],
        'angle_y_inertia': data[14],
        'distance_sensivity': data[15],
        'azimuth_angle_sensivity': data[16],
        'height_angle_sensivity': data[17],
        'height_angle_onship_sensivity': data[18],
        'invert_mouse_x': data[19],
        'invert_mouse_y': data[20],
        'center': { 'x': data[21], 'y': data[22], 'z': data[23] },
        'angle': { 'x': data[24], 'y': data[25], 'z': data[26] },
        'model_atan_y': data[27],
        'ship_code': data[28],
        'num_islands': data[29],
        'is_on': data[30],
        'is_active': data[31],
        'perspective': data[32]
    }
    assert(len(data) == 33)

    ship_cam['character'], cur_ptr = read_attr_ptr(buffer, cur_ptr)

    return ship_cam, cur_ptr


def read_fireplace():
    return


def read_seasave(buffer):
    seasave_data = {}

    cur_ptr = 0
    aihelper, cur_ptr = read_aihelper(buffer, cur_ptr)

    ship_cam, cur_ptr = read_ship_cam(buffer, cur_ptr)
    free_cam, cur_ptr = read_free_cam(buffer, cur_ptr)
    deck_cam, cur_ptr = read_deck_cam(buffer, cur_ptr)

    aiballs, curptr = read_aiballs(buffer, cur_ptr)


def write_seasave():
    return