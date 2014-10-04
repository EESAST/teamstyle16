from .. import gamebody
import unittest

class TestGameBody(unittest.TestCase):
    """Test case for module gamebody"""

    def test_constants(self):
        """Test constants in module gamebody"""
        cont = gamebody.STATE_CONTINUE
        tie = gamebody.STATE_TIE
        self.assertNotEqual(cont, 0)
        self.assertNotEqual(cont, 1)
        self.assertNotEqual(tie, 0)
        self.assertNotEqual(tie, 1)
        self.assertNotEqual(cont, tie)

if __name__ == '__main__':
    unittest.main()
