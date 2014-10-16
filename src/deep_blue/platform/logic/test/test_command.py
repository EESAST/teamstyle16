import unittest
from .. import basic
from .. import command

class TestCommand(unittest.TestCase):
    """Test case for Command"""

    def test_class_inherit(self):
        """Test Command inheritance"""
        self.assertTrue(issubclass(command.Command, object))
        self.assertTrue(issubclass(command.AttackPos, command.Command))
        self.assertTrue(issubclass(command.AttackUnit, command.Command))
        self.assertTrue(issubclass(command.ChangeDest, command.Command))
        self.assertTrue(issubclass(command.Fix, command.Command))
        self.assertTrue(issubclass(command.Produce, command.Command))
        self.assertTrue(issubclass(command.Supply, command.Command))
        self.assertTrue(issubclass(command.Cancel, command.Command))

    def test_constructor(self):
        """Test contruction of Command"""
        pos = basic.Position(3, 4, 5)

        cmd = command.AttackPos(2, pos)
        self.assertEqual(2, cmd.operand)
        self.assertEqual(pos, cmd.pos)

        cmd = command.AttackUnit(6, 9)
        self.assertEqual(6, cmd.operand)
        self.assertEqual(9, cmd.target)

        cmd = command.ChangeDest(4, pos)
        self.assertEqual(4, cmd.operand)
        self.assertEqual(pos, cmd.dest)

        cmd = command.Fix(8, 22)
        self.assertEqual(8, cmd.operand)
        self.assertEqual(22, cmd.target)

        cmd = command.Produce(6)
        self.assertEqual(6, cmd.kind)

        cmd = command.Supply(99, 88, -1, -1, 76)
        self.assertEqual(99, cmd.operand)
        self.assertEqual(88, cmd.target)
        self.assertEqual(-1, cmd.fuel)
        self.assertEqual(-1, cmd.ammo)
        self.assertEqual(76, cmd.metal)

if __name__ == '__main__':
    unittest.main()
