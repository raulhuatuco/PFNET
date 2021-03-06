#cython: embedsignature=True

#***************************************************#
# This file is part of PFNET.                       #
#                                                   #
# Copyright (c) 2015-2017, Tomas Tinoco De Rubira.  #
#                                                   #
# PFNET is released under the BSD 2-clause license. #
#***************************************************#

cimport cbranch

# Infinite
BRANCH_INF_RATIO = cbranch.BRANCH_INF_RATIO
BRANCH_INF_PHASE = cbranch.BRANCH_INF_PHASE

class BranchError(Exception):
    """
    Branch error exception.
    """

    pass

cdef class Branch:
    """
    Branch class.
    """

    cdef cbranch.Branch* _c_ptr

    def __init__(self,num_periods=1,alloc=True):
        """
        Branch class.

        Parameters
        ----------
        alloc : {``True``, ``False``}
        num_periods : int
        """

        pass

    def __cinit__(self,num_periods=1,alloc=True):

        if alloc:
            self._c_ptr = cbranch.BRANCH_new(num_periods)
        else:
            self._c_ptr = NULL

    def _get_c_ptr(self):

        return new_CPtr(self._c_ptr)

    def has_pos_ratio_v_sens(self):
        """
        Determines whether tap-changing transformer has positive
        sensitivity between tap ratio and controlled bus voltage magnitude.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_has_pos_ratio_v_sens(self._c_ptr)

    def is_equal(self,other):
        """
        Determines whether branch is equal to given branch.

        Parameters
        ----------
        other : :class:`Branch <pfnet.Branch>`
        """

        cdef Branch b_other

        if not isinstance(other,Branch):
            return False

        b_other = other

        return cbranch.BRANCH_is_equal(self._c_ptr,b_other._c_ptr)

    def set_ratio(self,r,t=0):
        """
        Sets branch taps ratio.
        
        Parameters
        ----------
        r : float
        t : int
        """
        
        cbranch.BRANCH_set_ratio(self._c_ptr,r,t)

    def set_phase(self,p,t=0):
        """
        Sets branch phase shift.

        Parameters
        ----------
        p : float
        t : int
        """

        cbranch.BRANCH_set_phase(self._c_ptr,p,t)

    def __richcmp__(self,other,op):
        """
        Compares two branches.

        Parameters
        ----------
        other : Branch
        op : comparison type

        Returns
        -------
        flag : {``True``, ``False``}
        """

        if op == 2:
            return self.is_equal(other)
        elif op == 3:
            return not self.is_equal(other)
        else:
            return False

    def is_on_outage(self):
        """
        Determines whether branch in on outage.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_on_outage(self._c_ptr)

    def is_fixed_tran(self):
        """
        Determines whether branch is fixed transformer.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_fixed_tran(self._c_ptr)

    def is_line(self):
        """
        Determines whether branch is transmission line.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_line(self._c_ptr)

    def is_phase_shifter(self):
        """
        Determines whether branch is phase shifter.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_phase_shifter(self._c_ptr)

    def is_tap_changer(self):
        """
        Determines whether branch is tap-changing transformer.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_tap_changer(self._c_ptr)

    def is_tap_changer_v(self):
        """
        Determines whether branch is tap-changing transformer
        that regulates bus voltage magnitude.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_tap_changer_v(self._c_ptr)

    def is_tap_changer_Q(self):
        """
        Determines whether branch is tap-changing transformer
        that regulates reactive power flow.

        Returns
        -------
        flag : {``True``, ``False``}
        """

        return cbranch.BRANCH_is_tap_changer_Q(self._c_ptr)

    def has_flags(self,flag_type,q):
        """
        Determines whether the branch has the flags associated with
        specific quantities set.

        Parameters
        ----------
        flag_type : string (:ref:`ref_net_flag`)
        q : string or list of strings (:ref:`ref_branch_q`)

        Returns
        -------
        flag : {``True``, ``False``}
        """

        q = q if isinstance(q,list) else [q]

        return cbranch.BRANCH_has_flags(self._c_ptr,
                                        str2flag[flag_type],
                                        reduce(lambda x,y: x|y,[str2q[self.obj_type][qq] for qq in q],0))

    def get_P_km(self,var_values=None):
        """
        Gets the real power flow at bus "k" towards bus "m" (from -> to) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_km : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_km(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_km(self,var_values=None):
        """
        Gets the reactive power flow at bus "k" towards bus "m" (from -> to) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_km : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_km(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_P_mk(self,var_values=None):
        """
        Gets the real power flow at bus "m" towards bus "k" (to -> from) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_mk : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_mk(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_mk(self,var_values=None):
        """
        Gets the reactive power flow at bus "m" towards bus "k" (to -> from) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_mk : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_mk(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_P_km_series(self,var_values=None):
        """
        Gets the real power flow at bus "k" towards bus "m" over the series impedance of the line (from -> to) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_km_series : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_km_series(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_km_series(self,var_values=None):
        """
        Gets the reactive power flow at bus "k" towards bus "m" over the series impedance of the line (from -> to) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_km_series : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_km_series(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_P_mk_series(self,var_values=None):
        """
        Gets the real power flow at bus "m" towards bus "k" over the series impedance of the line (to -> from) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_mk_series : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_mk_series(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_mk_series(self,var_values=None):
        """
        Gets the reactive power flow at bus "m" towards bus "k" over the series impedance of the line (to -> from) (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_mk_series : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_mk_series(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_P_k_shunt(self,var_values=None):
        """
        Gets the real power flow into the shunt element at bus "k" (aka "from") (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_k_shunt : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_k_shunt(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_k_shunt(self,var_values=None):
        """
        Gets the reactive power flow into the shunt element bus "k" (aka "from") (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_k_shunt : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_k_shunt(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_P_m_shunt(self,var_values=None):
        """
        Gets the real power flow into the shunt element at bus "m" (aka "to") (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        P_m_shunt : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_P_m_shunt(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    def get_Q_m_shunt(self,var_values=None):
        """
        Gets the reactive power flow into the shunt element at bus "m" (aka "to") (p.u.)

        Parameters
        ----------
        var_values : :class:`ndarray <numpy.ndarray>`

        Returns
        -------
        Q_m_shunt : float or :class:`ndarray <numpy.ndarray>`
        """

        cdef np.ndarray[double,mode='c'] x = var_values
        cdef cvec.Vec* v = cvec.VEC_new_from_array(&(x[0]),len(x)) if (var_values is not None and var_values.size) else NULL
        r = [cbranch.BRANCH_get_Q_m_shunt(self._c_ptr,v,t) for t in range(self.num_periods)]
        if self.num_periods == 1:
            return AttributeFloat(r[0])
        else:
            return np.array(r)

    property num_periods:
        """ Number of time periods (int). """
        def __get__(self): return cbranch.BRANCH_get_num_periods(self._c_ptr)

    property obj_type:
        """ Object type (string). """
        def __get__(self): return obj2str[cbranch.BRANCH_get_obj_type(self._c_ptr)]

    property index:
        """ Branch index (int). """
        def __get__(self): return cbranch.BRANCH_get_index(self._c_ptr)

    property index_ratio:
        """ Index of transformer tap ratio variable (int or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_index_ratio(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeInt(r[0])
            else:
                return np.array(r)

    property index_phase:
        """ Index of transformer phase shift variable (int or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_index_phase(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeInt(r[0])
            else:
                return np.array(r)

    property ratio:
        """ Transformer tap ratio (float or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_ratio(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return AttributeArray(r,self.set_ratio)
        def __set__(self,r):
            cdef int t
            cdef np.ndarray rar = np.array(r).flatten()
            for t in range(np.minimum(rar.size,self.num_periods)):
                cbranch.BRANCH_set_ratio(self._c_ptr,rar[t],t)

    property ratio_max:
        """ Transformer tap ratio upper limit (float). """
        def __get__(self): return cbranch.BRANCH_get_ratio_max(self._c_ptr)
        def __set__(self,value): cbranch.BRANCH_set_ratio_max(self._c_ptr,value)

    property ratio_min:
        """ Transformer tap ratio lower limit (float). """
        def __get__(self): return cbranch.BRANCH_get_ratio_min(self._c_ptr)
        def __set__(self,value): cbranch.BRANCH_set_ratio_min(self._c_ptr,value)

    property bus_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`bus_k <pfnet.Branch.bus_k>`. """
        def __get__(self): return self.bus_k

    property bus_k:
        """ :class:`Bus <pfnet.Bus>` connected to the "k" (aka "from" or "i") side. """
        def __get__(self): return new_Bus(cbranch.BRANCH_get_bus_k(self._c_ptr))

    property bus_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`bus_m <pfnet.Branch.bus_m>`. """
        def __get__(self): return self.bus_m

    property bus_m:
        """ :class:`Bus <pfnet.Bus>` connected to the "m" (aka "to" or "j") side. """
        def __get__(self): return new_Bus(cbranch.BRANCH_get_bus_m(self._c_ptr))

    property reg_bus:
        """ :class:`Bus <pfnet.Bus>` whose voltage is regulated by this tap-changing transformer. """
        def __get__(self): return new_Bus(cbranch.BRANCH_get_reg_bus(self._c_ptr))

    property b:
        """ Branch series susceptance (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_b(self._c_ptr)

    property b_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`b_k <pfnet.Branch.b_k>`. """
        def __get__(self): return self.b_k

    property b_k:
        """ Branch shunt susceptance at the "k" (aka "from" or "i") side (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_b_k(self._c_ptr)

    property b_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`b_m <pfnet.Branch.b_m>`. """
        def __get__(self): return self.b_m

    property b_m:
        """ Branch shunt susceptance at the "m" (aka "to" or "j") side (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_b_m(self._c_ptr)

    property g:
        """ Branch series conductance (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_g(self._c_ptr)

    property g_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`g_k <pfnet.Branch.g_k>`. """
        def __get__(self): return self.g_k

    property g_k:
        """ Branch shunt conductance at the "k" (aka "from" or "i") side (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_g_k(self._c_ptr)

    property g_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`g_m <pfnet.Branch.g_m>`. """
        def __get__(self): return self.g_m

    property g_m:
        """ Branch shunt conductance at the "m" (aka "to" or "j") side (p.u.) (float). """
        def __get__(self): return cbranch.BRANCH_get_g_m(self._c_ptr)

    property phase:
        """ Transformer phase shift (radians) (float or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_phase(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return AttributeArray(r,self.set_phase)
        def __set__(self,p):
            cdef int t
            cdef np.ndarray par = np.array(p).flatten()
            for t in range(np.minimum(par.size,self.num_periods)):
                cbranch.BRANCH_set_phase(self._c_ptr,par[t],t)

    property phase_max:
        """ Transformer phase shift upper limit (radians) (float). """
        def __get__(self): return cbranch.BRANCH_get_phase_max(self._c_ptr)

    property phase_min:
        """ Transformer phase shift lower limit (radians) (float). """
        def __get__(self): return cbranch.BRANCH_get_phase_min(self._c_ptr)

    property P_km:
        """ Real power flow at bus "k" towards bus "m" (from -> to) (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_km()

    property Q_km:
        """ Reactive power flow at bus "k" towards bus "m" (from -> to) (p.u.) (float or array). """
        def __get__(self):
            return self.get_Q_km()

    property P_mk:
        """ Real power flow at bus "m" towards bus "k" (to -> from) (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_mk()

    property Q_mk:
        """ Reactive power flow at bus "m" towards bus "k" (to -> from) (p.u.) (float or array). """
        def __get__(self):
             return self.get_Q_mk()

    property P_km_series:
        """ Real power flow at bus "k" towards bus "m" over the series impedance of the line (from -> to) (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_km_series()

    property Q_km_series:
        """ Reactive power flow at bus "k" towards bus "m" over the series impedance of the line (from -> to) (p.u.) (float or array). """
        def __get__(self):
            return self.get_Q_km_series()

    property P_mk_series:
        """ Real power flow at bus "m" towards bus "k" over the series impedance of the line (to -> from) (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_mk_series()

    property Q_mk_series:
        """ Reactive power flow at bus "m" towards bus "k" over the series impedance of the line (to -> from) (p.u.) (float or array). """
        def __get__(self):
            return self.get_Q_mk_series()

    property P_k_shunt:
        """ Real power flow into the shunt element at bus "k" (aka "from") (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_k_shunt()

    property Q_k_shunt:
        """ Reactive power flow into the shunt element bus "k" (aka "from") (p.u.) (float or array). """
        def __get__(self):
            return self.get_Q_k_shunt()

    property P_m_shunt:
        """ Real power flow into the shunt element at bus "m" (aka "to") (p.u.) (float or array). """
        def __get__(self):
            return self.get_P_m_shunt()

    property Q_m_shunt:
        """ Reactive power flow into the shunt element at bus "m" (aka "to") (p.u.) (float or array). """
        def __get__(self):
            return self.get_Q_m_shunt()

    property P_from_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_km <pfnet.Branch.P_km>`. """
        def __get__(self): return self.P_km

    property Q_from_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_km <pfnet.Branch.Q_km>`. """
        def __get__(self): return self.Q_km

    property P_to_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_mk <pfnet.Branch.P_mk>`. """
        def __get__(self): return self.P_mk

    property Q_to_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_mk <pfnet.Branch.Q_mk>`. """
        def __get__(self): return self.Q_mk

    property P_series_from_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_km_series <pfnet.Branch.P_km_series>`. """
        def __get__(self): return self.P_km_series

    property Q_series_from_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_km_series <pfnet.Branch.Q_km_series>`.
        """
        def __get__(self): return self.Q_km_series

    property P_series_to_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_mk_series <pfnet.Branch.P_mk_series>`. """
        def __get__(self): return self.P_mk_series

    property Q_series_to_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_mk_series <pfnet.Branch.Q_mk_series>`. """
        def __get__(self): return self.Q_mk_series

    property P_shunt_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_k_shunt <pfnet.Branch.P_k_shunt>`. """
        def __get__(self): return self.P_k_shunt

    property Q_shunt_from:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_k_shunt <pfnet.Branch.Q_k_shunt>`. """
        def __get__(self): return self.Q_k_shunt

    property P_shunt_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`P_m_shunt <pfnet.Branch.P_m_shunt>`. """
        def __get__(self): return self.P_m_shunt

    property Q_shunt_to:
        """ .. deprecated:: 1.2.5  Same as :attr:`Q_m_shunt <pfnet.Branch.Q_m_shunt>`. """
        def __get__(self): return self.Q_m_shunt

    property ratingA:
        """ Branch thermal rating A (p.u. system base power) (float). """
        def __get__(self): return cbranch.BRANCH_get_ratingA(self._c_ptr)
        def __set__(self,r): cbranch.BRANCH_set_ratingA(self._c_ptr,r)

    property ratingB:
        """ Branch thermal rating B (p.u. system base power) (float). """
        def __get__(self): return cbranch.BRANCH_get_ratingB(self._c_ptr)
        def __set__(self,r): cbranch.BRANCH_set_ratingB(self._c_ptr,r)

    property ratingC:
        """ Branch thermal rating C (p.u. system base power) (float). """
        def __get__(self): return cbranch.BRANCH_get_ratingC(self._c_ptr)
        def __set__(self,r): cbranch.BRANCH_set_ratingC(self._c_ptr,r)

    property P_km_DC:
        """ Active power flow (DC approx.) from bus "k/from" to bus "m/to" (float). """
        def __get__(self):
            r = [cbranch.BRANCH_get_P_km_DC(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return np.array(r)

    property P_mk_DC:
        """ Active power flow (DC approx.) from bus "m/to" to bus "k/from" (float). """
        def __get__(self):
            r = [cbranch.BRANCH_get_P_mk_DC(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return np.array(r)

    property sens_P_u_bound:
        """ Objective function sensitivity with respect to active power flow upper bound (float or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_sens_P_u_bound(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return np.array(r)

    property sens_P_l_bound:
        """ Objective function sensitivity with respect to active power flow lower bound (float or array). """
        def __get__(self):
            r = [cbranch.BRANCH_get_sens_P_l_bound(self._c_ptr,t) for t in range(self.num_periods)]
            if self.num_periods == 1:
                return AttributeFloat(r[0])
            else:
                return np.array(r)

    property outage:
        """ Flag that indicates whehter branch is on outage. """
        def __get__(self): return cbranch.BRANCH_is_on_outage(self._c_ptr)

cdef new_Branch(cbranch.Branch* b):
    if b is not NULL:
        branch = Branch(alloc=False)
        branch._c_ptr = b
        return branch
    else:
        raise BranchError('no branch data')
