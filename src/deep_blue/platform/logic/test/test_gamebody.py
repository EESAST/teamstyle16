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
        m = self.gamebody.map_info

        # bases, assign ranges explicitly to avoid changes in basic.py
        self.base0 = Base(0, Rectangle(Position(0, 0, 1),
                                       Position(2, 2, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.base1 = Base(1, Rectangle(Position(12, 12, 1),
                                       Position(14, 14, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.assertIsNotNone(m.add_element(self.base0))
        self.assertIsNotNone(m.add_element(self.base1))
        # forts
        self.fort0 = Fort(1, Position(9, 12, 1), sight_ranges=[2, 2, 2])
        self.fort1 = Fort(2, Rectangle(Position(6, 6, 1),
                                       Position(7, 7, 1)))
        self.fort2 = Fort(2, Position(9, 4, 1))
        self.assertIsNotNone(m.add_element(self.fort0))
        self.assertIsNotNone(m.add_element(self.fort1))
        self.assertIsNotNone(m.add_element(self.fort2))

        # oilfield
        self.oilfield0 = Oilfield(Position(13, 2, 1))
        self.oilfield1 = Oilfield(Position(3, 7, 1))
        self.assertIsNotNone(m.add_element(self.oilfield0))
        self.assertIsNotNone(m.add_element(self.oilfield1))

        # mine
        self.mine0 = Mine(Position(14, 5, 1))
        self.mine1 = Mine(Position(3, 10, 1))
        self.assertIsNotNone(m.add_element(self.mine0))
        self.assertIsNotNone(m.add_element(self.mine1))

    def add(self, cls, team, pos, **kw):
        """Helper method, add element into the map"""
        element = cls(team, Position(pos[0], pos[1], 0), **kw)
        self.assertIsNotNone(self.gamebody.map_info.add_element(element))
        return element

    def varify_attrs(self, element, ghost, pub_attrs, priv_attrs):
        for attr in pub_attrs:
            if hasattr(element, attr):
                # Should be visible to everyone
                self.assertEqual(getattr(element, attr),
                                 getattr(ghost, attr))
        for attr in priv_attrs:
            if hasattr(element, attr):
                # Should not be visible to enemy
                if attr == 'pos' or attr == 'dest':
                    # pos/dest should be set to None
                    expected = None
                elif attr == 'team':
                    expected = 2
                else:
                    expected = 0
                self.assertEqual(expected, getattr(ghost, attr))

    def test_constants(self):
        """Test constants in module gamebody"""
        cont = gamebody.STATE_CONTINUE
        tie = gamebody.STATE_TIE
        self.assertNotEqual(cont, 0)
        self.assertNotEqual(cont, 1)
        self.assertNotEqual(tie, 0)
        self.assertNotEqual(tie, 1)
        self.assertNotEqual(cont, tie)

    def test_view_friendly(self):
        """Test behavior of viewing friendly (see everything)"""
        friendly1 = self.add(Fighter, 1, (14, 14))
        friendly2 = self.add(Carrier, 1, (8, 5))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base1, self.fort0, friendly1, friendly2]:
            self.assertIn(element, units_in_sight.values())

    def test_view_insight_enemy(self):
        """Test behavior of viewing enemies in sight (no fuel, ammo, metal,
        dest)"""
        self.add(Scout, 1, (0, 2))  # light up enemy base
        self.fort1.team = 0  # Create an enemy fort
        enemy1 = self.add(Fighter, 0, (14, 4), dest=Position(1, 1, 2))
        enemy2 = self.add(Destroyer, 0, (11, 13))
        enemy3 = self.add(Submarine, 0, (8, 12), dest=Position(8, 11, 0))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [self.base0, self.fort1, enemy1, enemy2, enemy3]:
            self.assertIn(element.index, units_in_sight)
            ghost = units_in_sight[element.index]

            pub_attrs = ['kind', 'position', 'size', 'team', 'health']
            priv_attrs = ['fuel', 'ammo', 'metal', 'dest']

            self.varify_attrs(element, ghost, pub_attrs, priv_attrs)

    def test_view_outsight_enemy_unit(self):
        """Test behavior of viewing enemy units out of sight (see nothing)"""
        enemy1 = self.add(Fighter, 0, (13, 3))
        enemy2 = self.add(Destroyer, 0, (5, 8))
        enemy3 = self.add(Submarine, 0, (7, 14))

        units_in_sight = self.gamebody.view_elements(1)
        for element in [enemy1, enemy2, enemy3]:
            self.assertNotIn(element.index, units_in_sight)

    def test_view_outsight_fort(self):
        """Test behavior of viewing forts out of sight (only pos)"""
        for team in [0, 2]:
            self.fort2.team = team  # Create an enemy/neutral fort

            units_in_sight = self.gamebody.view_elements(1)
            self.assertIn(self.fort2.index, units_in_sight)
            ghost = units_in_sight[self.fort2.index]

            pub_attrs = ['kind', 'position', 'size']
            priv_attrs = ['team', 'health', 'fuel', 'ammo', 'metal']

            self.varify_attrs(self.fort2, ghost, pub_attrs, priv_attrs)

    def test_view_outrange_resource(self):
        """Test behavior of viewing resources out of sight (see only pos)"""
        units_in_sight = self.gamebody.view_elements(1)

        pub_attrs = ['kind', 'position', 'size']
        priv_attrs = ['fuel', 'metal']

        for element in [self.mine1, self.oilfield1]:
            self.assertIn(element.index, units_in_sight)
            ghost = units_in_sight[element.index]
            self.varify_attrs(element, ghost, pub_attrs, priv_attrs)

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
