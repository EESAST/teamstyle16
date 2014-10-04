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
        self.assertTrue(self.gamebody.map_info.add_element(
            Base(0, Rectangle(Position(0, 0, 1),
                              Position(2, 2, 1)), sight_ranges=[4, 10, 8],
                                                  fire_ranges=[0, 7, 5])))
        self.assertTrue(self.gamebody.map_info.add_element(
            Base(1, Rectangle(Position(12, 12, 1),
                              Position(14, 14, 1)), sight_ranges=[4, 10, 8],
                                                    fire_ranges=[0, 7, 5])))
        # forts
        self.assertTrue(self.gamebody.map_info.add_element(
            Fort(2, Rectangle(Position(6, 6, 1),
                              Position(7, 7, 1)))))
        self.assertTrue(self.gamebody.map_info.add_element(
            Fort(2, Position(9, 4, 1))))
        self.assertTrue(self.gamebody.map_info.add_element(
            Fort(2, Position(9, 12, 1))))

        # oilfield
        self.assertTrue(self.gamebody.map_info.add_element(
            Oilfield(Position(13, 2, 1))))
        self.assertTrue(self.gamebody.map_info.add_element(
            Oilfield(Position(3, 7, 1))))

        # mine
        self.assertTrue(self.gamebody.map_info.add_element(
            Mine(Position(14, 5, 1))))
        self.assertTrue(self.gamebody.map_info.add_element(
            Mine(Position(3, 10, 1))))

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
        unit_in_sight = self.gamebody.view_elements(1)
        print unit_in_sight
        pass

    def test_view_outrange_base(self):
        """Test """
        pass

    def test_view_outrange_resource(self):
        pass

    def test_(self):
        self.assertTrue(self.gamebody.map_info.add_element(
            Destroyer(1, Position(5, 2, 1), sight_ranges=[1, 3, 2],
                                            fire_ranges=[1, 3, 2])))




if __name__ == '__main__':
    unittest.main()
