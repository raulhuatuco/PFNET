#***************************************************#
# This file is part of PFNET.                       #
#                                                   #
# Copyright (c) 2015-2016, Tomas Tinoco De Rubira.  #
#                                                   #
# PFNET is released under the BSD 2-clause license. #
#***************************************************#

import pfnet as pf
import unittest
from . import test_cases
import numpy as np

class TestFlags(unittest.TestCase):
    
    def setUp(self):
        
        # Network
        self.net = pf.Network()
        
    def test_variables(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)

            # add vargens
            net.add_vargens(net.get_gen_buses(),50.,30.,5,0.05)

            # loads
            lcount = 0
            for load in net.loads:
                if load.index % 2 == 0:
                    load.P_min = -10.
                    load.P_max = 20.
                    lcount += 1
            self.assertEqual(lcount,net.get_num_P_adjust_loads())
            self.assertGreater(lcount,0)

            num_vars = 0

            self.assertEqual(net.num_vars,0)

            # Buses
            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_SLACK,
                          ['voltage magnitude','voltage angle'])
            num_vars += 2*net.get_num_slack_buses()
            self.assertEqual(net.num_vars,num_vars)
            
            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_REG_BY_GEN,
                          ['voltage magnitude','voltage angle'])
            num_vars += 2*(net.get_num_buses_reg_by_gen()-net.get_num_slack_buses())
            self.assertEqual(net.num_vars,num_vars)

            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_REG_BY_TRAN,
                          'voltage angle')
            for i in range(net.num_buses):
                bus = net.get_bus(i)
                if (bus.is_regulated_by_tran() and 
                    not bus.is_regulated_by_gen() and
                    not bus.is_slack()):
                    num_vars +=1
            self.assertEqual(net.num_vars,num_vars)
            
            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_ANY,
                          'voltage magnitude')
            for i in range(net.num_buses):
                bus = net.get_bus(i)
                if (not bus.is_regulated_by_gen() and
                    not bus.is_slack()):
                    num_vars +=1
            self.assertEqual(net.num_vars,num_vars)
            
            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_ANY,
                          'voltage angle')
            for i in range(net.num_buses):
                bus = net.get_bus(i)
                if (not bus.is_regulated_by_tran() and 
                    not bus.is_regulated_by_gen() and
                    not bus.is_slack()):
                    num_vars +=1
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_vars,2*net.num_buses)

            # Generators
            net.set_flags('generator',
                          'variable',
                          pf.GEN_PROP_SLACK,
                          'reactive power')
            net.set_flags('generator',
                          'variable',
                          pf.GEN_PROP_SLACK,
                          'reactive power')
            num_vars += net.get_num_slack_gens()
            self.assertEqual(net.num_vars,num_vars)

            net.set_flags('generator',
                          'variable',
                          pf.GEN_PROP_REG,
                          ['active power','reactive power'])
            num_vars += 2*net.get_num_reg_gens() - net.get_num_slack_gens()
            self.assertEqual(net.num_vars,num_vars)

            # Loads
            net.set_flags('load',
                          'variable',
                          pf.LOAD_PROP_P_ADJUST,
                          'active power')
            num_vars += net.get_num_P_adjust_loads()
            self.assertEqual(net.num_vars,num_vars)

            net.set_flags('load',
                          'variable',
                          pf.LOAD_PROP_P_ADJUST,
                          'active power')
            self.assertEqual(net.num_vars,num_vars)

            # Branches
            net.set_flags('branch',
                          'variable',
                          pf.BRANCH_PROP_TAP_CHANGER_V,
                          'tap ratio')
            num_vars += net.get_num_tap_changers_v()
            self.assertEqual(net.num_vars,num_vars)
            
            net.set_flags('branch',
                          'variable',
                          pf.BRANCH_PROP_PHASE_SHIFTER,
                          'phase shift')
            num_vars += net.get_num_phase_shifters()
            self.assertEqual(net.num_vars,num_vars)
            
            # Shunts
            net.set_flags('shunt',
                          'variable',
                          pf.SHUNT_PROP_SWITCHED_V,
                          'susceptance')
            num_vars += net.get_num_switched_shunts()
            self.assertEqual(net.num_vars,num_vars)

            # Vargens
            self.assertGreater(net.num_var_generators,0)
            net.set_flags('variable generator',
                          'variable',
                          pf.VARGEN_PROP_ANY,
                          'active power')
            num_vars += net.num_var_generators
            self.assertEqual(net.num_vars,num_vars)
            net.set_flags('variable generator',
                          'variable',
                          pf.VARGEN_PROP_ANY,
                          'reactive power')
            num_vars += net.num_var_generators
            self.assertEqual(net.num_vars,num_vars)
            net.set_flags('variable generator',
                          'variable',
                          pf.VARGEN_PROP_ANY,
                          ['active power','reactive power'])
            num_vars += 0
            self.assertEqual(net.num_vars,num_vars)

            # Batteries
            net.set_flags('battery',
                          'variable',
                          pf.BAT_PROP_ANY,
                          ['charging power','energy level'])
            num_vars += 3*net.num_batteries
            self.assertEqual(net.num_vars,num_vars)

            net.set_flags('battery',
                          'variable',
                          pf.BAT_PROP_ANY,
                          ['charging power','energy level'])
            num_vars += 0
            self.assertEqual(net.num_vars,num_vars)

            # Clear
            num_vars = 0
            net.clear_flags()
            self.assertEqual(net.num_vars,0)
            self.assertEqual(net.num_fixed,0)
            self.assertEqual(net.num_bounded,0)

            # Bus
            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_REG_BY_SHUNT,
                          ['voltage magnitude','voltage magnitude violation'])
            num_vars += 3*net.get_num_buses_reg_by_shunt()
            self.assertEqual(net.num_vars,num_vars)
            
    def test_init_point(self):
        
        net = self.net
        
        for case in test_cases.CASES:
            
            net.load(case)

            # add vargens
            net.add_vargens(net.get_gen_buses(),50.,30.,5,0.05)
            for vargen in net.var_generators:
                vargen.P = np.random.rand()
                vargen.Q = np.random.rand()

            self.assertEqual(net.num_vars,0)

            point = net.get_var_values()

            self.assertTrue(type(point) is np.ndarray)
            self.assertTupleEqual(point.shape,(0,))
            self.assertEqual(point.size,0)

            net.set_flags('bus',
                          'variable',
                          pf.BUS_PROP_NOT_REG_BY_GEN,
                          'voltage magnitude')
            num_vars = 0
            for i in range(net.num_buses):
                bus = net.get_bus(i)
                if not bus.is_regulated_by_gen():
                    num_vars += 1
            self.assertEqual(num_vars,net.num_vars)

            net.set_flags('generator',
                          'variable',
                          pf.GEN_PROP_ANY,
                          'reactive power')
            num_vars += net.num_generators
            self.assertEqual(num_vars,net.num_vars)

            net.set_flags('load',
                          'variable',
                          pf.LOAD_PROP_ANY,
                          'active power')
            num_vars += net.num_loads
            self.assertEqual(num_vars,net.num_vars)

            net.set_flags('shunt',
                          'variable',
                          pf.SHUNT_PROP_SWITCHED_V,
                          'susceptance')
            num_vars += net.get_num_switched_shunts()
            self.assertEqual(num_vars,net.num_vars)
           
            net.set_flags('variable generator',
                          'variable',
                          pf.VARGEN_PROP_ANY,
                          ['active power','reactive power'])
            num_vars += net.num_var_generators*2
            self.assertEqual(num_vars,net.num_vars)

            net.set_flags('battery',
                          'variable',
                          pf.BAT_PROP_ANY,
                          ['charging power','energy level'])
            num_vars += 3*net.num_batteries
            self.assertEqual(net.num_vars,num_vars)
 
            point = net.get_var_values()
            self.assertTrue(type(point) is np.ndarray)
            self.assertTupleEqual(point.shape,(num_vars,))
            self.assertEqual(point.size,num_vars)
            self.assertTrue(not np.any(np.isinf(point)))
            self.assertTrue(not np.any(np.isnan(point)))

            # check switched shunt vals
            for i in range(net.num_shunts):
                s = net.get_shunt(i)
                if s.is_switched_v():
                    self.assertLess(np.abs(point[s.index_b]-s.b),1e-10)

            # check vargens
            self.assertGreater(len(net.var_generators),0)
            for vargen in net.var_generators:
                self.assertNotEqual(vargen.P,0.)
                self.assertNotEqual(vargen.Q,0.)
                self.assertLess(np.abs(point[vargen.index_P]-vargen.P),1e-10)
                self.assertLess(np.abs(point[vargen.index_Q]-vargen.Q),1e-10)

            # check loads
            for i in range(net.num_loads):
                l = net.get_load(i)
                self.assertTrue(l.has_flags('variable','active power'))
                self.assertEqual(point[l.index_P],l.P)

            # check bats
            for i in range(net.num_batteries):
                b = net.get_bat(i)
                self.assertTrue(b.has_flags('variable','charging power'))
                self.assertTrue(b.has_flags('variable','energy level'))
                if (b.P >= 0.):
                    self.assertEqual(point[b.index_Pc],b.P)
                    self.assertEqual(point[b.index_Pd],0.)
                else:
                    self.assertEqual(point[b.index_Pd],-b.P)
                    self.assertEqual(point[b.index_Pc],0.)
                self.assertEqual(point[b.index_E],b.E)

    def test_tap_changer_v(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)

            num_vars = 0

            self.assertEqual(net.num_vars,0)

            # Tap changer v
            manual_num_vars = 0
            net.set_flags('branch',
                          'variable',
                          pf.BRANCH_PROP_TAP_CHANGER_V,
                          'tap ratio')
            for i in range(net.num_buses):
                bus = net.get_bus(i)
                if bus.is_regulated_by_tran():
                    self.assertGreater(len(bus.reg_trans),0)
                    self.assertTrue(all([t.is_tap_changer_v() for t in bus.reg_trans]))
                    for tran in bus.reg_trans:
                        manual_num_vars += 1
                        self.assertTrue(tran.has_flags('variable','tap ratio'))
            self.assertEqual(manual_num_vars,net.num_vars)
            
    def test_bounded(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)

            self.assertEqual(net.num_bounded,0)

            net.set_flags('bus',
                          'bounded',
                          pf.BUS_PROP_REG_BY_TRAN,
                          'voltage magnitude')
            net.set_flags('generator',
                          'bounded',
                          pf.GEN_PROP_SLACK,
                          'active power')
            net.set_flags('load',
                          'bounded',
                          pf.LOAD_PROP_ANY,
                          'active power')
            net.set_flags('branch',
                          'bounded',
                          pf.BRANCH_PROP_TAP_CHANGER_V,
                          'tap ratio')                    
            net.set_flags('shunt',
                          'bounded',
                          pf.SHUNT_PROP_ANY,
                          'susceptance')
            net.set_flags('battery',
                          'bounded',
                          pf.BAT_PROP_ANY,
                          'energy level')
            
            self.assertEqual(net.num_bounded,
                             (net.get_num_tap_changers_v() +
                              net.get_num_buses_reg_by_tran() + 
                              net.get_num_slack_gens() +
                              net.num_shunts +
                              net.num_loads+
                              net.num_batteries))
                             
            # loads
            for load in net.loads:
                self.assertTrue(load.has_flags('bounded','active power'))

            # bats
            for bat in net.batteries:
                self.assertTrue(bat.has_flags('bounded','energy level'))
                self.assertFalse(bat.has_flags('bounded','charging power'))

    def test_fixed(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)

            net.add_vargens(net.get_gen_buses(),50.,30.,5,0.05)

            self.assertEqual(net.num_fixed,0)

            net.set_flags('bus',
                          'fixed',
                          pf.BUS_PROP_REG_BY_GEN,
                          ['voltage angle','voltage magnitude'])

            net.set_flags('generator',
                          'fixed',
                          pf.GEN_PROP_NOT_REG,
                          'reactive power')

            net.set_flags('load',
                          'fixed',
                          pf.LOAD_PROP_ANY,
                          'active power')

            net.set_flags('branch',
                          'fixed',
                          pf.BRANCH_PROP_PHASE_SHIFTER,
                          'phase shift')

            net.set_flags('shunt',
                          'fixed',
                          pf.SHUNT_PROP_SWITCHED_V,
                          'susceptance')

            net.set_flags('variable generator',
                          'fixed',
                          pf.VARGEN_PROP_ANY,
                          ['active power','reactive power'])

            net.set_flags('battery',
                          'fixed',
                          pf.BAT_PROP_ANY,
                          ['charging power','energy level'])
            
            self.assertEqual(net.num_fixed,
                             (net.get_num_buses_reg_by_gen()*2 + 
                              net.num_generators - net.get_num_reg_gens() + 
                              net.get_num_phase_shifters() +
                              net.get_num_switched_shunts()+
                              net.num_var_generators*2 +
                              net.num_loads+
                              3*net.num_batteries))

            # loads
            for load in net.loads:
                self.assertFalse(load.has_flags('bounded','active power'))
                self.assertFalse(load.has_flags('variable','active power'))
                self.assertTrue(load.has_flags('fixed','active power'))

            # batteries
            for bat in net.batteries:
                self.assertFalse(bat.has_flags('bounded','charging power'))
                self.assertFalse(bat.has_flags('variable','charging power'))
                self.assertTrue(bat.has_flags('fixed','charging power'))
                self.assertFalse(bat.has_flags('bounded','energy level'))
                self.assertFalse(bat.has_flags('variable','energy level'))
                self.assertTrue(bat.has_flags('fixed','energy level'))

    def test_multiple_flags(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)            

            self.assertEqual(net.num_vars,0)
            self.assertEqual(net.num_fixed,0)
            self.assertEqual(net.num_sparse,0)
            self.assertEqual(net.num_bounded,0)
            
            net.set_flags('bus',
                          ['variable','sparse'],
                          pf.BUS_PROP_REG_BY_GEN,
                          'voltage magnitude')

            self.assertEqual(net.num_vars,net.get_num_buses_reg_by_gen())
            self.assertEqual(net.num_fixed,0)
            self.assertEqual(net.num_sparse,net.get_num_buses_reg_by_gen())
            self.assertEqual(net.num_bounded,0)

            net.clear_flags()

            self.assertEqual(net.num_vars,0)
            self.assertEqual(net.num_fixed,0)
            self.assertEqual(net.num_sparse,0)
            self.assertEqual(net.num_bounded,0)

            net.set_flags('generator',
                          ['fixed','bounded'],
                          pf.GEN_PROP_SLACK,
                          ['active power','reactive power'])

            self.assertEqual(net.num_vars,0)
            self.assertEqual(net.num_fixed,2*net.get_num_slack_gens())
            self.assertEqual(net.num_sparse,0)
            self.assertEqual(net.num_bounded,2*net.get_num_slack_gens())

    def test_custom_flags(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)            

            # add vargens
            net.add_vargens(net.get_gen_buses(),50.,30.,5,0.05)
            self.assertGreater(net.num_var_generators,0)

            self.assertEqual(net.num_vars,0)
            self.assertEqual(net.num_fixed,0)
            self.assertEqual(net.num_sparse,0)
            self.assertEqual(net.num_bounded,0)

            num_vars = 0
            num_bounded = 0
            num_fixed = 0

            # buses
            for bus in net.buses:
                if bus.index % 3 == 0:
                    net.set_flags_of_component(bus,
                                               ['variable','bounded'],
                                               ['voltage angle','voltage magnitude'])
                    self.assertTrue(bus.has_flags('variable','voltage angle'))
                    self.assertTrue(bus.has_flags('variable','voltage magnitude'))
                    self.assertFalse(bus.has_flags('variable','voltage magnitude violation'))
                    self.assertTrue(bus.has_flags('bounded','voltage angle'))
                    self.assertTrue(bus.has_flags('bounded','voltage magnitude'))
                    self.assertFalse(bus.has_flags('bounded','voltage magnitude deviation'))
                    self.assertFalse(bus.has_flags('fixed','voltage angle'))
                    self.assertFalse(bus.has_flags('fixed','voltage magnitude'))
                    num_vars += 2
                    num_bounded += 2
                else:
                    self.assertFalse(bus.has_flags('variable','voltage angle'))
                    self.assertFalse(bus.has_flags('variable','voltage magnitude'))
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_bounded,num_bounded)

            # branches
            for branch in net.branches:
                if branch.index == 5:
                    net.set_flags_of_component(branch,
                                               ['fixed','variable'],
                                               'tap ratio')
                    self.assertTrue(branch.has_flags('fixed','tap ratio'))
                    self.assertTrue(branch.has_flags('variable','tap ratio'))
                    self.assertFalse(branch.has_flags('fixed','phase shift'))
                    self.assertFalse(branch.has_flags('variable','phase shift'))
                    num_fixed += 1
                    num_vars += 1
                else:
                    self.assertFalse(branch.has_flags('fixed','tap ratio'))
                    self.assertFalse(branch.has_flags('variable','tap ratio'))
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)

            # gens
            for gen in net.generators:
                if not gen.is_regulator():
                    net.set_flags_of_component(gen,
                                               'variable',
                                               'active power')
                    self.assertTrue(gen.has_flags('variable','active power'))
                    self.assertFalse(gen.has_flags('variable','reactive power'))
                    self.assertFalse(gen.has_flags('fixed','active power'))
                else:
                    self.assertFalse(gen.has_flags('variable','active power'))
            num_vars += net.num_generators-net.get_num_reg_gens()
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)

            # loads
            lcount = 0
            for load in net.loads:
                if load.index % 5 == 0:
                    net.set_flags_of_component(load,
                                               'variable',
                                               'active power')
                    lcount += 1
            for load in net.loads:
                if load.index % 5 == 0:
                    self.assertTrue(load.has_flags('variable','active power'))
                    self.assertFalse(load.has_flags('fixed','active power'))
                else:
                    self.assertFalse(load.has_flags('variable','active power'))
            num_vars += lcount
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)

            # shunts
            for shunt in net.shunts:
                if shunt.index % 2 == 0:
                    net.set_flags_of_component(shunt,
                                               'bounded',
                                               'susceptance')
                    self.assertTrue(shunt.has_flags('bounded','susceptance'))
                    self.assertFalse(shunt.has_flags('variable','susceptance'))
                    self.assertFalse(shunt.has_flags('fixed','susceptance'))
                    num_bounded += 1
                else:
                    self.assertFalse(shunt.has_flags('bounded','susceptance'))
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)
            self.assertEqual(net.num_bounded,num_bounded)

            # vargens
            for vargen in net.var_generators:
                if vargen.index % 3 == 0:
                    net.set_flags_of_component(vargen,
                                               'variable',
                                               ['active power','reactive power'])
                    net.set_flags_of_component(vargen,
                                               'variable',
                                               ['active power','reactive power'])
                    self.assertTrue(vargen.has_flags('variable','active power'))
                    self.assertTrue(vargen.has_flags('variable','reactive power'))
                    self.assertFalse(vargen.has_flags('fixed','active power'))
                    self.assertFalse(vargen.has_flags('fixed','reactive power'))
                    num_vars += 2
                else:
                    self.assertFalse(vargen.has_flags('variable','active power'))
                    self.assertFalse(vargen.has_flags('variable','reactive power'))
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)
            self.assertEqual(net.num_bounded,num_bounded)

            # batteries
            bcount = 0
            for bat in net.batteries:
                if bat.index % 2 == 0:
                    net.set_flags_of_component(bat,
                                               'variable',
                                               'charging power')
                    bcount += 2
            for bat in net.batteries:
                if bat.index % 2 == 0:
                    self.assertTrue(bat.has_flags('variable','charging power'))
                    self.assertFalse(bat.has_flags('fixed','charging power'))
                else:
                    self.assertFalse(bat.has_flags('variable','charging power'))
            num_vars += bcount
            self.assertEqual(net.num_vars,num_vars)
            self.assertEqual(net.num_fixed,num_fixed)

    def test_errors(self):

        net = self.net

        for case in test_cases.CASES:
            
            net.load(case)

            self.assertRaises(KeyError,
                              net.set_flags,
                              -1,
                              'variable',
                              pf.BUS_PROP_ANY,
                              'voltage angle')
            self.assertRaises(KeyError,
                              net.set_flags,
                              'bus',
                              -1,
                              pf.BUS_PROP_ANY,
                              'voltage angle')
            self.assertRaises(KeyError,
                              net.set_flags,
                              'generator',
                              -1,
                              pf.GEN_PROP_ANY,
                              'active power')
            self.assertRaises(KeyError,
                              net.set_flags,
                              'branch',
                              -1,
                              pf.BRANCH_PROP_TAP_CHANGER_V,
                              'tap ratio')
            
    def tearDown(self):
        
        pass




