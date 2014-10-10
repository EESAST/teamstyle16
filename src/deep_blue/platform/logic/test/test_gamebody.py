from .. import gamebody
from .. import map_info
from ..basic import *
from ..command import *

import unittest

class TestGameBody(unittest.TestCase):
    """Test case for module gamebody"""

    def setUp(self):
        """Build a gamebody for testing"""
        # a 15 * 15 map
        types = [[1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0],
                 [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
                 [0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1],
                 [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1],
                 [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1],
                 [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1]]
        m = map_info.MapInfo(15, 15,
                             max_population=100,
                             max_round=100,
                             record_interval=1,
                             time_per_round=1.0,
                             weather=0,
                             types=types)
        self.gamebody = gamebody.GameBody(m)

        # bases, assign ranges explicitly to avoid changes in basic.py
        self.base0 = Base(0, Rectangle(Position(0, 0, 1),
                                       Position(2, 2, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.base1 = Base(1, Rectangle(Position(12, 12, 1),
                                       Position(14, 14, 1)),
                     sight_ranges=[4, 10, 8],
                     fire_ranges=[0, 7, 5])
        self.base0_index = self.gamebody.map_info.add_element(self.base0)
        self.base1_index = self.gamebody.map_info.add_element(self.base1)
        self.assertIsNotNone(self.base0_index)
        self.assertIsNotNone(self.base1_index)
        # forts
        self.fort0 = Fort(2, Position(9, 12, 1))
        self.fort1 = Fort(2, Rectangle(Position(6, 6, 1),
                                       Position(7, 7, 1)))
        self.fort2 = Fort(2, Position(9, 4, 1))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.fort0))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.fort1))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.fort2))

        # oilfield
        self.oilfield0 = Oilfield(Position(13, 2, 1))
        self.oilfield1 = Oilfield(Position(3, 7, 1))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.oilfield0))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.oilfield1))

        # mine
        self.mine0 = Mine(Position(14, 5, 1))
        self.mine1 = Mine(Position(3, 10, 1))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.mine0))
        self.assertIsNotNone(self.gamebody.map_info.add_element(self.mine1))

    def test_constants(self):
        """Test constants in module gamebody"""
        cont = gamebody.STATE_CONTINUE
        tie = gamebody.STATE_TIE
        self.assertNotEqual(cont, 0)
        self.assertNotEqual(cont, 1)
        self.assertNotEqual(tie, 0)
        self.assertNotEqual(tie, 1)
        self.assertNotEqual(cont, tie)

    def test_basic_view(self):
        """Test baisc behavior of view"""
        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base1, self.oilfield0, self.mine0]:
            self.assertIn(element, units_in_sight.values())

    def test_view_outrange_base(self):
        """Test behavior of viewing a base out of sight"""
        # units_in_sight = self.gamebody.view_elements(1)
        # self.assertIn(self.base0, units_in_sight)
        pass

    def test_view_outrange_resource(self):
        """Test behavior of viewing a base out of sight (see nothing)"""
        # units_in_sight = self.gamebody.view_elements(1)
        # self.assertNotIn(self.oilfield1, units_in_sight)
        # self.assertNotIn(self.mine1, units_in_sight)
        pass

    def test_attack_pos(self):
        """Test behavior of attack position"""
        # Add attcker
        attacker = self.gamebody.map_info.add_element(
            Destroyer(1, Position(3,2,1), sight_ranges = [1,3,2],
                                          fire_ranges = [2,2,2]))
        self.assertIsNotNone(attacker)

        # Attack an element out of fire range
        self.assertFalse(self.gamebody.set_command(1, AttackPos(attacker, Position(0,0,1))))
        self.assertEqual(self.base0.health, self.base0.health_max)

        # Attack an element in range
        self.assertTrue(
            self.gamebody.set_command(1, AttackPos(attacker, Position(2,2,1))))
        self.assertNotEqual(self.base0.health, self.base0.health_max)     #shame on myself, I failed to calculate it accurately.(hhh)

    def test_attack_unit(self):
        """Test behavior of attack unit"""
        index_1 = self.gamebody.map_info.add_element(Destroyer(0, Position(0,4,1), fire_ranges = [2,2,2]))
        index_2 = self.gamebody.map_info.add_element(Destroyer(1, Position(0,5,1)))
        index_3 = self.gamebody.map_info.add_element(Destroyer(1, Position(0,7,1)))
        self.assertIsNotNone(index_1)
        self.assertIsNotNone(index_2)
        self.assertIsNotNone(index_3)

        elements = self.gamebody.map_info.elements

        self.assertTrue(self.gamebody.set_command(0, AttackUnit(index_1, index_2)))
        self.assertNotEqual(elements[index_2].health, elements[index_2].health_max)
        self.assertFalse(self.gamebody.set_command(0, AttackUnit(index_1, index_3)))
        self.assertEqual(elements[index_3].health, elements[index_3].health_max)

    # def test_change_dest(self):
    #     """Test behavior of change destination"""
    #     index = self.gamebody.map_info.add_element(Destroyer(0, Position(0,4,1)))
    #     self.assertIsNotNone(index)
    #     self.assertTrue(self.gamebody.set_command(0, ChangeDest(index, Position(0,6,1))))
    #     self.assertEqual(self.gamebody.map_info.elements[index].dest, Position(0,6,1))

    def test_fix(self):
        """Test behavior of fix"""
        index_1 = self.gamebody.map_info.add_element(Destroyer(0, Position(3,2,1)))
        index_2 = self.gamebody.map_info.add_element(Destroyer(0, Position(0,4,1)))
        self.assertIsNotNone(index_1)
        self.assertIsNotNone(index_2)
        self.gamebody.map_info.elements[index_1].health = self.gamebody.map_info.elements[index_1].health_max - 1
        self.gamebody.map_info.elements[index_2].health = self.gamebody.map_info.elements[index_2].health_max - 1
        self.assertTrue(self.gamebody.set_command(0, Fix(self.base0_index, index_1)))
        self.assertFalse(self.gamebody.set_command(0, Fix(self.base0_index, index_2)))
        self.assertEqual(self.gamebody.map_info.elements[index_1].health, self.gamebody.map_info.elements[index_1].health_max)
        self.assertEqual(self.gamebody.map_info.elements[index_1].health, self.gamebody.map_info.elements[index_1].health_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[base0_index].metal, self.gamebody.map_info.elements[base0_index].metal_max - METAL_PER_HEALTH)

    def test_produce(self):
        """Test behavior of produce"""
        destroyer = Destroyer(0, Position(0,4,1))
        self.assertTrue(self.gamebody.set_command(0, Produce(self.base0_index, DESTROYER)))
        for i in range(destroyer.build_round()):
            self.gamebody.run()

    def test_supply(self):
        """Test behavior of supply"""
        index_1 = self.gamebody.map_info.add_element(Destroyer(0, Position(3,2,1)))
        index_2 = self.gamebody.map_info.add_element(Destroyer(0, Position(0,4,1)))
        self.assertIsNotNone(index_1)
        self.assertIsNotNone(index_2)
        self.gamebody.map_info.elements[index_1].fuel -= 1
        self.gamebody.map_info.elements[index_1].metal -= 1
        self.gamebody.map_info.elements[index_1].ammo -= 1
        self.gamebody.map_info.elements[index_2].fuel -= 1
        self.gamebody.map_info.elements[index_2].metal -= 1
        self.gamebody.map_info.elements[index_2].ammo -= 1
        self.assertTrue(self.gamebody.set_command(0, Supply(self.base0_index, index_1)))
        self.assertFalse(self.gamebody.set_command(0, Supply(self.base0_index, index_2)))
        self.assertEqual(self.gamebody.map_info.elements[self.base0_index].fuel, self.gamebody.map_info.elements[self.base0_index].fuel_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[self.base0_index].metal, self.gamebody.map_info.elements[self.base0_index].metal_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[self.base0_index].ammo, self.gamebody.map_info.elements[self.base0_index].ammo_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[index_1].fuel, self.gamebody.map_info.elements[index_1].fuel_max)
        self.assertEqual(self.gamebody.map_info.elements[index_1].metal, self.gamebody.map_info.elements[index_1].metal_max)
        self.assertEqual(self.gamebody.map_info.elements[index_1].ammo, self.gamebody.map_info.elements[index_1].ammo_max)
        self.assertEqual(self.gamebody.map_info.elements[index_2].fuel, self.gamebody.map_info.elements[index_2].fuel_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[index_2].metal, self.gamebody.map_info.elements[index_2].metal_max - 1)
        self.assertEqual(self.gamebody.map_info.elements[index_2].ammo, self.gamebody.map_info.elements[index_2].ammo_max - 1)

    def test_cancel(self):
        """Test behavior of cancel"""
        index_1 = self.gamebody.map_info.add_element(Destroyer(0, Position(0,4,1), fire_ranges = [2,2,2]))
        index_2 = self.gamebody.map_info.add_element(Destroyer(1, Position(0,5,1)))
        self.assertIsNotNone(index_1)
        self.assertIsNotNone(index_2)
        self.assertTrue(self.gamebody.set_command(0, AttackUnit(index_1, index_2)))
        self.assertTrue(self.gamebody.set_command(0, Cancel(index_1)))
        self.assertEqual(self.gamebody.map_info.elements[index_2].health, self.gamebody.map_info.elements[index_2].health_max)

    def test_(self):
        pass

if __name__ == '__main__':
    unittest.main()
