import ctypes
from ctypes import Structure, Union, POINTER
from ctypes import c_int, c_char, c_char_p, c_uint32, c_int64

from enum import Enum

OPJ_BOOL  = c_int
OPJ_BYTE  = c_char
OPJ_BYTE_PTR = c_char_p
OPJ_OFF_T = c_int64

class OPJ_PROG_ORDER(Enum):
    OPJ_PROG_UNKNOWN = -1   # place-holder
    OPJ_LRCP = 0            # layer-resolution-component-precinct order
    OPJ_RLCP = 1            # resolution-layer-component-precinct order
    OPJ_RPCL = 2            # resolution-precinct-component-layer order
    OPJ_PCRL = 3            # precinct-component-resolution-layer order
    OPJ_CPRL = 4            # component-precinct-resolution-layer order

class opj_tcp(Structure):
    _fields_ = [
        ('csty', c_uint32),
        ('prg', c_int),      # OPJ_PROG_ORDER
        ('numlayers', c_uint32),
        ('num_layers_to_decode', c_uint32),
        ('mct',c_uint32),
        # incomplete
    ]

class opj_j2k_dec(Structure):
    _fields_ = [
        ('m_state',c_uint32),
        ('m_default_tcp',POINTER(opj_tcp)),
        # incomplete
    ]

class opj_j2k_enc(Structure):
    _fields_ = [
        ('m_current_poc_tile_part_number', c_uint32),
        ('m_current_tile_part_number', c_uint32),
        ('m_tlm_start', OPJ_OFF_T),
        ('m_tlm_sot_offsets_buffer', OPJ_BYTE_PTR),
        ('m_tlm_sot_offsets_current', OPJ_BYTE_PTR),
        ('m_total_tile_parts',c_uint32),
        ('m_encoded_tile_data',OPJ_BYTE_PTR),
        ('m_encoded_tile_size',c_uint32),
        ('m_header_tile_data',OPJ_BYTE_PTR),
        ('m_header_tile_data_size',c_uint32),
    ]

class U_SPECIFIC_PARAM(Union):
    _fields_ = [
        ('m_decoder', opj_j2k_dec),
        ('m_encoder', opj_j2k_enc),
    ]

class opj_j2k(Structure):
    _fields_ = [
        ('m_is_decoder',OPJ_BOOL),
        ('m_specific_param',  U_SPECIFIC_PARAM),
    ]
    _anonymous_ = [
        'm_specific_param'
    ]



