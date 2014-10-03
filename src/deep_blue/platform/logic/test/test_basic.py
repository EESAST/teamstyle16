from .. import basic
import unittest

class TestBasic(unittest.TestCase):
    """Test case for Basic"""

    def setUp(self):
        """Build Elements for testing"""
        self.pos1 = basic.Position(9, 10, 0)
        self.pos2 = basic.Position(12, 15, 0)
        self.rec = basic.Rectangle(self.pos1, self.pos2)

        base = basic.Base(0, self.rec)
        fort = basic.Fort(1, self.rec)
        mine = basic.Mine(self.rec)
        oilfield = basic.Oilfield(self.rec)
        submarine = basic.Submarine(1, self.pos1)
        destroyer = basic.Destroyer(1, self.pos2)
        carrier = basic.Carrier(1, self.pos2)
        cargo = basic.Cargo(1, self.pos2)
        fighter = basic.Fighter(1, self.pos2)
        scout = basic.Scout(1, self.pos2)

        self.elements = [base, fort, mine, oilfield, submarine,
                   destroyer, carrier, cargo, fighter, scout]
        self.unit_bases = [base, fort, submarine, destroyer,
                     carrier, cargo, fighter, scout]
        self.units = [submarine, destroyer, carrier, cargo, fighter, scout]

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
        self.pos1.x = 2
        self.pos1.y = 5
        self.pos1.z = 8
        self.assertEqual(2, self.pos1.x)
        self.assertEqual(5, self.pos1.y)
        self.assertEqual(8, self.pos1.z)

    def test_rectangle(self):
        """Test class basic.Rectangle"""
        self.assertEqual(self.pos1, self.rec.upper_left)
        self.assertEqual(self.pos2, self.rec.lower_right)
        self.assertEqual(self.pos1.x, self.rec.x)
        self.assertEqual(self.pos1.y, self.rec.y)
        self.assertEqual(self.pos1.z, self.rec.z)
        self.assertEqual((self.pos2.x - self.pos1.x + 1,
                          self.pos2.y - self.pos1.y + 1), self.rec.size)

    def test_element_attr(self):
        """Test attributes of Elements"""

        element_attrs = ['index', 'pos', 'size', 'visible']
        unit_base_attrs = ['team', 'sight_ranges', 'fire_ranges', 'health',
                         'health_max', 'fuel', 'fuel_max', 'ammo', 'ammo_once',
                         'ammo_max', 'metal', 'metal_max', 'attacks', 'defences']
        unit_attrs = ['speed', 'dest', 'cost', 'population', 'build_round']

        # Check attributes
        for element in self.elements:
            for attr in element_attrs:
                self.assertTrue(hasattr(element, attr))

        for element in self.unit_bases:
            for attr in unit_base_attrs:
                self.assertTrue(hasattr(element, attr))

        for element in self.units:
            for attr in unit_attrs:
                self.assertTrue(hasattr(element, attr))

    def test_element_default_init(self):
        """Test behavior of default initialization of Elements"""
        for element in self.elements:
            prop = basic.PROPERTY[element.kind]  # get const attrs
            # test constants
            for attr, value in prop.items():
                if (hasattr(element, attr)):
                    self.assertEqual(value, getattr(element, attr))
            # test default state
            if isinstance(element, basic.UnitBase):
                for attr in ['health', 'fuel', 'ammo', 'metal']:
                    self.assertEqual(prop[attr + '_max'], getattr(element, attr))

    def test_element_complete_init(self):
        """Test behavior of complete initialization of Elements"""
        classes = ['Base', 'Fort', 'Mine', 'Oilfield', 'Submarine', 'Cargo',
                   'Carrier', 'Destroyer', 'Fighter', 'Scout']

        attrs = {'sight_ranges': [11, 12, 13], 'fire_ranges': [34, 35, 36],
                 'health': 200, 'health_max': 100,  # overloaded
                 'fuel': 200, 'fuel_max': 250,      # not full
                 'ammo': basic.INFINITY, 'ammo_max': basic.INFINITY,  # infinity
                 'metal': 100, 'metal_max': 100,    # normal
                 'speed': 42, 'population': 43,
                 'attacks': [88, 88], 'defences': [77, 77]}

        for cls in classes:
            element = getattr(basic, cls)(team=1, pos=self.pos1, **attrs)
            if isinstance(element, basic.Mine):
                self.assertEqual(attrs['metal'], element.metal)
            elif isinstance(element, basic.Oilfield):
                self.assertEqual(attrs['fuel'], element.fuel)
            else:  # UnitBase
                for attr, value in attrs.items():
                    value = min(value, attrs.get(attr + '_max', value))
                    self.assertEqual(value, getattr(element, attr))

if __name__ == '__main__':
    unittest.main()
