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
    if str_len > 0:
        s = struct.unpack_from(f'{str_len - 1}s', buffer, cur_ptr)[0]  # str_len-1 to skip trailing '\0'
        s = s.decode(encoding)
        cur_ptr += str_len
    else:
        s = ''
    return s, cur_ptr


def read_attr_ptr(buffer, cur_ptr):
    index = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    s, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    attr_ptr = {
        'index': index,
        'str': s
    }
    return attr_ptr, cur_ptr


def read_aiballs(buffer, cur_ptr):
    num_ball_types = 4  # hardcode
    ball_types = []
    for _ in range(num_ball_types):
        size = struct.unpack_from('I', buffer, cur_ptr)[0]
        cur_ptr += 4
        balls = []
        for _ in range(size):
            first_pos = struct.unpack_from('3f', buffer, cur_ptr)[0]
            cur_ptr += 3 * 4
            pos = struct.unpack_from('3f', buffer, cur_ptr)[0]
            cur_ptr += 3 * 4
            particle_ptr = struct.unpack_from('p', buffer, cur_ptr)[0]
            cur_ptr += 8
            ball_event, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')

            fmt = 'IffffffffffI'
            data = struct.unpack_from(fmt, buffer, cur_ptr)
            cur_ptr += struct.calcsize(fmt)

            ball_params = {
                'first_pos': first_pos,
                'pos': pos,
                'particle_ptr': particle_ptr,
                'ball_event': ball_event,
                'ball_owner': data[0],
                'time': data[1],
                'speedV0': data[2],
                'dir_x': data[3],
                'dir_z': data[4],
                'sin_angle': data[5],
                'cos_angle': data[6],
                'height_multiply': data[7],
                'size_multiply': data[8],
                'time_speed_multiply': data[9],
                'max_fire_distance': data[10],
                'cannon_type': data[11]
            }
            assert (len(data) == 12)

            balls.append(ball_params)

        ball_types.append(balls)

    aiballs = {'ball_types': ball_types}

    return aiballs, cur_ptr


def read_aicannon(buffer, cur_ptr):
    fmt = '3f3ffff3fIfIIIII'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    cannon = {
        'pos': {'x': data[0], 'y': data[1], 'z': data[2]},
        'dir': {'x': data[3], 'y': data[4], 'z': data[5]},
        'time2action': data[6],
        'total_time2action': data[7],
        'speedV0': data[8],
        'enemy_pos': {'x': data[9], 'y': data[10], 'z': data[11]},
        'empty': data[12],
        'damaged': data[13],
        'fired': data[14],
        'ready2fire': data[15],
        'recharged': data[16],
        'load': data[17],
        'can_recharge': data[18]
    }
    assert (len(data) == 19)

    return cannon, cur_ptr


def read_aifort(buffer, cur_ptr):
    min_cannon_damage_distance = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4

    num_forts = 4  # hardcode
    forts = []
    for _ in range(num_forts):
        pos_data = struct.unpack_from('3f', buffer, cur_ptr)
        pos = {'x': pos_data[0], 'y': pos_data[1], 'z': pos_data[2]}
        cur_ptr += 3 * 4

        num_cannons = 4  # hardcode
        cannons = []
        for _ in range(num_cannons):
            cannon, cur_ptr = read_aicannon(buffer, cur_ptr)
            cannons.append(cannon)

        num_culevrins = 4  # hardcode
        culevrins = []
        for _ in range(num_culevrins):
            culevrin, cur_ptr = read_aicannon(buffer, cur_ptr)
            culevrins.append(culevrin)

        num_mortars = 4  # hardcode
        mortars = []
        for _ in range(num_mortars):
            mortar, cur_ptr = read_aicannon(buffer, cur_ptr)
            mortars.append(mortar)

        fort = {
            'pos': pos,
            'cannons': cannons,
            'culevrins': culevrins,
            'mortars': mortars,
        }
        forts.append(fort)

    aifort = {
        'min_cannon_damage_distance': min_cannon_damage_distance,
        'forts': forts,
    }

    return aifort, cur_ptr


def read_aigroup(buffer, cur_ptr):
    commander, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    group_name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    command, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    command_group, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    location_near_other_group, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    group_type, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    init_group_pos_data = struct.unpack_from('3f', buffer, cur_ptr)
    init_group_pos = {'x': init_group_pos_data[0], 'y': init_group_pos_data[1], 'z': init_group_pos_data[2]}
    cur_ptr += 3 * 4
    move_point_data = struct.unpack_from('3f', buffer, cur_ptr)
    move_point = {'x': move_point_data[0], 'y': move_point_data[1], 'z': move_point_data[2]}
    cur_ptr += 3 * 4
    first_execute = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    num_ships = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    ships = []
    for _ in range(num_ships):
        ship, cur_ptr = read_aiship(buffer, cur_ptr)
        ships.append(ship)

    aigroup = {
        'commander': commander,
        'group_name': group_name,
        'command': command,
        'command_group': command_group,
        'location_near_other_group': location_near_other_group,
        'group_type': group_type,
        'init_group_pos': init_group_pos,
        'move_point': move_point,
        'first_execute': first_execute,
        'ships': ships,
    }

    return aigroup, cur_ptr


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


def read_ship(buffer, cur_ptr):
    character, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    ship_attr_ptr, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    realize_layer, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    execute_layer, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    ship_name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    ship_priority_execute = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    gravity = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    sail_state = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    uni_idx = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    use = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    ship_points, cur_ptr = read_buffer(buffer, cur_ptr)
    speed_accel_data = struct.unpack_from('3f', buffer, cur_ptr)
    speed_accel = {'x': speed_accel_data[0], 'y': speed_accel_data[1], 'z': speed_accel_data[2]}
    cur_ptr += 3 * 4
    sp, cur_ptr = read_buffer(buffer, cur_ptr)
    pos_data = struct.unpack_from('3f', buffer, cur_ptr)
    pos = {'x': pos_data[0], 'y': pos_data[1], 'z': pos_data[2]}
    cur_ptr += 3 * 4
    angle_data = struct.unpack_from('3f', buffer, cur_ptr)
    angle = {'x': angle_data[0], 'y': angle_data[1], 'z': angle_data[2]}
    cur_ptr += 3 * 4
    water_line = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_dead = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_visible = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    dead_dir_data = struct.unpack_from('3f', buffer, cur_ptr)
    dead_dir = {'x': dead_dir_data[0], 'y': dead_dir_data[1], 'z': dead_dir_data[2]}
    cur_ptr += 3 * 4
    cur_dead_dir_data = struct.unpack_from('3f', buffer, cur_ptr)
    cur_dead_dir = {'x': cur_dead_dir_data[0], 'y': cur_dead_dir_data[1], 'z': cur_dead_dir_data[2]}
    cur_ptr += 3 * 4
    keel_contour, cur_ptr = read_buffer(buffer, cur_ptr)
    is_ship2strand = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_mounted = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_keel_contour = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_perk_turn_active = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    initial_perk_angle = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    result_perk_angle = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    strength, cur_ptr = read_buffer(buffer, cur_ptr)
    is_set_light_and_fog = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    save_ambient = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    save_fog_color = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    save_light, cur_ptr = read_buffer(buffer, cur_ptr)
    state, cur_ptr = read_buffer(buffer, cur_ptr)

    num_masts = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    masts = []
    for _ in range(num_masts):
        fmt = '3f3fIIf'
        data = struct.unpack_from(fmt, buffer, cur_ptr)
        cur_ptr += struct.calcsize(fmt)

        mast = {
            'v_src': {'x': data[0], 'y': data[1], 'z': data[2]},
            'v_dst': {'x': data[3], 'y': data[4], 'z': data[5]},
            'mast_num': data[6],
            'is_broken': data[7],
            'damage': data[8]
        }
        assert (len(data) == 9)
        masts.append(mast)

    num_hulls = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    hulls = []
    for _ in range(num_hulls):
        fmt = '3f3fIIf'
        data = struct.unpack_from(fmt, buffer, cur_ptr)
        cur_ptr += struct.calcsize(fmt)

        hull = {
            'v_src': {'x': data[0], 'y': data[1], 'z': data[2]},
            'v_dst': {'x': data[3], 'y': data[4], 'z': data[5]},
            'hull_num': data[6],
            'is_broken': data[7],
            'damage': data[8]
        }
        assert (len(data) == 9)
        hulls.append(hull)

    num_fireplaces = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    fireplaces = []
    for _ in range(num_fireplaces):
        fireplace, cur_ptr = read_fireplace(buffer, cur_ptr)
        fireplaces.append(fireplace)

    x_heel = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4
    z_heel = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4

    ship = {
        'character': character,
        'ship_attr_ptr': ship_attr_ptr,
        'realize_layer': realize_layer,
        'execute_layer': execute_layer,
        'ship_name': ship_name,
        'ship_priority_execute': ship_priority_execute,
        'gravity': gravity,
        'sail_state': sail_state,
        'uni_idx': uni_idx,
        'use': use,
        'ship_points': ship_points,
        'speed_accel': speed_accel,
        'sp': sp,
        'pos': pos,
        'angle': angle,
        'water_line': water_line,
        'is_dead': is_dead,
        'is_visible': is_visible,
        'dead_dir': dead_dir,
        'cur_dead_dir': cur_dead_dir,
        'keel_contour': keel_contour,
        'is_ship2strand': is_ship2strand,
        'is_mounted': is_mounted,
        'is_keel_contour': is_keel_contour,
        'is_perk_turn_active': is_perk_turn_active,
        'initial_perk_angle': initial_perk_angle,
        'result_perk_angle': result_perk_angle,
        'strength': strength,
        'is_set_light_and_fog': is_set_light_and_fog,
        'save_ambient': save_ambient,
        'save_fog_color': save_fog_color,
        'save_light': save_light,
        'state': state,
        'masts': masts,
        'hulls': hulls,
        'fireplaces': fireplaces,
        'x_heel': x_heel,
        'z_heel': z_heel
    }

    return ship, cur_ptr


def read_aiship(buffer, cur_ptr):
    base_ship, cur_ptr = read_ship(buffer, cur_ptr)

    character, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    obj_type = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    is_dead = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4

    cannon_ctrl, cur_ptr = read_aiship_cannon_controller(buffer, cur_ptr)

    has_cam_ctrl = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    if has_cam_ctrl:
        cam_ctrl, cur_ptr = read_aiship_camera_controller(buffer, cur_ptr)
    else:
        cam_ctrl = None

    move_ctrl, cur_ptr = read_aiship_move_controller(buffer, cur_ptr)
    rotate_ctrl, cur_ptr = read_aiship_rotate_controller(buffer, cur_ptr)
    speed_ctrl, cur_ptr = read_aiship_speed_controller(buffer, cur_ptr)
    task_ctrl, cur_ptr = read_aiship_task_controller(buffer, cur_ptr)
    touch_ctrl, cur_ptr = read_aiship_touch_controller(buffer, cur_ptr)

    aiship = {
        'base_ship': base_ship,
        'character': character,
        'obj_type': obj_type,
        'is_dead': is_dead,
        'cannon_ctrl': cannon_ctrl,
        'cam_ctrl': cam_ctrl,
        'move_ctrl': move_ctrl,
        'rotate_ctrl': rotate_ctrl,
        'speed_ctrl': speed_ctrl,
        'task_ctrl': task_ctrl,
        'touch_ctrl': touch_ctrl,
    }

    return aiship, cur_ptr


def read_aiship_camera_controller(buffer, cur_ptr):
    fmt = 'IfI'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'target': data[0],
        'delta': data[1],
        'cam_outside': data[2]
    }
    assert (len(data) == 3)

    return controller, cur_ptr


def read_aiship_cannon_controller(buffer, cur_ptr):
    fmt = '3I'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'reload': data[0],
        'not_enough_balls': data[1],
        'temp_flag': data[2]
    }
    assert (len(data) == 3)

    num_borts = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    borts = []
    for _ in range(num_borts):
        name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')

        fmt = '7fIff3f'
        data = struct.unpack_from(fmt, buffer, cur_ptr)
        cur_ptr += struct.calcsize(fmt)

        bort = {
            'name': name,
            'fire_zone': data[0],
            'fire_angle_min': data[1],
            'fire_angle_max': data[2],
            'fire_dir': data[3],
            'fire_heigh': data[4],
            'charge_percent': data[5],
            'cos_fire_zone': data[6],
            'num_cannons': data[7],
            'speedV0': data[8],
            'max_fire_distance': data[9],
            'direction': {'x': data[10], 'y': data[11], 'z': data[12]}
        }
        assert (len(data) == 13)

        num_cannons = struct.unpack_from('I', buffer, cur_ptr)[0]
        cur_ptr += 4
        cannons = []
        for _ in range(num_cannons):
            cannon, cur_ptr = read_aicannon(buffer, cur_ptr)
            cannons.append(cannon)

        bort['cannons'] = cannons
        borts.append(bort)

    controller['borts'] = borts

    return controller, cur_ptr


def read_aiship_move_controller(buffer, cur_ptr):
    fmt = 'I3f3f3ffI'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'stopped': data[0],
        'dest_point': {'x': data[1], 'y': data[2], 'z': data[3]},
        'braking_force': {'x': data[4], 'y': data[5], 'z': data[6]},
        'deviation_force': {'x': data[7], 'y': data[8], 'z': data[9]},
        'move_time': data[10],
        'cur_point': data[11]
    }
    assert (len(data) == 12)

    return controller, cur_ptr


def read_aiship_rotate_controller(buffer, cur_ptr):
    fmt = 'I5f'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'rotate_num': data[0],
        'rotate_mode': data[1],
        'rotate_time': data[2],
        'rotate_smooth': data[3],
        'rotate': data[4],
        'global_multiply': data[5]
    }
    assert (len(data) == 6)

    return controller, cur_ptr


def read_aiship_speed_controller(buffer, cur_ptr):
    fmt = 'I5f'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'speed_num': data[0],
        'speed_smooth': data[1],
        'speed': data[2],
        'speed_time': data[3],
        'top_speed': data[4],
        'global_multiply': data[5]
    }
    assert (len(data) == 6)

    return controller, cur_ptr


def read_aiship_task_controller(buffer, cur_ptr):
    extra_distance = struct.unpack_from('f', buffer, cur_ptr)[0]
    cur_ptr += 4

    # primary
    active = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    task_type = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    task_character, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    fmt = '3f'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)
    task_point = {'x': data[0], 'y': data[1], 'z': data[2]}
    assert (len(data) == 3)
    primary_task = {
        'active': active,
        'task_type': task_type,
        'task_character': task_character,
        'task_point': task_point
    }

    # secondary
    active = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    task_type = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    task_character, cur_ptr = read_attr_ptr(buffer, cur_ptr)
    fmt = '3f'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)
    task_point = {'x': data[0], 'y': data[1], 'z': data[2]}
    assert (len(data) == 3)
    secondary_task = {
        'active': active,
        'task_type': task_type,
        'task_character': task_character,
        'task_point': task_point
    }

    controller = {
        'extra_distance': extra_distance,
        'primary_task': primary_task,
        'secondary_task': secondary_task
    }

    return controller, cur_ptr


def read_aiship_touch_controller(buffer, cur_ptr):
    num_rays = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    rays = []
    for _ in range(num_rays):
        fmt = '3f'
        data = struct.unpack_from(fmt, buffer, cur_ptr)
        cur_ptr += struct.calcsize(fmt)
        ray = {'x': data[0], 'y': data[1], 'z': data[2]}
        assert (len(data) == 3)
        rays.append(ray)

    fmt = '5f15f'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    controller = {
        'rays': rays,
        'left_rays_free': data[0],
        'right_rays_free': data[1],
        'ray_size': data[2],
        'speed_factor': data[3],
        'rotate_factor': data[4],
        'box1': {'x': data[5], 'y': data[6], 'z': data[7]},
        'box2': {'x': data[8], 'y': data[9], 'z': data[10]},
        'box3': {'x': data[11], 'y': data[12], 'z': data[13]},
        'box4': {'x': data[14], 'y': data[15], 'z': data[16]},
        'box5': {'x': data[17], 'y': data[18], 'z': data[19]}
    }
    assert (len(data) == 20)

    return controller, cur_ptr


def read_deck_cam(buffer, cur_ptr):
    vertices, cur_ptr = read_buffer(buffer, cur_ptr)

    fmt = '13f3f3f3f3f3ff'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    deck_cam = {
        'vertices': vertices,
        'distance_sensitivity': data[0],
        'height_angle_sensitivity': data[1],
        'azimuth_angle_sensitivity': data[2],
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
        'g_vec1': {'x': data[13], 'y': data[14], 'z': data[15]},
        'g_vec2': {'x': data[16], 'y': data[17], 'z': data[18]},
        'g_vec3': {'x': data[19], 'y': data[20], 'z': data[21]},
        'cam_pos': {'x': data[22], 'y': data[23], 'z': data[24]},
        'cam_angle': {'x': data[25], 'y': data[26], 'z': data[27]},
        'eye_height': data[28]
    }
    assert (len(data) == 29)

    deck_cam['screen_rect'], cur_ptr = read_buffer(buffer, cur_ptr)

    fmt = '4If'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)
    deck_cam['lock_x'] = data[0]
    deck_cam['lock_y'] = data[1]
    deck_cam['is_on'] = data[2]
    deck_cam['is_active'] = data[3]
    deck_cam['perspective'] = data[4]
    assert (len(data) == 5)

    deck_cam['character'], cur_ptr = read_attr_ptr(buffer, cur_ptr)

    return deck_cam, cur_ptr


def read_free_cam(buffer, cur_ptr):
    fmt = '3f3ffIIIf'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    free_cam = {
        'pos': {'x': data[0], 'y': data[1], 'z': data[2]},
        'angle': {'x': data[3], 'y': data[4], 'z': data[5]},
        'fov': data[6],
        'lock_x': data[7],
        'lock_y': data[8],
        'is_onland': data[9],
        'cam_onland_height': data[10]
    }
    assert (len(data) == 11)

    return free_cam, cur_ptr


def read_ship_cam(buffer, cur_ptr):
    fmt = 'II19f3f3ff4If'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    ship_cam = {
        'lock_x': data[0],
        'lock_y': data[1],
        'min_height_on_sea': data[2],
        'max_height_on_ship': data[3],
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
        'distance_sensitivity': data[15],
        'azimuth_angle_sensitivity': data[16],
        'height_angle_sensitivity': data[17],
        'height_angle_onship_sensitivity': data[18],
        'invert_mouse_x': data[19],
        'invert_mouse_y': data[20],
        'center': {'x': data[21], 'y': data[22], 'z': data[23]},
        'angle': {'x': data[24], 'y': data[25], 'z': data[26]},
        'model_atan_y': data[27],
        'ship_code': data[28],
        'islands_init_count': data[29],
        'is_on': data[30],
        'is_active': data[31],
        'perspective': data[32]
    }
    assert (len(data) == 33)

    ship_cam['character'], cur_ptr = read_attr_ptr(buffer, cur_ptr)

    return ship_cam, cur_ptr


def read_fireplace(buffer, cur_ptr):
    fmt = '3fIfI'
    data = struct.unpack_from(fmt, buffer, cur_ptr)
    cur_ptr += struct.calcsize(fmt)

    particle_smoke_name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    particle_fire_name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')
    sound_name, cur_ptr = read_string(buffer, cur_ptr, 'cp1251')

    fireplace = {
        'orig_pos': {'x': data[0], 'y': data[1], 'z': data[2]},
        'active': data[3],
        'run_time': data[4],
        'ball_character_index': data[5],
        'particle_smoke_name': particle_smoke_name,
        'particle_fire_name': particle_fire_name,
        'sound_name': sound_name
    }
    assert (len(data) == 6)

    return fireplace, cur_ptr


def read_seasave(buffer):
    cur_ptr = 0
    aihelper, cur_ptr = read_aihelper(buffer, cur_ptr)

    ship_cam, cur_ptr = read_ship_cam(buffer, cur_ptr)
    free_cam, cur_ptr = read_free_cam(buffer, cur_ptr)
    deck_cam, cur_ptr = read_deck_cam(buffer, cur_ptr)

    aiballs, cur_ptr = read_aiballs(buffer, cur_ptr)

    num_groups = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += 4
    aigroups = []
    for _ in range(num_groups):
        group, cur_ptr = read_aigroup(buffer, cur_ptr)
        aigroups.append(group)

    if len(buffer) > cur_ptr:
        aifort, cur_ptr = read_aifort(buffer, cur_ptr)
    else:
        aifort = None

    seasave = {
        'aihelper': aihelper,
        'ship_cam': ship_cam,
        'free_cam': free_cam,
        'deck_cam': deck_cam,
        'aiballs': aiballs,
        'aigroups': aigroups,
        'aifort': aifort
    }

    return seasave


# Serialization

def write_buffer(buf, buffer):
    size = len(buf)
    buffer += struct.pack('I', size)
    buffer += struct.pack(f'{size}s', buf)
    return buffer


def write_string(s, buffer):
    if s != '':
        s = s.encode('utf-8') + b'\x00'
        str_len = len(s)
        buffer += struct.pack('I', str_len)
        buffer += struct.pack(f'{str_len}s', s)
    else:
        buffer += struct.pack('I', 0)
    return buffer


def write_attr_ptr(attr_ptr, buffer):
    buffer += struct.pack('I', attr_ptr['index'])
    buffer = write_string(attr_ptr['str'], buffer)
    return buffer


def write_aiballs(aiballs, buffer):
    for balls in aiballs['ball_types']:
        buffer += struct.pack('I', len(balls))
        for ball in balls:
            buffer += struct.pack('3f', ball['first_pos']['x'], ball['first_pos']['y'], ball['first_pos']['z'])
            buffer += struct.pack('3f', ball['pos']['x'], ball['pos']['y'], ball['pos']['z'])
            buffer += struct.pack('p', ball['particle_ptr'])
            buffer = write_string(ball['ball_event'], buffer)
            buffer += struct.pack('I', ball['ball_owner'])
            buffer += struct.pack('f', ball['time'])
            buffer += struct.pack('f', ball['speedV0'])
            buffer += struct.pack('f', ball['dir_x'])
            buffer += struct.pack('f', ball['dir_z'])
            buffer += struct.pack('f', ball['sin_angle'])
            buffer += struct.pack('f', ball['cos_angle'])
            buffer += struct.pack('f', ball['height_multiply'])
            buffer += struct.pack('f', ball['size_multiply'])
            buffer += struct.pack('f', ball['time_speed_multiply'])
            buffer += struct.pack('f', ball['max_fire_distance'])
            buffer += struct.pack('I', ball['cannon_type'])

    return buffer


def write_aicannon(aicannon, buffer):
    buffer += struct.pack('3f', aicannon['pos']['x'], aicannon['pos']['y'], aicannon['pos']['z'])
    buffer += struct.pack('3f', aicannon['dir']['x'], aicannon['dir']['y'], aicannon['dir']['z'])
    buffer += struct.pack('f', aicannon['time2action'])
    buffer += struct.pack('f', aicannon['total_time2action'])
    buffer += struct.pack('f', aicannon['speedV0'])
    buffer += struct.pack('3f', aicannon['enemy_pos']['x'], aicannon['enemy_pos']['y'], aicannon['enemy_pos']['z'])
    buffer += struct.pack('I', aicannon['empty'])
    buffer += struct.pack('f', aicannon['damaged'])
    buffer += struct.pack('I', aicannon['fired'])
    buffer += struct.pack('I', aicannon['ready2fire'])
    buffer += struct.pack('I', aicannon['recharged'])
    buffer += struct.pack('I', aicannon['load'])
    buffer += struct.pack('I', aicannon['can_recharge'])

    return buffer


def write_aifort(aifort, buffer):
    buffer += struct.pack('f', aifort['min_cannon_damage_distance'])

    for fort in aifort['forts']:
        buffer += struct.pack('3f', fort['pos']['x'], fort['pos']['y'], fort['pos']['z'])
        for cannon in fort['cannons']:
            buffer = write_aicannon(cannon, buffer)
        for culevrin in fort['culevrins']:
            buffer = write_aicannon(culevrin, buffer)
        for mortar in fort['mortars']:
            buffer = write_aicannon(mortar, buffer)

    return buffer


def write_ship(ship, buffer):
    buffer = write_attr_ptr(ship['character'], buffer)
    buffer = write_attr_ptr(ship['ship_attr_ptr'], buffer)
    buffer += struct.pack('I', ship['realize_layer'])
    buffer += struct.pack('I', ship['execute_layer'])
    buffer = write_string(ship['ship_name'], buffer)
    buffer += struct.pack('I', ship['ship_priority_execute'])
    buffer += struct.pack('f', ship['gravity'])
    buffer += struct.pack('f', ship['sail_state'])
    buffer += struct.pack('I', ship['uni_idx'])
    buffer += struct.pack('I', ship['use'])
    buffer = write_buffer(ship['ship_points'], buffer)
    buffer += struct.pack('3f', ship['speed_accel']['x'], ship['speed_accel']['y'], ship['speed_accel']['z'])
    buffer = write_buffer(ship['sp'], buffer)
    buffer += struct.pack('3f', ship['pos']['x'], ship['pos']['y'], ship['pos']['z'])
    buffer += struct.pack('3f', ship['angle']['x'], ship['angle']['y'], ship['angle']['z'])
    buffer += struct.pack('f', ship['water_line'])
    buffer += struct.pack('I', ship['is_dead'])
    buffer += struct.pack('I', ship['is_visible'])
    buffer += struct.pack('3f', ship['dead_dir']['x'], ship['dead_dir']['y'], ship['dead_dir']['z'])
    buffer += struct.pack('3f', ship['cur_dead_dir']['x'], ship['cur_dead_dir']['y'], ship['cur_dead_dir']['z'])
    buffer = write_buffer(ship['keel_contour'], buffer)
    buffer += struct.pack('I', ship['is_ship2strand'])
    buffer += struct.pack('I', ship['is_mounted'])
    buffer += struct.pack('I', ship['is_keel_contour'])
    buffer += struct.pack('I', ship['is_perk_turn_active'])
    buffer += struct.pack('f', ship['initial_perk_angle'])
    buffer += struct.pack('f', ship['result_perk_angle'])
    buffer = write_buffer(ship['strength'], buffer)
    buffer += struct.pack('I', ship['is_set_light_and_fog'])
    buffer += struct.pack('I', ship['save_ambient'])
    buffer += struct.pack('I', ship['save_fog_color'])
    buffer = write_buffer(ship['save_light'], buffer)
    buffer = write_buffer(ship['state'], buffer)

    buffer += struct.pack('I', len(ship['masts']))
    for mast in ship['masts']:
        buffer += struct.pack('3f', mast['v_src']['x'], mast['v_src']['y'], mast['v_src']['z'])
        buffer += struct.pack('3f', mast['v_dst']['x'], mast['v_dst']['y'], mast['v_dst']['z'])
        buffer += struct.pack('I', mast['mast_num'])
        buffer += struct.pack('I', mast['is_broken'])
        buffer += struct.pack('f', mast['damage'])

    buffer += struct.pack('I', len(ship['hulls']))
    for hull in ship['hulls']:
        buffer += struct.pack('3f', hull['v_src']['x'], hull['v_src']['y'], hull['v_src']['z'])
        buffer += struct.pack('3f', hull['v_dst']['x'], hull['v_dst']['y'], hull['v_dst']['z'])
        buffer += struct.pack('I', hull['hull_num'])
        buffer += struct.pack('I', hull['is_broken'])
        buffer += struct.pack('f', hull['damage'])

    buffer += struct.pack('I', len(ship['fireplaces']))
    for fireplace in ship['fireplaces']:
        buffer = write_fireplace(fireplace, buffer)

    return buffer


def write_aiship(aiship, buffer):
    buffer = write_ship(aiship['base_ship'], buffer)

    buffer = write_attr_ptr(aiship['character'], buffer)
    buffer += struct.pack('I', aiship['obj_type'])
    buffer += struct.pack('I', aiship['is_dead'])

    buffer = write_aiship_cannon_controller(aiship['cannon_ctrl'], buffer)

    has_cam_ctrl = 1 if aiship['cam_ctrl'] is not None else 0
    buffer += struct.pack('I', has_cam_ctrl)
    if has_cam_ctrl != 0:
        buffer = write_aiship_camera_controller(aiship['cam_ctrl'], buffer)

    buffer = write_aiship_move_controller(aiship['move_ctrl'], buffer)
    buffer = write_aiship_rotate_controller(aiship['rotate_ctrl'], buffer)
    buffer = write_aiship_speed_controller(aiship['speed_ctrl'], buffer)
    buffer = write_aiship_task_controller(aiship['task_ctrl'], buffer)
    buffer = write_aiship_touch_controller(aiship['touch_ctrl'], buffer)

    return buffer


def write_aiship_camera_controller(controller, buffer):
    buffer += struct.pack('I', controller['target'])
    buffer += struct.pack('f', controller['delta'])
    buffer += struct.pack('I', controller['cam_outside'])

    return buffer


def write_aiship_cannon_controller(controller, buffer):
    buffer += struct.pack('I', controller['reload'])
    buffer += struct.pack('I', controller['not_enough_balls'])
    buffer += struct.pack('I', controller['temp_flag'])

    buffer += struct.pack('I', len(controller['borts']))
    for bort in controller['borts']:
        buffer = write_string(bort['name'], buffer)
        buffer += struct.pack('f', bort['fire_zone'])
        buffer += struct.pack('f', bort['fire_angle_min'])
        buffer += struct.pack('f', bort['fire_angle_max'])
        buffer += struct.pack('f', bort['fire_dir'])
        buffer += struct.pack('f', bort['fire_heigh'])
        buffer += struct.pack('f', bort['charge_percent'])
        buffer += struct.pack('f', bort['cos_fire_zone'])
        buffer += struct.pack('I', bort['num_cannons'])
        buffer += struct.pack('f', bort['speedV0'])
        buffer += struct.pack('f', bort['max_fire_distance'])
        buffer += struct.pack('3f', bort['direction']['x'], bort['direction']['y'], bort['direction']['z'])

        buffer += struct.pack('I', len(bort['cannons']))
        for cannon in bort['cannons']:
            buffer = write_aicannon(cannon, buffer)

    return buffer


def write_aiship_move_controller(controller, buffer):
    buffer += struct.pack('I', controller['stopped'])
    buffer += struct.pack('3f', controller['dest_point']['x'], controller['dest_point']['y'],
                          controller['dest_point']['z'])
    buffer += struct.pack('3f', controller['braking_force']['x'], controller['braking_force']['y'],
                          controller['braking_force']['z'])
    buffer += struct.pack('3f', controller['deviation_force']['x'], controller['deviation_force']['y'],
                          controller['deviation_force']['z'])
    buffer += struct.pack('f', controller['move_time'])
    buffer += struct.pack('I', controller['cur_point'])

    return buffer


def write_aiship_rotate_controller(controller, buffer):
    buffer += struct.pack('I', controller['rotate_num'])
    buffer += struct.pack('f', controller['rotate_mode'])
    buffer += struct.pack('f', controller['rotate_time'])
    buffer += struct.pack('f', controller['rotate_smooth'])
    buffer += struct.pack('f', controller['rotate'])
    buffer += struct.pack('f', controller['global_multiply'])

    return buffer


def write_aiship_speed_controller(controller, buffer):
    buffer += struct.pack('I', controller['speed_num'])
    buffer += struct.pack('f', controller['speed_smooth'])
    buffer += struct.pack('f', controller['speed'])
    buffer += struct.pack('f', controller['speed_time'])
    buffer += struct.pack('f', controller['top_speed'])
    buffer += struct.pack('f', controller['global_multiply'])

    return buffer


def write_aiship_task_controller(controller, buffer):
    buffer += struct.pack('f', controller['extra_distance'])

    primary_task = controller['primary_task']
    buffer += struct.pack('I', primary_task['active'])
    buffer += struct.pack('I', primary_task['task_type'])
    buffer = write_attr_ptr(primary_task['task_character'], buffer)
    buffer += struct.pack('3f', primary_task['task_point']['x'], primary_task['task_point']['y'],
                          primary_task['task_point']['z'])

    secondary_task = controller['secondary_task']
    buffer += struct.pack('I', secondary_task['active'])
    buffer += struct.pack('I', secondary_task['task_type'])
    buffer = write_attr_ptr(secondary_task['task_character'], buffer)
    buffer += struct.pack('3f', secondary_task['task_point']['x'], secondary_task['task_point']['y'],
                          secondary_task['task_point']['z'])

    return buffer


def write_aiship_touch_controller(controller, buffer):
    buffer += struct.pack('I', len(controller['rays']))
    for ray in controller['rays']:
        buffer += struct.pack('3f', ray['x'], ray['y'], ray['z'])

    buffer += struct.pack('f', controller['left_rays_free'])
    buffer += struct.pack('f', controller['right_rays_free'])
    buffer += struct.pack('f', controller['ray_size'])
    buffer += struct.pack('f', controller['speed_factor'])
    buffer += struct.pack('f', controller['rotate_factor'])
    buffer += struct.pack('3f', controller['box1']['x'], controller['box1']['y'],
                          controller['box1']['z'])
    buffer += struct.pack('3f', controller['box2']['x'], controller['box2']['y'],
                          controller['box2']['z'])
    buffer += struct.pack('3f', controller['box3']['x'], controller['box3']['y'],
                          controller['box3']['z'])
    buffer += struct.pack('3f', controller['box4']['x'], controller['box4']['y'],
                          controller['box4']['z'])
    buffer += struct.pack('3f', controller['box5']['x'], controller['box5']['y'],
                          controller['box5']['z'])

    return buffer


def write_aigroup(aigroup, buffer):
    buffer = write_attr_ptr(aigroup['commander'], buffer)
    buffer = write_string(aigroup['group_name'], buffer)
    buffer = write_string(aigroup['command'], buffer)
    buffer = write_string(aigroup['command_group'], buffer)
    buffer = write_string(aigroup['location_near_other_group'], buffer)
    buffer = write_string(aigroup['group_type'], buffer)
    buffer += struct.pack('3f', aigroup['init_group_pos']['x'], aigroup['init_group_pos']['y'],
                          aigroup['init_group_pos']['z'])
    buffer += struct.pack('3f', aigroup['move_point']['x'], aigroup['move_point']['y'], aigroup['move_point']['z'])
    buffer += struct.pack('I', aigroup['first_execute'])
    buffer += struct.pack('I', len(aigroup['ships']))
    for ship in aigroup['ships']:
        buffer = write_aiship(ship, buffer)

    return buffer


def write_aihelper(aihelper, buffer):
    buffer += struct.pack('f', aihelper['gravity'])
    buffer += struct.pack('I', aihelper['num_relations'])

    buffer = write_buffer(aihelper['relations'], buffer)
    buffer = write_attr_ptr(aihelper['sea_cameras'], buffer)

    buffer += struct.pack('I', len(aihelper['characters']))
    for c in aihelper['characters']:
        buffer = write_attr_ptr(c, buffer)

    buffer += struct.pack('I', len(aihelper['main_characters']))
    for c in aihelper['main_characters']:
        buffer = write_attr_ptr(c, buffer)

    return buffer


def write_deck_cam(cam, buffer):
    buffer = write_buffer(cam['vertices'], buffer)

    buffer += struct.pack('f', cam['distance_sensitivity'])
    buffer += struct.pack('f', cam['height_angle_sensitivity'])
    buffer += struct.pack('f', cam['azimuth_angle_sensitivity'])
    buffer += struct.pack('f', cam['rocking_x'])
    buffer += struct.pack('f', cam['rocking_z'])
    buffer += struct.pack('f', cam['men_step_up'])
    buffer += struct.pack('f', cam['men_step_min'])
    buffer += struct.pack('f', cam['height_max'])
    buffer += struct.pack('f', cam['height_min'])
    buffer += struct.pack('f', cam['height_step'])
    buffer += struct.pack('f', cam['cam_max_x'])
    buffer += struct.pack('f', cam['cam_min_x'])
    buffer += struct.pack('f', cam['default_height'])
    buffer += struct.pack('3f', cam['g_vec1']['x'], cam['g_vec1']['y'], cam['g_vec1']['z'])
    buffer += struct.pack('3f', cam['g_vec2']['x'], cam['g_vec2']['y'], cam['g_vec2']['z'])
    buffer += struct.pack('3f', cam['g_vec3']['x'], cam['g_vec3']['y'], cam['g_vec3']['z'])
    buffer += struct.pack('3f', cam['cam_pos']['x'], cam['cam_pos']['y'], cam['cam_pos']['z'])
    buffer += struct.pack('3f', cam['cam_angle']['x'], cam['cam_angle']['y'], cam['cam_angle']['z'])
    buffer += struct.pack('f', cam['eye_height'])

    buffer = write_buffer(cam['screen_rect'], buffer)

    buffer += struct.pack('I', cam['lock_x'])
    buffer += struct.pack('I', cam['lock_y'])
    buffer += struct.pack('I', cam['is_on'])
    buffer += struct.pack('I', cam['is_active'])
    buffer += struct.pack('f', cam['perspective'])

    buffer = write_attr_ptr(cam['character'], buffer)

    return buffer


def write_free_cam(cam, buffer):
    buffer += struct.pack('3f', cam['pos']['x'], cam['pos']['y'], cam['pos']['z'])
    buffer += struct.pack('3f', cam['angle']['x'], cam['angle']['y'], cam['angle']['z'])
    buffer += struct.pack('f', cam['fov'])
    buffer += struct.pack('I', cam['lock_x'])
    buffer += struct.pack('I', cam['lock_y'])
    buffer += struct.pack('I', cam['is_onland'])
    buffer += struct.pack('f', cam['cam_onland_height'])

    return buffer


def write_ship_cam(cam, buffer):
    buffer += struct.pack('I', cam['lock_x'])
    buffer += struct.pack('I', cam['lock_y'])
    buffer += struct.pack('f', cam['min_height_on_sea'])
    buffer += struct.pack('f', cam['max_height_on_ship'])
    buffer += struct.pack('f', cam['distance'])
    buffer += struct.pack('f', cam['max_distance'])
    buffer += struct.pack('f', cam['min_distance'])
    buffer += struct.pack('f', cam['distance_delta'])
    buffer += struct.pack('f', cam['distance_inertia'])
    buffer += struct.pack('f', cam['min_angle_x'])
    buffer += struct.pack('f', cam['max_angle_x'])
    buffer += struct.pack('f', cam['angle_x_delta'])
    buffer += struct.pack('f', cam['angle_x_inertia'])
    buffer += struct.pack('f', cam['angle_y_delta'])
    buffer += struct.pack('f', cam['angle_y_inertia'])
    buffer += struct.pack('f', cam['distance_sensitivity'])
    buffer += struct.pack('f', cam['azimuth_angle_sensitivity'])
    buffer += struct.pack('f', cam['height_angle_sensitivity'])
    buffer += struct.pack('f', cam['height_angle_onship_sensitivity'])
    buffer += struct.pack('f', cam['invert_mouse_x'])
    buffer += struct.pack('f', cam['invert_mouse_y'])
    buffer += struct.pack('3f', cam['center']['x'], cam['center']['y'], cam['center']['z'])
    buffer += struct.pack('3f', cam['angle']['x'], cam['angle']['y'], cam['angle']['z'])
    buffer += struct.pack('f', cam['model_atan_y'])
    # buffer += struct.pack('I', cam['ship_code'])
    buffer += struct.pack('I', cam['islands_init_count'])
    buffer += struct.pack('I', cam['is_on'])
    buffer += struct.pack('I', cam['is_active'])
    buffer += struct.pack('f', cam['perspective'])

    buffer = write_attr_ptr(cam['character'], buffer)

    return buffer


def write_fireplace(fireplace, buffer):
    buffer += struct.pack('3f', fireplace['orig_pos']['x'], fireplace['orig_pos']['y'], fireplace['orig_pos']['z'])
    buffer += struct.pack('I', fireplace['active'])
    buffer += struct.pack('f', fireplace['run_time'])
    buffer += struct.pack('I', fireplace['ball_character_index'])

    buffer = write_string(fireplace['particle_smoke_name'], buffer)
    buffer = write_string(fireplace['particle_fire_name'], buffer)
    buffer = write_string(fireplace['sound_name'], buffer)

    return buffer


def write_seasave(seasave, buffer):
    buffer = write_aihelper(seasave['aihelper'], buffer)

    buffer = write_ship_cam(seasave['ship_cam'], buffer)
    buffer = write_free_cam(seasave['free_cam'], buffer)
    buffer = write_deck_cam(seasave['deck_cam'], buffer)

    buffer = write_aiballs(seasave['aiballs'], buffer)

    buffer += struct.pack('I', len(seasave['aigroups']))
    for aigroup in seasave['aigroups']:
        buffer = write_aigroup(aigroup, buffer)

    if seasave['aifort'] is not None:
        buffer = write_aifort(seasave['aifort'], buffer)

    return buffer


# Conversion

def convert_107_to_173(seasave):
    layers = {
        'execute': 0,
        'realize': 1,
        'sea_execute': 2,
        'sea_realize': 3,
        'interface_execute': 4,
        'interface_realize': 5,
        'fader_execute': 6,
        'fader_realize': 7,
        'lighter_execute': 8,
        'lighter_realize': 9,
        'video_execute': 10,
        'video_realize': 11,
        'editor_realize': 12,
        'info_realize': 13,
        'sound_debug_realize': 14,
        'sea_reflection': 15,
        'sea_reflection2': 16,
        'sea_sunroad': 17,
        'sun_trace': 18,
        'sails_trace': 19,
        'hull_trace': 20,
        'mast_island_trace': 21,
        'mast_ship_trace': 22,
        'ship_cannon_trace': 23,
        'fort_cannon_trace': 24,
        'island_trace': 25,
        'shadow': 26,
        'blood': 27,
        'rain_drops': 28
    }

    for aigroup in seasave['aigroups']:
        for aiship in aigroup['ships']:
            strval = aiship['base_ship']['realize_layer']
            aiship['base_ship']['realize_layer'] = layers[strval]

            strval = aiship['base_ship']['execute_layer']
            aiship['base_ship']['execute_layer'] = layers[strval]

            del aiship['base_ship']['x_heel']
            del aiship['base_ship']['z_heel']

    del seasave['ship_cam']['ship_code']

    return seasave
