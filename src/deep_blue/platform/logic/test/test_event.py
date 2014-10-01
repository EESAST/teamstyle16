import unittest
# from .. import basic
from .. import event

class TestEvent(unittest.TestCase):
    """Test case for Event"""

    def test_class_inherit(self):
        """Test class relationships based on API"""
        self.assertTrue(issubclass(event.Event, object))
        self.assertTrue(issubclass(AddProductionEntry, event.Event))
        self.assertTrue(issubclass(AttackPos, event.Event))
        self.assertTrue(issubclass(AttackUnit, event.Event))
        self.assertTrue(issubclass(Supply, event.Event))
        self.assertTrue(issubclass(Fix, event.Event))
        self.assertTrue(issubclass(Collect, event.Event))
        self.assertTrue(issubclass(ChangeDest, event.Event))
        self.assertTrue(issubclass(Move, event.Event))
        self.assertTrue(issubclass(Create, event.Event))
        self.assertTrue(issubclass(Destroy, event.Event))
        self.assertTrue(issubclass(Capture, event.Event))

    def test_constructor(self):
        
 
if __name__ == '__main__':
    unittest.main()
