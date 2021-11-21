#!python

import sys
import argparse
import os
import struct
import zlib
import logging
from enum import Enum

import str_db
import seasave

# Python 3.7: Dictionary iteration order is guaranteed to be in order of insertion
MIN_PYTHON = (3, 7)
if sys.version_info < MIN_PYTHON:
    sys.exit("Python {'.'.join([str(n) for n in MIN_PYTHON])} or later is required.")

FILEINFO_CONFIG = {
    'ver 1.0.7': {'str_encoding': 'cp1251', 'obj_id_format': 'QQQ'},
    'ver 1.7.3': {'str_encoding': 'utf-8', 'obj_id_format': 'Q'}
}


class VarType(Enum):
    Integer = 6
    Float = 7
    String = 8
    Object = 9
    Reference = 10
    AttributeReference = 11
    Pointer = 12


# Deserialization

def read_int8_16_32(buffer, cur_ptr):
    int8 = buffer[cur_ptr]
    cur_ptr += 1
    if int8 < 0xfe:
        return int8, cur_ptr
    elif int8 == 0xfe:
        int16 = struct.unpack_from('H', buffer, cur_ptr)[0]
        cur_ptr += 2
        return int16, cur_ptr
    else:
        int32 = struct.unpack_from('I', buffer, cur_ptr)[0]
        cur_ptr += 4
        return int32, cur_ptr


def read_string(buffer, cur_ptr, encoding):
    str_len, cur_ptr = read_int8_16_32(buffer, cur_ptr)
    if str_len == 0:
        return None, cur_ptr
    s = struct.unpack_from(f'{str_len - 1}s', buffer, cur_ptr)[0]  # str_len-1 to skip trailing '\0'
    s = s.decode(encoding, 'ignore')  # leave the character out in case of a decoding error
    cur_ptr += str_len
    return s, cur_ptr


def read_attributes_data(buffer, cur_ptr, s_db, str_encoding):
    num_attributes, cur_ptr = read_int8_16_32(buffer, cur_ptr)
    name_code, cur_ptr = read_int8_16_32(buffer, cur_ptr)
    value, cur_ptr = read_string(buffer, cur_ptr, str_encoding)

    a = {
        'name_code': name_code,
        'value': value
    }

    if num_attributes > 0:
        attributes = {}
        for _ in range(num_attributes):
            attr, cur_ptr = read_attributes_data(buffer, cur_ptr, s_db, str_encoding)
            attr_name = str_db.get_str(s_db, attr['name_code'])
            attributes[attr_name] = attr
        a['attributes'] = attributes

    return a, cur_ptr


def read_value(var_type, buffer, cur_ptr, s_db, str_encoding, obj_id_format):
    if var_type == VarType.Integer:
        v = struct.unpack_from('l', buffer, cur_ptr)[0]
        cur_ptr += 4
    elif var_type == VarType.Float:
        v = struct.unpack_from('f', buffer, cur_ptr)[0]
        cur_ptr += 4
    elif var_type == VarType.String:
        v, cur_ptr = read_string(buffer, cur_ptr, str_encoding)
    elif var_type == VarType.Object:
        v = {'id': struct.unpack_from(obj_id_format, buffer, cur_ptr)}
        cur_ptr += struct.calcsize(obj_id_format)
        attributes = {}
        attr, cur_ptr = read_attributes_data(buffer, cur_ptr, s_db, str_encoding)
        attr_name = str_db.get_str(s_db, attr['name_code'])
        attributes[attr_name] = attr
        v['attributes'] = attributes

    elif var_type == VarType.Reference:
        v = {}
        var_index, cur_ptr = read_int8_16_32(buffer, cur_ptr)
        v['var_index'] = var_index
        if var_index != 0xffffffff:
            array_index, cur_ptr = read_int8_16_32(buffer, cur_ptr)
            v['array_index'] = array_index
    elif var_type == VarType.AttributeReference:
        v = {}
        var_index, cur_ptr = read_int8_16_32(buffer, cur_ptr)
        v['var_index'] = var_index
        if var_index != 0xffffffff:
            array_index, cur_ptr = read_int8_16_32(buffer, cur_ptr)
            v['array_index'] = array_index
            s, cur_ptr = read_string(buffer, cur_ptr, str_encoding)
            v['str'] = s
    elif var_type == VarType.Pointer:
        v = struct.unpack_from('P', buffer, cur_ptr)[0]
        cur_ptr += 8
    else:
        raise RuntimeError(f'{var_type} was not handled')

    return v, cur_ptr


def read_variable(buffer, cur_ptr, s_db, str_encoding, obj_id_format):
    t = struct.unpack_from('I', buffer, cur_ptr)[0]
    t = VarType(t)
    cur_ptr += 4

    num_values = struct.unpack_from('I', buffer, cur_ptr)[0]
    cur_ptr += struct.calcsize('I')

    values = []
    for _ in range(num_values):
        v, cur_ptr = read_value(t, buffer, cur_ptr, s_db, str_encoding, obj_id_format)
        values.append(v)

    var = {
        'type': t,
        'values': values
    }

    return var, cur_ptr


def read_save(file_name):
    with open(file_name, 'rb') as f:
        save_data = {}
        file_info = struct.unpack('32s', f.read(32))[0]
        file_info = file_info[0:file_info.find(b'\x00')]
        file_info = file_info.decode('utf-8')

        if file_info != 'ver 1.0.7':
            return None, None

        save_data['file_info'] = file_info

        str_encoding = FILEINFO_CONFIG[file_info]['str_encoding']
        obj_id_format = FILEINFO_CONFIG[file_info]['obj_id_format']

        extdata_offset = struct.unpack('I', f.read(4))[0]
        extdata_size_decompressed = struct.unpack('I', f.read(4))[0]

        size_decompressed = struct.unpack('I', f.read(4))[0]
        size_compressed = struct.unpack('I', f.read(4))[0]

        buffer_compressed = struct.unpack(f'{size_compressed}s', f.read(size_compressed))[0]
        buffer = zlib.decompress(buffer_compressed)

        cur_ptr = 0
        save_data['program_dir'], cur_ptr = read_string(buffer, cur_ptr, str_encoding)

        strings = []
        num_strings, cur_ptr = read_int8_16_32(buffer, cur_ptr)
        for _ in range(num_strings):
            s, cur_ptr = read_string(buffer, cur_ptr, str_encoding)
            if s is not None:
                # if not s.isascii():
                #   logging.warning(f'non ascii string: {s}
                #       (cp1251: {s.encode("cp1251").hex(" ")},
                #       utf-8: {s.encode("utf-8").hex(" ")})')
                strings.append(s)

        s_db = str_db.create_db(strings, str_encoding)
        save_data['strings'] = {}
        for s in strings:
            save_data['strings'][s] = str_db.get_int(s_db, s, str_encoding)

        save_data['segments'] = []
        num_segments, cur_ptr = read_int8_16_32(buffer, cur_ptr)
        for _ in range(num_segments):
            s, cur_ptr = read_string(buffer, cur_ptr, str_encoding)
            save_data['segments'].append(s)

        vars = {}
        num_vars, cur_ptr = read_int8_16_32(buffer, cur_ptr)
        for _ in range(num_vars):
            name, cur_ptr = read_string(buffer, cur_ptr, str_encoding)
            var, cur_ptr = read_variable(buffer, cur_ptr, s_db, str_encoding, obj_id_format)
            vars[name] = var

        if 'oSeaSave' in vars and 'attributes' in vars['oSeaSave']['values'][0]['attributes']['skip']:
            seasave_data = vars['oSeaSave']['values'][0]['attributes']['skip']['attributes']['save']['value']
            data = seasave.read_seasave(bytes.fromhex(seasave_data[8:]))
            vars['oSeaSave']['values'][0]['attributes']['skip']['attributes']['save']['value'] = data

        save_data['vars'] = vars

        assert (cur_ptr == size_decompressed)

        # read extdata
        assert (extdata_offset == f.tell())
        extdata_size_compressed = struct.unpack('I', f.read(4))[0]
        extdata_buffer_compressed = struct.unpack(f'{extdata_size_compressed}s', f.read(extdata_size_compressed))[0]
        extdata_buffer = zlib.decompress(extdata_buffer_compressed)
        cur_ptr = 0
        save_name_len = struct.unpack_from('l', extdata_buffer, cur_ptr)[0]
        cur_ptr += 4
        save_icon_size = struct.unpack_from('l', extdata_buffer, cur_ptr)[0]
        cur_ptr += 4
        save_name = struct.unpack_from(f'{save_name_len-1}s', extdata_buffer, cur_ptr)[0]  # len-1 to skip trailing '\0'
        save_data['save_name'] = save_name.decode(str_encoding)
        cur_ptr += save_name_len
        save_data['save_icon'] = struct.unpack_from(f'{save_icon_size}s', extdata_buffer, cur_ptr)[0]
        cur_ptr += save_icon_size

        assert (cur_ptr == extdata_size_decompressed)

        # check EOF
        assert (f.tell() == os.fstat(f.fileno()).st_size)

        return save_data, s_db


# Serialization

def write_int8_16_32(value, buffer):
    if value < 0xfe:
        buffer += struct.pack('B', value)
        return buffer
    elif value < 0xffff:
        buffer += struct.pack('B', 0xfe)
        buffer += struct.pack('H', value)
        return buffer
    else:
        buffer += struct.pack('B', 0xff)
        buffer += struct.pack('I', value)
        return buffer


def write_string(s, buffer, encoding):
    if s is not None:
        s = s.encode(encoding) + b'\x00'
        str_len = len(s)
        buffer = write_int8_16_32(str_len, buffer)
        buffer += struct.pack(f'{str_len}s', s)
    else:
        buffer += b'\x00'
    return buffer


def write_attributes_data(a, buffer, str_encoding):
    num_attributes = len(a['attributes']) if 'attributes' in a else 0
    buffer = write_int8_16_32(num_attributes, buffer)
    buffer = write_int8_16_32(a['name_code'], buffer)
    buffer = write_string(a['value'], buffer, str_encoding)

    if num_attributes > 0:
        for attr in a['attributes'].values():
            buffer = write_attributes_data(attr, buffer, str_encoding)
    return buffer


def write_value(var_type, value, buffer, fileinfo_config):
    if var_type == VarType.Integer:
        buffer += struct.pack('l', value)
    elif var_type == VarType.Float:
        buffer += struct.pack('f', value)
    elif var_type == VarType.String:
        buffer = write_string(value, buffer, fileinfo_config['str_encoding'])
    elif var_type == VarType.Object:
        obj_id = value['id']
        buffer += struct.pack(fileinfo_config['obj_id_format'], *obj_id)
        root_attr = next(iter(value['attributes']))
        buffer = write_attributes_data(value['attributes'][root_attr], buffer, fileinfo_config['str_encoding'])
    elif var_type == VarType.Reference:
        buffer = write_int8_16_32(value['var_index'], buffer)
        if value['var_index'] != 0xffffffff:
            buffer = write_int8_16_32(value['array_index'], buffer)
    elif var_type == VarType.AttributeReference:
        buffer = write_int8_16_32(value['var_index'], buffer)
        if value['var_index'] != 0xffffffff:
            buffer = write_int8_16_32(value['array_index'], buffer)
            buffer = write_string(value['str'], buffer, fileinfo_config['str_encoding'])
    elif var_type == VarType.Pointer:
        struct.pack_into('P', buffer, value)
    else:
        raise RuntimeError(f'{var_type} was not handled')

    return buffer


def write_variable(var, buffer, fileinfo_config):
    buffer += struct.pack('I', var['type'].value)
    buffer += struct.pack('I', len(var['values']))

    for value in var['values']:
        buffer = write_value(var['type'], value, buffer, fileinfo_config)

    return buffer


def write_save(save_data, filename):
    file_info = save_data['file_info']
    fileinfo_config = FILEINFO_CONFIG[file_info]
    str_encoding = fileinfo_config['str_encoding']

    buffer = bytearray()
    buffer = write_string(save_data['program_dir'], buffer, str_encoding)

    strings = save_data['strings']
    buffer = write_int8_16_32(len(strings), buffer)
    for s in strings.keys():
        buffer = write_string(s, buffer, 'cp1251')

    segments = save_data['segments']
    buffer = write_int8_16_32(len(segments), buffer)
    for s in segments:
        buffer = write_string(s, buffer, str_encoding)

    variables = save_data['vars']
    buffer = write_int8_16_32(len(variables), buffer)
    if 'oSeaSave' in variables and 'attributes' in variables['oSeaSave']['values'][0]['attributes']['skip']:
        seasave_data = variables['oSeaSave']['values'][0]['attributes']['skip']['attributes']['save']['value']
        seasave_buf = bytearray()
        seasave_buf = seasave.write_seasave(seasave_data, seasave_buf)
        seasave_size = f'{len(seasave_buf):08x}'  # as hexadecimal string without prefix 8 bytes long
        variables['oSeaSave']['values'][0]['attributes']['skip']['attributes']['save']['value'] = \
            seasave_size + seasave_buf.hex()

    for varname in variables:
        buffer = write_string(varname, buffer, str_encoding)
        buffer = write_variable(variables[varname], buffer, fileinfo_config)
    buffer_compressed = zlib.compress(buffer, zlib.Z_BEST_COMPRESSION)

    # write extdata
    extdata_buffer = bytearray()
    save_name_bytes = save_data['save_name'].encode(str_encoding) + b'\x00'
    save_name_len = len(save_name_bytes)
    extdata_buffer += struct.pack('l', save_name_len)
    save_icon_size = len(save_data['save_icon'])
    extdata_buffer += struct.pack('l', save_icon_size)
    extdata_buffer += struct.pack(f'{save_name_len}s', save_name_bytes)
    extdata_buffer += struct.pack(f'{save_icon_size}s', save_data['save_icon'])
    extdata_buffer_compressed = zlib.compress(extdata_buffer, zlib.Z_BEST_COMPRESSION)

    # compose all the data
    raw_data = bytearray()
    raw_data += struct.pack('32s', file_info.encode(str_encoding) + b'\x00')
    raw_data += struct.pack('I', struct.calcsize('32sIIII') + len(buffer_compressed))  # extdata_offset
    raw_data += struct.pack('I', len(extdata_buffer))
    raw_data += struct.pack('I', len(buffer))
    raw_data += struct.pack('I', len(buffer_compressed))
    raw_data += buffer_compressed
    raw_data += struct.pack('I', len(extdata_buffer_compressed))
    raw_data += extdata_buffer_compressed

    with open(filename, 'wb') as f:
        f.write(raw_data)


# Conversion

def convert_107_to_173(save_data, s_db):
    if save_data['file_info'] == 'ver 1.7.3':
        return save_data, s_db

    save_data['file_info'] = 'ver 1.7.3'

    removed_vars = [
        'bNetActive',
        'Net',
        'NetServer',
        'NetClient',
        'sMasterServerAddress',
        'iMasterServerPort',
        'sUserFacesPath',
        'sUserSailsPath',
        'sUserFlagsPath',
        'sNetBortNames',
        'NetModes',
        'NetTeamColor',
        'NetTeamName',
        'NSSortedPlayers',
        'NSClients',
        'NSBanList',
        'NSPlayers',
        'iServerTime',
        'iPingTime',
        'bNetServerIsStarted',
        'wClientID',
        'NCBalls',
        'NCConsole',
        'NCIsland',
        'NCCoastFoam',
        'NCSail',
        'NCVant',
        'NCRope',
        'NCFlag',
        'NCSay',
        'NCShipTracks',
        'NCLightPillar',
        'NCServer',
        'NCClients',
        'NCProfiles',
        'NCFavorites',
        'NCInetServers',
        'iLangNetClient',
        'sClientNetCamera',
        'iClientTime',
        'iClientServerTime',
        'iClientDeltaTime',
        'iTestDeltaTime',
        'bFG_Deathmatch',
        'bFG_TeamDeathmatch',
        'bFG_DefendConvoy',
        'bFG_CaptureTheFort',
        'bFG_ActiveServers',
        'bFG_WOPassword',
        'bFG_FreeServers',
        'iFG_Credit',
        'iFG_MaxShipClass',
        'iFG_Ping',
        'iCurrentServersList',
        'iNumLanServers',
        'iNumInternetServers',
        'iNumLanServersList',
        'iNumInternetServersList',
        'iNumFavServersList',
        'LanServers',
        'InternetServers',
        'LanServersList',
        'InternetServersList',
        'FavServersList',
        'g_nCompanionTaskQuantity',
        'g_objCompanionTask',
        'g_nTaskCheckRetResult',
        'iNetCannonsNum',
        'NETCANNON_CANNON',
        'NETCANNON_CULVERINE',
        'NETCANNON_MORTAR',
        'NetGoods',
        'iNetGoodsNum',
        'iNumNetIslands',
        'iNetIslandsIndex',
        'NetPerks',
        'NetShips',
        'iNetShipsNum',
        'NetSkills',
        'iNetSkillsNum',
        'NetRanks',
        'NetRewardAccuracy',
        'NetRewardVitality',
        'NetRewardVictorious',
        'iNetRanksNum',
        'NetShipHullUpgrades',
        'NetShipSailUpgrade',
        'NSBalls',
        'iServerBallIndex',
        'iServerCurrentBallIndex',
        'bServerGameStarted',
        'iServerEnvSMsg',
        'NSSail',
        'NSIsland',
        'NSTouch',
        'iServerDeltaTimeFraction',
        'iSecondsToStartGame',
        'NSFortModel',
        'NSFortBlots',
        'TopList',
        'NSWeather',
        'iServerCurWeatherNum',
        'fServerWeatherDelta',
        'fServerWeatherAngle',
        'fServerWeatherSpeed',
        'bServerWeatherIsNight',
        'bServerWeatherIsStorm',
        'bServerWhrTornado',
        'sServerLightingPath',
        'NCShipCamera',
        'NCDeckCamera',
        'NCShipLights',
        'iClientDeltaTimeFraction',
        'NCFortModel',
        'NCFortBlots',
        'rgbGameMessage',
        'iClientShipPriorityExecute',
        'iClientShipPriorityRealize',
        'iClientLastPingCode',
        'iClientLastPingTime',
        'iClientLastPingCodeTime',
        'oGameOver',
        'NCWeather',
        'iClientCurWeatherNum',
        'sClientCurrentFog',
        'bClientWeatherIsNight',
        'bClientWeatherIsLight',
        'bClientWeatherIsRain',
        'bClientWeatherIsStorm',
        'sClientLightingPath',
        'NetBI_intNRetValue',
        'NetBI_ChargeState',
        'NetBI_retComValue',
        'NetBInterface',
        'NetCannons',
        'iServerLightningSubTexX',
        'iServerLightningSubTexY',
        'NSSea',
        'NCLightning',
        'fClientLightningScaleX',
        'fClientLightningScaleY',
        'NCRain',
        'NCSea',
        'NCSky',
        'NCSunGlow',
        'NCAstronomy'
    ]
    for var in removed_vars:
        del save_data['vars'][var]

    to_int = [
        'sCurrentSeaExecute',
        'sCurrentSeaRealize',
        'sNewExecuteLayer',
        'sNewRealizeLayer'
    ]
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

    for name, var in save_data['vars'].items():
        if name in to_int:
            assert (var['type'] == VarType.String)
            var['type'] = VarType.Integer

            assert (len(var['values']) == 1)
            strval = var['values'][0]
            var['values'][0] = layers[strval]

        if var['type'] == VarType.Object:
            for val in var['values']:
                val['id'] = (sum(val['id']),)  # one item tuple

        if name == 'oSeaSave' and 'attributes' in var['values'][0]['attributes']['skip']:
            seasave_data = var['values'][0]['attributes']['skip']['attributes']['save']['value']
            seasave_data = seasave.convert_107_to_173(seasave_data)
            var['values'][0]['attributes']['skip']['attributes']['save']['value'] = seasave_data

    return save_data, s_db


def dump_save(save_data, filename):
    def fallback(v):
        if isinstance(v, Enum):
            return v.name
        return f'Unsupported type: {type(v)}'

    import json
    with open(filename, 'w', encoding='utf-8') as f:
        copy = dict(save_data)
        json.dump(copy, f, ensure_ascii=False, indent=4, default=fallback)


def process_file(file):
    file = os.path.abspath(file)
    logging.info(f'processing file "{file}".')

    save_data, s_db = read_save(file)
    if save_data is None:
        logging.warning(f'could not process file "{file}".')
        return

    filename, file_extension = os.path.splitext(file)

    # dump_save(save_data, f'{filename}_read.json')

    save_data, s_db = convert_107_to_173(save_data, s_db)
    filename += '_173'

    # dump_save(save_data, f'{filename}_write.json')

    output_file = filename + file_extension
    write_save(save_data, output_file)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input_file', help='name of a save file to convert', type=str)
    args = parser.parse_args()

    process_file(args.input_file)
