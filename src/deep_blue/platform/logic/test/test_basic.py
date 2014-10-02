from .. import basic
import unittest

class TestBasic(unittest.TestCase):
    """Test case for Basic"""

    def test_class_inheritance(self):
        """Test Element inheritance"""
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
        """Test kind number of elements"""
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
        """Test class basic.Position"""
        # test initialize
        pos = basic.Position(31, 45, 2)
        self.assertEqual(31, pos.x)
        self.assertEqual(45, pos.y)
        self.assertEqual(2, pos.z)
        # test modify
        pos.x = 2
        pos.y = 5
        pos.z = 8
        self.assertEqual(2, pos.x)
        self.assertEqual(5, pos.y)
        self.assertEqual(8, pos.z)

    def test_rectangle(self):
        """Test class basic.Rectangle"""
        pos1 = basic.Position(6, 10, 0)
        pos2 = basic.Position(122, 15, 0)
        rec = basic.Rectangle(pos1, pos2)

        self.assertEqual(pos1, rec.upper_left)
        self.assertEqual(pos2, rec.lower_right)
        self.assertEqual(6, rec.x)
        self.assertEqual(10, rec.y)
        self.assertEqual(0, rec.z)
        self.assertEqual((116, 5), rec.size)

    def test_element(self):
        """Test initialization & attributes of Elements"""
        pos1 = basic.Position(9, 10, 0)
        pos2 = basic.Position(12, 15, 0)
        rec = basic.Rectangle(pos1, pos2)

        base = basic.Base(0, rec)
        fort = basic.Fort(1, rec)
        mine = basic.Mine(rec)
        oilfield = basic.Oilfield(rec)
        submarine = basic.Submarine(1, pos1)
        destroyer = basic.Destroyer(1, pos2)
        carrier = basic.Carrier(1, pos2)
        cargo = basic.Cargo(1, pos2)
        fighter = basic.Fighter(1, pos2)
        scout = basic.Scout(1, pos2)

        elements = [base, fort, mine, oilfield, submarine,
                   destroyer, carrier, cargo, fighter, scout]
        unit_bases = [base, fort, submarine, destroyer,
                     carrier, cargo, fighter, scout]
        units = [submarine, destroyer, carrier, cargo, fighter, scout]

        # Check attributes
        for element in elements:
            self.assertTrue(hasattr(element, 'index'))
            self.assertTrue(hasattr(element, 'pos'))
            self.assertTrue(hasattr(element, 'size'))
            self.assertTrue(hasattr(element, 'visible'))
        for element in unit_bases:
            self.assertTrue(hasattr(element, 'team'))
            self.assertTrue(hasattr(element, 'sight_ranges'))
            self.assertTrue(hasattr(element, 'fire_ranges'))
            self.assertTrue(hasattr(element, 'health'))
            self.assertTrue(hasattr(element, 'health_max'))
            self.assertTrue(hasattr(element, 'fuel'))
            self.assertTrue(hasattr(element, 'fuel_max'))
            self.assertTrue(hasattr(element, 'ammo'))
            self.assertTrue(hasattr(element, 'ammo_once'))
            self.assertTrue(hasattr(element, 'ammo_max'))
            self.assertTrue(hasattr(element, 'metal'))
            self.assertTrue(hasattr(element, 'metal_max'))
            self.assertTrue(hasattr(element, 'attacks'))
            self.assertTrue(hasattr(element, 'defences'))
        for element in units:
            self.assertTrue(hasattr(element, 'speed'))
            self.assertTrue(hasattr(element, 'dest'))
            self.assertTrue(hasattr(element, 'cost'))
            self.assertTrue(hasattr(element, 'population'))
            self.assertTrue(hasattr(element, 'build_round'))


if __name__ == '__main__':
    unittest.main()
