from .. import basic
import unittest

class TestMapInfo(unittest.TestCase):
    """Test case for MapInfo"""

    def test_class_inherit(self):
        """Test class relationships based on API"""
        self.assertTrue(issubclass(basic.Position, object))
        self.assertTrue(issubclass(basic.Rectangle, object))
        self.assertTrue(issubclass(basic.Element, object))

        self.assertTrue(issubclass(basic.Resource, basic.Element))
        self.assertTrue(issubclass(basic.Mine, basic.Resource))
        self.assertTrue(issubclass(basic.Oilfield, basic.Resource))

        self.assertTrue(issubclass(basic.UnitBase, basic.Element))
        self.assertTrue(issubclass(basic.Building, basic.UnitBase))
        self.assertTrue(issubclass(basic.Base, basic.Building))
        self.assertTrue(issubclass(basic.Fort, basic.Building))

        self.assertTrue(issubclass(basic.Unit, basic.UnitBase))
        self.assertTrue(issubclass(basic.Submarine, basic.Unit))
        self.assertTrue(issubclass(basic.Ship, basic.Unit))
        self.assertTrue(issubclass(basic.Cargo, basic.Unit))
        self.assertTrue(issubclass(basic.Carrier, basic.Unit))
        self.assertTrue(issubclass(basic.Destroyer, basic.Unit))

        self.assertTrue(issubclass(basic.Plane, basic.Unit))
        self.assertTrue(issubclass(basic.Fighter, basic.Plane))
        self.assertTrue(issubclass(basic.Scout, basic.Plane))

    def test_element_kind(self):
        self.assertEqual(0, basic.Base.kind)
        self.assertEqual(1, basic.Fort.kind)
        self.assertEqual(2, basic.Mine.kind)
        self.assertEqual(3, basic.Oilfield.kind)
        self.assertEqual(4, basic.Submarine.kind)
        self.assertEqual(5, basic.Destroyer.kind)
        self.assertEqual(6, basic.Carrier.kind)
        self.assertEqual(7, basic.Cargo.kind)
        self.assertEqual(8, basic.Fighter.kind)
        self.assertEqual(9, basic.Scout.kind)

    def test_position(self):
        # test initialize
        pos = basic.Position(31, 45, 67)
        self.assertEqual(31, pos.x)
        self.assertEqual(45, pos.y)
        self.assertEqual(67, pos.z)
        # test modify
        pos.x = 2
        pos.y = 5
        pos.z = 8
        self.assertEqual(2, pos.x)
        self.assertEqual(5, pos.y)
        self.assertEqual(8, pos.z)

    def test_element(self):
        pass


if __name__ == '__main__':
    unittest.main()
