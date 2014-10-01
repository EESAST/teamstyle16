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

if __name__ == '__main__':
    unittest.main()
