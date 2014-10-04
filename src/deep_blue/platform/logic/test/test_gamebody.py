from .. import gamebody
from .. import map_info
from ..basic import *
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
        self.gamebody = gamebody.GameBody(m, ['', ''])

        # bases, assign ranges explicitly to avoid changes in basic.py
        self.base0 = Base(0, Rectangle(Position(0, 0, 1),
                                       Position(2, 2, 1)),
                          sight_ranges=[4, 10, 8],
                          fire_ranges=[0, 7, 5])
        self.base1 = Base(1, Rectangle(Position(12, 12, 1),
                                       Position(14, 14, 1)),
                     sight_ranges=[4, 10, 8],
                     fire_ranges=[0, 7, 5])
        self.assertTrue(self.gamebody.map_info.add_element(self.base0))
        self.assertTrue(self.gamebody.map_info.add_element(self.base1))
        # forts
        self.fort0 = Fort(2, Position(9, 12, 1))
        self.fort1 = Fort(2, Rectangle(Position(6, 6, 1),
                                       Position(7, 7, 1)))
        self.fort2 = Fort(2, Position(9, 4, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort0))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort1))
        self.assertTrue(self.gamebody.map_info.add_element(self.fort2))

        # oilfield
        self.oilfield0 = Oilfield(Position(13, 2, 1))
        self.oilfield1 = Oilfield(Position(3, 7, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.oilfield0))
        self.assertTrue(self.gamebody.map_info.add_element(self.oilfield1))

        # mine
        self.mine0 = Mine(Position(14, 5, 1))
        self.mine1 = Mine(Position(3, 10, 1))
        self.assertTrue(self.gamebody.map_info.add_element(self.mine0))
        self.assertTrue(self.gamebody.map_info.add_element(self.mine1))

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
        for element in [self.base1, self.fort0, self.fort1, self.oilfield0,
                        self.mine0]:
            self.assertIn(element, units_in_sight.values())

    def test_view_outrange_base(self):
        """Test behavior of viewing a base out of sight (see everything)"""
        units_in_sight = self.gamebody.view_elements(1)
        self.assertIn(self.base0, units_in_sight)

    def test_view_outrange_resource(self):
        """Test behavior of viewing a base out of sight (see nothing)"""
        units_in_sight = self.gamebody.view_elements(1)
        self.assertNotIn(self.oilfield1, units_in_sight)
        self.assertNotIn(self.mine1, units_in_sight)

    def test_(self):
        self.assertTrue(self.gamebody.map_info.add_element(
            Destroyer(1, Position(5, 2, 1), sight_ranges=[1, 3, 2],
                                            fire_ranges=[1, 3, 2])))




if __name__ == '__main__':
    unittest.main()
