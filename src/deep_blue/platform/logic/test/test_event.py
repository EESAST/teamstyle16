import unittest
# from .. import basic
from .. import event

class TestEvent(unittest.TestCase):
    """Test case for Event"""

    def test_class_inherit(self):
        """Test class relationships based on API"""
        self.assertTrue(issubclass(event.Event, object))
        self.assertTrue(issubclass(event.AddProductionEntry, event.Event))
        self.assertTrue(issubclass(event.AttackPos, event.Event))
        self.assertTrue(issubclass(event.AttackUnit, event.Event))
        self.assertTrue(issubclass(event.Supply, event.Event))
        self.assertTrue(issubclass(event.Fix, event.Event))
        self.assertTrue(issubclass(event.Collect, event.Event))
        self.assertTrue(issubclass(event.ChangeDest, event.Event))
        self.assertTrue(issubclass(event.Move, event.Event))
        self.assertTrue(issubclass(event.Create, event.Event))
        self.assertTrue(issubclass(event.Destroy, event.Event))
        self.assertTrue(issubclass(event.Capture, event.Event))
 
if __name__ == '__main__':
    unittest.main()
