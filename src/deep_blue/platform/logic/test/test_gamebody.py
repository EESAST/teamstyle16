from .. import gamebody
import unittest

class TestGameBody(unittest.TestCase):
    """Test case for module gamebody"""

    def test_constants(self):
        """Test constants in module gamebody"""
        self.assertNotEqual(STATE_CONTINUE, 0)
        self.assertNotEqual(STATE_CONTINUE, 1)
        self.assertNotEqual(STATE_TIE, 0)
        self.assertNotEqual(STATE_TIE, 1)
        self.assertNotEqual(STATE_CONTINUE, STATE_TIE)

if __name__ == '__main__':
    unittest.main()
