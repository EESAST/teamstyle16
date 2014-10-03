from .. import map_info
import unittest
import random

class TestMapInfo(unittest.TestCase):
    """Test case for MapInfo"""

    def setUp(self):
        """Set up a MapInfo object for testing"""

        self.paras = {
            "x_max": random.randint(50, 150),
            "y_max": random.randint(50, 150),
            "max_population": random.randint(50, 500),
            "record_interval":3,
            "time_per_round": 0.8,
            "weather":-1,
        }
        self.map_2d_array = self.generate_random_2d_array(self.paras['x_max'],
                                                          self.paras['y_max'],
                                                          random.random())

        self.map_info = map_info.MapInfo(types=self.map_2d_array,
                                         **self.paras)

    def test_axes(self):
        """Test the direction of axes (x go down, y go right)"""
        tmp_map_info = map_info.MapInfo(10, 20, 0, 0, 0, 0)
        self.assertEqual(10, len(tmp_map_info.types))
        self.assertEqual(20, len(tmp_map_info.types[0]))

    def test_initialize(self):
        """Check whether MapInfo has been initialized correctly"""
        for attr, value in self.paras.items():
            self.assertEqual(value, getattr(self.map_info, attr))
        for x in xrange(self.paras['x_max']):
            for y in xrange(self.paras['y_max']):
                self.assertEqual(self.map_2d_array[x][y],
                                 self.map_info.map_type(x, y))

    def test_set_map_type(self):
        """Test setting map_types"""
        for i in xrange(200):  # take 200 samples
            x = random.randint(0, self.paras['x_max'] * 2)
            y = random.randint(0, self.paras['y_max'] * 2)
            new_map_type = random.randint(0, 1)

            result = self.map_info.set_map_type(x, y, new_map_type)
            if x < self.paras['x_max'] and y < self.paras['y_max']:
                self.assertTrue(result)
                self.assertEqual(new_map_type, self.map_info.map_type(x, y))
            else:
                self.assertFalse(result)

    def test_save_load(self):
        """Test save and load function provided"""
        # string version
        # no == provided, so compare string instead
        map_info_str = self.map_info.saves()
        self.assertEqual(map_info_str,
                         map_info.loads(map_info_str).saves())
        # file version
        file_name = 'tmp_map_info.map'
        self.map_info.save(file_name)
        self.assertEqual(map_info_str,
                         map_info.load(file_name).saves())

    def generate_random_2d_array(self, x_max, y_max, land_ratio):
        array = []
        for x in xrange(x_max):
            new_row = []
            for y in xrange(y_max):
                new_row.append(1 if random.random() < land_ratio else 0)
            array.append(new_row)
        return array

if __name__ == '__main__':
    unittest.main()
