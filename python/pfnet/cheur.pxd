#***************************************************#
# This file is part of PFNET.                       #
#                                                   #
# Copyright (c) 2015, Tomas Tinoco De Rubira.       #
#                                                   #
# PFNET is released under the BSD 2-clause license. #
#***************************************************#

cdef extern from "pfnet/heur.h":

    ctypedef struct Heur:
        pass
    
    cdef char HEUR_TYPE_PVPQ
    
